#include "omp_apps.h"

/* -------------------------------Utilties, Do Not Modify------------------------------*/
double* gen_array(int n) {
    double* array = (double*)malloc(n * sizeof(double));
    for (int i = 0; i < n; i++) array[i] = drand48();
    return array;
}

int verify(double* x, double* y, void(*funct)(double *x, double *y, double *z)) {
    double *z_v_add = (double*) malloc(ARRAY_SIZE*sizeof(double));
    double *z_oracle = (double*) malloc(ARRAY_SIZE*sizeof(double));
    (*funct)(x, y, z_v_add);
    for(int i=0; i<ARRAY_SIZE; i++){
        z_oracle[i] = x[i] + y[i];
    }
    for(int i=0; i<ARRAY_SIZE; i++){
        if(z_oracle[i] != z_v_add[i])
            return 0;
    }
    return 1;
}

/* -------------------------------Vector Addition------------------------------*/
void v_add_naive(double* x, double* y, double* z) {
    #pragma omp parallel
    {
        for (int i = 0; i < ARRAY_SIZE; i++) {
            z[i] = x[i] + y[i];
        }
    }
}

// Adjacent Method
void v_add_optimized_adjacent(double* x, double* y, double* z) {
    #pragma omp parallel
    {
        int n_threads = omp_get_num_threads();
        int thread_id = omp_get_thread_num();

        // Bad, threads will cross cache lines and that causes
        // false sharing (coherency/communication miss)
        for (int i = thread_id; i < ARRAY_SIZE; i += n_threads) {
            z[i] = x[i] + y[i];
        }
    }
}

// Chunks Method
void v_add_optimized_chunks(double* x, double* y, double* z) {
    #pragma omp parallel
    {
        int n_threads = omp_get_num_threads();
        int chunk_size = ARRAY_SIZE/n_threads;

        int thread_id = omp_get_thread_num();
        int thread_start = thread_id*chunk_size;

        // One thread handles one chunk
        for (int i = thread_start; i < thread_start + chunk_size; i++) {
            z[i] = x[i] + y[i];
        }

        if (thread_id == n_threads - 1) { // Tail/Edge case
            int remainder = ARRAY_SIZE/n_threads*n_threads;
            for (int i = remainder; i < ARRAY_SIZE; i++) {
                z[i] = x[i] + y[i];
            }
        }
    }
}

/* -------------------------------Dot Product------------------------------*/
double dotp_naive(double* x, double* y, int arr_size) {
    double global_sum = 0.0;
    #pragma omp parallel
    {
        #pragma omp for
        for (int i = 0; i < arr_size; i++)
            #pragma omp critical // Bad, everyone runs the reduction every iteration!
            global_sum += x[i] * y[i];
    }
    return global_sum;
}

// Manual Reduction
double dotp_manual_optimized(double* x, double* y, int arr_size) {
    double global_sum = 0.0;
    #pragma omp parallel
    {
        double local_sum = 0.0;
        #pragma omp for
        for (int i = 0; i < arr_size; i++)
            local_sum += x[i] * y[i];

        #pragma omp critical // Good, everyone reduces at the end only
        global_sum += local_sum;
    }
    return global_sum;
}

// Reduction Keyword
double dotp_reduction_optimized(double* x, double* y, int arr_size) {
   double global_sum = 0.0;
   #pragma omp parallel
   {
       #pragma omp for reduction(+:global_sum)
       for (int i = 0; i < arr_size; i++) {
           global_sum += x[i] * y[i];
       }
   }
   return global_sum;
}


char* compute_dotp(int arr_size) {
    // Generate input vectors
    char* report_buf = (char*)malloc(BUF_SIZE), *pos = report_buf;
    double start_time, run_time;

    double *x = gen_array(arr_size), *y = gen_array(arr_size);
    double serial_result = 0.0, result = 0.0;

    // calculate result serially
    for (int i = 0; i < arr_size; i++) {
        serial_result += x[i] * y[i];
    }

    int num_threads = omp_get_max_threads();
    for (int i = 1; i <= num_threads; i++) {
        omp_set_num_threads(i);
        start_time = omp_get_wtime();
        for (int j = 0; j < REPEAT; j++) result = dotp_manual_optimized(x, y, arr_size);
        run_time = omp_get_wtime() - start_time;
        pos += sprintf(pos, "Manual Optimized: %d thread(s) took %f seconds\n", i, run_time);

        // verify result is correct (within some threshold)
        if (fabs(serial_result - result) > 0.001) {
            pos += sprintf(pos, "Incorrect result!\n");
            *pos = '\0';
            return report_buf;
        }
    }

    for (int i = 1; i <= num_threads; i++) {
        omp_set_num_threads(i);
        start_time = omp_get_wtime();

        for (int j = 0; j < REPEAT; j++) {
            result = dotp_reduction_optimized(x, y, arr_size);
        }

        run_time = omp_get_wtime() - start_time;
        pos += sprintf(pos, "Reduction Optimized: %d thread(s) took %f seconds\n",
                       i, run_time);

        // verify result is correct (within some threshold)
        if (fabs(serial_result - result) > 0.001) {
            pos += sprintf(pos, "Incorrect result!\n");
            *pos = '\0';
            return report_buf;
        }
    }

    // Only run this once because it's too slow..
    omp_set_num_threads(1);
    start_time = omp_get_wtime();
    for (int j = 0; j < REPEAT; j++) result = dotp_naive(x, y, arr_size);
    run_time = omp_get_wtime() - start_time;

    pos += sprintf(pos, "Naive: %d thread(s) took %f seconds\n", 1, run_time);
    *pos = '\0';
    return report_buf;
}

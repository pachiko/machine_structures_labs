.globl factorial

.data
n: .word 4 # n!

.text
# Don't worry about understanding the code in main
# You'll learn more about function calls in lecture soon
main:
    la t0, n # load the address of n into t0
    lw a0, 0(t0) # dereference n
    jal ra, factorial

    addi a1, a0, 0 # load the result
    addi a0, x0, 1 # print integer
    ecall # Print Result

    addi a1, x0, '\n' # load the char
    addi a0, x0, 11 # print char
    ecall # Print newline

    addi a0, x0, 10 # quit
    ecall # Exit

# factorial takes one argument:
# a0 contains the number which we want to compute the factorial of
# The return value should be stored in a0
factorial:
    add t0, x0, a0 # init counter with n
    addi a0, x0, 1 # init result with 1
    
    loop:
        beq t0, x0, done # while n > 0
        mul a0, a0, t0 # res = res * n
        addi t0, t0, -1 # n--
        j loop

    # This is how you return from a function. You'll learn more about this later.
    # This should be the last line in your program.
    done:
        jr ra

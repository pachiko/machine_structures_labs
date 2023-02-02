.data
source: .word  3, 1, 4, 1, 5, 9, 0
dest: .word 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
sum:   .asciiz "Sum:  "

.text
 main:
     # If someone calls main, main should save sX and ra registers; then restore them at the end
     # BEGIN PROLOGUE
     # END PROLOGUE
    add t0, x0, x0 # k = 0
    add s0, x0, x0 # sum = 0
    
    la s1, source # &source[0]
    la s2, dest # &dest[0]
        
    loop: 
        add t1, s1, t0 # &source[k]
        lw a0, 0(t1) # *source[k]
        
        beq a0, x0, print_sum # DONE source[k] == 0
        
        addi sp, sp, -4 # save...
        sw t0, 0(sp) # t0

        jal fun

        lw t0, 0(sp) # t0
        addi sp, sp, 4 # loaded...

        add t2, s2, t0 # &dest[k]
        sw a1, 0(t2) # *dest[k] = ret val

        add s0, s0, a1 # sum += dest[k]
        addi t0, t0, 4 # k++ (FOUR BYTES!)
        j loop
    
    print_sum:
        la a0, sum
        jal print_str
        add a0, x0, s0
        jal print_int
        jal print_newline
        
     # BEGIN EPILOGUE
     # END EPILOGUE
    jal terminate
        
fun: # let x = a0
    add t0, a0, x0
    xori t0, t0, 0xFFFFFFFF
    addi t0, t0, 1 # -x
    
    addi t1, a0, 1 # x + 1
    
    mul a1, t0, t1 # -x * (x + 1)
    jr ra

# prints out one integer
# input values: a0: the integer to print
# does not return anything
print_int:
	# to print an integer, we need to make an ecall with a0 set to 1
    # the thing that will be printed is stored in register a1
    # this line copies the integer to be printed into a1
    mv a1, a0
    # set register a0 to 1 so that the ecall will print
    li a0, 1
    # print the integer
    ecall
    # return to the calling function
    jr    ra

# prints out a string
print_str:
    mv a1, a0
    li a0, 4 # tells ecall to print the string that a1 points to
    ecall
    jr    ra

print_newline:
    li a1, '\n'
    li a0, 11 # tells ecall to print the character in a1
    ecall
    jr    ra

terminate:
	# passing 10 to ecall will terminate the program
    li a0, 10
    ecall
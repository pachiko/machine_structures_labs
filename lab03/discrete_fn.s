.globl f # this allows other files to find the function f

# f takes in two arguments:
# a0 is the value we want to evaluate f at
# a1 is the address of the "output" array (defined above).
# The return value should be stored in a0
f:
    addi t0, a0, 3 # why on earth do we have negative index?
    slli t0, t0, 2 # multiply the offset index by 4 bytes (2^2)
    add t0, t0, a1 # offset cannot be register, so apply it directly on the address
    lw a0, 0(t0) # "zero" offset
    # This is how you return from a function. You'll learn more about this later.
    # This should be the last line in your program.
    jr ra  
.global ret42
ret42:
    mov x0, #42  // Return 42
    ret

.global ret0
ret0:
    mov x0, #0   // Return 0
    ret

.global add_two_numbers
add_two_numbers:
    // x0 = first argument, x1 = second argument
    add x0, x0, x1  // Add the two numbers and store the result in x0 (return register)
    ret

.global subtract_two_numbers
subtract_two_numbers:
    // x0 = first argument, x1 = second argument
    sub x0, x0, x1  // Subtract the second number from the first and store the result in x0 (return register)
    ret

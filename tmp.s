.global f
f:
  stp x29, x30, [sp, -16]!
  mov x29, sp
  sub sp, sp, #16
  str x0, [fp, -8]
  add x9, fp, -8
  ldr x9, [x9]
  str x9, [sp, -16]!
  mov x9, 1
  mov x10, 8
  mul x9, x10, x9
  ldr x10, [sp], 16
  add x9, x9, x10
  ldr x9, [x9]
  b .L.return.f
.L.return.f:
  add sp, sp, #16
  ldp x29, x30, [sp], 16
  mov x0, x9
  ret
.global main
main:
  stp x29, x30, [sp, -16]!
  mov x29, sp
  sub sp, sp, #16
  mov x9, 5
  str x9, [fp, -16]
  mov x9, 130
  str x9, [fp, -8]
  add x9, fp, -16
  str x9, [sp, -16]!
  ldr x0, [sp], 16
  bl f
  mov x9, x0
  b .L.return.main
.L.return.main:
  add sp, sp, #16
  ldp x29, x30, [sp], 16
  mov x0, x9
  ret

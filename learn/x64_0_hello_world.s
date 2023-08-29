
section .data:
  ; db: "Define Byte (8 bits)
  ;   Alternative include:
  ;     define word (dw)
  ;     define double word (dd)
  ; 
  ; This defines "Hello World" + '\n' + null terminating values at the back
  msg: db "Hello World", 10, 0 

  ; 2nd param is actually "$ - msg"
  ;   $ means current address while msg means msg's address.
  ;   The result is, assuming each char is 1 byte, the sizeof("Hello World")
  msglen: equ $ - msg 
  



section .text
  global _start


fn_exit_program:
  mov rax, 60       ; exit(
  mov rdi, 0        ;  EXIT_SUCCESS
  syscall           ; );

_start:
  ; Writes "Hello World"
  mov rax, 1        ; write(
  mov rdi, 1        ;   STDOUT_FILENO,
  mov rsi, msg      ;   "Hello World",
  mov rdx, msglen   ;   sizeof("Hello World"\n")
  syscall           ; ); // invokes the call

  ; Exits
  call fn_exit_program





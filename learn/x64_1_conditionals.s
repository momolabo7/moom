
section .data:
  msg1: db "a >= b", 10, 0 
  msg1len: equ $ - msg1 
  

  msg2: db "a < b", 10, 0 
  msg2len: equ $ - msg2


section .text
  global _start


fn_exit_program:
  mov rax, 60       ; exit(
  mov rdi, 0        ;  EXIT_SUCCESS
  syscall           ; );
  ret

;
; Description:
;   Prints a string based on what's given in rsi and rdx
;
; Parameers:
;   rsi - message string
;   rdx - message string length
fn_print:
  push rax
  push rdi

  mov rax, 1        ; write(
  mov rdi, 1        ;   STDOUT_FILENO,
  ;mov rsi, msg1    ;   rsi, 
  ;mov rdx, msg1len ;   rdx,
  syscall           ; ); // invokes the call

  pop rax
  pop rdi
  ret

; Description:
;   Compares rax and rbx. 
;   Prints "a < b" if rax is less than rbx
;   Otherwise, prints "a >= b"
;
; Parameters:
;   rax - lhs operand
;   rbx - rhs operand
; 
; Emulates:
;   if (rax < rbx) printf("a < b\n");
;   else printf("rax >= rbx\n");
;
fn_compare_and_print:

  push rsi
  push rdx
  ; 'cmp' will subtract rbx FROM rax without modifying them
  ; The results are stored in some 'flags' register  which 
  ; are used in other instructions like cmovl or jl.
  ; 
  ; Abstractly, if rax < rbx, we can  think that cmp 'returns' true.
  ; 
  cmp rax, rbx    

  ; jl will jump to a label if previous cmp is true
  jl fn_compare_and_print_less  

  mov rsi, msg1
  mov rdx, msg1len
  call fn_print ; printf("a >= b\n")
  jmp fn_compare_and_print_continue

fn_compare_and_print_less:
  mov rsi, msg2
  mov rdx, msg2len
  call fn_print ; printf("a < b\n")

fn_compare_and_print_continue:
  pop rdx
  pop rsi
  ret


_start:
  ; Want to do something like:
  ; 
  ; int a = 5;
  ; int b = 7;
  ;
  ; if (a < b) printf("a < b\n");
  ; else printf("a >= b\n");
  ; 
  ; a = 10;
  ;   
  ; if (a < b) printf("a < b\n");
  ; else printf("a >= b\n");
  ;

  mov rax, 5      ; int a = 5;
  mov rbx, 7      ; int b = 7;

  call fn_compare_and_print

  mov rax, 10         ; a = 10
  call fn_compare_and_print

  ; Exits
  call fn_exit_program





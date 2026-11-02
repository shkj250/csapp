movq %rsp, %rax 
ret    
movq %rax,%rdi
ret
pop %rax
ret
mov %rax,%rsi
ret
lea (%rdi,%rsi,1), %rax 
ret
movq %rax ,%rdi
ret
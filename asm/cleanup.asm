;; Ran at end
call flush_output        ;; clear any remaining chars in the ouput buffer
mov       rax, 60        ;; exit
xor       rdi, rdi     
syscall                
section   .data        

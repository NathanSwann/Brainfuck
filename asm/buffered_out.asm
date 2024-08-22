output:
    mov r10, output_buffer                    ;; start of the message
    add r10b, byte [output_buffer_pointer]    ;; move to nearest free byte
    mov r11b, byte [tape+r9*1]                ;; get the current byte on the tape
    mov byte [r10], r11b                      ;; copy current tape head into buffer
    inc byte [output_buffer_pointer]          ;; move the head by 1
    cmp byte [output_buffer_pointer], 255     ;; is buffer full?
    jb .__ok                                  ;; if not jump to done
    call flush_output                         ;; flush the ouput buffer
.__ok:                                        ;; buffer is not full:
    ret                                       ;; done
flush_output:
    mov rsi, output_buffer                    ;; output the contents of the output_buffer
    mov rdx, 0                                ;; clear upper parts of rdx
    mov dl, byte [output_buffer_pointer]      ;; we want to write pointer number of chars
    mov rax, 1                                ;; to standard ouput
    mov rdi, 1                                ;; write syscall number
    syscall                                   ;; call the syscall
    mov byte [output_buffer_pointer], 0       ;; reset the pointer
    ret                                       ;; done
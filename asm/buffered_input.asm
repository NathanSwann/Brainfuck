input:
    mov r10, input_buffer
    mov r11, 0
    mov r11b, byte[input_buffer_pointer]
    cmp r11b, byte[input_buffer_len]
    jb .__get_data
    call fetch_more_input
    mov r11, 0
.__get_data:
    cmp byte[input_buffer_len],0
    je .__stream_end
    mov r10b, byte[input_buffer+r11*1]
    mov byte[tape+r9*1], r10b
    inc r11
    mov byte [input_buffer_pointer], r11b
    ret
.__stream_end:
    mov byte[tape+r9*1], 0
    ret

fetch_more_input:
    call flush_output               ;; incase we wrote a prompt flush before reading
    mov rax, 0                      ;; read
    mov rdi, 0                      ;; standard in 
    mov rsi, input_buffer           ;; store result in input_buffer
    mov rdx, 255                    ;; at most 255 characters
    syscall
    mov byte [input_buffer_len], al ;; store n chars read
    ret
tape: times 255 db 0 ;; main tape

;; for buffered output
output_buffer_pointer: db 0 
output_buffer: times 255 db 0 

;; for buffered input
input_buffer_len: db 0
input_buffer_pointer: db 0
input_buffer: times 255 db 0
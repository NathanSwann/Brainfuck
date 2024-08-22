All ASM that gets statically added to the main source,

During the build process all .asm files get added to "asm_data.h" from which they can then be added to the 
compiled output.

| File                | Description                                                                         |
| ------------------- | ----------------------------------------------------------------------------------- |
| buffered_input.asm  | For reading from standard input in a buffered fashion to prevent excessive syscalls |
| buffered_output.asm | For buffering writes to prevent excessive syscalls                                  |
| cleanup.asm         | Called at the end of script execution, any final actions go here                    |
| data_segment.asm    | Injected into the data segment for any variables that need to be defined            |


https://www.chromium.org/chromium-os/developer-library/reference/linux-constants/syscalls/
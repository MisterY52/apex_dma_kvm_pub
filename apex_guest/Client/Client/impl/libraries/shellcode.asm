PUBLIC _spoofer_stub
 
.code
 
_spoofer_stub PROC
    pop r11
    add rsp, 8
    mov rax, [rsp + 24]
    
    mov r10, [rax]
    mov [rsp], r10
    
    mov r10, [rax + 8] 
    mov [rax + 8], r11 
 
    mov [rax + 16], rsi
    lea rsi, fixup
    mov [rax], rsi
    mov rsi, rax
    
    jmp r10
 
fixup:
    sub rsp, 16
    mov rcx, rsi
    mov rsi, [rcx + 16]
    jmp QWORD PTR [rcx + 8]
_spoofer_stub ENDP
 
END 
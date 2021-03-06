.section .data
.global sys_call_table
sys_call_table:
.long sys_print
.long yield
.long handle_exit
.long get_pid
.long fork

.section .text

# System Call Handler
.global interupt_handler_128
interupt_handler_128:
    push %eax
    push %ebx
    push %ecx
    push %edx

    push %esi
    push %edi

    push %ebp

    shl $2, %eax                   # eax *= 4
    mov sys_call_table(%eax), %eax # eax = sys_call_table[eax]
    call *%eax                     # call eax

    call sys_return                # sys calls should call this method explicitly at their end to stop compiler errors, otherwise gcc modifies stack

.global sys_return
sys_return:
    add $4, %esp
    pop %ebp

    pop %edi
    pop %esi

    pop %edx
    pop %ecx
    pop %ebx
    pop %eax
    iret

.macro no_error_code_interupt_handler n
.global interupt_handler_\n
interupt_handler_\n:
    push $0
    push $\n
    jmp common_interupt_handler
.endm
    
.macro interupt_handler n
.global interupt_handler_\n
interupt_handler_\n:
    push $\n
    jmp common_interupt_handler
.endm

common_interupt_handler:
    push %eax
    push %ebx
    push %ecx
    push %edx

    push %esi
    push %edi
 
    push %ebp

    call interupt_handler

    pop %ebp
    
    pop %edi
    pop %esi

    pop %edx
    pop %ecx
    pop %ebx
    pop %eax
    
    add $8, %esp
    iret

no_error_code_interupt_handler 0
no_error_code_interupt_handler 1
no_error_code_interupt_handler 2
no_error_code_interupt_handler 3
no_error_code_interupt_handler 4
no_error_code_interupt_handler 5
no_error_code_interupt_handler 6
interupt_handler 7
no_error_code_interupt_handler 8
no_error_code_interupt_handler 9
interupt_handler 10
interupt_handler 11
interupt_handler 12
interupt_handler 13
interupt_handler 14
no_error_code_interupt_handler 15
no_error_code_interupt_handler 16
interupt_handler 17
no_error_code_interupt_handler 18
no_error_code_interupt_handler 19
no_error_code_interupt_handler 20
no_error_code_interupt_handler 21
no_error_code_interupt_handler 22
no_error_code_interupt_handler 23
no_error_code_interupt_handler 24
no_error_code_interupt_handler 25
no_error_code_interupt_handler 26
no_error_code_interupt_handler 27
no_error_code_interupt_handler 28
no_error_code_interupt_handler 29
no_error_code_interupt_handler 30
no_error_code_interupt_handler 31
no_error_code_interupt_handler 32
no_error_code_interupt_handler 33
no_error_code_interupt_handler 34
no_error_code_interupt_handler 35
no_error_code_interupt_handler 36
no_error_code_interupt_handler 37
no_error_code_interupt_handler 38
no_error_code_interupt_handler 39
no_error_code_interupt_handler 40
no_error_code_interupt_handler 41
no_error_code_interupt_handler 42
no_error_code_interupt_handler 43
no_error_code_interupt_handler 44
no_error_code_interupt_handler 45
no_error_code_interupt_handler 46
no_error_code_interupt_handler 47
no_error_code_interupt_handler 48
no_error_code_interupt_handler 49
no_error_code_interupt_handler 50
no_error_code_interupt_handler 51
no_error_code_interupt_handler 52
no_error_code_interupt_handler 53
no_error_code_interupt_handler 54
no_error_code_interupt_handler 55
no_error_code_interupt_handler 56
no_error_code_interupt_handler 57
no_error_code_interupt_handler 58
no_error_code_interupt_handler 59
no_error_code_interupt_handler 60
no_error_code_interupt_handler 61
no_error_code_interupt_handler 62
no_error_code_interupt_handler 63
no_error_code_interupt_handler 64
no_error_code_interupt_handler 65
no_error_code_interupt_handler 66
no_error_code_interupt_handler 67
no_error_code_interupt_handler 68
no_error_code_interupt_handler 69
no_error_code_interupt_handler 70
no_error_code_interupt_handler 71
no_error_code_interupt_handler 72
no_error_code_interupt_handler 73
no_error_code_interupt_handler 74
no_error_code_interupt_handler 75
no_error_code_interupt_handler 76
no_error_code_interupt_handler 77
no_error_code_interupt_handler 78
no_error_code_interupt_handler 79
no_error_code_interupt_handler 80
no_error_code_interupt_handler 81
no_error_code_interupt_handler 82
no_error_code_interupt_handler 83
no_error_code_interupt_handler 84
no_error_code_interupt_handler 85
no_error_code_interupt_handler 86
no_error_code_interupt_handler 87
no_error_code_interupt_handler 88
no_error_code_interupt_handler 89
no_error_code_interupt_handler 90
no_error_code_interupt_handler 91
no_error_code_interupt_handler 92
no_error_code_interupt_handler 93
no_error_code_interupt_handler 94
no_error_code_interupt_handler 95
no_error_code_interupt_handler 96
no_error_code_interupt_handler 97
no_error_code_interupt_handler 98
no_error_code_interupt_handler 99
no_error_code_interupt_handler 100
no_error_code_interupt_handler 101
no_error_code_interupt_handler 102
no_error_code_interupt_handler 103
no_error_code_interupt_handler 104
no_error_code_interupt_handler 105
no_error_code_interupt_handler 106
no_error_code_interupt_handler 107
no_error_code_interupt_handler 108
no_error_code_interupt_handler 109
no_error_code_interupt_handler 110
no_error_code_interupt_handler 111
no_error_code_interupt_handler 112
no_error_code_interupt_handler 113
no_error_code_interupt_handler 114
no_error_code_interupt_handler 115
no_error_code_interupt_handler 116
no_error_code_interupt_handler 117
no_error_code_interupt_handler 118
no_error_code_interupt_handler 119
no_error_code_interupt_handler 120
no_error_code_interupt_handler 121
no_error_code_interupt_handler 122
no_error_code_interupt_handler 123
no_error_code_interupt_handler 124
no_error_code_interupt_handler 125
no_error_code_interupt_handler 126
no_error_code_interupt_handler 127
# no_error_code_interupt_handler 128 SYSTEM CALL NOW
no_error_code_interupt_handler 129
no_error_code_interupt_handler 130
no_error_code_interupt_handler 131
no_error_code_interupt_handler 132
no_error_code_interupt_handler 133
no_error_code_interupt_handler 134
no_error_code_interupt_handler 135
no_error_code_interupt_handler 136
no_error_code_interupt_handler 137
no_error_code_interupt_handler 138
no_error_code_interupt_handler 139
no_error_code_interupt_handler 140
no_error_code_interupt_handler 141
no_error_code_interupt_handler 142
no_error_code_interupt_handler 143
no_error_code_interupt_handler 144
no_error_code_interupt_handler 145
no_error_code_interupt_handler 146
no_error_code_interupt_handler 147
no_error_code_interupt_handler 148
no_error_code_interupt_handler 149
no_error_code_interupt_handler 150
no_error_code_interupt_handler 151
no_error_code_interupt_handler 152
no_error_code_interupt_handler 153
no_error_code_interupt_handler 154
no_error_code_interupt_handler 155
no_error_code_interupt_handler 156
no_error_code_interupt_handler 157
no_error_code_interupt_handler 158
no_error_code_interupt_handler 159
no_error_code_interupt_handler 160
no_error_code_interupt_handler 161
no_error_code_interupt_handler 162
no_error_code_interupt_handler 163
no_error_code_interupt_handler 164
no_error_code_interupt_handler 165
no_error_code_interupt_handler 166
no_error_code_interupt_handler 167
no_error_code_interupt_handler 168
no_error_code_interupt_handler 169
no_error_code_interupt_handler 170
no_error_code_interupt_handler 171
no_error_code_interupt_handler 172
no_error_code_interupt_handler 173
no_error_code_interupt_handler 174
no_error_code_interupt_handler 175
no_error_code_interupt_handler 176
no_error_code_interupt_handler 177
no_error_code_interupt_handler 178
no_error_code_interupt_handler 179
no_error_code_interupt_handler 180
no_error_code_interupt_handler 181
no_error_code_interupt_handler 182
no_error_code_interupt_handler 183
no_error_code_interupt_handler 184
no_error_code_interupt_handler 185
no_error_code_interupt_handler 186
no_error_code_interupt_handler 187
no_error_code_interupt_handler 188
no_error_code_interupt_handler 189
no_error_code_interupt_handler 190
no_error_code_interupt_handler 191
no_error_code_interupt_handler 192
no_error_code_interupt_handler 193
no_error_code_interupt_handler 194
no_error_code_interupt_handler 195
no_error_code_interupt_handler 196
no_error_code_interupt_handler 197
no_error_code_interupt_handler 198
no_error_code_interupt_handler 199
no_error_code_interupt_handler 200
no_error_code_interupt_handler 201
no_error_code_interupt_handler 202
no_error_code_interupt_handler 203
no_error_code_interupt_handler 204
no_error_code_interupt_handler 205
no_error_code_interupt_handler 206
no_error_code_interupt_handler 207
no_error_code_interupt_handler 208
no_error_code_interupt_handler 209
no_error_code_interupt_handler 210
no_error_code_interupt_handler 211
no_error_code_interupt_handler 212
no_error_code_interupt_handler 213
no_error_code_interupt_handler 214
no_error_code_interupt_handler 215
no_error_code_interupt_handler 216
no_error_code_interupt_handler 217
no_error_code_interupt_handler 218
no_error_code_interupt_handler 219
no_error_code_interupt_handler 220
no_error_code_interupt_handler 221
no_error_code_interupt_handler 222
no_error_code_interupt_handler 223
no_error_code_interupt_handler 224
no_error_code_interupt_handler 225
no_error_code_interupt_handler 226
no_error_code_interupt_handler 227
no_error_code_interupt_handler 228
no_error_code_interupt_handler 229
no_error_code_interupt_handler 230
no_error_code_interupt_handler 231
no_error_code_interupt_handler 232
no_error_code_interupt_handler 233
no_error_code_interupt_handler 234
no_error_code_interupt_handler 235
no_error_code_interupt_handler 236
no_error_code_interupt_handler 237
no_error_code_interupt_handler 238
no_error_code_interupt_handler 239
no_error_code_interupt_handler 240
no_error_code_interupt_handler 241
no_error_code_interupt_handler 242
no_error_code_interupt_handler 243
no_error_code_interupt_handler 244
no_error_code_interupt_handler 245
no_error_code_interupt_handler 246
no_error_code_interupt_handler 247
no_error_code_interupt_handler 248
no_error_code_interupt_handler 249
no_error_code_interupt_handler 250
no_error_code_interupt_handler 251
no_error_code_interupt_handler 252
no_error_code_interupt_handler 253
no_error_code_interupt_handler 254
no_error_code_interupt_handler 255

.global load_idt
load_idt:
    push %eax
    mov 8(%esp), %eax

    lidt (%eax)
    sti

    pop %eax
    ret
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <kernel/input.h>
#include "interupts.h"
#include "io.h"
#include "sysreturn.h"
#include <stdio.h>
#include <stdlib.h>
#include <kernel/process.h>

#define PIC1		0x20		/* IO base address for master PIC */
#define PIC2		0xA0		/* IO base address for slave PIC */
#define PIC1_COMMAND	PIC1
#define PIC1_DATA	(PIC1+1)
#define PIC2_COMMAND	PIC2
#define PIC2_DATA	(PIC2+1)

#define PIC_EOI		0x20		/* End-of-interrupt command code */
 
void PIC_sendEOI(unsigned char irq)
{
	if(irq >= 8)
		outb(PIC2_COMMAND,PIC_EOI);
 
	outb(PIC1_COMMAND,PIC_EOI);
}

#define ICW1_ICW4	0x01		/* ICW4 (not) needed */
#define ICW1_SINGLE	0x02		/* Single (cascade) mode */
#define ICW1_INTERVAL4	0x04		/* Call address interval 4 (8) */
#define ICW1_LEVEL	0x08		/* Level triggered (edge) mode */
#define ICW1_INIT	0x10		/* Initialization - required! */
 
#define ICW4_8086	0x01		/* 8086/88 (MCS-80/85) mode */
#define ICW4_AUTO	0x02		/* Auto (normal) EOI */
#define ICW4_BUF_SLAVE	0x08		/* Buffered mode/slave */
#define ICW4_BUF_MASTER	0x0C		/* Buffered mode/master */
#define ICW4_SFNM	0x10		/* Special fully nested (not) */
 
/*
arguments:
	offset1 - vector offset for master PIC
		vectors on the master become offset1..offset1+7
	offset2 - same for slave PIC: offset2..offset2+7
*/
void PIC_remap(int offset1, int offset2)
{
	unsigned char a1, a2;
 
	a1 = inb(PIC1_DATA);                        // save masks
	a2 = inb(PIC2_DATA);
 
	outb(PIC1_COMMAND, ICW1_INIT+ICW1_ICW4);  // starts the initialization sequence (in cascade mode)
	io_wait();
	outb(PIC2_COMMAND, ICW1_INIT+ICW1_ICW4);
	io_wait();
	outb(PIC1_DATA, offset1);                 // ICW2: Master PIC vector offset
	io_wait();
	outb(PIC2_DATA, offset2);                 // ICW2: Slave PIC vector offset
	io_wait();
	outb(PIC1_DATA, 4);                       // ICW3: tell Master PIC that there is a slave PIC at IRQ2 (0000 0100)
	io_wait();
	outb(PIC2_DATA, 2);                       // ICW3: tell Slave PIC its cascade identity (0000 0010)
	io_wait();
 
	outb(PIC1_DATA, ICW4_8086);
	io_wait();
	outb(PIC2_DATA, ICW4_8086);
	io_wait();
 
	outb(PIC1_DATA, a1);   // restore saved masks.
	outb(PIC2_DATA, a2);
}

void IRQ_set_mask(unsigned char IRQline) {
    uint16_t port;
    uint8_t value;
 
    if(IRQline < 8) {
        port = PIC1_DATA;
    } else {
        port = PIC2_DATA;
        IRQline -= 8;
    }
    value = inb(port) | (1 << IRQline);
    outb(port, value);        
}
 
void IRQ_clear_mask(unsigned char IRQline) {
    uint16_t port;
    uint8_t value;
 
    if(IRQline < 8) {
        port = PIC1_DATA;
    } else {
        port = PIC2_DATA;
        IRQline -= 8;
    }
    value = inb(port) & ~(1 << IRQline);
    outb(port, value);        
}

    #define KBD_DATA_PORT   0x60

    /** read_scan_code:
     *  Reads a scan code from the keyboard
     *
     *  @return The scan code (NOT an ASCII character!)
     */
    unsigned char read_scan_code(void)
    {
        return inb(KBD_DATA_PORT);
    }

void init_handlers(uint32_t handlers[]);

char key_code_map[130];

void init_key_code_map() {
    key_code_map[0] = '\0';//no code
    key_code_map[1] = 0x1b;//esc
    key_code_map[2] = '1';
    key_code_map[3] = '2';
    key_code_map[4] = '3';
    key_code_map[5] = '4';
    key_code_map[6] = '5';
    key_code_map[7] = '6';
    key_code_map[8] = '7';
    key_code_map[9] = '8';
    key_code_map[10] = '9';
    key_code_map[11] = '0';
    key_code_map[12] = '-';
    key_code_map[13] = '=';
    key_code_map[14] = 0x8;//backspace
    key_code_map[15] = '\t';
    key_code_map[16] = 'q';
    key_code_map[17] = 'w';
    key_code_map[18] = 'e';
    key_code_map[19] = 'r';
    key_code_map[20] = 't';
    key_code_map[21] = 'y';
    key_code_map[22] = 'u';
    key_code_map[23] = 'i';
    key_code_map[24] = 'o';
    key_code_map[25] = 'p';
    key_code_map[26] = '[';
    key_code_map[27] = ']';
    key_code_map[28] = '\n';//enter
    key_code_map[29] = '\0';//ctrl
    key_code_map[30] = 'a';
    key_code_map[31] = 's';
    key_code_map[32] = 'd';
    key_code_map[33] = 'f';
    key_code_map[34] = 'g';
    key_code_map[35] = 'h';
    key_code_map[36] = 'j';
    key_code_map[37] = 'k';
    key_code_map[38] = 'l';
    key_code_map[39] = ';';
    key_code_map[40] = '\'';
    key_code_map[41] = '`';
    key_code_map[42] = '\0';//L shift
    key_code_map[43] = '\\';
    key_code_map[44] = 'z';
    key_code_map[45] = 'x';
    key_code_map[46] = 'c';
    key_code_map[47] = 'v';
    key_code_map[48] = 'b';
    key_code_map[49] = 'n';
    key_code_map[50] = 'm';
    key_code_map[51] = ',';
    key_code_map[52] = '.';
    key_code_map[53] = '/';
    key_code_map[54] = '\0';//R shift
    key_code_map[55] = '*';//Num Pad only
    key_code_map[56] = '\0';//Alt
    key_code_map[57] = ' ';
    key_code_map[58] = '\0';//Caps Locks
}

void initialize_input(void) {
    struct idt_entry idt[256];
    uint32_t interupt_handlers[256];
    init_handlers(interupt_handlers);
    for (size_t i = 0; i < sizeof(idt) / sizeof(idt[0]); i++) {
        uint32_t addr = (uint32_t) interupt_handlers[i];
        idt[i].address_low = (uint16_t) addr;
        idt[i].segment = 0x0008;
        if (i == 0x80)
            idt[i].flags = 0xEE00;
        else
            idt[i].flags = 0x8E00;
        idt[i].address_high = (uint16_t) (addr >> 16);
    }

    struct idt_descriptor idt_desc;
    idt_desc.length = sizeof(idt) - 1;
    idt_desc.address = (uint32_t) &idt;
    load_idt((uint32_t) &idt_desc);

    PIC_remap(0x20, 0x28);
    IRQ_set_mask(0);
    init_key_code_map();
}

void print_eax(uint32_t eax) {
    printf("ASM: %#.8X\n", eax);
}

void print_edx() {
    uint32_t edx;
    asm("mov %%edx, %0" : "=r"(edx));
    printf("EDX: %#.8X\n", edx);
}

void interupt_handler(cpu_state_t cpu, uint32_t interupt, uint32_t error_code, stack_state_t stack) {
    //printf("Int: %d, Eax: %#X, Error: %d\n", interupt, cpu.eax, error_code);
    //printf("Error Code: %d\nEip: %#.8X\nEdx: %#.8X\nEcx: %#.8X\nEbx: %#.8X\nEax: %#.8X\nEsi: %#.8X\nEdi: %#.8X\nEbp: %#.8X\nCs:  %#.2X\nEsp: %#.8X\nSs:  %#.2X\nEflags: %#.8X\n", error_code, stack.eip, cpu.edx, cpu.ecx, cpu.ebx, cpu.eax, cpu.esi, cpu.edi, cpu.ebp, stack.cs, stack.esp, stack.ss, stack.eflags);
    //uint32_t error_code = 0xABCD;
    if (interupt > 0x20 - 1 && interupt < 0x20 + 16) {
        if (interupt - 0x20 == 1) {
            uint8_t code = read_scan_code();
            //printf("Eip: %#.8X\n", stack.eip);
            if (code < 129) {
                char c = key_code_map[code];
                printf("%c", c);
            }
        } else {
            printf("Int: %d, Eax: %#X, Error: %d\n", interupt, cpu.eax, error_code);    
        }
        PIC_sendEOI(interupt - 0x20);
    } else if (interupt != 255) {
        if (interupt == 14) {
            uint32_t cr2;
            uint32_t cr3;
            asm("mov %%cr2, %0" : "=r" (cr2));
            asm("mov %%cr3, %0" : "=r" (cr3));
            printf("CR2: %#.8X Error Code: %d Eip: %#.8X CR3: %#.8X\nEdx: %#.8X Ecx: %#.8X Ebx: %#.8X Eax: %#.8X\nEsi: %#.8X Edi: %#.8X Ebp: %#.8X Cs: %#.2X\nEsp: %#.8X Ss: %#.2X Eflags: %#.8X\n", cr2, error_code, stack.eip, cr3, cpu.edx, cpu.ecx, cpu.ebx, cpu.eax, cpu.esi, cpu.edi, cpu.ebp, stack.cs, stack.esp, stack.ss, stack.eflags);
            while (1);
        } else { 
            printf("Int: %d, Eax: %#.8X, Error: %d, Eip: %#.8X\n", interupt, cpu.eax, error_code, stack.eip);
        }
    }
}

void sys_print(process_state_t process_state) {
    printf("%s", (char*) process_state.cpu.ebx);
    process_state.cpu.ecx = true;
    sys_return(process_state);
}

void init_handlers(uint32_t handlers[]) {
    handlers[0] = (uint32_t) &interupt_handler_0;
    handlers[1] = (uint32_t) &interupt_handler_1;
    handlers[2] = (uint32_t) &interupt_handler_2;
    handlers[3] = (uint32_t) &interupt_handler_3;
    handlers[4] = (uint32_t) &interupt_handler_4;
    handlers[5] = (uint32_t) &interupt_handler_5;
    handlers[6] = (uint32_t) &interupt_handler_6;
    handlers[7] = (uint32_t) &interupt_handler_7;
    handlers[8] = (uint32_t) &interupt_handler_8;
    handlers[9] = (uint32_t) &interupt_handler_9;
    handlers[10] = (uint32_t) &interupt_handler_10;
    handlers[11] = (uint32_t) &interupt_handler_11;
    handlers[12] = (uint32_t) &interupt_handler_12;
    handlers[13] = (uint32_t) &interupt_handler_13;
    handlers[14] = (uint32_t) &interupt_handler_14;
    handlers[15] = (uint32_t) &interupt_handler_15;
    handlers[16] = (uint32_t) &interupt_handler_16;
    handlers[17] = (uint32_t) &interupt_handler_17;
    handlers[18] = (uint32_t) &interupt_handler_18;
    handlers[19] = (uint32_t) &interupt_handler_19;
    handlers[20] = (uint32_t) &interupt_handler_20;
    handlers[21] = (uint32_t) &interupt_handler_21;
    handlers[22] = (uint32_t) &interupt_handler_22;
    handlers[23] = (uint32_t) &interupt_handler_23;
    handlers[24] = (uint32_t) &interupt_handler_24;
    handlers[25] = (uint32_t) &interupt_handler_25;
    handlers[26] = (uint32_t) &interupt_handler_26;
    handlers[27] = (uint32_t) &interupt_handler_27;
    handlers[28] = (uint32_t) &interupt_handler_28;
    handlers[29] = (uint32_t) &interupt_handler_29;
    handlers[30] = (uint32_t) &interupt_handler_30;
    handlers[31] = (uint32_t) &interupt_handler_31;
    handlers[32] = (uint32_t) &interupt_handler_32;
    handlers[33] = (uint32_t) &interupt_handler_33;
    handlers[34] = (uint32_t) &interupt_handler_34;
    handlers[35] = (uint32_t) &interupt_handler_35;
    handlers[36] = (uint32_t) &interupt_handler_36;
    handlers[37] = (uint32_t) &interupt_handler_37;
    handlers[38] = (uint32_t) &interupt_handler_38;
    handlers[39] = (uint32_t) &interupt_handler_39;
    handlers[40] = (uint32_t) &interupt_handler_40;
    handlers[41] = (uint32_t) &interupt_handler_41;
    handlers[42] = (uint32_t) &interupt_handler_42;
    handlers[43] = (uint32_t) &interupt_handler_43;
    handlers[44] = (uint32_t) &interupt_handler_44;
    handlers[45] = (uint32_t) &interupt_handler_45;
    handlers[46] = (uint32_t) &interupt_handler_46;
    handlers[47] = (uint32_t) &interupt_handler_47;
    handlers[48] = (uint32_t) &interupt_handler_48;
    handlers[49] = (uint32_t) &interupt_handler_49;
    handlers[50] = (uint32_t) &interupt_handler_50;
    handlers[51] = (uint32_t) &interupt_handler_51;
    handlers[52] = (uint32_t) &interupt_handler_52;
    handlers[53] = (uint32_t) &interupt_handler_53;
    handlers[54] = (uint32_t) &interupt_handler_54;
    handlers[55] = (uint32_t) &interupt_handler_55;
    handlers[56] = (uint32_t) &interupt_handler_56;
    handlers[57] = (uint32_t) &interupt_handler_57;
    handlers[58] = (uint32_t) &interupt_handler_58;
    handlers[59] = (uint32_t) &interupt_handler_59;
    handlers[60] = (uint32_t) &interupt_handler_60;
    handlers[61] = (uint32_t) &interupt_handler_61;
    handlers[62] = (uint32_t) &interupt_handler_62;
    handlers[63] = (uint32_t) &interupt_handler_63;
    handlers[64] = (uint32_t) &interupt_handler_64;
    handlers[65] = (uint32_t) &interupt_handler_65;
    handlers[66] = (uint32_t) &interupt_handler_66;
    handlers[67] = (uint32_t) &interupt_handler_67;
    handlers[68] = (uint32_t) &interupt_handler_68;
    handlers[69] = (uint32_t) &interupt_handler_69;
    handlers[70] = (uint32_t) &interupt_handler_70;
    handlers[71] = (uint32_t) &interupt_handler_71;
    handlers[72] = (uint32_t) &interupt_handler_72;
    handlers[73] = (uint32_t) &interupt_handler_73;
    handlers[74] = (uint32_t) &interupt_handler_74;
    handlers[75] = (uint32_t) &interupt_handler_75;
    handlers[76] = (uint32_t) &interupt_handler_76;
    handlers[77] = (uint32_t) &interupt_handler_77;
    handlers[78] = (uint32_t) &interupt_handler_78;
    handlers[79] = (uint32_t) &interupt_handler_79;
    handlers[80] = (uint32_t) &interupt_handler_80;
    handlers[81] = (uint32_t) &interupt_handler_81;
    handlers[82] = (uint32_t) &interupt_handler_82;
    handlers[83] = (uint32_t) &interupt_handler_83;
    handlers[84] = (uint32_t) &interupt_handler_84;
    handlers[85] = (uint32_t) &interupt_handler_85;
    handlers[86] = (uint32_t) &interupt_handler_86;
    handlers[87] = (uint32_t) &interupt_handler_87;
    handlers[88] = (uint32_t) &interupt_handler_88;
    handlers[89] = (uint32_t) &interupt_handler_89;
    handlers[90] = (uint32_t) &interupt_handler_90;
    handlers[91] = (uint32_t) &interupt_handler_91;
    handlers[92] = (uint32_t) &interupt_handler_92;
    handlers[93] = (uint32_t) &interupt_handler_93;
    handlers[94] = (uint32_t) &interupt_handler_94;
    handlers[95] = (uint32_t) &interupt_handler_95;
    handlers[96] = (uint32_t) &interupt_handler_96;
    handlers[97] = (uint32_t) &interupt_handler_97;
    handlers[98] = (uint32_t) &interupt_handler_98;
    handlers[99] = (uint32_t) &interupt_handler_99;
    handlers[100] = (uint32_t) &interupt_handler_100;
    handlers[101] = (uint32_t) &interupt_handler_101;
    handlers[102] = (uint32_t) &interupt_handler_102;
    handlers[103] = (uint32_t) &interupt_handler_103;
    handlers[104] = (uint32_t) &interupt_handler_104;
    handlers[105] = (uint32_t) &interupt_handler_105;
    handlers[106] = (uint32_t) &interupt_handler_106;
    handlers[107] = (uint32_t) &interupt_handler_107;
    handlers[108] = (uint32_t) &interupt_handler_108;
    handlers[109] = (uint32_t) &interupt_handler_109;
    handlers[110] = (uint32_t) &interupt_handler_110;
    handlers[111] = (uint32_t) &interupt_handler_111;
    handlers[112] = (uint32_t) &interupt_handler_112;
    handlers[113] = (uint32_t) &interupt_handler_113;
    handlers[114] = (uint32_t) &interupt_handler_114;
    handlers[115] = (uint32_t) &interupt_handler_115;
    handlers[116] = (uint32_t) &interupt_handler_116;
    handlers[117] = (uint32_t) &interupt_handler_117;
    handlers[118] = (uint32_t) &interupt_handler_118;
    handlers[119] = (uint32_t) &interupt_handler_119;
    handlers[120] = (uint32_t) &interupt_handler_120;
    handlers[121] = (uint32_t) &interupt_handler_121;
    handlers[122] = (uint32_t) &interupt_handler_122;
    handlers[123] = (uint32_t) &interupt_handler_123;
    handlers[124] = (uint32_t) &interupt_handler_124;
    handlers[125] = (uint32_t) &interupt_handler_125;
    handlers[126] = (uint32_t) &interupt_handler_126;
    handlers[127] = (uint32_t) &interupt_handler_127;
    handlers[128] = (uint32_t) &interupt_handler_128;
    handlers[129] = (uint32_t) &interupt_handler_129;
    handlers[130] = (uint32_t) &interupt_handler_130;
    handlers[131] = (uint32_t) &interupt_handler_131;
    handlers[132] = (uint32_t) &interupt_handler_132;
    handlers[133] = (uint32_t) &interupt_handler_133;
    handlers[134] = (uint32_t) &interupt_handler_134;
    handlers[135] = (uint32_t) &interupt_handler_135;
    handlers[136] = (uint32_t) &interupt_handler_136;
    handlers[137] = (uint32_t) &interupt_handler_137;
    handlers[138] = (uint32_t) &interupt_handler_138;
    handlers[139] = (uint32_t) &interupt_handler_139;
    handlers[140] = (uint32_t) &interupt_handler_140;
    handlers[141] = (uint32_t) &interupt_handler_141;
    handlers[142] = (uint32_t) &interupt_handler_142;
    handlers[143] = (uint32_t) &interupt_handler_143;
    handlers[144] = (uint32_t) &interupt_handler_144;
    handlers[145] = (uint32_t) &interupt_handler_145;
    handlers[146] = (uint32_t) &interupt_handler_146;
    handlers[147] = (uint32_t) &interupt_handler_147;
    handlers[148] = (uint32_t) &interupt_handler_148;
    handlers[149] = (uint32_t) &interupt_handler_149;
    handlers[150] = (uint32_t) &interupt_handler_150;
    handlers[151] = (uint32_t) &interupt_handler_151;
    handlers[152] = (uint32_t) &interupt_handler_152;
    handlers[153] = (uint32_t) &interupt_handler_153;
    handlers[154] = (uint32_t) &interupt_handler_154;
    handlers[155] = (uint32_t) &interupt_handler_155;
    handlers[156] = (uint32_t) &interupt_handler_156;
    handlers[157] = (uint32_t) &interupt_handler_157;
    handlers[158] = (uint32_t) &interupt_handler_158;
    handlers[159] = (uint32_t) &interupt_handler_159;
    handlers[160] = (uint32_t) &interupt_handler_160;
    handlers[161] = (uint32_t) &interupt_handler_161;
    handlers[162] = (uint32_t) &interupt_handler_162;
    handlers[163] = (uint32_t) &interupt_handler_163;
    handlers[164] = (uint32_t) &interupt_handler_164;
    handlers[165] = (uint32_t) &interupt_handler_165;
    handlers[166] = (uint32_t) &interupt_handler_166;
    handlers[167] = (uint32_t) &interupt_handler_167;
    handlers[168] = (uint32_t) &interupt_handler_168;
    handlers[169] = (uint32_t) &interupt_handler_169;
    handlers[170] = (uint32_t) &interupt_handler_170;
    handlers[171] = (uint32_t) &interupt_handler_171;
    handlers[172] = (uint32_t) &interupt_handler_172;
    handlers[173] = (uint32_t) &interupt_handler_173;
    handlers[174] = (uint32_t) &interupt_handler_174;
    handlers[175] = (uint32_t) &interupt_handler_175;
    handlers[176] = (uint32_t) &interupt_handler_176;
    handlers[177] = (uint32_t) &interupt_handler_177;
    handlers[178] = (uint32_t) &interupt_handler_178;
    handlers[179] = (uint32_t) &interupt_handler_179;
    handlers[180] = (uint32_t) &interupt_handler_180;
    handlers[181] = (uint32_t) &interupt_handler_181;
    handlers[182] = (uint32_t) &interupt_handler_182;
    handlers[183] = (uint32_t) &interupt_handler_183;
    handlers[184] = (uint32_t) &interupt_handler_184;
    handlers[185] = (uint32_t) &interupt_handler_185;
    handlers[186] = (uint32_t) &interupt_handler_186;
    handlers[187] = (uint32_t) &interupt_handler_187;
    handlers[188] = (uint32_t) &interupt_handler_188;
    handlers[189] = (uint32_t) &interupt_handler_189;
    handlers[190] = (uint32_t) &interupt_handler_190;
    handlers[191] = (uint32_t) &interupt_handler_191;
    handlers[192] = (uint32_t) &interupt_handler_192;
    handlers[193] = (uint32_t) &interupt_handler_193;
    handlers[194] = (uint32_t) &interupt_handler_194;
    handlers[195] = (uint32_t) &interupt_handler_195;
    handlers[196] = (uint32_t) &interupt_handler_196;
    handlers[197] = (uint32_t) &interupt_handler_197;
    handlers[198] = (uint32_t) &interupt_handler_198;
    handlers[199] = (uint32_t) &interupt_handler_199;
    handlers[200] = (uint32_t) &interupt_handler_200;
    handlers[201] = (uint32_t) &interupt_handler_201;
    handlers[202] = (uint32_t) &interupt_handler_202;
    handlers[203] = (uint32_t) &interupt_handler_203;
    handlers[204] = (uint32_t) &interupt_handler_204;
    handlers[205] = (uint32_t) &interupt_handler_205;
    handlers[206] = (uint32_t) &interupt_handler_206;
    handlers[207] = (uint32_t) &interupt_handler_207;
    handlers[208] = (uint32_t) &interupt_handler_208;
    handlers[209] = (uint32_t) &interupt_handler_209;
    handlers[210] = (uint32_t) &interupt_handler_210;
    handlers[211] = (uint32_t) &interupt_handler_211;
    handlers[212] = (uint32_t) &interupt_handler_212;
    handlers[213] = (uint32_t) &interupt_handler_213;
    handlers[214] = (uint32_t) &interupt_handler_214;
    handlers[215] = (uint32_t) &interupt_handler_215;
    handlers[216] = (uint32_t) &interupt_handler_216;
    handlers[217] = (uint32_t) &interupt_handler_217;
    handlers[218] = (uint32_t) &interupt_handler_218;
    handlers[219] = (uint32_t) &interupt_handler_219;
    handlers[220] = (uint32_t) &interupt_handler_220;
    handlers[221] = (uint32_t) &interupt_handler_221;
    handlers[222] = (uint32_t) &interupt_handler_222;
    handlers[223] = (uint32_t) &interupt_handler_223;
    handlers[224] = (uint32_t) &interupt_handler_224;
    handlers[225] = (uint32_t) &interupt_handler_225;
    handlers[226] = (uint32_t) &interupt_handler_226;
    handlers[227] = (uint32_t) &interupt_handler_227;
    handlers[228] = (uint32_t) &interupt_handler_228;
    handlers[229] = (uint32_t) &interupt_handler_229;
    handlers[230] = (uint32_t) &interupt_handler_230;
    handlers[231] = (uint32_t) &interupt_handler_231;
    handlers[232] = (uint32_t) &interupt_handler_232;
    handlers[233] = (uint32_t) &interupt_handler_233;
    handlers[234] = (uint32_t) &interupt_handler_234;
    handlers[235] = (uint32_t) &interupt_handler_235;
    handlers[236] = (uint32_t) &interupt_handler_236;
    handlers[237] = (uint32_t) &interupt_handler_237;
    handlers[238] = (uint32_t) &interupt_handler_238;
    handlers[239] = (uint32_t) &interupt_handler_239;
    handlers[240] = (uint32_t) &interupt_handler_240;
    handlers[241] = (uint32_t) &interupt_handler_241;
    handlers[242] = (uint32_t) &interupt_handler_242;
    handlers[243] = (uint32_t) &interupt_handler_243;
    handlers[244] = (uint32_t) &interupt_handler_244;
    handlers[245] = (uint32_t) &interupt_handler_245;
    handlers[246] = (uint32_t) &interupt_handler_246;
    handlers[247] = (uint32_t) &interupt_handler_247;
    handlers[248] = (uint32_t) &interupt_handler_248;
    handlers[249] = (uint32_t) &interupt_handler_249;
    handlers[250] = (uint32_t) &interupt_handler_250;
    handlers[251] = (uint32_t) &interupt_handler_251;
    handlers[252] = (uint32_t) &interupt_handler_252;
    handlers[253] = (uint32_t) &interupt_handler_253;
    handlers[254] = (uint32_t) &interupt_handler_254;
    handlers[255] = (uint32_t) &interupt_handler_255;
}
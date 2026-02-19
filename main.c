/******************************************************************************
Prac 2 - AVR ASM OpCode Decoder
*******************************************************************************/

#include <stdio.h>
#include <inttypes.h>

const uint8_t flash_mem[] = { 
    0x00, 0x24, 0xA0, 0xE0, 0xB2, 0xE0, 0x0D, 0x91, 0x00, 0x30, 0xE9, 0xF7, 0x11, 0x97, 0xC0, 0xE0, 0xC4, 
    0xD2, 0xE0, 0x09, 0x91, 0x1E, 0x91, 0x01, 0x17, 0x51, 0xF4, 0x0A, 0x2F, 0x0A, 0x95, 0x1C, 0x2F, 0x65, 
    0x01, 0x17, 0xB9, 0xF7, 0x0B, 0x2F, 0x1D, 0x2F, 0x01, 0x17, 0x99, 0xF7, 0x03, 0x94, 0x00, 0x00
};

const uint16_t inst16_table[] = {
    {0x0},  // NOP
    {0x9},  // CLR
    {0xE},  // LDI
    {0x48}, // LD
    {0x3},  // CPI
    {0x3D}, // BRNE
    {0x97}, // SBIW
    {0xD},  // RCALL
    {0x2},  // SBC
    {0x7},  // ADC
    {0x1},  // MOVW
    {0x5},  // SUBI
    {0x6},   // ORI
    {0x5},  // CP
    {0xB},  // MOV
    {0x4A}  // INC
};

    // 1)
    // 0010 0100 0000 0000
    // 0x2400 -> CLR, R0

    // 2)
    // 1110 0000 1010 0000
    // 0xE0A0 -> LDI RD26, 0
    
    // 3)
    // 1110 0000 1011 0010
    // 0xE0B2 -> LDI RD27, 2
    
    // 4)
    // 1001 0001 0000 1101
    // 0x910D -> LD R16, X+
    
    // 5)
    // 0011 0000 0000 0000
    // 0x3000 -> CPI R16, 0

    // 6)
    // 1111 01 11 1110 1001
    // 0xF7E9 -> BRNE 0xFFFD = -2   

    // 7)
    // 1001 0111 0001 0001
    // 0x9711 -> SBIW R26, 1

    // 8)
    // 1110 0000 1100 0000
    // 0xE0C0 -> LDI R28, 0
    
    // 9)
    // 1101 0010 1100 0100
    // D2C4 -> RCALL 2C5
    
    // 10)
    // 0000 10 01 1110 0000
    // 0x09E0 -> SBC R30, R0
    
    // 11)
    // 0001 1110 1001 0001
    // 0x1E91 -> ADC R9, R17
    
    // 12)
    // 0000 0001 1001 0001
    // 0000 0001 dddd rrrr
    // 0x0191 -> MOVW R19:R18, R3:R2
    
    // 13)
    // 0101 0001 0001 0111
    // 0x5117 -> SUBI R17, 0x17
    
    // 14)
    // 0000 1010 1111 0100
    // 0x0AF4 -> SBC R15, R20
    
    // 15)
    // 0000 1010 0010 1111
    // 0x0A2F -> SBC R2, R31
    
    // 16)
    // 0001 1100 1001 0101 
    // Ox1C95 -> ADC R9, R5
    
    // 17)
    // 0110 0101 0010 1111
    // 0x652F -> ORI R18, 0x5F
    
    // 18)
    // 0001 0111 0000 0001
    // 0x1701 -> CP R16, R17
    
    // 19)
    // 1111 0111 1011 1001    
    // 0xF7B9 -> BRNE -8
    
    // 20)
    // 0010 1111 0000 1011
    // 0x2F0B -> MOV R16, R27
    
    // 21)
    // 0010 1111 0001 1101
    // 0x2F1D -> MOV R17, R29
    
    //22)
    // 0001 0111 0000 0001
    // 0x1701 -> CP R16, R17
    
    // 23)
    // 1111 0111 1001 1001
    // 111 0011
    // 000 1101
    // 0xF799 -> BRNE -12
    
    // 24)
    // 1001 0100 0000 0011
    // 0x9403 -> INC R0

    // 25)
    // 0000 0000 0000 0000
    // 0x0000 -> NOP

enum {
    e_NOP,
    e_CLR,
    e_LDI,
    e_LD,
    e_CPI,
    e_BRNE,
    e_SBIW,
    e_RCALL,
    e_SBC,
    e_ADC,
    e_MOVW,
    e_SUBI,
    e_ORI,
    e_CP,
    e_MOV,
    e_INC
};

// Op Code struct
typedef union {
    uint16_t op16; // e.g.: watchdog, nop
    struct{
        uint16_t op4:4;
        uint16_t d5:5;
        uint16_t op7:7;
    }type1; // e.g: LSR, INC, LD
    struct{
        uint16_t r4:4;
        uint16_t d5:5;
        uint16_t r1:1;
        uint16_t op6:6;
    }type2; // e.g.: MOV,MUL,ADC,ADD,AND, SBC, CP
    struct {
        uint16_t d10:10;
        uint16_t op6:6;
    }type3; // CLR
    struct {
        uint16_t kl4:4;
        uint16_t d4:4;
        uint16_t kh4:4;
        uint16_t op4:4;
    }type4; // LDI, CPI, SUBI, ORI
    struct {
        uint16_t mode3:3;
        uint16_t k7:7;
        uint16_t op6:6;
    }type5; // BRNE
    struct {
        uint16_t kl4:4;
        uint16_t d2:2;
        uint16_t kh2:2;
        uint16_t op8:8;
    }type6; // SBIW
    struct {
        uint16_t k12:12;
        uint16_t op4:4;
    }type7; // RCALL
    struct {
        uint16_t r4:4;
        uint16_t d4:4;
        uint16_t op8:8;
    }type8; // MOVw
} Op_Code_t;


int main(void) {
    Op_Code_t *instruction;
    printf("- Practica 2: AVR OpCode -\n");
    // Decode the instructions by cycling through the array
    for (uint8_t idx = 0; idx < sizeof(flash_mem); idx+=2) {
        instruction = (Op_Code_t*) &flash_mem[idx];
        if (instruction->op16 == inst16_table[e_NOP]) {
            printf("NOP\n");

        } else if (instruction->type3.op6 == inst16_table[e_CLR]) {
            uint8_t rd = instruction->type3.d10;
            printf("CLR R%d\n", rd);

        } else if(instruction->type4.op4 == inst16_table[e_LDI]) {
            uint8_t k = (instruction->type4.kh4 << 4) | instruction->type4.kl4;
            uint8_t rd = instruction->type4.d4 + 16;
            printf("LDI R%d, 0x%X\n", rd, k);

        } else if(instruction->type1.op7 == inst16_table[e_LD]) {
            uint8_t rd = instruction->type1.d5;
            uint8_t op = instruction->type1.op4;
            if(op == 0xC) {
                printf("LD R%d, X\n", rd);
            } else if(op == 0xD) {
                printf("LD R%d, X+\n", rd);
            } else printf("LD R%d, -X\n", rd);

        } else if(instruction->type4.op4 == inst16_table[e_CPI]) {
            uint8_t rd = instruction->type4.d4 + 16;
            uint8_t k = (instruction->type4.kh4 << 4) | instruction->type4.kl4;
            printf("CPI R%d, %X\n", rd, k);

        } else if (instruction->type5.op6 == inst16_table[e_BRNE]) {
            int8_t k = 0; 
            if((instruction->type5.k7) & 0x40) {
                k = (int8_t)(instruction->type5.k7 | 0x80);
            } else {
                k = (int8_t)(instruction->type5.k7);
            }
            printf("BRNE %+d\n", k+1); 

        } else if (instruction->type6.op8 == inst16_table[e_SBIW]){
            uint8_t rd = instruction->type6.d2 * 2 + 24;
            uint8_t k = (instruction->type6.kh2 << 4) | instruction->type6.kl4;
            printf("SBIW R%d, %d\n", rd, k);

        } else if(instruction->type7.op4 == inst16_table[e_RCALL]) {
            int16_t k = instruction->type7.k12 + 1;
            printf("RCALL %X\n", k);

        } else if (instruction->type2.op6 == inst16_table[e_SBC]) {
            uint8_t rd = instruction->type2.d5;
            uint8_t rr = (instruction->type2.r1 << 4) | instruction->type2.r4;

            printf("SBC R%d, R%d\n", rd, rr);
        } else if (instruction->type2.op6 == inst16_table[e_ADC]){
            uint8_t rd = instruction->type2.d5;
            uint8_t rr = (instruction->type2.r1 << 4) | instruction->type2.r4;

            printf("ADC R%d, R%d\n", rd, rr);
        } else if(instruction->type8.op8 == inst16_table[e_MOVW]) {
            uint8_t rd = instruction->type8.d4 * 2;
            uint8_t rr = instruction->type8.r4 * 2;

            printf("MOVW R%d:R%d, R%d:R%d\n", rd + 1, rd, rr + 1, rr );
        } else if(instruction->type4.op4 == inst16_table[e_SUBI]) {
            uint8_t rd = instruction->type4.d4 + 16;
            uint8_t k = (instruction->type4.kh4 << 4) | instruction->type4.kl4;
            printf("SUBI R%d, 0x%X\n", rd, k);

        } else if(instruction->type4.op4 == inst16_table[e_ORI]) {
            uint8_t rd = instruction->type4.d4 + 16;
            uint8_t k = (instruction->type4.kh4 << 4) | instruction->type4.kl4;
            printf("ORI R%d, 0x%X\n", rd, k);

        } else if (instruction->type2.op6 == inst16_table[e_CP]){
            uint8_t rd = instruction->type2.d5;
            uint8_t rr = (instruction->type2.r1 << 4) | instruction->type2.r4;

            printf("CP R%d, R%d\n", rd, rr);
        } else if (instruction->type2.op6 == inst16_table[e_MOV]){
            uint8_t rd = instruction->type2.d5;
            uint8_t rr = (instruction->type2.r1 << 4) | instruction->type2.r4;

            printf("MOV R%d, R%d\n", rd, rr);
        } else if (instruction->type1.op7 == inst16_table[e_INC]) {
            uint8_t rd = instruction->type1.d5;

            printf("INC R%d\n", rd);
        } else {
            printf("unknown\n");
        }
    }
    return 0;
}
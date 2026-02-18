/******************************************************************************
Prac 2 - AVR ASM OpCode Decoder
*******************************************************************************/

#include <stdio.h>
#include <inttypes.h>

const uint8_t flash_mem[] ={ 
    0x00, 0x24, 0xA0, 0xE0, 0xB2, 0xE0, 0x0D, 0x91, 0x00, 0x30, 0xE9, 0xF7, 0x11, 0x97, 0xC0, 0xE0, 0xC4, 
    0xD2, 0xE0, 0x09, 0x91, 0x1E, 0x91, 0x01, 0x17, 0x51, 0xF4, 0x0A, 0x2F, 0x0A, 0x95, 0x1C, 0x2F, 0x65, 
    0x01, 0x17, 0xB9, 0xF7, 0x0B, 0x2F, 0x1D, 0x2F, 0x01, 0x17, 0x99, 0xF7, 0x03, 0x94, 0x00, 0x00 };

const uint16_t inst16_table[] = {
  {0x0},    //NOP
  {0x2400}, // CLR
  {0xE0A0}, // 

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
    // 0xF7E9 -> BRNE 0xFFFD = -3

    // 7)
    // 1001 0111 0001 0001
    // 0x9711 -> SBIW R26, 1
    
    // 8)
    // 1110 0000 1100 0000
    // 0xE0C0 -> LDI R28, 0

    // 9)
    // 1101 0010 1100 0100
    // D2C4 -> RCALL 2C4

    // 10)
    // 0000 10 01 1110 0000
    // 0x09E0 -> SBC R30, R0

    // 11)
    // 0001 1110 1001 0001
    // 0x1E91 -> ADC R9, R17

    // 12)
    // 0000 0001 1001 0001
    // 0x0191 -> MOVW R9:R8, R1:R0

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
    // 0x652F -> SBR R18, 0x5F

    // 18)
    // 0001 0111 0000 0001
    // 0x1701 -> CP R16, R17

    // 19)
    // 1111 0111 1011 1001

    // 0xF7B9 -> BRNE 0xFFF7

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
    // 0xF799 -> BRNE 0xFFF3

    // 24)
    // 1001 0100 0000 0011
    // 0x9403 -> INC R0

    // 25)
    // 0000 0000 0000 0000
    // 0x0000 -> NOP

enum{
    e_NOP,
};

// Op Code struct
typedef union {
    uint16_t op16; // e.g.: watchdog, nop
    struct{
        uint16_t op4:4;
        uint16_t d5:5;
        uint16_t op7:7;
    }type1; // e.g: LSR
    struct{
        uint16_t r4:4;
        uint16_t d5:5;
        uint16_t r1:1;
        uint16_t op6:6;
    }type2; // e.g.: MOV,MUL,ADC,ADD,AND,
    // TO-DO: Add more types as needed
} Op_Code_t;


int main(void) {
    Op_Code_t *instruction;
    printf("- Practica 2: AVR OpCode -\n");
    // Decode the instructions by cycling through the array
    for (uint8_t idx = 0; idx < sizeof(flash_mem); idx+=2) {
        instruction = (Op_Code_t*) &flash_mem[idx];
        if (instruction->op16 == inst16_table[e_NOP]) {
            printf("NOP\n");
        }
        else {
            printf("unknown\n");
        }
    }
    return 0;
}
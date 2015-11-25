#include <stdio.h>
#include <assert.h>
#include <stdbool.h>

typedef unsigned char byte ; //8 bits
typedef unsigned short word; //16 bits
typedef bool flag; //1 bit

struct state {
    //Registers ect
    byte A, X, Y;
    word PC;
    //flags
    flag C, N, V, Z;

};

#define BIT(x,n) (((x) >> (n)) & 1)
#define DATA_SIZE 0x10000

byte data[DATA_SIZE];

byte read(word addr){
    //assert(addr < DATA_SIZE);
    return data[addr];
}

void write(word addr, byte value){
    //assert(addr < DATA_SIZE);
    data[addr] = value;
}

//Executes one instruction from memory.
unsigned step(struct state *s, byte (*read)(word), void (*write)(word, byte)) {

    //TODO: make this work
    //byte x = read(0x1234);
    //write(0x1234, x);

    word t = 0; //a temporary value used in instructions
    byte M = 0; //the value being applied to the instruction
    word m = 0;
    word l = 0;
    word h = 0;
    unsigned cycles = 0; //the number of cycles used to complete the instruction

    byte opcode = read(s->PC++);

    // opcode is read as aaabbbcc with cc bits being read first to determine
    // which instruction to run accordingly
    switch((opcode) & 0x3) {
        case 0x1:
            goto CC01;
        default:
            break;
    }

    // use these switch statements when cc is 01
CC01:
    int static_cycle = 0;
    // check addressing mode
    switch((opcode >> 2) & 0x7) {
        case 2: // immediate
            m = s->PC++;
            cycles = 2;
            break;
        case 1: // zero page
            m = read(s->PC++);
            cycles = 3;
            break;
        case 5: // zero page, x
            m = (read(s->PC++) + s->X) & 0xFF;
            cycles = 4;
            break;
        case 0: // (indirect, x)
            M = read(s->PC++);
            l = read( (M + s->X) & 0xFF );
            h = read( (M + s->X + 1) & 0xFF ) <<8;
            m = h | l;
            cycles = 6;
            break;
        case 3: // absolute
            h = read(s->PC++) << 8;
            m = h | read(s->PC++);
            cycles = 4;
            break;
        case 4: // (indirect), y
            M = read(s->PC++);
            l = read(M);
            h = read((M+1) & 0xFF) << 8;
            m = ((h | l) + s->Y) & 0xFFFF;
            cycles = ((((m + s->Y) >> 8) & 0xFF ) == ((h >> 8) & 0xFF )) ? 5 : 6;
            static_cycle = 6;
            break;
        case 6: // absolute, y
            l = read(s->PC++);
            h = read(s->PC++) << 8;
            m = h | l;
            m = m + s->Y;
            cycles = ((((m + s->Y) >> 8) & 0xFF ) == ((h >> 8) & 0xFF )) ? 4 : 5;
            static_cycle = 5;
            break;
        case 7: // absolute, x
            l = read(s->PC++);
            h = read(s->PC++) << 8;
            m = h | l;
            m = m + s->X;
            cycles = ((((m + s->X) >> 8) & 0xFF ) == ((h >> 8) & 0xFF )) ? 4 : 5;
            static_cycle = 5;
            break;
    }

    M = read(m);

    // check instruction
    switch((opcode >> 5) & 0x7) {
        case 0:
            goto ORA;
        case 1:
            goto AND;
        case 2:
            goto EOR;
        case 0x3: 
            goto ADC;
        case 4:
            cycles = static_cycle;
            goto STA;
        case 5:
            goto LDA;
        case 6:
            goto CMP;
        case 7:
            goto SBC;
    }

       
ADC:
    t = s->A + M + s->C;
    s->V = (BIT(s->A,7) != BIT(t,7)) ? 1 : 0;
    s->N = (BIT(s->A,7));
    s->Z = (t==0) ? 1 : 0;
    s->C = (t>255) ? 1 : 0;
    s->A = t & 0xFF;
    return cycles;

ORA:
    A = A | M;
    s->N = (BIT(s->A,7));
    s->Z = (t==0) ? 1 : 0;
    return cycles;

AND:
    A = A & M;
    s->N = (BIT(s->A,7));
    s->Z = (t==0) ? 1 : 0;
    return cycles;

EOR:
    A = A & M;
    s->N = (BIT(s->A,7));
    s->Z = (t==0) ? 1 : 0;
    return cycles;

STA:
    write(m, A);
    return cycles;

LDA:
    return cycles;

CMP:
    return cycles;

SBC:

    return cycles;

CC10:

CC00:
 

}

void print_state (struct state *s) {

}

int main(int argc, char *argv[]) {

    struct state s = {};

    if (argc < 2 || argc > 2) {
        //Add 1 to A
        data[0] = 0x69;
        data[1] = 0x03;
        data[2] = 0x02;
        data[3] = 0x0F;
    } else {
        for (int i = 0; i < (sizeof(argv[1]) / sizeof(char)); i++) {
            data[i] = argv[1][i];
        }
    }

    printf("%u\n", s.A);
    printf("%x\n", s.PC);
    step(&s, read, write);
    printf("%u\n", s.A);
    printf("%x\n", s.PC);
    return 0;
}

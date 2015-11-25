#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>

typedef unsigned char byte;

int eatspace(FILE *f){
	int c;
	do c = fgetc(f); while(isspace(c));
	return c;
}

struct opentry {char *op; byte code;} ops[] = {
	{"ORA", (0u<<5) + 1},
	{"AND", (1u<<5) + 1},
	{"EOR", (2u<<5) + 1},
	{"ADC", (3u<<5) + 1},
	{"STA", (4u<<5) + 1},
	{"LDA", (5u<<5) + 1},
	{"CMP", (6u<<5) + 1},
	{"SBC", (7u<<5) + 1},

	{"ASL", (0u<<5) + 2},
	{"ROL", (1u<<5) + 2},
	{"LSR", (2u<<5) + 2},
	{"ROR", (3u<<5) + 2},
	{"STX", (4u<<5) + 2},
	{"LDX", (5u<<5) + 2},
	{"DEC", (6u<<5) + 2},
	{"INC", (7u<<5) + 2},

	{"BIT", (1u<<5)},
	{"JMP", (2u<<5)},
	{"JMA", (3u<<5)},
	{"STY", (4u<<5)},
	{"LDY", (5u<<5)},
	{"CPY", (6u<<5)},
	{"CPX", (7u<<5)},

	{"BPL", 0x10},
	{"BMI", 0x20},
	{"BVC", 0x50},
	{"BVS", 0x70},
	{"BCC", 0x90},
	{"BCS", 0xB0},
	{"BNE", 0xE0},
	{"BEQ", 0xF0},

	{"BRK", 0x00},
	{"JSA", 0x20},
	{"RTI", 0x40},
	{"RTS", 0x60},

	{"PHP", 0x08},
	{"PLP", 0x28},
	{"PHA", 0x48},
	{"PLA", 0x68},
	{"DEY", 0x88},
	{"TAY", 0xA8},
	{"INY", 0xC8},
	{"INX", 0xE8},
	{"CLC", 0x18},
	{"SEC", 0x38},
	{"CLI", 0x58},
	{"SEI", 0x78},
	{"TYA", 0x98},
	{"CLV", 0xB8},
	{"CLD", 0xD8},
	{"SED", 0xF8},
	{"TXA", 0x8A},
	{"TXS", 0x9A},
	{"TAX", 0xAA},
	{"TSX", 0xBA},
	{"DEX", 0xCA},
	{"NOP", 0xEA},
};


byte read_op(FILE *f){
	char s[4];

	s[0] = (unsigned)eatspace(f);
	s[1] = fgetc(f);
	s[2] = fgetc(f);
	s[3] = 0;

	struct opentry *o = ops;
	while(o->op[0]){
		if(!strcmp(o->op, s))
			return o->code;
		o++;
	}
	
	return 0xF;
}

struct addr {
	enum addr_mode {
		IMM = 0,
		ABS,
		ABSX,
		ABSY,
		ZERO,
		ZEROX,
		ZEROY,
		IND,
		INDX,
		INDY,
	} mode;
	unsigned i;
};

void error(char *err){
	fprintf(stderr, err);
	exit(1);
}

unsigned read_hex(FILE *f){
	unsigned ret = 0;
	int c = eatspace(f);

	while(isalnum(c)){
		if(c >= 'A' && c <= 'F') c -= 'A' - 10;
		else if(c >= 'a' && c <= 'f') c -= 'a' - 10;
		else if(c >= '0' && c <= '9') c -= '0';
		else error("Unexpected non-hex alnum");

		ret = (ret << 4) + (byte)c;
		c = fgetc(f);
	}

	return ret;
}

struct addr read_addr(FILE *f){
	struct addr ret;
	int c = eatspace(f);

	if(c == '#') {
		ret.mode = IMM;
		ret.i = read_hex(f);
		return ret;
	}

	if(c == '(') {
		c = eatspace(f);
		ret.mode = IND;
	} else 	ret.mod = ZERO;

	ret.i = read_hex(f);
	if(ret.i > 0xFFFF) error("Hex value greater than 0xFFFF");
	if(ret.i > 0xFF && ret.mode == ZERO) ret.mode = ABS;

	
	c = eatspace(f);

	//I
	if(c == ')') {
		c = eatspace(f);
	}

	if(c == ',') {
		c = eatspace(f);
		if(c == 'X') 
	}

	if(c == ')') {
		c = eatspace(f);

	}

	return (struct addr){-1};
}


int main(int argc, char *argv[]){
	printf("6502 Assembler\n");

	FILE *f = stdin;

	byte opcode = read_op(f);
	printf("opcode: 0x%X\n", opcode);
	struct addr addr = read_addr(f);
	printf("addr mode: %x i: 0x%X\n", addr.mode, addr.i);

	return 0;
}

#include <vector>

#ifndef MACHINE_H_
#define MACHINE_H_

class Machine {

private:
	// index which points to current operation
	unsigned short pc;

	unsigned short opcode;

	// 16 8-bit registers named V0 - VF
	unsigned char registers [16] = {0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0};

	// one 12-bit address/index register named I
	unsigned short address_register;

	// 4096 bytes of memory
	unsigned char memory [4096];

	std::vector <unsigned short> stack;

	// number font
	unsigned char font [80] = {
	0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
	0x20, 0x60, 0x20, 0x20, 0x70, // 1
	0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
	0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
	0x90, 0x90, 0xF0, 0x10, 0x10, // 4
	0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
	0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
	0xF0, 0x10, 0x20, 0x40, 0x40, // 7
	0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
	0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
	0xF0, 0x90, 0xF0, 0x90, 0x90, // A
	0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
	0xF0, 0x80, 0x80, 0x80, 0xF0, // C
	0xE0, 0x90, 0x90, 0x90, 0xE0, // D
	0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
	0xF0, 0x80, 0xF0, 0x80, 0x80  // F
	};


	// CPU functions:
	void fetchDecodeAndExecute();

	// OPCODES: 0xABCD

	// 0x00E0
	void clearDisplay();
	// 0x00EE
	void returnFromSubroutine();
	// 0x1NNN
	void jump();
	// 0x2NNN
	void call();
	// 0x3XNN
	void skipEquals(unsigned char b, unsigned char c, unsigned char d);
	// 0x4XNN
	void skipNotEquals(unsigned char b, unsigned char c, unsigned char d);
	// 0x5XY0
	void skipEqualsXY(unsigned char b, unsigned char c);
	// 0x6XNN
	void setXNN(unsigned char b, unsigned char c, unsigned char d);
	// 0x7XNN
	void addXNN(unsigned char b, unsigned char c, unsigned char d);
	// 0x8XY0
	void setXY(unsigned char b, unsigned char c);
	// 0x8XY1
	void setOR(unsigned char b, unsigned char c);
	// 0x8XY2
	void setAND(unsigned char b, unsigned char c);
	// 0x8XY3
	void setXOR(unsigned char b, unsigned char c);
	// 0x8XY4
	void addOverflow(unsigned char b, unsigned char c);
	// 0x8XY5
	void subtractUnderflow(unsigned char b, unsigned char c);
	// 0x8XY6
	void rightShift(unsigned char b);
	// 0x8XY7
	void reverseSubtractUnderflow(unsigned char b, unsigned char c);
	// 0x8XYE
	void leftShift(unsigned char b);
	// 0x9XY0
	void skipNotEqualsXY(unsigned char b, unsigned char c);
	// 0xANNN
	void setAddress(unsigned char b, unsigned char c, unsigned char d);
	// 0xBNNN
	void jumpV0(unsigned char b, unsigned char c, unsigned char d);
	// 0xCXNN
	void setRandomAND(unsigned char b, unsigned char c, unsigned char d);
	// 0xDXYN
	void draw(unsigned char b, unsigned char c, unsigned char d);
	// 0xEX9E
	void skipKeyPressed(unsigned char b);
	// 0xEXA1
	void skipKeyNotPressed(unsigned char b);
	// 0xFX07
	void setXDelay(unsigned char b);
	// 0xFX0A
	void awaitKeyPress(unsigned char b);
	// 0xFX15
	void setDelayTimer(unsigned char b);
	// 0xFX18
	void setSoundTimer(unsigned char b);
	// 0xFX1E
	void addAddressRegister(unsigned char b);
	// 0xFX29
	void fontCharacter(unsigned char b);
	// 0xFX33
	void binaryCodedDecimal(unsigned char b);
	// 0xFX55
	void registerDump(unsigned char b);
	// 0xFX65
	void registerLoad(unsigned char b);



public:
	// execute 1 CPU cycle
	// about 700 instructions per second
	void tick();

	// constructor
	Machine();

	// INIT functions:
	void readFileIntoMemory(const char* path);

	// Key function
	void setKeysToFalse();

	// width x height = 64 x 32 pixels; value 1 = white; value 0 = blue;
	bool pixels [32] [64]; // (x,y) at pixels[y][x]

	const int PIXEL_WIDTH = 64;
	const int PIXEL_HEIGHT = 32;

	// 16 keys 0 through F pressed
	bool keys [16] = {false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false};

	// decrements at 60 Hz while not zero
	unsigned char delay_timer;
	unsigned char sound_timer;

};




#endif /* MACHINE_H_ */

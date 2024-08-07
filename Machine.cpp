#include "Machine.h"
#include <iostream>
#include <fstream>
#include <cstdlib>

// constructor
Machine::Machine() {

	pc = 0x200;
	opcode = 0;
	address_register = 0;

	// init memory
	for(int i = 0; i < 4096; i++) {
		memory[i] = 0;
	}

	delay_timer = 0;
	sound_timer = 0;

	// init display pixels
	for(int x = 0; x < 32; x++) {
		for(int y = 0; y < 64; y++) {
			pixels[x][y] = false;
		}
	}

	// store font data for numbers
	for(int i = 0; i < 80; i++) {
		memory[0x50 + i] = font[i];
	}


}

// main method for cpu executing 1 cycle
void Machine::tick() {
	fetchDecodeAndExecute();
	// std::cout << "Executing Opcode 0x" << std::hex << (int) opcode << "\n";
	return;
}

void Machine::fetchDecodeAndExecute() {
	// fetch
	unsigned char upper = memory[pc];
	unsigned char lower = memory[pc + 1];
	opcode = (upper << 8) + lower;

	// increase program counter
	pc += 2;

	// format: 0xABCD
	unsigned char a = (opcode & 0xF000) >> 12;
	unsigned char b = (opcode & 0xF00) >> 8;
	unsigned char c = (opcode & 0xF0) >> 4;
	unsigned char d = opcode & 0xF;

	// decode and execute
	switch(a) {

		case 0:
		  if(c == 0xE) {
			switch(d) {
				case 0:
					clearDisplay();
					break;
				case 0xE:
					returnFromSubroutine();
					break;
				default:
					std::cout << "Unknown Op-Code: 0x" << std::hex << opcode << "\n";
					break;
			}
		  }
		  else {std::cout << "Unknown Op-Code: 0x" << std::hex << opcode << "\n";}
			break;
		case 1:
			jump();
			break;
		case 2:
			call();
			break;
		case 3:
			skipEquals(b, c, d);
			break;
		case 4:
			skipNotEquals(b, c, d);
			break;
		case 5:
			skipEqualsXY(b, c);
			break;
		case 6:
			setXNN(b, c, d);
			break;
		case 7:
			addXNN(b, c, d);
			break;
		case 8:
			switch(d) {
				case 0:
					setXY(b, c);
					break;
				case 1:
					setOR(b, c);
					break;
				case 2:
					setAND(b, c);
					break;
				case 3:
					setXOR(b, c);
					break;
				case 4:
					addOverflow(b, c);
					break;
				case 5:
					subtractUnderflow(b, c);
					break;
				case 6:
					rightShift(b);
					break;
				case 7:
					reverseSubtractUnderflow(b, c);
					break;
				case 0xE:
					leftShift(b);
					break;

			}
			break;

		case 9:
			if(d == 0) {
			skipNotEqualsXY(b, c);
			}
			else {
			std::cout << "Unknown Op-Code: 0x" << std::hex << opcode << "\n";
			}
			break;
		case 0xA:
			setAddress(b, c, d);
			break;
		case 0xB:
			jumpV0(b, c, d);
			break;
		case 0xC:
			setRandomAND(b, c, d);
			break;
		case 0xD:
			draw(b, c, d);
			break;
		case 0xE:
			if(c == 9 && d == 0xE) {
				skipKeyPressed(b);
			}
			else if(c == 0xA && d == 1) {
				skipKeyNotPressed(b);
			}
			else {
				std::cout << "Unknown Op-Code: 0x" << std::hex << opcode << "\n";
			}
			break;
		case 0xF:
			if(c == 0 && d == 7) {
				setXDelay(b);
			}
			else if(c == 0 && d == 0xA) {
				awaitKeyPress(b);
			}
			else if(c == 1 && d == 5) {
				setDelayTimer(b);
			}
			else if(c == 1 && d == 8) {
				setSoundTimer(b);
			}
			else if(c == 1 && d == 0xE) {
				addAddressRegister(b);
			}
			else if(c == 2 && d == 9) {
				fontCharacter(b);
			}
			else if(c == 3 && d == 3) {
				binaryCodedDecimal(b);
			}
			else if(c == 5 && d == 5) {
				registerDump(b);
			}
			else if(c == 6 && d == 5) {
				registerLoad(b);
			}
			else {
				std::cout << "Unknown Op-Code: 0x" << std::hex << opcode << "\n";
			}
			break;
		default:
			std::cout << "Unknown Op-Code: 0x" << std::hex << opcode << "\n";
			break;
	}

	return;
}

// read binary file and copy it into memory array
void Machine::readFileIntoMemory(const char* path) {

	std::streampos size;
	char* temporary_memory;

	std::ifstream file (path, std::ios::in|std::ios::binary|std::ios::ate);

	  if (file.is_open()) {

	    size = file.tellg();
	    temporary_memory = new char [size];
	    file.seekg (0, std::ios::beg);
	    file.read (temporary_memory, size);
	    file.close();

	    std::cout << "File is in memory!\n";

	    // copy temp into memory and cast it
	    for(int i=0; i < size; i++) {
	    	memory[0x200 + i] = (unsigned char) temporary_memory[i];
	    	// std::cout << "0x" << std::hex << (int) memory[0x200 + i] << "\n";
	    }

	  }

	  else {
		  std::cout << "Error reading file!\n";
	  }

	return;
}

void Machine::clearDisplay() {

	for(int x = 0; x < 32; x++) {
		for(int y = 0; y < 64; y++) {
			pixels[x][y] = false;
		}
	}
	return;
}

void Machine::returnFromSubroutine() {
	unsigned short address = stack.back();
	stack.pop_back();
	pc = address;
	return;
}

void Machine::jump() {
	unsigned short address = opcode & 0xFFF;
	pc = address;
	return;
}

void Machine::call() {
	unsigned short address = opcode & 0xFFF;
	stack.push_back(pc);
	pc = address;
	return;
}

void Machine::skipEquals(unsigned char b, unsigned char c, unsigned char d) {
	// 0x3XNN and Format: 0xABCD
	unsigned char nn = (c << 4) + d;
	if(registers[b] == nn) {
		pc += 2;
	}
	return;
}

void Machine::skipNotEquals(unsigned char b, unsigned char c, unsigned char d) {
	// 0x4XNN and Format: 0xABCD
	unsigned char nn = (c << 4) + d;
	if(registers[b] != nn) {
		pc += 2;
	}
	return;
}

void Machine::skipEqualsXY(unsigned char b, unsigned char c) {
	// 0x5XY0 and Format: 0xABCD
	if(registers[b] == registers[c]) {
		pc += 2;
	}
	return;
}

void Machine::setXNN(unsigned char b, unsigned char c, unsigned char d) {
	// 0x6XNN and Format: 0xABCD
	unsigned char nn = (c << 4) + d;
	registers[b] = nn;
	return;
}

void Machine::addXNN(unsigned char b, unsigned char c, unsigned char d) {
	// 0x7XNN and Format: 0xABCD
	unsigned char nn = (c << 4) + d;
	registers[b] += nn;
	return;
}

void Machine::setXY(unsigned char b, unsigned char c) {
	// 0x8XY0 and Format: 0xABCD
	registers[b] = registers[c];
	return;
}

void Machine::setOR(unsigned char b, unsigned char c) {
	// 0x8XY1 and Format: 0xABCD
	registers[b] = registers[b] | registers[c];
	return;
}

void Machine::setAND(unsigned char b, unsigned char c) {
	// 0x8XY2 and Format: 0xABCD
	registers[b] = registers[b] & registers[c];
	return;
}

void Machine::setXOR(unsigned char b, unsigned char c) {
	// 0x8XY3 and Format: 0xABCD
	registers[b] = registers[b] ^ registers[c];
	return;
}

void Machine::addOverflow(unsigned char b, unsigned char c) {
	// 0x8XY4 and Format: 0xABCD
	unsigned int x = registers[b];
	unsigned int y = registers[c];
	// addition
	registers[b] += registers[c];
	// set overflow flag
	x += y;
	if(x > 255) {
		registers[15] = 1;
	}
	else {
		registers[15] = 0;
	}
	return;
}

void Machine::subtractUnderflow(unsigned char b, unsigned char c) {
	// 0x8XY5 and Format: 0xABCD
	// set no-underflow flag
	bool flag;

	if(registers[b] >= registers[c]) {
		flag = true;
	}
	else {
		flag = false;
	}

	registers[b] -= registers[c];

	if(flag) {
		registers[15] = 1;
	}
	else {
		registers[15] = 0;
	}
	return;
}

void Machine::rightShift(unsigned char b) {
	// 0x8XY6 and Format: 0xABCD
	// store least significant bit prior to shift
	unsigned char flag = registers[b] & 1;
	registers[b] = registers[b] >> 1;
	registers[15] = flag;
	return;
}

void Machine::reverseSubtractUnderflow(unsigned char b, unsigned char c) {
	// 0x8XY7 and Format: 0xABCD
	// set no-underflow flag
	bool flag;
	if(registers[c] >= registers[b]) {
		flag = true;
	}
	else {
		flag = false;
	}

	registers[b] = registers[c] - registers[b];

	if(flag) {
		registers[15] = 1;
	}
	else {
		registers[15] = 0;
	}
	return;
}

void Machine::leftShift(unsigned char b) {
	// 0x8XYE and Format: 0xABCD
	// store most significant bit prior to shift
	unsigned char flag = (registers[b] & 0b10000000) >> 7;
	registers[b] = registers[b] << 1;
	registers[15] = flag;
	return;
}

void Machine::skipNotEqualsXY(unsigned char b, unsigned char c) {
	// 0x9XY0 and Format: 0xABCD
	if(registers[b] != registers[c]) {
		pc += 2;
	}
	return;
}

void Machine::setAddress(unsigned char b, unsigned char c, unsigned char d) {
	// 0xANNN and Format: 0xABCD
	unsigned char nn = (c << 4) + d;
	unsigned short address = (b << 8) + nn;
	address_register = address;
	return;
}

void Machine::jumpV0(unsigned char b, unsigned char c, unsigned char d) {
	// 0xBNNN and Format: 0xABCD
	unsigned char nn = (c << 4) + d;
	unsigned short address = (b << 8) + nn;
	pc = address + registers[0];
	return;
}

void Machine::setRandomAND(unsigned char b, unsigned char c, unsigned char d) {
	// 0xCXNN and Format: 0xABCD
	unsigned char nn = (c << 4) + d;
	// this is not a uniform distribution on [0,255] but it is the simplest way
	unsigned char random = (unsigned char) (rand() % 256);
	registers[b] = random & nn;
	return;
}

void Machine::skipKeyPressed(unsigned char b) {
	// 0xEX9E and Format: 0xABCD
	if(keys[registers[b]] == true) {
		pc += 2;
	}
	return;
}

void Machine::skipKeyNotPressed(unsigned char b) {
	// 0xEXA1 and Format: 0xABCD
	if(keys[registers[b]] == false) {
		pc += 2;
	}
	return;
}

void Machine::setXDelay(unsigned char b) {
	// 0xFX07 and Format: 0xABCD
	registers[b] = delay_timer;
	return;
}

void Machine::awaitKeyPress(unsigned char b) {
	// 0xFX0A and Format: 0xABCD
	char input;
	std::cout << "Waiting for key press and input ... \n";
	// halts execution: freeglut library event cannot be triggered -> manual console input!
	std::cin >> input;

	switch(input) {
	case '1': keys[0] = true; registers[b] = 0; break;
	case '2': keys[1] = true; registers[b] = 1; break;
	case '3': keys[2] = true; registers[b] = 2; break;
	case '4': keys[3] = true; registers[b] = 3; break;
	case 'q': keys[4] = true; registers[b] = 4; break;
	case 'w': keys[5] = true; registers[b] = 5; break;
	case 'e': keys[6] = true; registers[b] = 6; break;
	case 'r': keys[7] = true; registers[b] = 7; break;
	case 'a': keys[8] = true; registers[b] = 8; break;
	case 's': keys[9] = true; registers[b] = 9; break;
	case 'd': keys[10] = true; registers[b] = 10; break;
	case 'f': keys[11] = true; registers[b] = 11; break;
	case 'y': keys[12] = true; registers[b] = 12; break;
	case 'x': keys[13] = true; registers[b] = 13; break;
	case 'c': keys[14] = true; registers[b] = 14; break;
	case 'v': keys[15] = true; registers[b] = 15; break;
	default: std::cout << "Wrong key! Pressed 1 for you!\n"; keys[0] = true; registers[b] = 0; break;
	}
	return;
}

void Machine::setDelayTimer(unsigned char b) {
	// 0xFX15 and Format: 0xABCD
	delay_timer = registers[b];
	return;
}

void Machine::setSoundTimer(unsigned char b) {
	// 0xFX18 and Format: 0xABCD
	sound_timer = registers[b];
	return;
}

void Machine::addAddressRegister(unsigned char b) {
	address_register += registers[b];
	return;
}

void Machine::fontCharacter(unsigned char b) {
	unsigned char number = registers[b];
	address_register = 0x50 + 5 * number;
	return;
}

void Machine::binaryCodedDecimal(unsigned char b) {
	unsigned char number = registers[b];
	unsigned char l = number % 10;
	unsigned char m = (number / 10) % 10;
	unsigned char h = (number / 100) % 10;
	memory[address_register] = h;
	memory[address_register + 1] = m;
	memory[address_register + 2] = l;
	return;
}

void Machine::registerDump(unsigned char b) {
	// 0xFX55 and Format: 0xABCD
	for(unsigned char i = 0; i <= b; i++) {
		memory[address_register + i] = registers[i];
	}
	return;
}

void Machine::registerLoad(unsigned char b) {
	for(unsigned char i = 0; i <= b; i++) {
		registers[i] = memory[address_register + i];
	}
	return;
}

void Machine::draw(unsigned char b, unsigned char c, unsigned char d) {
	// 0xDXYN
	unsigned short x = registers[b] % 64;
	unsigned short y = registers[c] % 32;
	unsigned char n = d;
	unsigned char sprite;
	unsigned char bits [8];
	// set collision flag to 0
	registers[15] = 0;

	// draw 8 x n sprite
	for(int row = 0; row < n; row++) {

		sprite = memory[address_register + row];
		bits[0] = (sprite & 0b10000000) >> 7;
		bits[1] = (sprite & 0b1000000) >> 6;
		bits[2] = (sprite & 0b100000) >> 5;
		bits[3] = (sprite & 0b10000) >> 4;
		bits[4] = (sprite & 0b1000) >> 3;
		bits[5] = (sprite & 0b100) >> 2;
		bits[6] = (sprite & 0b10) >> 1;
		bits[7] = (sprite & 1);

		for(int i = 0; i < 8; i++) {
			// draw pixel?
			if(bits[i] == 1) {
			  // check array bounds
			  if(y + row < 32 && (x + i) < 64)	{
				// XOR pixels on screen
				if(pixels[y + row][x + i] == true) {
					pixels[y + row][x + i] = false;
					// set collision flag to 1
					registers[15] = 1;
				}
				else {
					pixels[y + row][x + i] = true;
				}
			  }
			}
	    }
     }

	return;
}

void Machine::setKeysToFalse() {
	for(int i = 0; i < 16; i++) {
		keys[i] = false;
	}
	return;
}



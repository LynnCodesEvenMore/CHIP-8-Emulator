#include "Machine.h"
#include <GL/freeglut.h>
#include <stdio.h>


// Global Variables
const int WIDTH_WINDOW = 640;
const int HEIGHT_WINDOW = 320;
const int X_POS_WINDOW = 100;
const int Y_POS_WINDOW = 100;
const char* NAME_WINDOW = "Chip-8 Emulator";
const int FPS = 30;
const int CPU_INSTRUCTIONS_PER_SECOND = 700;

GLubyte* PixelBuffer = new GLubyte[WIDTH_WINDOW * HEIGHT_WINDOW * 3];
Machine cpu;



void display()
{
    glClear(GL_COLOR_BUFFER_BIT);
    glDrawPixels(WIDTH_WINDOW, HEIGHT_WINDOW, GL_RGB, GL_UNSIGNED_BYTE, PixelBuffer);
    glutSwapBuffers();
}


// (x,y) = (0,0) is at bottom left corner!
void makePixel(int x, int y, int r, int g, int b, GLubyte* pixels, int width, int height)
{
     if (0 <= x && x < width && 0 <= y && y < height) {
        int position = (x + y * width) * 3;
        pixels[position] = r;
        pixels[position + 1] = g;
        pixels[position + 2] = b;
     }

    return;
}

void updatePixelBuffer() {
	// since (x,y) = (0,0) is at bottom left corner, recalibrate (0,0) to top left corner!
	int real_x;
	int real_y;

	for(int y = 0; y < cpu.PIXEL_WIDTH; y++) {
		for(int x = 0; x < cpu.PIXEL_HEIGHT; x++) {
			if(cpu.pixels[x][y] == false) {
				// BLUE pixel background
				for(int w = 0; w < 10; w++) {
					for(int z = 0; z < 10; z++) {
						// 10 x 10 display pixels for 1 machine pixel
						real_y = HEIGHT_WINDOW - ((x*10) + w);
						real_x = (y*10) + z;
						makePixel(real_x, real_y, 0, 0, 255, PixelBuffer, WIDTH_WINDOW, HEIGHT_WINDOW);
					}
				}
			}
			else {
				// WHITE pixel foreground
				for(int w = 0; w < 10; w++) {
					for(int z = 0; z < 10; z++) {
						// 10 x 10 display pixels for 1 machine pixel
						real_y = HEIGHT_WINDOW - ((x*10) + w);
						real_x = (y*10) + z;
						makePixel(real_x, real_y, 255, 255, 255, PixelBuffer, WIDTH_WINDOW, HEIGHT_WINDOW);
					}
				}
			}
		}
	}

	return;
}

void normal_keyboard_keys_down(unsigned char key, int x, int y) {
	// ASCII codes for normal keys
	switch(key)
	{
	case 49: cpu.keys[0] = true; break;
	case 50: cpu.keys[1] = true; break;
	case 51: cpu.keys[2] = true; break;
	case 52: cpu.keys[3] = true; break;
	case 113: cpu.keys[4] = true; break;
	case 119: cpu.keys[5] = true; break;
	case 101: cpu.keys[6] = true; break;
	case 114: cpu.keys[7] = true; break;
	case 97: cpu.keys[8] = true; break;
	case 115: cpu.keys[9] = true; break;
	case 100: cpu.keys[10] = true; break;
	case 102: cpu.keys[11] = true; break;
	case 121: cpu.keys[12] = true; break;
	case 120: cpu.keys[13] = true; break;
	case 99: cpu.keys[14] = true; break;
	case 118: cpu.keys[15] = true; break;
	}
}

void normal_keyboard_keys_up(unsigned char key, int x, int y) {
	// ASCII codes for normal keys
	switch(key)
	{
	case 49: cpu.keys[0] = false; break;
	case 50: cpu.keys[1] = false; break;
	case 51: cpu.keys[2] = false; break;
	case 52: cpu.keys[3] = false; break;
	case 113: cpu.keys[4] = false; break;
	case 119: cpu.keys[5] = false; break;
	case 101: cpu.keys[6] = false; break;
	case 114: cpu.keys[7] = false; break;
	case 97: cpu.keys[8] = false; break;
	case 115: cpu.keys[9] = false; break;
	case 100: cpu.keys[10] = false; break;
	case 102: cpu.keys[11] = false; break;
	case 121: cpu.keys[12] = false; break;
	case 120: cpu.keys[13] = false; break;
	case 99: cpu.keys[14] = false; break;
	case 118: cpu.keys[15] = false; break;
	}
}

void timer(int t) {

	// Animation code
	updatePixelBuffer();

	// Update display
	glutPostRedisplay();

	// Decrement machine timers at 30 Hz instead of 60 Hz
	if(cpu.delay_timer > 0) {
		cpu.delay_timer--;
	}

	if(cpu.sound_timer > 0) {
		cpu.sound_timer--;
	}

	// Reset timer (one-shot timer!)
	glutTimerFunc( 1000/FPS, timer, 0);

}

void cpu_timer(int t) {

	// execute 1 cpu cycle
	cpu.tick();

	// Reset timer (one-shot timer!)
	glutTimerFunc(1000/CPU_INSTRUCTIONS_PER_SECOND, cpu_timer, 0);
}

void initializeWindow(int argc, char* argv[], int xPos, int yPos, int width, int height, const char* name) {
	  // GLUT requires main context
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);

    glutInitWindowSize(width, height);
    glutInitWindowPosition(xPos, yPos);

    // key repeat off for key up and key down events to work properly
    glutSetKeyRepeat(GLUT_KEY_REPEAT_OFF);

    int MainWindow = glutCreateWindow(name);
    glClearColor(0.0, 0.0, 0.0, 0);
    return;
}

void registerCallbackHandlers() {
    glutDisplayFunc(display);
    glutKeyboardFunc(normal_keyboard_keys_down);
    glutKeyboardUpFunc(normal_keyboard_keys_up);
	  glutTimerFunc( 1000/FPS , timer, 0);
	  glutTimerFunc(1000/CPU_INSTRUCTIONS_PER_SECOND, cpu_timer, 0);
    return;
}

int main(int argc, char *argv[])
{

	  // Initialize Window
    initializeWindow(argc, argv, X_POS_WINDOW, Y_POS_WINDOW, WIDTH_WINDOW, HEIGHT_WINDOW, NAME_WINDOW);

    // register callback handlers for display, keyboard and timer for drawing frames
    registerCallbackHandlers();

    // read ROM into memory
    cpu.readFileIntoMemory("C:/quirks_test.ch8");

    // Mainloop
    for(;;) {

    // cpu_timer will be called periodically for executing 1 cpu cycle

    // timer will be called periodically for drawing frames


    // make changes visible
    glutMainLoopEvent();

    }


    return 0;
}





#ifndef VIEW_H
#define VIEW_H

#ifdef __APPLE__
#include <GLUT/glut.h>
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#else
#include <GL/glut.h>
#endif

#define FPS24 41.67
#define FPS30 33.33
#define FPS60 16.67
#define FPS90 11.11

class View{
public:
	float FR = FPS60;
	void initializeWindow();
	void assignCallBacks(void reshape(int, int),
							void display(),
							void keyboard(unsigned char, int, int),
							void mouse(int, int, int, int),
							void motion(int, int),
							void timer(int));
	void kickOff();

	void updateDisplay();
	void refresh();
	void timerReset(void timer(int));
	void winRestore();
	void winFullscreen();
};

#endif

#include <iostream>

#include "View.h"

using namespace std;

void View::initializeWindow(){
	int i=0;
	glutInit(&i, NULL);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_MULTISAMPLE | GLUT_DEPTH);
	
	glutCreateWindow("Hazel");
	glutFullScreen();
	glutSetCursor(GLUT_CURSOR_NONE);
}

void View::assignCallBacks(void reshape(int, int),
							void display(),
							void keyboard(unsigned char, int, int),
							void mouse(int, int, int, int),
							void motion(int, int),
							void timer(int)){
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutMouseFunc(mouse);
	glutPassiveMotionFunc(motion);
	glutTimerFunc(FR, timer, 0);
}

void View::kickOff(){
	glutMainLoop();
}

void View::refresh(){
	glutPostRedisplay();
}

void View::updateDisplay(){
	glutSwapBuffers();
}

void View::timerReset(void timer(int)){
	glutTimerFunc(FR, timer, 0);
}

void View::winRestore(){
	glutReshapeWindow(500,500);
	glutPositionWindow(900,300);
}

void View::winFullscreen(){
	glutFullScreen();
}
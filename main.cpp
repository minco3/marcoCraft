#include <iostream>
#include <GL/glew.h>
#include <GL/glut.h>
#include "includes/utils/a.h"

void processKey(unsigned char c, int x, int y) {
    std::cout << c << '\n';
    if (c == 'x') {
        exit(0);
    }
}
void processKeyUp(unsigned char c, int x, int y) {
    std::cout << c << '\n';
}

void display(void) {
    glClear(GL_COLOR_BUFFER_BIT);

    glBegin(GL_POLYGON);
        glColor3f(1,0,0); glVertex3f(-0.6,-0.75, 0.5);
        glColor3f(0,1,0); glVertex3f(0.6,-0.75, 0);
        glColor3f(0,0,1); glVertex3f(0,0.75, 0);
    glEnd();

    glFlush();
}

int main(int argc, char** argv) {

    glutInit(&argc, argv);
    glutCreateWindow("GLEW TEST");
    glutKeyboardFunc(processKey);
    glutKeyboardUpFunc(processKeyUp);
    glutDisplayFunc(display);
    glutMainLoop();
}

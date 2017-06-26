//
//  MacLinux.h
//
//  Header file to include the correct header files depending on
//  operating system.
//
//  Created by  on 2/1/17.
//
//

#ifndef ____MacLinux__
#define ____MacLinux__

#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>
#include <stdlib.h>
#include <stdio.h>
#else
#include <stdlib.h>
#include <stdio.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
using namespace std;

#endif

#endif /* defined(____MacLinux__) */




#define Z_AXIS 0,0,1
#define X_AXIS  1,0,0
#define Y_AXIS  0,1,0

#define WHITE   1,1,1
#define RED     1,0,0
#define GREEN   0,1,0
#define BLUE    0,0,1
#define BLACK   0,0,0
#define YELLOW  1,1,0
#define PURPLE  1,0,1
#define GRAY    .4,.4,.4
#define OTHER_BLUE 46.0/255, 183.0/255, 232.0/255
#define COORD  0,0,0, 0, 50, 0, 50, 0, 0
#define true 1
#define false 0
#define POS 100,100,100

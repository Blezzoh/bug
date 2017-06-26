/*Blaise Iradukunda
  Final Project
  - The display function is the same as the animation functions
  - The user instructions are printed out in the terminal
  - In order to activate the UI press the middle button
  - Extra: light, animation, texture mapping for the lower torso
  */


// Cylinder for base, scaled cube for arms 
// Shows use of instance transformation to define parts (symbols) 
// The cylinder is a quadric object from the GLU library 
// The cube is also obtained from GLU 

#include "MacLinux.h"

/* Let's start using #defines so we can better
interpret the constants (and change them) */
#define ORIGIN  0.0,0.0,0.0
#define BASE_HEIGHT      2.0
#define BASE_RADIUS      1.0
#define LOWER_ARM_HEIGHT 5.0
#define LOWER_ARM_WIDTH  0.5
#define UPPER_ARM_HEIGHT 5.0
#define UPPER_ARM_WIDTH  0.5
// for the final project
// 1.head dimensions
#define HEAD_RADIUS 1
#define HEAD_Y_TRANSLATE 10
#define HEAD_SCALE_X 0.7
#define HEAD_SCALE_Y 1
#define HEAD_SCALE_Z .7

// 2. upper torso dimension
#define U_TORSO_RADIUS 1.5
#define U_TORSO_Y_TRANSLATE .7
#define U_TORSO_SCALE_X 0.8
#define U_TORSO_SCALE_Y 1
#define U_TORSO_SCALE_Z .8

// 3. lower torso dimension
#define L_TORSO_RADIUS 2.5
#define L_TORSO_Y_TRANSLATE 2
#define L_TORSO_SCALE_X 0.6
#define L_TORSO_SCALE_Y 1
#define L_TORSO_SCALE_Z .6

// 4. base for an arm
#define ARM_COMPONET_RADIUS .6
#define PART_II_VS_PART_I 1.5

//5. wings
#define WINGS_RADIUS 2


// 6. colors
#define GLORIOUS_SEWANEE_PURPLE 121/255.,  37/255.,  255/255
#define KHAKI 100/255.0, 66/255.0, 38/255.0

static GLfloat theta[] = {0, 0, 0, 0, 0, 0};
static GLint axis = 0;
GLUquadricObj *quadObj;
GLuint texture_head;

// boolean and angle for helping the animation
int disp_bool = 0;
float angle = 0.0f;
int wing_bool = 0;
int cnt = 0;


const char *image_name = "1.raw";
int w = 320;
int texture;

// draws the axis coordinates
void axes(void);

//draws a line
void line(int x1, int y1, int z1, int x2, int y2, int z2, float r, float g, float b);

void text(const char *text, float x, float y, float z);

// draws a glSphere 
// the boolean is there for the checking if the texture is needed
// if no texture the color rgb(r,g,b) is used
void draw_sphere(GLdouble radius, GLint slices, GLint stacks, int boolean, float r, float g, float b);

//body parts & helper functions
void head();

void upper_torso();

void lower_torso();

void arm_component();

void arm_component_2();

void arm_component_3();

void any_arm();

void lower_arm_right();

void middle_arm_right();

void upper_arm_right();

void lower_arm_left();

void middle_arm_left();

void upper_arm_left();

void antenna_right();

void antenna_left();

void wing_right();

void wing_left();

//texture used
const char *LuciaDaleFileName = "mos.bmp";


#define ESCAPE 27

float xTheta, yTheta, zTheta;

// storage for one texture  

struct Image {
    unsigned int sizeX;             // ie, width
    unsigned int sizeY;             // ie, height
    char *data;
};

// quick and dirty bitmap loader...for 24 bit bitmaps with 1 plane only.  
// See http://www.dcs.ed.ac.uk/~mxr/gfx/2d/BMP.txt for more info.
int ImageLoad(const char *filename, Image *image) {
    FILE *file;
    unsigned int size;              // size of image in bytes
    unsigned long i;                // counter
    unsigned short int planes;      // num planes image (1)
    unsigned short int bpp;         // num bits per pixel (8 each RGB so must be 24)
    char temp;                      // temporary color storage for bgr-rgb conversion
    // make sure the file is there
    if ((file = fopen(filename, "rb")) == NULL) {
//        printf("File Not Found : %s\n", filename);
        return 0;
    }
    // seek through bmp header, up to width/height
    fseek(file, 18, SEEK_CUR);

    // read width
    if ((i = fread(&image->sizeX, 4, 1, file)) != 1) {
//        printf("Error reading width from %s.\n", filename);
        return 0;
    }
//    printf("Width of %s: %i\n", filename, image->sizeX);

    // read height
    if ((i = fread(&image->sizeY, 4, 1, file)) != 1) {
//        printf("Error reading height from %s.\n", filename);
        return 0;
    }
//    printf("Height of %s: %i\n", filename, image->sizeY);

    // read planes
    if ((fread(&planes, 2, 1, file)) != 1) {
//        printf("Error reading planes from %s.\n", filename);
        return 0;
    }
    if (planes != 1) {
        printf("Planes from %s is not 1: %u\n", filename, planes);
        return 0;
    }
    // read bpp, bits per pixel
    if ((i = fread(&bpp, 2, 1, file)) != 1) {
//        printf("Error reading bpp from %s.\n", filename);
        return 0;
    }

    if (bpp == 24) {
        // calculate size assuming 24 bits (ie, 3 bytes per pixel)
        size = image->sizeX * image->sizeY * 3;
    } else {
//        printf("bits per pixel from %s is not 24 : %u\n", filename, bpp);
        return 0;
    }

    // seek past the rest of the bitmap header
    fseek(file, 24, SEEK_CUR);

    // allocate memory & read data into it
    image->data = (char *) malloc(size);
    if (image->data == NULL) {
//        printf("Error allocating memory for color-corrected image data");
        return 0;
    }

    if ((i = fread(image->data, size, 1, file)) != 1) {
//        printf("Error reading image data from %s.\n", filename);
        return 0;
    }
    // lkd - ARGHH, if you don't reverse the colors; it all goes BLUE
    for (i = 0; i < size; i += 3) {         // why why why??? reverse colors (bgr -> rgb)
        temp = image->data[i];
        image->data[i] = image->data[i + 2];
        image->data[i + 2] = temp;
    }
    // whew, done!
    return 1;
}

// load bitmaps & convert to textures
void loadTextures(const char *filename) {

    Image *image1;

    image1 = (Image *) malloc(sizeof(Image));
    if (image1 == NULL) {
        printf("Error allocating space for image");
        exit(0);
    }

    if (!ImageLoad(LuciaDaleFileName, image1))
        exit(1);

    // create texture
    glGenTextures(1, (GLuint *) &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    // scale linearly when image bigger/smaller than texture
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    // 2d texture, level of detail 0 (normal),
    //   3 components (red, green, blue), x size from image, y size from image,
    //   border 0 (normal), rgb color data, unsigned byte data, and finally the data itself.
    glTexImage2D(GL_TEXTURE_2D, 0, 3, image1->sizeX, image1->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, image1->data);
};

void line(int x1, int y1, int z1, int x2, int y2, int z2, float r, float g, float b) {
    glColor3f(r, g, b);
    glBegin(GL_LINES);
    glVertex3i(x1, y1, z1);
    glVertex3i(x2, y2, z2);
    glEnd();
}

//function to draw a triangle at some point
void text(const char *text, float x, float y, float z) {
    glRasterPos3f(x, y, z);
    for (int k = 0; text[k] != 0; k++)
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, text[k]);
}

void axes(void) {

    line(ORIGIN, 500, 0, 0, RED);
    text("X", 2, 0, 0);

    line(ORIGIN, 0, 500, 0, GREEN);
    text("Y", 0, 2, 0);

    line(ORIGIN, 0, 0, 500, OTHER_BLUE);
    text("Z", 0, 0, 2);
}

//draws a simple quadric sphere
void draw_sphere(GLdouble radius, GLint slices, GLint stacks, int boolean, GLfloat r, GLfloat g, GLfloat b) {

    GLUquadric *quad;
    quad = gluNewQuadric();
    gluQuadricNormals(quad, GLU_SMOOTH);
    if (boolean) {
        gluQuadricTexture(quad, GL_TRUE);
        glBindTexture(GL_TEXTURE_2D, texture);
    } else {
        glEnable(GL_COLOR_MATERIAL);
        glColor3f(r, g, b);
    }

    gluSphere(quad, radius, slices, stacks);
}


// body parts
// for some parts, i cancelled the transformation the basic way because push and pop were not working well for me
void head() {
    glPushMatrix();
    glScalef(HEAD_SCALE_X, HEAD_SCALE_Y, HEAD_SCALE_Z);
    glTranslatef(0, HEAD_Y_TRANSLATE, 0);
    draw_sphere(HEAD_RADIUS, 20, 20, 0, BLACK);
    glPopMatrix();
}


void upper_torso() {
    glPushMatrix();
    glScalef(U_TORSO_SCALE_X + theta[3], U_TORSO_SCALE_Y, U_TORSO_SCALE_Z);
    glTranslatef(0, HEAD_Y_TRANSLATE - U_TORSO_RADIUS - U_TORSO_Y_TRANSLATE, 0);
    glRotatef(theta[4], Z_AXIS);
    draw_sphere(U_TORSO_RADIUS, 15, 15, 0, KHAKI);
    glPopMatrix();
}

void lower_torso() {
    glPushMatrix();
    glScalef(L_TORSO_SCALE_X, L_TORSO_SCALE_Y, L_TORSO_SCALE_Z);
    glTranslatef(0, U_TORSO_RADIUS + U_TORSO_Y_TRANSLATE + L_TORSO_Y_TRANSLATE, 0);
    draw_sphere(L_TORSO_RADIUS, 15, 15, 1, .7, .7, .7);
    glPopMatrix();
}

//reusable components for the arm
void arm_component() {
    glPushMatrix();
    glScalef(2, .5, .5);
    draw_sphere(ARM_COMPONET_RADIUS, 15, 15, 0, BLACK);
    glPopMatrix();
}

void arm_component_2() {
    glPushMatrix();
    glTranslatef(ARM_COMPONET_RADIUS * 1.8, -ARM_COMPONET_RADIUS * PART_II_VS_PART_I * 2, 0);
    glRotatef(90 + theta[4], Z_AXIS);
    glScalef(2, .3, .3);
    draw_sphere(ARM_COMPONET_RADIUS * PART_II_VS_PART_I, 15, 15, 0, BLACK);
    glPopMatrix();
}

void arm_component_3() {
    glPushMatrix();
    glTranslatef(ARM_COMPONET_RADIUS * 1.8 * 2.5, -ARM_COMPONET_RADIUS * PART_II_VS_PART_I * 4, 0);
    glRotatef(-10 + theta[4], Z_AXIS);
    glScalef(2, .3, .3);
    draw_sphere(ARM_COMPONET_RADIUS * PART_II_VS_PART_I, 15, 15, 0, BLACK);
    glPopMatrix();
}

void any_arm() {
    arm_component();
    arm_component_2();
    arm_component_3();
}

//-----------------------------------------------

// more body parts
void upper_arm_right() {

    glTranslatef(0, (U_TORSO_RADIUS + L_TORSO_RADIUS) * 1.9, U_TORSO_RADIUS);
    glScalef(.7, .7, .7);
    glRotatef(120, 1, 0, 0);
    glRotatef(90, 0, 0, 1);

    any_arm();

    glRotatef(-90, 0, 0, 1);
    glRotatef(-120, 1, 0, 0);
    glTranslatef(0, -(U_TORSO_RADIUS + L_TORSO_RADIUS) * 1.9, -U_TORSO_RADIUS);
}

void middle_arm_right() {

    glTranslatef(0, (U_TORSO_RADIUS + L_TORSO_RADIUS) * 2, U_TORSO_RADIUS);
    glRotatef(90, 1, 0, 0);
    glRotatef(90, 0, 0, 1);

    any_arm();

    glRotatef(-90, 0, 0, 1);
    glRotatef(-90, 1, 0, 0);
    glTranslatef(0, -(U_TORSO_RADIUS + L_TORSO_RADIUS) * 2, -U_TORSO_RADIUS);

}

void lower_arm_right() {
    glTranslatef(0, (U_TORSO_RADIUS + L_TORSO_RADIUS) * 2.1, U_TORSO_RADIUS);
    glRotatef(60, 1, 0, 0);
    glRotatef(90, 0, 0, 1);

    any_arm();

    glRotatef(-90, 0, 0, 1);
    glRotatef(-60, 1, 0, 0);
    glTranslatef(0, -(U_TORSO_RADIUS + L_TORSO_RADIUS) * 2.1, -U_TORSO_RADIUS);
}

void lower_arm_left() {
    glTranslatef(0, (U_TORSO_RADIUS + L_TORSO_RADIUS) * 2.1, -U_TORSO_RADIUS * 1.8);
    glRotatef(300, 1, 0, 0);
    glRotatef(90, 0, 0, 1);

    any_arm();

    glRotatef(-90, 0, 0, 1);
    glRotatef(-300, 1, 0, 0);
    glTranslatef(0, -(U_TORSO_RADIUS + L_TORSO_RADIUS) * 2.1, U_TORSO_RADIUS * 1.8);
}

void middle_arm_left() {
    glTranslatef(0, (U_TORSO_RADIUS + L_TORSO_RADIUS) * 2, -U_TORSO_RADIUS * 1.8);
    glRotatef(270, 1, 0, 0);
    glRotatef(90, 0, 0, 1);

    any_arm();

    glRotatef(-90, 0, 0, 1);
    glRotatef(-270, 1, 0, 0);
    glTranslatef(0, -(U_TORSO_RADIUS + L_TORSO_RADIUS) * 2, U_TORSO_RADIUS * 1.8);
}

void upper_arm_left() {
    glTranslatef(0, (U_TORSO_RADIUS + L_TORSO_RADIUS) * 1.9, -U_TORSO_RADIUS * 1.8);
    glRotatef(240, 1, 0, 0);
    glRotatef(90, 0, 0, 1);

    any_arm();

    glRotatef(-90, 0, 0, 1);
    glRotatef(-240, 1, 0, 0);
    glTranslatef(0, -(U_TORSO_RADIUS + L_TORSO_RADIUS) * 1.9, U_TORSO_RADIUS * 1.8);
}

void antenna_left() {
    glTranslatef(0, (U_TORSO_RADIUS + L_TORSO_RADIUS + HEAD_RADIUS) * 2.8, U_TORSO_RADIUS * .6);
    glRotatef(60, X_AXIS);
    glRotatef(120, Y_AXIS);
    glTranslatef(0, -(/*U_TORSO_RADIUS +*/ L_TORSO_RADIUS * 1.2), 0);
    arm_component();
    glTranslatef(0, (/*U_TORSO_RADIUS +*/ L_TORSO_RADIUS * 1.2), 0);
    glRotatef(-120, Y_AXIS);
    glRotatef(-60, X_AXIS);
    glTranslatef(0, -(U_TORSO_RADIUS + L_TORSO_RADIUS + HEAD_RADIUS) * 2.8, -U_TORSO_RADIUS * .6);
}

void antenna_right() {
    glTranslatef(0, (U_TORSO_RADIUS + L_TORSO_RADIUS + HEAD_RADIUS) * 2.5, U_TORSO_RADIUS * .3);
    glRotatef(-60, X_AXIS);
    glRotatef(-120, Y_AXIS);
    arm_component();
    glRotatef(120, Y_AXIS);
    glRotatef(60, X_AXIS);
    glTranslatef(0, -(U_TORSO_RADIUS + L_TORSO_RADIUS + HEAD_RADIUS) * 2.5, -U_TORSO_RADIUS * .3);

}

void wing_right() {
    glTranslatef(-U_TORSO_RADIUS * 1.4, (L_TORSO_RADIUS + U_TORSO_RADIUS) * 1.1, -U_TORSO_RADIUS * 1.8);
    glRotatef(-15 + theta[2], Z_AXIS);
    glRotatef(120, X_AXIS);
    glScalef(.2, .7, 2.2);
    draw_sphere(WINGS_RADIUS, 15, 15, 0, .7, .7, .7);
    glScalef(1 / .2, 1 / .7, 1 / 2.2);
    glRotatef(-120, X_AXIS);
    glRotatef(15 - theta[2], Z_AXIS);
    glTranslatef(U_TORSO_RADIUS * 1.4, -(L_TORSO_RADIUS + U_TORSO_RADIUS) * 1.1, U_TORSO_RADIUS * 1.8);

}

void wing_left() {

    glTranslatef(-U_TORSO_RADIUS * 1.25, (L_TORSO_RADIUS + U_TORSO_RADIUS) * 1.1, U_TORSO_RADIUS * 1.5);

    glRotatef(-15 + theta[1], Z_AXIS);
    glRotatef(240, X_AXIS);
    glScalef(.2, .7, 2.2);
    draw_sphere(WINGS_RADIUS, 15, 15, 0, 1, 1, 1);
    glScalef(1 / .2, 1 / .7, 1 / 2.2);
    glRotatef(-240, X_AXIS);
    glRotatef(15 - theta[1], Z_AXIS);
    glTranslatef(U_TORSO_RADIUS * 1.25, -(L_TORSO_RADIUS + U_TORSO_RADIUS) * 1.1, -U_TORSO_RADIUS * 1.5);

}


//----------------------------------------------
void display(void) {

//    this is the base of all animations, all the angles are being modified here
    if (wing_bool) {
//        printf("theta %f", theta[1]);
        if (cnt >= 0) {
            theta[1] += 2.0;
            theta[2] += 2.0;
            theta[3] += .002;
            theta[4] += .5;
            theta[5] += .02;
            cnt++;
        }
        if (cnt == 5) {
            cnt -= 10;
        }
        if (cnt < 0) {
            theta[1] -= 2.0;
            theta[2] -= 2.0;
            theta[3] -= .002;
            theta[4] -= .5;
            theta[5] += .02;
            cnt++;
        }
    }
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    gluLookAt(0.0f, 0.0f, 10.0f,
              0.0f, 0.0f, 0.0f,
              0.0f, 1.0f, 0.0f);

    glPushMatrix();

    glLoadIdentity();
    glScalef(.8, .8, .8);
    glRotatef(theta[0], 0, 1, 0);
    if (wing_bool) {
        glTranslatef(-4 + theta[5], -2 + theta[5], 0);
    }
    glRotatef(-90, Z_AXIS);
    glPushMatrix();
    // drawing axes
//    axes();
    head();
    upper_torso();
    lower_torso();

    upper_arm_right();
    middle_arm_right();
    lower_arm_right();

    lower_arm_left();
    middle_arm_left();
    upper_arm_left();

    antenna_left();
    antenna_right();

    wing_right();
    wing_left();
    glPopMatrix();


    glutSwapBuffers();
}


void mouse(int btn, int state, int x, int y) {

    /* left button increase joint angle, right button decreases it */
    if (axis == 0) {
        if (btn == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
            theta[0] += 5;
        } else if (btn == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) {
            theta[0] -= 5;
        }
        wing_bool = 0;
    }
    if (axis == 1) {
        wing_bool = 1;
    }
    if (axis == 2) {
        wing_bool = 0;
    }
    glutPostRedisplay();
}

void menu(int id) {

    // menu selects which angle to change or whether to quit
    if (id == 1) axis = 0;
    else if (id == 2) axis = 1;
    else if (id == 3) axis = 2;
    else if (id == 4) exit(0);
}

void reshape(int w, int h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    if (w <= h)
        glOrtho(-10, 10,
                -5.0 * (GLfloat) h / (GLfloat) w, 15.0 * (GLfloat) h / (GLfloat) w,
                -10, 10);
    else
        glOrtho(-10.0 * (GLfloat) w / (GLfloat) h, 10.0 * (GLfloat) w / (GLfloat) h,
                -5, 15,
                -10, 10);
    glMatrixMode(GL_MODELVIEW);
}

void myinit() {
    // for texture
    loadTextures(LuciaDaleFileName);
    glEnable(GL_TEXTURE_2D);
    glClearColor(WHITE, 1);
    glPointSize(2);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    // allocate quadric object & render as wireframe
    quadObj = gluNewQuadric();
    gluQuadricDrawStyle(quadObj, GLU_LINE);
}

int main(int argc, char **argv) {
	
    printf("\npress the middle button for instructions\n");
    printf("\nif the menu selected is change the viewing angle,\nright and left click to change the directions\n\n");
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(1000, 700);
    glutCreateWindow("Mosquito");

    myinit();

    glutReshapeFunc(reshape);
    glutDisplayFunc(display);
    glutMouseFunc(mouse);
    glutIdleFunc(display);

    glutCreateMenu(menu);
    glutAddMenuEntry("change the viewing z - angle", 1);
    glutAddMenuEntry("animate the bug", 2);
    glutAddMenuEntry("stop animation", 3);
    glutAddMenuEntry("quit", 4);
    glutAttachMenu(GLUT_MIDDLE_BUTTON);

    glutMainLoop();
    return 0;
}

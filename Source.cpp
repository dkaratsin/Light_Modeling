#include <stdlib.h>
#include <GL/glut.h>
#include <stdio.h>
#include <time.h>
#include <math.h>


int selectedOption = 1;
int selectedOptionGrass = 2;
int cameraAngle = 0;
GLsizei windowWidth = 1000;
GLsizei windowHeight = 1000;
GLuint id, id2;
GLubyte colours[6][3];
GLfloat angle = 0.0;
GLfloat size = 1.0;
GLboolean grow = true;
GLint a = 6;
GLint b = 90;
GLint radius = 20;
const int MAX_DEPTH = 4;
bool enableSpotlight = true;
bool flatShading = true;

float building_color[13] = { 0.55, 0.25, 0.25, 1.0,      /* ambient */
				   0.7, 0.25, 0.25, 1.0,    /* diffuse */
				   0.0, 0.0, 0.0, 0.0, /* specular */
				   0.0                               /* shininess */
};
float roof_color[13] = { 0.2, 0.2, 0.2, 1.0,      /* ambient */
				   0.4, 0.4, 0.4, 1.0,	/* diffuse */
				   0.871, 0.871, 0.871, 1.0,	/* specular */
				   100.0                             /* shininess */
};
float grass_color[13] = { 0.0, 0.0, 0.0, 1.0,      /* ambient */
				   0.3, 1.0, 0.3, 1.0,    /* diffuse */
				   0.0, 0.0, 0.0, 0.0, /* specular */
				   0.0                               /* shininess */
};
float sun_color[4] = { 1.0, 1.0, 0.0, 1.0 }; /* emission */

#define M_PI 3.14159265358979323846

void menu(int menuOption) {
	if (menuOption == 1) selectedOptionGrass = 1;
	if (menuOption == 2) selectedOptionGrass = 2;
	if (menuOption == 3) enableSpotlight = true;
	if (menuOption == 4) enableSpotlight = false;
	if (menuOption == 5) flatShading = true;
	if (menuOption == 6) flatShading = false;
	glutPostRedisplay();
}



// Define a simple structure for 3D points
struct Point {
	GLfloat x, y, z;
	Point(GLfloat x, GLfloat y, GLfloat z) : x(x), y(y), z(z) {}
};

// Function to normalize a point to lie on the surface of a sphere
Point normalize(const Point& p) {
	GLfloat length = sqrt(p.x * p.x + p.y * p.y + p.z * p.z);
	return Point(p.x / length, p.y / length, p.z / length);
}

// Function to draw a triangle given three vertices
void drawTriangle(const Point& p1, const Point& p2, const Point& p3) {
	glBegin(GL_TRIANGLES);
	glVertex3f(p1.x, p1.y, p1.z);
	glVertex3f(p2.x, p2.y, p2.z);
	glVertex3f(p3.x, p3.y, p3.z);
	glEnd();
}

// Recursive function to subdivide a triangle and draw the resulting triangles
void subdivide(const Point& a, const Point& b, const Point& c, int depth) {
	if (depth == 0) {
		drawTriangle(a, b, c);
		return;
	}

	// Midpoints of the edges
	Point ab = normalize(Point((a.x + b.x) / 2, (a.y + b.y) / 2, (a.z + b.z) / 2));
	Point bc = normalize(Point((b.x + c.x) / 2, (b.y + c.y) / 2, (b.z + c.z) / 2));
	Point ca = normalize(Point((c.x + a.x) / 2, (c.y + a.y) / 2, (c.z + a.z) / 2));

	// Recursively subdivide the new triangles
	subdivide(a, ab, ca, depth - 1);
	subdivide(b, bc, ab, depth - 1);
	subdivide(c, ca, bc, depth - 1);
	subdivide(ab, bc, ca, depth - 1);
}

void myinit(void)
{
	glEnable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	srand(time(NULL));
	//glEnable(GL_NORMALIZE);
	glEnable(GL_LIGHT0);  // Enable light source 0
	
	glEnable(GL_NORMALIZE);
	


	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-100.0, 100.0, -100.0, 100.0, -200.0, 200.0);
	glMatrixMode(GL_MODELVIEW);

	for (int i = 0; i < 6; i++) {
		colours[i][0] = rand() % 255;
		colours[i][1] = rand() % 255;
		colours[i][2] = rand() % 255;
	}

	id = glGenLists(1);
	glNewList(id, GL_COMPILE);
	glBegin(GL_QUADS);
		glVertex3f(-5, -5, 5);
		glVertex3f(5, -5, 5);
		glVertex3f(5, 5, 5);
		glVertex3f(-5, 5, 5);
	glEnd();
	glEndList();


	glutCreateMenu(menu);

	// Add menu items
	glutAddMenuEntry("1 POLYGON FOR GROUND", 1);
	glutAddMenuEntry("100 POLYGONS FOR GROUND", 2);
	glutAddMenuEntry("ENABLE SPOTLIGHT", 3);
	glutAddMenuEntry("DISABLE SPOTLIGHT", 4);
	glutAddMenuEntry("FLAT SHADING", 5);
	glutAddMenuEntry("SMOOTH SHADING", 6);


	// Associate a mouse button with menu
	glutAttachMenu(GLUT_RIGHT_BUTTON);
}

void reshape(int newWidth, int newHeight) {
	glutReshapeWindow(1000, 1000);
}

void idle(void) {
	glutPostRedisplay();
}

void catchKey(int key, int x, int y) {
	if (key == GLUT_KEY_LEFT)
		cameraAngle += 10;
	else if (key == GLUT_KEY_RIGHT)
		cameraAngle -= 10;
}

void display(void) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//glShadeModel(GL_FLAT);
	if (flatShading) {
		glShadeModel(GL_FLAT);
	}
	else {
		glShadeModel(GL_SMOOTH);
	}
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0, 40, 70, 0, 0, 0, 0, 1, 0);
	glRotatef(cameraAngle, 0.0, 1.0, 0);

	

	Point vertices[] = {
		Point(0.0, 0.0, 1.0),
		Point(0.0, 0.942809, -0.33333),
		Point(-0.816497, -0.471405, -0.333333),
		Point(0.816497, -0.471405, -0.333333)
	};
	glPushMatrix();
	glTranslated(0, 0, 5);
	glPushMatrix();//FRONT
	glTranslatef(0, 0, 5);
	glColor3ub(colours[0][0], colours[0][1], colours[0][2]);
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, building_color);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, &building_color[4]);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, &building_color[8]);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, building_color[12]);
	glBegin(GL_QUADS);
	glNormal3d(0, 0, 1);
	glVertex3f(-5, -5, 5);
	glNormal3d(0, 0, 1);
	glVertex3f(5, -5, 5);
	glNormal3d(0, 0, 1);
	glVertex3f(5, 5, 5);
	glNormal3d(0, 0, 1);
	glVertex3f(-5, 5, 5);
	glEnd();
	glPopMatrix();

	glPushMatrix();//FRONT-RIGHT
	glTranslatef(0, 0, 5);
	glRotatef(90.0, 0.0, 1.0, 0.0);
	glColor3ub(colours[1][0], colours[1][1], colours[1][2]);
	glBegin(GL_QUADS);
	glNormal3d(1, 0, 0);
	glVertex3f(-5, -5, 5);
	glNormal3d(1, 0, 0);
	glVertex3f(-5, 5, 5);
	glNormal3d(1, 0, 0);
	glVertex3f(5, 5, 5);
	glNormal3d(1, 0, 0);
	glVertex3f(5, -5, 5);
	glEnd();
	glPopMatrix();

	glPushMatrix();//BACK-RIGHT
	glTranslatef(0, 0, -5);
	glRotatef(90.0, 0.0, 1.0, 0.0);
	glColor3ub(colours[1][0], colours[1][1], colours[1][2]);
	glBegin(GL_QUADS);
	glNormal3d(1, 0, 0);
	glVertex3f(-5, -5, 5);
	glNormal3d(1, 0, 0);
	glVertex3f(-5, 5, 5);
	glNormal3d(1, 0, 0);
	glVertex3f(5, 5, 5);
	glNormal3d(1, 0, 0);
	glVertex3f(5, -5, 5);
	glEnd();
	glPopMatrix();

	glPushMatrix();//FRONT-LEFT
	glTranslatef(0, 0, 5);
	glRotatef(-90.0, 0.0, 1.0, 0.0);
	glColor3ub(colours[2][0], colours[2][1], colours[2][2]);
	glBegin(GL_QUADS);
	glNormal3d(-1, 0, 0);
	glVertex3f(-5, -5, 5);
	glNormal3d(-1, 0, 0);
	glVertex3f(5, -5, 5);
	glNormal3d(-1, 0, 0);
	glVertex3f(5, 5, 5);
	glNormal3d(-1, 0, 0);
	glVertex3f(-5, 5, 5);
	glEnd();
	glPopMatrix();

	glPushMatrix();//BACK-LEFT
	glTranslatef(0, 0, -5);
	glRotatef(-90.0, 0.0, 1.0, 0.0);
	glColor3ub(colours[2][0], colours[2][1], colours[2][2]);
	glBegin(GL_QUADS);
	glNormal3d(-1, 0, 0);
	glVertex3f(-5, -5, 5);
	glNormal3d(-1, 0, 0);
	glVertex3f(5, -5, 5);
	glNormal3d(-1, 0, 0);
	glVertex3f(5, 5, 5);
	glNormal3d(-1, 0, 0);
	glVertex3f(-5, 5, 5);
	glEnd();
	glPopMatrix();

	glPushMatrix();//BACK
	glTranslatef(0, 0, -5);
	glRotatef(180.0, 0.0, 1.0, 0.0);
	glColor3ub(colours[0][0], colours[0][1], colours[0][2]);
	glBegin(GL_QUADS);
	glNormal3d(0, 0, 1);
	glVertex3f(-5, -5, 5);
	glNormal3d(0, 0, 1);
	glVertex3f(-5, 5, 5);
	glNormal3d(0, 0, 1);
	glVertex3f(5, 5, 5);
	glNormal3d(0, 0, 1);
	glVertex3f(5, -5, 5);
	glEnd();
	glPopMatrix();

	glPushMatrix();//FRONT-DOWN
	glTranslatef(0, 0, 5);
	glRotatef(90.0, 1.0, 0.0, 0.0);
	glColor3ub(colours[4][0], colours[4][1], colours[4][2]);
	glBegin(GL_QUADS);
	glVertex3f(-5, -5, 5);
	glVertex3f(-5, 5, 5);
	glVertex3f(5, 5, 5);
	glVertex3f(5, -5, 5);
	glEnd();
	glPopMatrix();

	glPushMatrix();//BACK-DOWN
	glTranslatef(0, 0, -5);
	glRotatef(90.0, 1.0, 0.0, 0.0);
	glColor3ub(colours[4][0], colours[4][1], colours[4][2]);
	glCallList(id);
	glPopMatrix();

	glPushMatrix();//FRONT-UP
	glTranslatef(0, 0, 5);
	glRotatef(-90.0, 1.0, 0.0, 0.0);
	glColor3ub(colours[3][0], colours[3][1], colours[3][2]);
	glCallList(id);
	glPopMatrix();

	glPushMatrix();//BACK-UP
	glTranslatef(0, 0, -5);
	glRotatef(-90.0, 1.0, 0.0, 0.0);
	glColor3ub(colours[3][0], colours[3][1], colours[3][2]);
	glCallList(id);
	glPopMatrix();

	glPushMatrix();//ROOF-RIGHT
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, roof_color);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, &roof_color[4]);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, &roof_color[8]);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, roof_color[12]);
	glBegin(GL_QUADS);
	glNormal3f(0.59066, 0.80692, 0);
	glVertex3f(5, 5, 10);
	glNormal3f(0.59066, 0.80692, 0);
	glVertex3f(5, 5, -10);
	glNormal3f(0.59066, 0.80692, 0);
	glVertex3f(0, sqrt(75), -10);
	glNormal3f(0.59066, 0.80692, 0);
	glVertex3f(0, sqrt(75), 10);
	glEnd();
	glPopMatrix();

	glPushMatrix();//ROOF-LEFT
	glColor3ub(211, 211, 211);
	glBegin(GL_QUADS);
	glNormal3f(-0.59066, 0.80692, 0);
	glVertex3f(-5, 5, 10);
	glNormal3f(-0.59066, 0.80692, 0);
	glVertex3f(-5, 5, -10);
	glNormal3f(-0.59066, 0.80692, 0);
	glVertex3f(0, sqrt(75), -10);
	glNormal3f(-0.59066, 0.80692, 0);
	glVertex3f(0, sqrt(75), 10);
	glEnd();
	glPopMatrix();
	
	glPushMatrix();//TRIANGLES-FRONT-BACK
	glColor3ub(colours[1][0], colours[1][1], colours[1][2]);
	glBegin(GL_TRIANGLES);
	glNormal3f(0, 0, 1);
	glVertex3f(-5, 5, 10);
	glNormal3f(0, 0, 1);
	glVertex3f(0, sqrt(75), 10);
	glNormal3f(0, 0, 1);
	glVertex3f(5, 5, 10);
	glNormal3f(0, 0, 1);
	glVertex3f(-5, 5, -10);
	glNormal3f(0, 0, -1);
	glVertex3f(0, sqrt(75), -10);
	glNormal3f(0, 0, -1);
	glVertex3f(5, 5, -10);
	glNormal3f(0, 0, -1);
	glEnd();
	glPopMatrix();

	glPushMatrix();//GRASS
	glColor3ub(0, 255, 0);
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, grass_color);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, &grass_color[4]);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, &grass_color[8]);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, grass_color[12]);
	if (selectedOptionGrass == 1) {
		glBegin(GL_QUADS);
		glVertex3f(-40, -5, 40);
		glVertex3f(-40, -5, -40);
		glVertex3f(40, -5, -40);
		glVertex3f(40, -5, 40);
		glEnd();
	}
	else if (selectedOptionGrass == 2) {
		
		
		glBegin(GL_QUADS);
		int drawingPos[2] = { -40,-40 };
		for (int i = 0; i < 10; i++) {
			drawingPos[0] = -40;
			for (int j = 0; j < 10; j++) {
				glNormal3f(0, 1, 0);
				glVertex3f(drawingPos[0], -5, drawingPos[1]);
				glNormal3f(0, 1, 0);
				glVertex3f(drawingPos[0], -5, drawingPos[1] + 8);
				glNormal3f(0, 1, 0);
				glVertex3f(drawingPos[0] + 8, -5, drawingPos[1] + 8);
				glNormal3f(0, 1, 0);
				glVertex3f(drawingPos[0] + 8, -5, drawingPos[1]);
				drawingPos[0] += 8;
			}
			drawingPos[1] += 8;
		}
		glEnd();
		
	}
	glPopMatrix();
	if (enableSpotlight) {
		glPushMatrix();//SPOTLIGHT
		glEnable(GL_LIGHT1);
		GLfloat spotlightpos[4] = { 0,10,10,1 };
		glLightfv(GL_LIGHT1, GL_POSITION, spotlightpos);
		GLfloat spotlightColor[] = { 1, 1, 1, 1.0 };
		GLfloat light1SpotDirection[] = { 0.0,-1.0,1 };
		glLightf(GL_LIGHT1, GL_SPOT_CUTOFF, 30.0);
		glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, light1SpotDirection);
		glLightfv(GL_LIGHT1, GL_DIFFUSE, spotlightColor);
		glLightfv(GL_LIGHT1, GL_SPECULAR, spotlightColor);
		glPopMatrix();
	}
	else {
		glDisable(GL_LIGHT1);
	}

	glPopMatrix();

	angle += 0.5;
	if (angle > 180.0) {
		angle = 0;
	}
	
	glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, true);
	glPushMatrix();//LIGHT
	glRotatef(-angle, 0.0, 0.0, 0.0);
	GLfloat lightpos[4] = { -50,0,0,1 };
	glLightfv(GL_LIGHT0, GL_POSITION, lightpos);
	glPopMatrix();

	float angleIntensity = 1 - 0.7 * float(abs(int(angle) - 90))/90.0;
	GLfloat lightColor[] = {angleIntensity, angleIntensity, angleIntensity, 1.0};  // white light
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightColor);
	glLightfv(GL_LIGHT0, GL_SPECULAR, lightColor);


	glPushMatrix();//SUN
	glRotatef(-angle, 0.0, 0.0, 1.0);
	glTranslatef(-50, 0, 0);

	glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, sun_color);
	subdivide(vertices[0], vertices[1], vertices[2], MAX_DEPTH);
	subdivide(vertices[0], vertices[2], vertices[3], MAX_DEPTH);
	subdivide(vertices[0], vertices[3], vertices[1], MAX_DEPTH);
	subdivide(vertices[1], vertices[3], vertices[2], MAX_DEPTH);
	glPopMatrix();
	
	float default_emission[4] = { 0.0, 0.0, 0.0, 1.0 };
	glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, default_emission);

	glutSwapBuffers();
}

void main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH); 
	glutInitWindowSize(windowWidth, windowHeight); 
	glutInitWindowPosition(0, 0); 
	glutCreateWindow("House and Sun"); 
	glDrawBuffer(GL_BACK);
	myinit(); 
	glutDisplayFunc(display); 
	glutReshapeFunc(reshape);
	glutIdleFunc(idle);
	glutSpecialFunc(catchKey);
	glutMainLoop(); 
}
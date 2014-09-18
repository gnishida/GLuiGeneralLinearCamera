#ifdef _WIN32
#include <windows.h>
#endif
#include <stdio.h>
#include <GL/glut.h>
#include "OBJLoader.h"
#include <GL/glui.h>
#include "GeneralLinearCamera.h"

static float FOV = 60.0;
static float nearZ = 0.1;
static float farZ = 100.0;
static int winWidth = 512;
static int winHeight = 512;
static int winId = -1;
static GLUI *glui;

std::vector<Vertex> vertices;
std::vector<Vector3f> frameBuffer;
GeneralLinearCamera camera;

float view_rotate[16] = { 1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1 };
float trans_z[] = { 300 };

/*****************************************************************************
*****************************************************************************/
static void
leftButtonDownCB(void)
{
   printf("left down!\n");
}

/*****************************************************************************
*****************************************************************************/
static void
leftButtonUpCB(void)
{
   printf("left up!\n");
}

/*****************************************************************************
*****************************************************************************/
static void
middleButtonDownCB(void)
{
   printf("middle down!\n");
}


/*****************************************************************************
*****************************************************************************/
static void
middleButtonUpCB(void)
{
   printf("middle up!\n");
}

/*****************************************************************************
*****************************************************************************/
static void
rightButtonDownCB(void)
{
   printf("right down!\n");
}


/*****************************************************************************
*****************************************************************************/
static void
rightButtonUpCB(void)
{
   printf("right up!\n");
}

/*****************************************************************************
*****************************************************************************/
static void
mouseCB(int button, int state, int x, int y)
{
   /* invert y so that ymax is up */
   int y2 = (winHeight - y - 1);

   printf("mouse @ (%i, %i)\n", x, y2);

   if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
      leftButtonDownCB();
   else if (button == GLUT_LEFT_BUTTON && state == GLUT_UP)
      leftButtonUpCB();
   else if (button == GLUT_MIDDLE_BUTTON && state == GLUT_DOWN)
      middleButtonDownCB();
   else if (button == GLUT_MIDDLE_BUTTON && state == GLUT_UP)
      middleButtonUpCB();
   else if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
      rightButtonDownCB();
   else if (button == GLUT_RIGHT_BUTTON && state == GLUT_UP)
      rightButtonUpCB();
}


/*****************************************************************************
*****************************************************************************/
static void
motionCB(int x, int y)
{
   /* invert y so that ymax is up */
   int y2 = (winHeight - y - 1);

   printf("mouse moved to (%i, %i)\n", x, y2);
}


/*****************************************************************************
*****************************************************************************/
void
reshapeCB(int width, int height)
{
	float aspect = (float)width/(float)height;

	winWidth = width;
	winHeight = height;

	frameBuffer.resize(width * height);
	for (int y = 0; y < height; ++y) {
		for (int x = 0; x < width; ++x) {
			frameBuffer[y * width + x] = Vector3f(1.0f, 0.5f, 0.5f);
		}
	}

	printf("Window now of size %i x %i\n", winWidth, winHeight);
}

/*****************************************************************************
*****************************************************************************/
void
keyboardCB(unsigned char key, int x, int y)
{
	/* invert y so that ymax is up */
	int y2 = (winHeight - y - 1);

	printf("key '%c' pressed at (%i, %i)\n", key, x, y2);
}

/*****************************************************************************
*****************************************************************************/
void
idleFunc()
{		
	glutPostRedisplay();
}

/*****************************************************************************
*****************************************************************************/
void drawScene()
{
	std::vector<Vertex> vertices2(vertices.size());
	for (int i = 0; i < vertices.size(); ++i) {
		//vertices2[i] = vertices[i];

		vertices2[i].position[0] = view_rotate[0] * vertices[i].position[0] + view_rotate[1] * vertices[i].position[1] + view_rotate[2] * vertices[i].position[2];
		vertices2[i].position[1] = view_rotate[4] * vertices[i].position[0] + view_rotate[5] * vertices[i].position[1] + view_rotate[6] * vertices[i].position[2];
		vertices2[i].position[2] = view_rotate[8] * vertices[i].position[0] + view_rotate[9] * vertices[i].position[1] + view_rotate[10] * vertices[i].position[2];

		vertices2[i].position[2] -= trans_z[0];

		vertices2[i].color[0] = vertices[i].color[0];
		vertices2[i].color[1] = vertices[i].color[1];
		vertices2[i].color[2] = vertices[i].color[2];
	}

	for (int y = 0; y < winHeight; ++y) {
		for (int x = 0; x < winWidth; ++x) {
			float u = (float)x + 0.5f - winWidth * 0.5f;
			float v = (float)y + 0.5f - winHeight * 0.5f;

			frameBuffer[y * winWidth + x] = camera.castRay(u, v, vertices2, Vector3f(0.5f, 0.5f, 0.5f));
		}
	}


    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDrawPixels(winWidth, winHeight, GL_RGB, GL_FLOAT, (float*)frameBuffer.data());
}

/*****************************************************************************
*****************************************************************************/
void
refreshCB()
{
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	// render geometry
	glPushMatrix();
	glTranslatef(0, 0, -trans_z[0]);
	glMultMatrixf( view_rotate );

	drawScene();
	glPopMatrix();

	// let's see it!
	glutSwapBuffers();
}

/*****************************************************************************
*****************************************************************************/
char *
readFile(char *fileName)
{
	char *text = new char[10000];
	char line[256];
	FILE *fp;
	fopen_s(&fp, fileName, "r");
	if (!fp) return NULL;

	text[0] = '\0';
	while (fgets(line,256,fp)) {
		strcat_s(text, 10000, line);
	}
	
	fclose(fp);

	return (text);
}

/*****************************************************************************
*****************************************************************************/
void MakeGUI()
{
	glui = GLUI_Master.create_glui("GUI", 0, 0, 0);
	glui->add_statictext( "Simple GLUI Example" );
	glui->add_rotation("Rotation", view_rotate);
	glui->add_translation("Zoom in/out", GLUI_TRANSLATION_Z, trans_z);

	glui->set_main_gfx_window(winId);

	/* We register the idle callback with GLUI, *not* with GLUT */
	GLUI_Master.set_glutIdleFunc(idleFunc);
}

/*****************************************************************************
*****************************************************************************/
int
main(int argc, char *argv[])
{
	// init OpenGL/GLUT
	glutInit(&argc, argv);
	
	// create main window
	glutInitWindowPosition(0, 0);
	glutInitWindowSize(winWidth, winHeight);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	winId = glutCreateWindow("MyWindow");
	
	glClearColor(0,0,0,1);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_COLOR_MATERIAL);

	static GLfloat lightPosition[4] = {0.0f, 0.0f, 100.0f, 0.0f};
	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);

	// setup callbacks
	glutDisplayFunc(refreshCB);
	glutReshapeFunc(reshapeCB);
	glutKeyboardFunc(keyboardCB);
	glutMouseFunc(mouseCB);
	glutMotionFunc(motionCB);

	// load OBJ
	OBJLoader::load("models/cube2.obj", vertices);

	// assign a random color to each face
	for (int i = 0; i < vertices.size(); i+=3) {
		float r = (float)(rand() % 255) / 255.0f;
		float g = (float)(rand() % 255) / 255.0f;
		float b = (float)(rand() % 255) / 255.0f;

		for (int j = 0; j < 3; ++j) {
			vertices[i + j].color[0] = r;
			vertices[i + j].color[1] = g;
			vertices[i + j].color[2] = b;
		}
	}

	// force initial matrix setup
	reshapeCB(winWidth, winHeight);

	// set modelview matrix stack to identity
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// make GLUI GUI
	MakeGUI();
	glutMainLoop();

	return (TRUE);
}

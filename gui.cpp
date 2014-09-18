#include <windows.h>
#include <math.h>
#include <stdio.h>
#include <common.h>
#include "mycamera.h"
#include "track.h"
#include "robot.h"
#include "calibrate.h"
#include "comm.h"
#include "checkerboard.h"
#include "gui.h"
#include "GLee.h"
#include <GL/gl.h>
#include <GL/glut.h>
#include <GL/glui.h>
#include <timer.h>


#define NOID				1
#define QUITID				2
#define CALIBRATEID			3
#define CALIBLOADCAMID		4
#define CALIBLOADROBID		5
#define CALIBSAVECAMID		6
#define CALIBSAVEROBID		7
#define CALIBCLEARID		8
#define COMMID      		9
#define SAVEFRAMEID    		10
#define REFINEALLID    		11
#define ROBOTMOVEID       	12
#define ROBOTSHOWIMAGEID    13
#define SETMOVETOPTID       14
#define SETLOOKATPTID       15
#define POINTSELECTIONID	16
#define CURRROBOTID			17
#define QUITROBOTID			18
#define ROBOTFINDCENTERID	19
#define ROBOTPROJECTPOINT	20

extern void QuitProgram(void);
//extern float IntensityReMap(float);
extern VMImage *fbImage;
extern VMArray<Robot> robot;
extern Tsai *camTsai;
extern Tsai *prjTsai[ROBOT_MAXIMUM_COUNT];

static GLUI *glui       = NULL;
int winWidth            = 1024;
int winHeight           = 768;
int winId               = -1;
int doTracking          = FALSE;
int doShowThreshold     = FALSE;
int doShowImage         = TRUE;
int doVerbose           = FALSE;
int doPointSelection    = 0;
int doComm              = TRUE;
int doCornerRefinement  = TRUE;
int doUseSavedFrame     = FALSE;
int doSetMoveToPt       = FALSE;
int doSetLookAtPt       = FALSE;
int cornerPixThresOpt   = 1;
int currRobotID         = 0;

GLUI_Button *findCenterButton = NULL;
GLUI_Button *projPointButton = NULL;
GLUI_Button *moveButton = NULL;
GLUI_Button *showImageButton = NULL;
GLUI_Button *quitRobotButton = NULL;


/*****************************************************************************
*****************************************************************************/
void
	UpdateRobotButtons(int id)
{
	assert(id >= 0 && id < robot.size());
	if (robot[id].ready && currRobotID == id) {
		findCenterButton->enable();
		moveButton->enable();
		showImageButton->enable();
		quitRobotButton->enable();
	} else if (!robot[id].ready && currRobotID == id) {
		findCenterButton->disable();
		moveButton->disable();
		showImageButton->disable();
		quitRobotButton->disable();
	}
}


/*****************************************************************************
*****************************************************************************/
static void
	guiCB(int id)
{
	switch (id) {
	case CALIBLOADCAMID :
		{
			CalibrateLoadCamera();
			break;
		}

	case CALIBLOADROBID :
		{
			CalibrateLoadRobot(currRobotID);
			break;
		}
	case CALIBSAVECAMID :
		{
			CalibrateSaveCamera();
			break;
		}
	case CALIBSAVEROBID :
		{
			CalibrateSaveRobot(currRobotID);
			break;
		}
	case CALIBCLEARID :
		{
			CalibrateClear();
			break;
		}
	case REFINEALLID :
		{
			CalibrateRefineAll(doPointSelection, currRobotID);
			break;
		}
	case CALIBRATEID :
		{
			Calibrate(doPointSelection, currRobotID);
			break;
		}
	case COMMID :
		{
			if (!doComm) {
				CommDone();
			} else {
				if (!CommInit())
					exit(-1);
			}
			break;
		}
	case SAVEFRAMEID :
		{
			if (fbImage) {
				printf("Saving frame to \"%s\"...", "savedframe.jpg"); fflush(stdout);
				fbImage->write("savedframe.jpg");
				printf("done!\n");
			}
			break;
		}
	case POINTSELECTIONID :
		{
			if (doPointSelection > 0) {
				// cannot also set move-to/look-at pt
				doSetMoveToPt = FALSE;
				doSetLookAtPt = FALSE;
			    glui->sync_live();
			}
			break;
	    }
	case SETMOVETOPTID :
		{
			if (doSetMoveToPt) {
				// cannot also do point selection and look-at pt
				doPointSelection = 0;
				doSetLookAtPt = FALSE;
			    glui->sync_live();
			}
			break;
	    }
	case SETLOOKATPTID:
		{
			if (doSetLookAtPt) {
				// cannot also do point selection and move-to pt
				doPointSelection = 0;
				doSetMoveToPt = FALSE;
			    glui->sync_live();
			}
			break;
	    }
	case QUITROBOTID :
		{
			if (doComm) {
				char msge[64];
				sprintf(msge, "%-63s", "quit");
				CommSendAction(currRobotID, msge);
			}
			break;
		}
	case ROBOTMOVEID :
		{
			if (doComm) {
				char msge[64];
				sprintf(msge, "%-63s", "move");
				CommSendAction(currRobotID, msge);
			}
			break;
		}
	case ROBOTSHOWIMAGEID :
		{
			if (doComm) {
				char msge[64];
				sprintf(msge, "%-63s", "showimage");
				CommSendAction(currRobotID, msge);
			}
			break;
		}
	case ROBOTFINDCENTERID :
	{
		if (doComm) {
			char msge[64];
			sprintf(msge, "%-63s", "findcenter");
			CommSendAction(currRobotID, msge);
		}
		break;
	}
	case ROBOTPROJECTPOINT :
		{
			double xc, yc, zc;
			xc = 427.772;
			// point 7
			prjTsai[currRobotID]->world_coord_to_image_coord(253.64, 246.17, -129.751, &xc, &yc);
			printf("\nImage coords %f, %f", xc, yc);
			// point 14
			prjTsai[currRobotID]->world_coord_to_image_coord(332.02, 303.53, -76.587, &xc, &yc);
			printf("\nImage coords %f, %f", xc, yc);
			break;
		}

	case QUITID :
		{
			QuitProgram();
			break;
		}
	case CURRROBOTID :
		{
			if (currRobotID >= robot.size())
				currRobotID = robot.size()-1;
			else if (currRobotID < 0)
				currRobotID = 0;
			UpdateRobotButtons(currRobotID);

			glui->sync_live();
			break;
		}
	case NOID :
		{
			glui->sync_live();
			break;
		}
	default :
		{
			printf("GUI error!\n");
		}
	}
	glutSetWindow(winId);
	glutPostRedisplay();
}


/*****************************************************************************

Create the GUI.

*****************************************************************************/

//////////////
// these seem to cause run time problems in this version of GLUI!
//////////////
#define MAKE_INT_SPINNER_PANEL(PANEL, TEXT, VAR, ID, MIN, MAX, SPEED, WIDTH)   \
	if (1) { \
	GLUI_Spinner *_sp = glui->add_spinner_to_panel(PANEL, TEXT,\
	GLUI_SPINNER_INT, &(VAR), ID, guiCB);\
	_sp->set_int_limits(MIN, MAX);\
	_sp->set_speed(SPEED);\
	_sp->set_alignment(GLUI_ALIGN_CENTER);\
	/*_sp->edittext->set_w(WIDTH);*/\
	} else
#define MAKE_INT_SPINNER(TEXT, VAR, ID, MIN, MAX, SPEED, WIDTH)   \
	if (1) { \
	GLUI_Spinner *_sp = glui->add_spinner(TEXT,\
	GLUI_SPINNER_INT, &(VAR), ID, guiCB);\
	_sp->set_int_limits(MIN, MAX);\
	_sp->set_speed(SPEED);\
	_sp->set_alignment(GLUI_ALIGN_CENTER);\
	/*_sp->edittext->set_w(WIDTH);*/\
	} else

static void
	MakeGUI()
{
	glui->add_checkbox("Track All Robots", &doTracking, NOID, guiCB);
	glui->add_checkbox("Set Move To Pt", &doSetMoveToPt, SETMOVETOPTID, guiCB);
	glui->add_checkbox("Set Look At Pt", &doSetLookAtPt, SETLOOKATPTID, guiCB);
	findCenterButton = glui->add_button("Find Center", ROBOTFINDCENTERID, guiCB);
	moveButton = glui->add_button("Move Robot", ROBOTMOVEID, guiCB);
	projPointButton = glui->add_button("Project Point", ROBOTPROJECTPOINT, guiCB);
	showImageButton = glui->add_button("Show Image on Robot", ROBOTSHOWIMAGEID, guiCB);
	quitRobotButton = glui->add_button("Quit Robot", QUITROBOTID, guiCB);
    //MAKE_INT_SPINNER("Current Robot ID", currRobotID, NOID, 0, robot.size()-1, 1.0, 20);
	GLUI_RadioGroup *gp0 = glui->add_radiogroup(&currRobotID, CURRROBOTID, guiCB);
	if (robot.size() >= 1) glui->add_radiobutton_to_group(gp0, "Robot 0");
	if (robot.size() >= 2) glui->add_radiobutton_to_group(gp0, "Robot 1");
	if (robot.size() >= 3) glui->add_radiobutton_to_group(gp0, "Robot 2");
	if (robot.size() >= 4) glui->add_radiobutton_to_group(gp0, "Robot 3");
	assert(robot.size() <= 4);

	glui->add_separator();

	GLUI_Panel *tk = glui->add_rollout("Tracking", false);
	GLUI_Panel *cm = glui->add_rollout("Communication", false);
	GLUI_Panel *cl = glui->add_rollout("Calibration", false);
	glui->add_separator();

	glui->add_button("Save Frame", SAVEFRAMEID, guiCB);
	glui->add_checkbox("Use Saved Frame", &doUseSavedFrame, NOID, guiCB);
	glui->add_checkbox("Verbose Feedback", &doVerbose, NOID, guiCB);
	glui->add_button("Quit", QUITID, guiCB);

	// Tracking panel
	glui->add_checkbox_to_panel(tk, "Show Thresholded Image", &doShowThreshold, NOID, guiCB);
	glui->add_checkbox_to_panel(tk, "Show Captured Image", &doShowImage, NOID, guiCB);

	// Calibration panel
	glui->add_button_to_panel(cl, "Load Camera", CALIBLOADCAMID, guiCB);
	glui->add_button_to_panel(cl, "Load Robot", CALIBLOADROBID, guiCB);
	glui->add_button_to_panel(cl, "Save Camera", CALIBSAVECAMID, guiCB);
	glui->add_button_to_panel(cl, "Save Robot", CALIBSAVEROBID, guiCB);
	glui->add_button_to_panel(cl, "Clear", CALIBCLEARID, guiCB);
	glui->add_button_to_panel(cl, "Calibrate", CALIBRATEID, guiCB);

	glui->add_separator_to_panel(cl);
	GLUI_RadioGroup *gp1 = glui->add_radiogroup_to_panel(cl, &doPointSelection, POINTSELECTIONID, guiCB);
	glui->add_radiobutton_to_group(gp1, "None");
	glui->add_radiobutton_to_group(gp1, "Points for Camera");
	glui->add_radiobutton_to_group(gp1, "Points for Projector");
	
	glui->add_separator_to_panel(cl);
	glui->add_button_to_panel(cl, "Refine All", REFINEALLID, guiCB);
	glui->add_checkbox_to_panel(cl, "Do Corner Refinement", &doCornerRefinement, NOID, guiCB);
    //MAKE_INT_SPINNER_PANEL(cl, "Corner Pixel Thres", cornerPixThres, NOID, 0, 255, 1.0, 40);
	GLUI_RadioGroup *gp2 = glui->add_radiogroup_to_panel(cl, &cornerPixThresOpt, POINTSELECTIONID, guiCB);
	glui->add_radiobutton_to_group(gp2, "50");
	glui->add_radiobutton_to_group(gp2, "100");
	glui->add_radiobutton_to_group(gp2, "150");
	glui->add_radiobutton_to_group(gp2, "200");

	// Comm panel
	glui->add_checkbox_to_panel(cm, "Enable communication", &doComm, COMMID, guiCB);
}


/*****************************************************************************
*****************************************************************************/
static void
	mouseCB(int button, int state, int x, int y2)
{
	if (doSetMoveToPt) {

		// invert y so that ymax is up 
		VM2Point pt(x, winHeight - y2 - 1);

		assert(camTsai);
		assert(currRobotID >= 0 && currRobotID < ROBOT_MAXIMUM_COUNT);
		double xf = pt.x();
		double yf = pt.y();
		double xw, yw, zw = -1 * robot[currRobotID].ledZHeight;
		camTsai->image_coord_to_world_coord(xf, yf, zw, &xw, &yw);
		robot[currRobotID].moveToPt.set(xw, yw, zw);
		printf("R%02i: move to [%1.2f, %1.2f, %1.2f]\n",
			currRobotID, xw, yw, zw);

	} else if (doSetLookAtPt) {

		// invert y so that ymax is up 
		VM2Point pt(x, winHeight - y2 - 1);

		assert(camTsai);
		assert(currRobotID >= 0 && currRobotID < ROBOT_MAXIMUM_COUNT);
		double xf = pt.x();
		double yf = pt.y();
		double xw, yw, zw = -1 * robot[currRobotID].ledZHeight;
		camTsai->image_coord_to_world_coord(xf, yf, zw, &xw, &yw);
		robot[currRobotID].lookAtPt.set(xw, yw, zw);

		VM2Vector dir(robot[currRobotID].lookAtPt.x() - robot[currRobotID].moveToPt.x(),
		              robot[currRobotID].lookAtPt.y() - robot[currRobotID].moveToPt.y());
		dir.normalize();
		float theta = dir.heading();

		printf("R%02i: look at [%1.2f, %1.2f, %1.2f] (theta = %1.2f)\n",
			currRobotID, xw, yw, zw, theta/M_PI*180.0f);

	} else if (doPointSelection != 0) {
		VM2Point cornerPt;

		/* optional refinement */
		cornerPt.set((float)x, (float)y2);
		if (doCornerRefinement && fbImage) {
			RefineCheckerboardCorner(fbImage->data(), 
				fbImage->width(), fbImage->height(), cornerPt, (cornerPixThresOpt+1) * CHECKERBOARD_THRESHOLD_STEP );
		}

		/* invert y so that ymax is up */
		cornerPt.set(cornerPt.x(), winHeight - cornerPt.y() - 1);

		/* give to calibration */
		CalibrateSetPoint(doPointSelection, cornerPt, currRobotID);
	}
}


/*****************************************************************************
*****************************************************************************/
static void
	motionCB(int x, int y)
{
}


/*****************************************************************************
*****************************************************************************/
static void
	reshapeCB(int width, int height)
{
	float aspect = (float)width/(float)height;

	winWidth = width;
	winHeight = height;
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}



/*****************************************************************************
*****************************************************************************/
static void
	keyboardCB(unsigned char key, int x, int y)
{
	/* invert y so that ymax is up */
	int y2 = (winHeight - y - 1);

	//printf("key '%c' pressed at (%i, %i)\n", key, x, y2);

	switch (key) {
	case '+' : 
		{
			currRobotID = (currRobotID + 1) % robot.size();
			UpdateRobotButtons(currRobotID);
			glui->sync_live();
			break;
		}
	case '-' : 
		{
			currRobotID = (currRobotID == 0) ? robot.size()-1 : (currRobotID - 1);
			UpdateRobotButtons(currRobotID);
			glui->sync_live();
			break;
		}
	case 'q' : 
		{
			QuitProgram();
			break;
		}
	case 't' : 
		{
			doTracking = !doTracking;
			break;
		}
	case 's' : 
		{
			doShowThreshold = !doShowThreshold;
			break;
		}
	case 'v' : 
		{
			doVerbose = !doVerbose;
			break;
		}
	case 'i' : 
		{
			doShowImage = !doShowImage;
			break;
		}
	case 'm' :
		{
			doSetMoveToPt = !doSetMoveToPt;
			guiCB(SETMOVETOPTID);
			break;
		}
	case 'l' :
		{
			doSetLookAtPt = !doSetLookAtPt;
			guiCB(SETLOOKATPTID);
			break;
		}
	case 'h' :
		{
			printf("Key command summary:\n");
			printf("+ : increase robot ID\n");
			printf("- : decrease robot ID\n");
			printf("t : toggle tracking\n");
			printf("s : toggle show thresholded image\n");
			printf("v : toggle show (camera) image\n");
			printf("m : toggle set move to point mode\n");
			printf("l : toggle set look at point mode\n");
			printf("v : toggle verbose\n");
			printf("q : quit program!\n");

		}
	}
	glui->sync_live();
}


/*****************************************************************************
*****************************************************************************/
void
	refreshCB()
{
	static Timer timer;
	static int numFrames = 0;
	static char info[256] = "";

	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	// draw captured image
	if (fbImage) {
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluOrtho2D(0, fbImage->width()-1, 0, fbImage->height()-1);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		if (doShowImage) {
			glRasterPos2i(0, 0);
			glPixelZoom(1, 1);
			glDepthMask(FALSE);
			if (doShowThreshold) {
				VMImage *thresImage = fbImage->threshold((int)TRACK_LEDTHRESHOLD, FALSE, 0);
#if 0
				unsigned char *thresImage = fbImage->grayScale(); 
				for (int y=0; y<fbImage->height(); y++) {
					for (int x=0; x<fbImage->width(); x++)  {
						float val = thresImage[y*fbImage->width() + x];
						if (val >= TRACK_LEDTHRESHOLD) {
							val = IntensityReMap((float)thresImage[y*fbImage->width() + x]);
							thresImage[y*fbImage->width() + x] = (int)(val + 0.5);
						}
					}
				}
#endif
				glDrawPixels(thresImage->width(), thresImage->height(), GL_BLUE, GL_UNSIGNED_BYTE, thresImage->data());
				delete thresImage;
			} else {
				glDrawPixels(fbImage->width(), fbImage->height(), GL_RGB, GL_UNSIGNED_BYTE, fbImage->data());
			}
			glDepthMask(TRUE);
		}
	}

	// draw tracking data
	if (fbImage) {
		TrackAndRobotDraw(doTracking);
	}

	// draw calibration selection data
	if (doPointSelection == GUI_POINTS_FOR_CAM ||
		doPointSelection == GUI_POINTS_FOR_PROJ) {
		CalibrateDrawPoints(doPointSelection, currRobotID);
	}

	if (numFrames++ == 0) {
		timer.start();
	} else if (numFrames++ >= 30) {
		timer.end();
		double seconds = timer.seconds_elapsed();
		numFrames = 0;
		sprintf(info, "%1.2f fps", 30.0f/seconds);
	}

	// if draw string during first frames, program complains when in release mode (???)
	glRasterPos2i(0, 10);
	glutBitmapString(GLUT_BITMAP_HELVETICA_12, (const unsigned char *)info);

	// let's see it!
	glutSwapBuffers();
}


/*****************************************************************************
*****************************************************************************/
int
	GUIInit(int *argc, char *argv[])
{
	// init GLUT
	glutInit(argc, argv);

	// create main window
	glutInitWindowPosition(1920-1024, 0);
	glutInitWindowSize(winWidth, winHeight);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	winId = glutCreateWindow("PiTracking");

	// openGL basics
	glCullFace(GL_BACK);
	glDisable(GL_NORMALIZE);
	glDisable(GL_BLEND);
	glShadeModel(GL_SMOOTH);
	glReadBuffer(GL_BACK);
	glEnable(GL_SCISSOR_TEST);
	glEnable(GL_DEPTH_TEST);
	glClearColor(0,0,0,1);

	// setup callbacks
	glutDisplayFunc(refreshCB);
	glutReshapeFunc(reshapeCB);
	glutKeyboardFunc(keyboardCB);
	glutMouseFunc(mouseCB);
	glutMotionFunc(motionCB);

	// force initial matrix setup
	reshapeCB(winWidth, winHeight);

	// set modelview matrix stack to identity
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// make GLUI GUI
	glui = GLUI_Master.create_glui("GUI", 0, 0, 0);
	MakeGUI();

	// update robot buttons...
	UpdateRobotButtons(currRobotID);

	return (TRUE);
}
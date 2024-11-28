/******************************************************************************
 *
 * Computer Graphics Programming 2020 Project Template v1.0 (11/04/2021)
 *
 * Based on: Animation Controller v1.0 (11/04/2021)
 *
 * This template provides a basic FPS-limited render loop for an animated scene,
 * plus keyboard handling for smooth game-like control of an object such as a
 * character or vehicle.
 *
 * A simple static lighting setup is provided via initLights(), which is not
 * included in the animationalcontrol.c template. There are no other changes.
 *
 ******************************************************************************/
#define _CRT_SECURE_NO_WARNINGS

#include <Windows.h>
#include <freeglut.h>
#include <math.h>
#include <stdio.h>

 /******************************************************************************
  * Animation & Timing Setup
  ******************************************************************************/

  // Target frame rate (number of Frames Per Second).
#define TARGET_FPS 60				

// Ideal time each frame should be displayed for (in milliseconds).
const unsigned int FRAME_TIME = 1000 / TARGET_FPS;

// Frame time in fractional seconds.
// Note: This is calculated to accurately reflect the truncated integer value of
// FRAME_TIME, which is used for timing, rather than the more accurate fractional
// value we'd get if we simply calculated "FRAME_TIME_SEC = 1.0f / TARGET_FPS".
const float FRAME_TIME_SEC = (1000 / TARGET_FPS) / 1000.0f;

// Time we started preparing the current frame (in milliseconds since GLUT was initialized).
unsigned int frameStartTime = 0;

/******************************************************************************
 * Some Simple Definitions of Motion
 ******************************************************************************/

#define MOTION_NONE 0				// No motion.
#define MOTION_CLOCKWISE -1			// Clockwise rotation.
#define MOTION_ANTICLOCKWISE 1		// Anticlockwise rotation.
#define MOTION_BACKWARD -1			// Backward motion.
#define MOTION_FORWARD 1			// Forward motion.
#define MOTION_LEFT -1				// Leftward motion.
#define MOTION_RIGHT 1				// Rightward motion.
#define MOTION_DOWN -1				// Downward motion.
#define MOTION_UP 1					// Upward motion.

 // Represents the motion of an object on four axes (Yaw, Surge, Sway, and Heave).
 // 
 // You can use any numeric values, as specified in the comments for each axis. However,
 // the MOTION_ definitions offer an easy way to define a "unit" movement without using
 // magic numbers (e.g. instead of setting Surge = 1, you can set Surge = MOTION_FORWARD).
 //
typedef struct {
	int Yaw;		// Turn about the Z axis	[<0 = Clockwise, 0 = Stop, >0 = Anticlockwise]
	int Surge;		// Move forward or back		[<0 = Backward,	0 = Stop, >0 = Forward]
	int Sway;		// Move sideways (strafe)	[<0 = Left, 0 = Stop, >0 = Right]
	int Heave;		// Move vertically			[<0 = Down, 0 = Stop, >0 = Up]
} motionstate4_t;

/******************************************************************************
 * Keyboard Input Handling Setup
 ******************************************************************************/

// Represents the state of a single keyboard key.Represents the state of a single keyboard key.
typedef enum {
	KEYSTATE_UP = 0,	// Key is not pressed.
	KEYSTATE_DOWN		// Key is pressed down.
} keystate_t;

// Represents the states of a set of keys used to control an object's motion.
typedef struct {
	keystate_t MoveForward;
	keystate_t MoveBackward;
	keystate_t MoveLeft;
	keystate_t MoveRight;
	keystate_t MoveUp;
	keystate_t MoveDown;
	keystate_t TurnLeft;
	keystate_t TurnRight;
} motionkeys_t;

// Current state of all keys used to control our "player-controlled" object's motion.
motionkeys_t motionKeyStates = {
	KEYSTATE_UP, KEYSTATE_UP, KEYSTATE_UP, KEYSTATE_UP,
	KEYSTATE_UP, KEYSTATE_UP, KEYSTATE_UP, KEYSTATE_UP };

// How our "player-controlled" object should currently be moving, solely based on keyboard input.
//
// Note: this may not represent the actual motion of our object, which could be subject to
// other controls (e.g. mouse input) or other simulated forces (e.g. gravity).
motionstate4_t keyboardMotion = { MOTION_NONE, MOTION_NONE, MOTION_NONE, MOTION_NONE };

// Define all character keys used for input (add any new key definitions here).
// Note: USE ONLY LOWERCASE CHARACTERS HERE. The keyboard handler provided converts all
// characters typed by the user to lowercase, so the SHIFT key is ignored.

#define KEY_MOVE_FORWARD	'w'
#define KEY_MOVE_BACKWARD	's'
#define KEY_MOVE_LEFT		'a'
#define KEY_MOVE_RIGHT		'd'
#define KEY_RENDER_FILL		'l'
#define KEY_EXIT			27 // Escape key.

// Define all GLUT special keys used for input (add any new key definitions here).

#define SP_KEY_MOVE_UP		GLUT_KEY_UP
#define SP_KEY_MOVE_DOWN	GLUT_KEY_DOWN
#define SP_KEY_TURN_LEFT	GLUT_KEY_LEFT
#define SP_KEY_TURN_RIGHT	GLUT_KEY_RIGHT


//for obj file reading:

typedef struct {
	GLfloat x;
	GLfloat y;
	GLfloat z;
} vec3d;

typedef struct {
	GLfloat x;
	GLfloat y;
} vec2d;

typedef struct {
	int vertexIndex;    // Index of this vertex in the object's vertices array
	int texCoordIndex;  // Index of the texture coordinate for this vertex in the object's texCoords array
	int normalIndex;    // Index of the normal for this vertex in the object's normals array
} meshObjectFacePoint;

typedef struct {
	int pointCount;
	meshObjectFacePoint* points;
} meshObjectFace;

typedef struct {
	int vertexCount;
	vec3d* vertices;
	int texCoordCount;
	vec2d* texCoords;
	int normalCount;
	vec3d* normals;
	int faceCount;
	meshObjectFace* faces;
} meshObject;



/******************************************************************************
 * GLUT Callback Prototypes
 ******************************************************************************/

void display(void);
void reshape(int width, int h);
void keyPressed(unsigned char key, int x, int y);
void specialKeyPressed(int key, int x, int y);
void keyReleased(unsigned char key, int x, int y);
void specialKeyReleased(int key, int x, int y);
void idle(void);

/******************************************************************************
 * Animation-Specific Function Prototypes (add your own here)
 ******************************************************************************/

void main(int argc, char **argv);
void init(void);
void think(void);
void initLights(void);
void drawHelicopter(float x, float y, float z, float hYaw);
void updateCam(void);
void drawGrid(int gridSize, float squareSize);
void drawOval(float width, float length, float x, float y);

void loadImage(FILE* fileID);

void createWaterTexture();
void createLeavesTexture();
void createWoodTexture();
void drawEnvironment();
void drawLamp(void);
void drawDuckAnimation();
void createTreeDisplayList(void);
void drawTree();
void drawForest(GLuint tree);

void drawWoodBench(float x, float y, float z, float angle);

void drawTexturedCube();

void drawRock(float x, float y, float z, float angle, float scale, GLfloat A[], GLfloat D[], GLfloat S[], GLfloat Shine);

void createRockDisplayList(void);

void drawRocks(GLuint rock);

void drawLog();

void createLogDisplayList();

void drawLogs(GLuint log);




//OBJ
meshObject* loadMeshObject(char* fileName);
void renderMeshObject(meshObject* object, float scale, float x, float y, float z, float angle);
void initMeshObjectFace(meshObjectFace* face, char* faceData, int faceDataLength);
void close();
void freeMeshObject(meshObject* object);

/******************************************************************************
 * Animation-Specific Setup (Add your own definitions, constants, and globals here)
 ******************************************************************************/

// Render objects as filled polygons (0) or wireframes (1). Default filled.
int renderFillEnabled = 0;

#define PI 3.141592653589f
#define degToRad (PI / 180.0)

#define seaW 400
#define seaH 300

#define leafW 960
#define leafH 538

#define woodW 800
#define woodH 600

float rotorAngle = 0.0f;
float rotorAngle2 = 90.0f;
float tailRotorAngle = 0.0f;

float angle = 0.0f;

int cameraLookAtState = 0;

float hPos[3] = { 0.0f, 2.4f, 0.0f };
float cam[4] = { 0.0f, 0.0f, 0.0f, 0.0f};

float rotorRunningSpeed = 0.0f;

GLint windowWidth = 1000;
GLint windowHeight = 800;

GLUquadricObj* hBody;
GLUquadricObj* hTopRail;
GLUquadricObj* hTopRotor;

float hYaw = 0.0f;

//colours

GLfloat white[4] = { 1.0f, 1.0f, 1.0f, 1.0f };


GLfloat groundA[4] = { 0.18f, 0.35f, 0.15f, 1.0f };
GLfloat groundD[4] = { 0.23f, 0.45f, 0.19f, 1.0f };
GLfloat groundS[4] = { 0.05f, 0.05f, 0.05f, 1.0f };
GLfloat groundShine = 5.0f;

GLfloat duckColourA[4] = { 0.85f, 0.79f, 0.4f, 1.0f };
GLfloat duckColourD[4] = { 0.95f, 0.89f, 0.5f, 1.0f };
GLfloat duckColourS[4] = { 0.1f, 0.1f, 0.1f, 1.0f };
GLfloat duckShine = 10.0f;

GLfloat treeTrunkA[4] = { 0.35f, 0.24f, 0.1f, 1.0f };
GLfloat treeTrunkD[4] = { 0.41f, 0.28f, 0.13f, 1.0f };
GLfloat treeTrunkS[4] = { 0.05f, 0.05f, 0.05f, 1.0f };
GLfloat treeTrunkShine = 5.0f;


GLfloat treeLeavesA[4] = { 0.32f, 0.62f, 0.18f, 0.8f };
GLfloat treeLeavesD[4] = { 0.42f, 0.72f, 0.23f, 0.8f };
GLfloat treeLeavesS[4] = { 0.05f, 0.05f, 0.05f, 1.0f };
GLfloat treeLeafShine = 5.0f;


GLfloat logColourA[4] = { 0.27f, 0.17f, 0.08f, 1.0f };
GLfloat logColourD[4] = { 0.32f, 0.22f, 0.1f, 1.0f };
GLfloat logColourS[4] = { 0.05f, 0.05f, 0.05f, 1.0f };
GLfloat logShine = 5.0f;


GLfloat lampBaseA[4] = { 0.62f, 0.37f, 0.12f, 1.0f };
GLfloat lampBaseD[4] = { 0.72f, 0.47f, 0.14f, 1.0f };
GLfloat lampBaseS[4] = { 0.8f, 0.8f, 0.8f, 1.0f };
GLfloat lampBaseShine = 50.0f;


GLfloat lampA[4] = { 0.28f, 0.61f, 0.65f, 0.2f };
GLfloat lampD[4] = { 0.33f, 0.71f, 0.75f, 0.2f };
GLfloat lampS[4] = { 0.2f, 0.2f, 0.2f, 1.0f };
GLfloat lampShine = 15.0f;


GLfloat rock1A[4] = { 0.62f, 0.64f, 0.59f, 1.0f };
GLfloat rock1D[4] = { 0.72f, 0.74f, 0.69f, 1.0f };
GLfloat rock1S[4] = { 0.1f, 0.1f, 0.1f, 1.0f };
GLfloat rock1Shine = 10.0f;


GLfloat rock2A[4] = { 0.53f, 0.54f, 0.57f, 1.0f };
GLfloat rock2D[4] = { 0.63f, 0.64f, 0.67f, 1.0f };
GLfloat rock2S[4] = { 0.1f, 0.1f, 0.1f, 1.0f };
GLfloat rock2Shine = 10.0f;


GLfloat lightEmission[4] = { 0.87f, 0.37f, 0.0f, 1.0 };
GLfloat emissionNormal[4] = { 0.0f, 0.0f, 0.0f, 1.0 };

GLfloat pondS[4] = { 0.7f, 0.7f, 0.7f, 1.0f };
GLfloat pondShine = 90.0f;

GLfloat benchS[4] = { 0.2f, 0.2f, 0.2f, 1.0f };
GLfloat benchShine = 10.0f;


GLfloat helicopterBodyA[4] = { 0.04f, 0.15f, 0.3f, 1.0f };
GLfloat helicopterBodyD[4] = { 0.08f, 0.3f, 0.6f, 1.0f };
GLfloat helicopterBodyS[4] = { 0.5f, 0.5f, 0.5f, 1.0f };
GLfloat helicopterBodyShine = 60.0f;

GLfloat rotorA[4] = { 0.3f, 0.4f, 0.6f, 1.0f };
GLfloat rotorD[4] = { 0.55f, 0.67f, 0.9f, 1.0f };
GLfloat rotorS[4] = { 0.2f, 0.2f, 0.2f, 1.0f };
GLfloat rotorShine = 15.0f;


GLfloat fogColour[4] = { 0.91f, 0.75f, 0.58f, 1.0f };


//textures
 // image size
int imageWidth, imageHeight;

// the image data
GLubyte* imageData;
GLuint textureWater;
GLuint textureLeaves;
GLuint textureWood;


//OBJ

meshObject* treeLeavesMesh;
meshObject* duckMesh;
meshObject* logMesh;
meshObject* treeTrunkMesh;
meshObject* lampPostMesh;
meshObject* lampMesh;

GLuint treeDL;
GLuint rockDL;
GLuint logDL;

//duck animation

const float a = 6.0f;
const float b = 2.0f;
float t = 0.0f;
float duckX = 0.0f;
float duckZ = 0.0f;
float duckAngle = 30.0f;
float duckY = 0.35f;
int duckFloating = 1;

/******************************************************************************
 * Entry Point (don't put anything except the main function here)
 ******************************************************************************/

void main(int argc, char **argv)
{

	// Initialize the OpenGL window.
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(1000, 800);
	glutCreateWindow("Animation");

	// Set up the scene.
	init();

	// Disable key repeat (keyPressed or specialKeyPressed will only be called once when a key is first pressed).
	glutSetKeyRepeat(GLUT_KEY_REPEAT_OFF);

	// Register GLUT callbacks.
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyPressed);
	glutSpecialFunc(specialKeyPressed);
	glutKeyboardUpFunc(keyReleased);
	glutSpecialUpFunc(specialKeyReleased);
	glutIdleFunc(idle);

	// Record when we started rendering the very first frame (which should happen after we call glutMainLoop).
	frameStartTime = (unsigned int)glutGet(GLUT_ELAPSED_TIME);

	// Enter the main drawing loop (this will never return).
	glutMainLoop();

	free(imageData);
}

/******************************************************************************
 * GLUT Callbacks (don't add any other functions here)
 ******************************************************************************/

 /*
	 Called when GLUT wants us to (re)draw the current animation frame.

	 Note: This function must not do anything to update the state of our simulated
	 world. Animation (moving or rotating things, responding to keyboard input,
	 etc.) should only be performed within the think() function provided below.
 */
void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	gluLookAt(cam[0], cam[1], cam[2], hPos[0], hPos[1], hPos[2], 0.0f, 1.0f, 0.0f);

	drawGrid(60, 1.5f);

	drawHelicopter(hPos[0], hPos[1], hPos[2], hYaw);

	drawEnvironment();
	

	glutSwapBuffers();
	
}

/*
	Called when the OpenGL window has been resized.
*/
void reshape(int width, int h)
{
	windowWidth = width;
	windowHeight = h;

	glViewport(0, 0, windowWidth, windowHeight);

	glMatrixMode(GL_PROJECTION);

	glLoadIdentity();

	gluPerspective(45, (float)windowWidth / (float)windowHeight, 0.01, 100);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
}

/*
	Called each time a character key (e.g. a letter, number, or symbol) is pressed.
*/
void keyPressed(unsigned char key, int x, int y)
{
	switch (tolower(key)) {

	case KEY_MOVE_FORWARD:
		motionKeyStates.MoveForward = KEYSTATE_DOWN;
		keyboardMotion.Surge = MOTION_FORWARD;
		break;
	case KEY_MOVE_BACKWARD:
		motionKeyStates.MoveBackward = KEYSTATE_DOWN;
		keyboardMotion.Surge = MOTION_BACKWARD;
		break;
	case KEY_MOVE_LEFT:
		motionKeyStates.MoveLeft = KEYSTATE_DOWN;
		keyboardMotion.Sway = MOTION_LEFT;
		break;
	case KEY_MOVE_RIGHT:
		motionKeyStates.MoveRight = KEYSTATE_DOWN;
		keyboardMotion.Sway = MOTION_RIGHT;
		break;

	case KEY_RENDER_FILL:
		renderFillEnabled = !renderFillEnabled;
		break;
	case KEY_EXIT:
		exit(0);
		break;
	}
}

/*
	Called each time a "special" key (e.g. an arrow key) is pressed.
*/
void specialKeyPressed(int key, int x, int y)
{
	switch (key) {

	case SP_KEY_MOVE_UP:
		motionKeyStates.MoveUp = KEYSTATE_DOWN;
		keyboardMotion.Heave = MOTION_UP;
		break;
	case SP_KEY_MOVE_DOWN:
		motionKeyStates.MoveDown = KEYSTATE_DOWN;
		keyboardMotion.Heave = MOTION_DOWN;
		break;
	case SP_KEY_TURN_LEFT:
		motionKeyStates.TurnLeft = KEYSTATE_DOWN;
		keyboardMotion.Yaw = MOTION_ANTICLOCKWISE;
		break;
	case SP_KEY_TURN_RIGHT:
		motionKeyStates.TurnRight = KEYSTATE_DOWN;
		keyboardMotion.Yaw = MOTION_CLOCKWISE;
		break;
	}
}

/*
	Called each time a character key (e.g. a letter, number, or symbol) is released.
*/
void keyReleased(unsigned char key, int x, int y)
{
	switch (tolower(key)) {

	case KEY_MOVE_FORWARD:
		motionKeyStates.MoveForward = KEYSTATE_UP;
		keyboardMotion.Surge = (motionKeyStates.MoveBackward == KEYSTATE_DOWN) ? MOTION_BACKWARD : MOTION_NONE;
		break;
	case KEY_MOVE_BACKWARD:
		motionKeyStates.MoveBackward = KEYSTATE_UP;
		keyboardMotion.Surge = (motionKeyStates.MoveForward == KEYSTATE_DOWN) ? MOTION_FORWARD : MOTION_NONE;
		break;
	case KEY_MOVE_LEFT:
		motionKeyStates.MoveLeft = KEYSTATE_UP;
		keyboardMotion.Sway = (motionKeyStates.MoveRight == KEYSTATE_DOWN) ? MOTION_RIGHT : MOTION_NONE;
		break;
	case KEY_MOVE_RIGHT:
		motionKeyStates.MoveRight = KEYSTATE_UP;
		keyboardMotion.Sway = (motionKeyStates.MoveLeft == KEYSTATE_DOWN) ? MOTION_LEFT : MOTION_NONE;
		break;

	}
}

/*
	Called each time a "special" key (e.g. an arrow key) is released.
*/
void specialKeyReleased(int key, int x, int y)
{
	switch (key) {
	
	case SP_KEY_MOVE_UP:
		motionKeyStates.MoveUp = KEYSTATE_UP;
		keyboardMotion.Heave = (motionKeyStates.MoveDown == KEYSTATE_DOWN) ? MOTION_DOWN : MOTION_NONE;
		break;
	case SP_KEY_MOVE_DOWN:
		motionKeyStates.MoveDown = KEYSTATE_UP;
		keyboardMotion.Heave = (motionKeyStates.MoveUp == KEYSTATE_DOWN) ? MOTION_UP : MOTION_NONE;
		break;
	case SP_KEY_TURN_LEFT:
		motionKeyStates.TurnLeft = KEYSTATE_UP;
		keyboardMotion.Yaw = (motionKeyStates.TurnRight == KEYSTATE_DOWN) ? MOTION_CLOCKWISE : MOTION_NONE;
		break;
	case SP_KEY_TURN_RIGHT:
		motionKeyStates.TurnRight = KEYSTATE_UP;
		keyboardMotion.Yaw = (motionKeyStates.TurnLeft == KEYSTATE_DOWN) ? MOTION_ANTICLOCKWISE : MOTION_NONE;
		break;

	}
}

/*
	Called by GLUT when it's not rendering a frame.

	Note: We use this to handle animation and timing. You shouldn't need to modify
	this callback at all. Instead, place your animation logic (e.g. moving or rotating
	things) within the think() method provided with this template.
*/
void idle(void)
{
	// Wait until it's time to render the next frame.

	unsigned int frameTimeElapsed = (unsigned int)glutGet(GLUT_ELAPSED_TIME) - frameStartTime;
	if (frameTimeElapsed < FRAME_TIME)
	{
		unsigned int timeLeft = FRAME_TIME - frameTimeElapsed;
		Sleep(timeLeft);
	}

	// Begin processing the next frame.

	frameStartTime = glutGet(GLUT_ELAPSED_TIME); // Record when we started work on the new frame.

	think(); // Update our simulated world before the next call to display().

	glutPostRedisplay(); // Tell OpenGL there's a new frame ready to be drawn.
}

/******************************************************************************
 * Animation-Specific Functions (Add your own functions at the end of this section)
 ******************************************************************************/

 /*
	 Initialise OpenGL and set up our scene before we begin the render loop.
 */
void init(void)
{
	
	initLights();
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_FOG);
	glDisable(GL_COLOR_MATERIAL);

	glClearColor(0.87, 0.57, 0.53, 1.0);

	loadImage(fopen("assets/water.ppm", "r"));
	glGenTextures(1, &textureWater);
	createWaterTexture();
	loadImage(fopen("assets/leaf.ppm", "r"));
	glGenTextures(1, &textureLeaves);
	createLeavesTexture();
	loadImage(fopen("assets/wood.ppm", "r"));
	glGenTextures(1, &textureWood);
	createWoodTexture();

	hBody = gluNewQuadric();
	hTopRail = gluNewQuadric();
	hTopRotor = gluNewQuadric();
	gluQuadricDrawStyle(hBody, GLU_LINE);
	gluQuadricDrawStyle(hTopRail, GLU_LINE);
	gluQuadricDrawStyle(hTopRotor, GLU_LINE);

	treeLeavesMesh = loadMeshObject("assets/TreeLeaves.obj");
	duckMesh = loadMeshObject("assets/duck.obj");
	logMesh = loadMeshObject("assets/log.obj");
	treeTrunkMesh = loadMeshObject("assets/TreeTrunk.obj");
	lampPostMesh = loadMeshObject("assets/lampPost.obj");
	lampMesh = loadMeshObject("assets/lamp.obj");

	glFogfv(GL_FOG_COLOR, fogColour);
	glFogf(GL_FOG_MODE, GL_EXP2);
	glFogf(GL_FOG_DENSITY, 0.015f);

	createTreeDisplayList();
	createRockDisplayList();
	createLogDisplayList();
	// Anything that relies on lighting or specifies normals must be initialised after initLights.
}

/*
	Advance our animation by FRAME_TIME milliseconds.

	Note: Our template's GLUT idle() callback calls this once before each new
	frame is drawn, EXCEPT the very first frame drawn after our application
	starts. Any setup required before the first frame is drawn should be placed
	in init().
*/
void think(void)
{
	updateCam();
	GLfloat lightPosition1[] = { hPos[0], hPos[1] - 5.0f, hPos[2], 1.0f };
	glLightfv(GL_LIGHT1, GL_POSITION, lightPosition1);

	//duck animation
	t += FRAME_TIME_SEC / 5;

	if (t > 2 * PI) {
		t -= 2 * PI; //reset t
	}

	duckX = (float)(a * cos(t) + 30.0f); // x coordinate
	duckZ = (float)(b * sin(t) - 7.5f); // z coordinate

	duckAngle = t * (-180.0f / PI);

	if (duckFloating) {
		duckY += FRAME_TIME_SEC / 10;
		if (duckY >= 0.6f) {
			duckY = 0.6f;
			duckFloating = 0;
		}
	}
	else {
		duckY -= FRAME_TIME_SEC / 10;
		if (duckY <= 0.2f) {
			duckY = 0.2f;
			duckFloating = 1;
		}
	}
	
	//rotor speeds and roations
	float rotorLiftOffSpeed = 400.0f;
	float descentFactor = (hPos[1] - 2.4f) / (5.0f - 2.4f);

	if (hPos[1] > 2.4f) {
		if (hPos[1] <= 5.0f && keyboardMotion.Heave < 0) {
			rotorAngle += 360.0f * FRAME_TIME_SEC * descentFactor;
			if (rotorAngle > 360.0f) rotorAngle -= 360.0f;

			rotorAngle2 += 360.0f * FRAME_TIME_SEC * descentFactor;
			if (rotorAngle2 > 360.0f) rotorAngle2 -= 360.0f;

			tailRotorAngle += 360.0f * FRAME_TIME_SEC * descentFactor;
			if (tailRotorAngle > 360.0f) tailRotorAngle -= 360.0f;
		}
		else {
			rotorAngle += rotorRunningSpeed * FRAME_TIME_SEC;
			if (rotorAngle > 360.0f) rotorAngle -= 360.0f;

			rotorAngle2 += rotorRunningSpeed * FRAME_TIME_SEC;
			if (rotorAngle2 > 360.0f) rotorAngle2 -= 360.0f;

			tailRotorAngle += rotorRunningSpeed * FRAME_TIME_SEC;
			if (tailRotorAngle > 360.0f) tailRotorAngle -= 360.0f;
		}
	}
	else if (keyboardMotion.Heave == MOTION_NONE){
		rotorRunningSpeed = 0.0f;
		rotorAngle += rotorRunningSpeed * FRAME_TIME_SEC;
		rotorAngle2 += rotorRunningSpeed * FRAME_TIME_SEC;
		tailRotorAngle += rotorRunningSpeed * FRAME_TIME_SEC;
	}

	if (hPos[1] <= 3.0f && keyboardMotion.Heave < 0) {
		rotorRunningSpeed = 0.0f;
	}


	//quadric for helicopter model
	if (renderFillEnabled == 0) {
		gluQuadricDrawStyle(hBody, GLU_FILL);
		gluQuadricDrawStyle(hTopRail, GLU_FILL);
		gluQuadricDrawStyle(hTopRotor, GLU_FILL);
	}
	else {
		gluQuadricDrawStyle(hBody, GLU_LINE);
		gluQuadricDrawStyle(hTopRail, GLU_LINE);
		gluQuadricDrawStyle(hTopRotor, GLU_LINE);
	}


	//helicopter movement
	float movementSpeed = 0.05f;
	float turnSpeed = 1.0f;



	if (hPos[1] >= 2.4f) {
		if (keyboardMotion.Yaw != MOTION_NONE) {
			if (keyboardMotion.Yaw < 0) { // clockwise
				hYaw -= turnSpeed;
			}
			else { // anticlockwise
				hYaw += turnSpeed;
			}
		}
		if (keyboardMotion.Surge != MOTION_NONE) {
			if (keyboardMotion.Surge < 0) { // back
				hPos[0] += (float)(movementSpeed * sin(hYaw * PI / -180.0f));
				hPos[2] -= (float)(movementSpeed * cos(hYaw * PI / -180.0f));
			}
			else { // forward
				hPos[0] -= (float)(movementSpeed * sin(hYaw * PI / -180.0f));
				hPos[2] += (float)(movementSpeed * cos(hYaw * PI / -180.0f));
			}
		}
		if (keyboardMotion.Sway != MOTION_NONE) {
			if (keyboardMotion.Sway < 0) { // left
				hPos[0] += (float)(movementSpeed * cos(hYaw * PI / -180.0f));
				hPos[2] += (float)(movementSpeed * sin(hYaw * PI / -180.0f));
			}
			else { // right
				hPos[0] -= (float)(movementSpeed * cos(hYaw * PI / -180.0f));
				hPos[2] -= (float)(movementSpeed * sin(hYaw * PI / -180.0f));
			}
		}
	}
	if (keyboardMotion.Heave != MOTION_NONE) {
			if (keyboardMotion.Heave < 0) { // down
				if (hPos[1] >= 2.4f) {
					if (hPos[1] <= 5.0f) {
						hPos[1] -= movementSpeed * descentFactor * 0.6f;
					}
					else {
						hPos[1] -= movementSpeed;
					}
				}
			}
			else { // up
				if (rotorRunningSpeed < rotorLiftOffSpeed) {
					rotorRunningSpeed += 50.0f * FRAME_TIME_SEC;
					rotorAngle += rotorRunningSpeed * FRAME_TIME_SEC;
					rotorAngle2 += rotorRunningSpeed * FRAME_TIME_SEC;
					tailRotorAngle += rotorRunningSpeed * FRAME_TIME_SEC;
					if (rotorRunningSpeed > rotorLiftOffSpeed) {
						rotorRunningSpeed = rotorLiftOffSpeed;
						hPos[1] += movementSpeed;
					}
				}
				else {
					hPos[1] += movementSpeed;
				}
			}
	}

	if (hYaw >= 360.0f) {
		hYaw -= 360.0f;
	}
	else {
		hYaw += 360.0f;
	}
}

/*
	Initialise OpenGL lighting before we begin the render loop.
	
	Note (advanced): If you're using dynamic lighting (e.g. lights that move around, turn on or
	off, or change colour) you may want to replace this with a drawLights function that gets called
	at the beginning of display() instead of init().
*/
void initLights(void)
{
	// Simple lighting setup
	GLfloat globalAmbient[] = { 0.4f, 0.4f, 0.4f, 1 };
	GLfloat lightPosition[] = { 0.0f, 1.0f, 1.0f, 0.0f };
	GLfloat ambientLight[] = { 0.1f, 0.05f, 0.0f, 1.0f };
	GLfloat diffuseLight[] = { 0.8f, 0.4f, 0.2f, 1.0f };
	GLfloat specularLight[] = { 0.8f, 0.4f, 0.2f, 1.0f };

	GLfloat lightPosition1[] = { hPos[0], hPos[1] - 5.0f, hPos[2], 1.0f};
	GLfloat diffuseLight1[] = { 1.0f, 0.5f, 0.3f, 1.0f };
	GLfloat specularLight1[] = { 1.0f, 0.5f, 0.3f, 1.0f };
	GLfloat spotDirection[] = { 0.0f, -1.0f, -1.0f };
	GLfloat spotCutoff = 40.0f;
	GLfloat spotExp = 1.0f;
	
	// Configure global ambient lighting.
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, globalAmbient);
	
	// Configure Light 0.
	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specularLight);
	
	//animated spotlight

	glLightfv(GL_LIGHT1, GL_POSITION, lightPosition1);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuseLight1);
	glLightfv(GL_LIGHT1, GL_SPECULAR, specularLight1);
	glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, spotDirection);
	glLightf(GL_LIGHT1, GL_SPOT_CUTOFF, spotCutoff);
	glLightf(GL_LIGHT1, GL_SPOT_EXPONENT, spotExp);

	// Enable lighting
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	// Make GL normalize the normal vectors we supply.
	glEnable(GL_NORMALIZE);
	
	// Enable use of simple GL colours as materials.
	glEnable(GL_COLOR_MATERIAL);
}

void drawHelicopter(float x, float y, float z, float hYaw) { // cut into parts cuz too long
	glPushMatrix();

	//body of helicopter
	glTranslatef(x, y, z);
	glRotatef(hYaw, 0.0f, 1.0f, 0.0f);
	glScalef(0.3f, 0.3f, 0.4f);
	glMaterialfv (GL_FRONT, GL_AMBIENT, helicopterBodyA);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, helicopterBodyD);
	glMaterialfv(GL_FRONT, GL_SPECULAR, helicopterBodyS);
	glMaterialf(GL_FRONT, GL_SHININESS, helicopterBodyShine);
	gluSphere(hBody, 6, 12, 12);


	//top rails
	glPushMatrix();

		glTranslatef(0.0f, 5.5f, 0.0f);
		glScalef(0.3f, 0.3f, 0.3f);
		if (renderFillEnabled == 1) {
			glutWireCube(6.0f);
		}
		else {
			glutSolidCube(6.0f);
		}

			//top rotors
			glPushMatrix();
			glMaterialfv(GL_FRONT, GL_AMBIENT, rotorA);
			glMaterialfv(GL_FRONT, GL_DIFFUSE, rotorD);
			glMaterialfv(GL_FRONT, GL_SPECULAR, rotorS);
			glMaterialf(GL_FRONT, GL_SHININESS, rotorShine);
				glTranslatef(0.0f, 1.65f, 0.0f);
				glRotatef(rotorAngle, 0.0f, 1.0f, 0.0f);
				glScalef(9.0f, 0.17f, 0.17f);
				if (renderFillEnabled == 1) {
					glutWireCube(6.0f);
				}
				else {
					glutSolidCube(6.0f);
				}
			glPopMatrix();

			glPushMatrix();
				glTranslatef(0.0f, 1.65f, 0.0f);
				glRotatef(rotorAngle2, 0.0f, 1.0f, 0.0f);
				glScalef(9.0f, 0.17f, 0.17f);
				if (renderFillEnabled == 1) {
					glutWireCube(6.0f);
				}
				else {
					glutSolidCube(6.0f);
				}
			glPopMatrix();
	glPopMatrix();

	glMaterialfv(GL_FRONT, GL_AMBIENT, helicopterBodyA);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, helicopterBodyD);
	glMaterialfv(GL_FRONT, GL_SPECULAR, helicopterBodyS);
	glMaterialf(GL_FRONT, GL_SHININESS, helicopterBodyShine);

	//tail front
	glPushMatrix();
		glTranslatef(0.0f, 0.0f, -4.0f);
		glRotatef(180.0f, 1.0f, 0.0f, 0.0f);
		glScalef(0.3f, 0.3f, 2.0f);
		
		if (renderFillEnabled == 1) {
			glutWireCone(4.0f, 6.0f, 8, 8);
		}
		else {
			glutSolidCone(4.0f, 6.0f, 8, 8);
		}

		glScalef(1.0f / 0.3f, 1.0f / 0.3f, 1.0f / 2.0f); //reset scaling back to 1, 1, 1 (fixes back rotor skewing issue)

		//tail back
		glPushMatrix();
			glTranslatef(0.0f, 0.0f, 9.0f);
			glScalef(1.0f, 1.0f, 4.0f);
			if (renderFillEnabled == 1) {
				glutWireCube(1.5f);
			}
			else {
				glutSolidCube(1.5f);
			}

			glScalef(1.0f, 1.0f, 1.0f / 4.0f); //reset scaling back to 1, 1, 1

			//back rotor
		glPushMatrix();
		glMaterialfv(GL_FRONT, GL_AMBIENT, rotorA);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, rotorD);
		glMaterialfv(GL_FRONT, GL_SPECULAR, rotorS);
		glMaterialf(GL_FRONT, GL_SHININESS, rotorShine);
			glTranslatef(1.0f, -0.3f, 0.8f);
			glRotatef(rotorAngle, 1.0f, 0.0f, .0f);
			glScalef(0.2f, 0.2f, 4.0f);

			if (renderFillEnabled == 1) {
				glutWireCube(1.0f);
			}
			else {
				glutSolidCube(1.0f);
			}
		glPopMatrix();

		glMaterialfv(GL_FRONT, GL_AMBIENT, helicopterBodyA);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, helicopterBodyD);
		glMaterialfv(GL_FRONT, GL_SPECULAR, helicopterBodyS);
		glMaterialf(GL_FRONT, GL_SHININESS, helicopterBodyShine);

		//back wing top
		glPushMatrix();
			glTranslatef(0.0f, -2.0f, 2.4f);
			glRotatef(80.0f, 1.0f, 0.0f, 0.0f);
			glScalef(0.5f, 0.8f, 1.5f);
			if (renderFillEnabled == 1) {
				glutWireCube(2.0f);
			}
			else {
				glutSolidCube(2.0f);
			}

		glPopMatrix();

		//back wing bottom
		glPushMatrix();
			glTranslatef(0.0f, 1.0f, 2.3f);
			glRotatef(-80.0f, 1.0f, 0.0f, 0.0f);
			glScalef(0.5f, 0.8f, 1.0f);
			if (renderFillEnabled == 1) {
				glutWireCube(2.0f);
			}
			else {
				glutSolidCube(2.0f);
			}
		glPopMatrix();
		glPopMatrix();
	glPopMatrix();

	//bottom rails left
	glMaterialfv(GL_FRONT, GL_AMBIENT, rotorA);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, rotorD);
	glMaterialfv(GL_FRONT, GL_SPECULAR, rotorS);
	glMaterialf(GL_FRONT, GL_SHININESS, rotorShine);

	glPushMatrix();
		glTranslatef(2.5f, -5.3f, 2.0f);
		glRotatef(15.0f, 0.0f, 0.0f, 1.0f);
		glScalef(0.1f, 0.5f, 0.1f);
		if (renderFillEnabled == 1) {
			glutWireCube(6.0f);
		}
		else {
			glutSolidCube(6.0f);
		}

		glTranslatef(0.0f, 0.0f, -40.0f);
		if (renderFillEnabled == 1) {
			glutWireCube(6.0f);
		}
		else {
			glutSolidCube(6.0f);
		}

		glPushMatrix();
			glTranslatef(0.0f, -3.0f, 20.0f);
			glScalef(2.0f, 0.4f, 10.0f);
			if (renderFillEnabled == 1) {
				glutWireCube(6.0f);
			}
			else {
				glutSolidCube(6.0f);
			}
		glPopMatrix();

	glPopMatrix();



	//back rails right
	glPushMatrix();
		glTranslatef(-2.5f, -5.3f, 2.0f);
		glRotatef(-15.0f, 0.0f, 0.0f, 1.0f);
		glScalef(0.1f, 0.5f, 0.1f);
		if (renderFillEnabled == 1) {
			glutWireCube(6.0f);
		}
		else {
			glutSolidCube(6.0f);
		}

		glTranslatef(0.0f, 0.0f, -40.0f);
		if (renderFillEnabled == 1) {
			glutWireCube(6.0f);
		}
		else {
			glutSolidCube(6.0f);
		}

		glPushMatrix();
			glTranslatef(0.0f, -3.0f, 20.0f);
			glScalef(2.0f, 0.4f, 10.0f);
			if (renderFillEnabled == 1) {
				glutWireCube(6.0f);
			}
			else {
				glutSolidCube(6.0f);
			}
		glPopMatrix();

	glPopMatrix();


	glPopMatrix();

	// 1 GLU unit == 1 meter
}

void drawGrid(int gridSize, float squareSize) {
	glPushMatrix();
	glMaterialfv(GL_FRONT, GL_AMBIENT, groundA);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, groundD);
	glMaterialfv(GL_FRONT, GL_SPECULAR, groundS);
	glMaterialf(GL_FRONT, GL_SHININESS, groundShine);


	if (renderFillEnabled == 1) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	else {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	int gridLimit = (int)(gridSize / 2);

	GLfloat normal[] = { 0.0f, 1.0f, 0.0f };

	for (int i = -gridLimit; i < gridLimit; ++i) {
		for (int j = -gridLimit; j < gridLimit; ++j) {
			glBegin(GL_QUADS);
			glNormal3fv(normal);
			glVertex3f(i * squareSize, 0.0f, j * squareSize);
			glVertex3f((i + 1) * squareSize, 0.0f, j * squareSize);
			glVertex3f((i + 1) * squareSize, 0.0f, (j + 1) * squareSize);
			glVertex3f(i * squareSize, 0.0f, (j + 1) * squareSize);
			glEnd();
		}
	}

	glPopMatrix();
}

void updateCam(void) {
	float camDist = 20.0f;
	float camHeight = 6.0f;

	cam[0] = (float) (hPos[0] - sin(hYaw * (PI / 180.0f)) * camDist);
	cam[1] = hPos[1] + camHeight;
	cam[2] = (float) (hPos[2] - cos(hYaw * (PI / 180.0f)) * camDist);
}

void close(void) {
	freeMeshObject(treeLeavesMesh);
	freeMeshObject(treeTrunkMesh);
	freeMeshObject(duckMesh);
	freeMeshObject(logMesh);
	freeMeshObject(lampMesh);
	freeMeshObject(lampPostMesh);
}

meshObject* loadMeshObject(char* fileName)
{
	FILE* inFile;
	meshObject* object;
	char line[512];
	char keyword[10];
	int currentVertexIndex = 0;
	int currentTexCoordIndex = 0;
	int currentNormalIndex = 0;
	int currentFaceIndex = 0;

	inFile = fopen(fileName, "r");

	if (inFile == NULL) {
		return NULL;
	}

	object = malloc(sizeof(meshObject));
	object->vertexCount = 0;
	object->vertices = NULL;
	object->texCoordCount = 0;
	object->texCoords = NULL;
	object->normalCount = 0;
	object->normals = NULL;
	object->faceCount = 0;
	object->faces = NULL;

	while (fgets(line, (unsigned)_countof(line), inFile))
	{
		if (sscanf_s(line, "%9s", keyword, (unsigned)_countof(keyword)) == 1) {
			if (strcmp(keyword, "v") == 0) {
				object->vertexCount++;
			}
			else if (strcmp(keyword, "vt") == 0) {
				object->texCoordCount++;
			}
			else if (strcmp(keyword, "vn") == 0) {
				object->normalCount++;
			}
			else if (strcmp(keyword, "f") == 0) {
				object->faceCount++;
			}
		}
	}

	if (object->vertexCount > 0) object->vertices = malloc(sizeof(vec3d) * object->vertexCount);
	if (object->texCoordCount > 0) object->texCoords = malloc(sizeof(vec2d) * object->texCoordCount);
	if (object->normalCount > 0) object->normals = malloc(sizeof(vec3d) * object->normalCount);
	if (object->faceCount > 0) object->faces = malloc(sizeof(meshObjectFace) * object->faceCount);

	rewind(inFile);

	while (fgets(line, (unsigned)_countof(line), inFile))
	{
		if (sscanf_s(line, "%9s", keyword, (unsigned)_countof(keyword)) == 1) {
			if (strcmp(keyword, "v") == 0) {
				vec3d vertex = { 0, 0, 0 };
				sscanf_s(line, "%*s %f %f %f", &vertex.x, &vertex.y, &vertex.z);
				memcpy_s(&object->vertices[currentVertexIndex], sizeof(vec3d), &vertex, sizeof(vec3d));
				currentVertexIndex++;
			}
			else if (strcmp(keyword, "vt") == 0) {
				vec2d texCoord = { 0, 0 };
				sscanf_s(line, "%*s %f %f", &texCoord.x, &texCoord.y);
				memcpy_s(&object->texCoords[currentTexCoordIndex], sizeof(vec2d), &texCoord, sizeof(vec2d));
				currentTexCoordIndex++;
			}
			else if (strcmp(keyword, "vn") == 0) {
				vec3d normal = { 0, 0, 0 };
				sscanf_s(line, "%*s %f %f %f", &normal.x, &normal.y, &normal.z);
				memcpy_s(&object->normals[currentNormalIndex], sizeof(vec3d), &normal, sizeof(vec3d));
				currentNormalIndex++;
			}
			else if (strcmp(keyword, "f") == 0) {
				initMeshObjectFace(&(object->faces[currentFaceIndex]), line, _countof(line));
				currentFaceIndex++;
			}
		}
	}

	fclose(inFile);

	return object;
}

/*
	Render the faces of the specified Mesh Object in OpenGL.
*/
void renderMeshObject(meshObject* object, float scale, float x, float y, float z, float angle) {
	glPushMatrix();
	glTranslatef(x, y, z);
	glRotatef(angle, 0.0f, 1.0f, 0.0f);
	glScalef(scale, scale, scale);

	for (int faceNo = 0; faceNo < object->faceCount; faceNo++) {
		meshObjectFace face = object->faces[faceNo];
		if (face.pointCount >= 3) {
			glBegin(GL_POLYGON);

			for (int pointNo = 0; pointNo < face.pointCount; pointNo++) {
				meshObjectFacePoint point = face.points[pointNo];

				if (point.normalIndex >= 0) {
					vec3d normal = object->normals[point.normalIndex];
					glNormal3d(normal.x, normal.y, normal.z);
				}

				if (point.texCoordIndex >= 0) {
					vec2d texCoord = object->texCoords[point.texCoordIndex];
					glTexCoord2d(texCoord.x, texCoord.y);
				}

				vec3d vertex = object->vertices[point.vertexIndex];
				glVertex3f(vertex.x, vertex.y, vertex.z);
			}

			glEnd();
		}
	}

	glPopMatrix();
}

/*
	Initialise the specified Mesh Object Face from a string of face data in the Wavefront OBJ file format.
*/
void initMeshObjectFace(meshObjectFace* face, char* faceData, int maxFaceDataLength) {
	int maxPoints = 0;
	int inWhitespace = 0;
	const char* delimiter = " ";
	char* token = NULL;
	char* context = NULL;

	for (int i = 0; i < maxFaceDataLength; i++)
	{
		char c = faceData[i];
		if (c == '\0') {
			break;
		}
		else if ((c == ' ') || (c == '\t')) {
			if (!inWhitespace) {
				inWhitespace = 1;
				maxPoints++;
			}
		}
		else {
			inWhitespace = 0;
		}
	}

	face->pointCount = 0;
	if (maxPoints > 0) {
		face->points = malloc(sizeof(meshObjectFacePoint) * maxPoints);

		token = strtok_s(faceData, delimiter, &context);
		while ((token != NULL) && (face->pointCount < maxPoints)) {
			meshObjectFacePoint parsedPoint = { 0, 0, 0 };

			if (strcmp(token, "f") != 0) {
				if (sscanf_s(token, "%d/%d/%d", &parsedPoint.vertexIndex, &parsedPoint.texCoordIndex, &parsedPoint.normalIndex) < 2) {
					sscanf_s(token, "%d//%d", &parsedPoint.vertexIndex, &parsedPoint.normalIndex);
				}

				if (parsedPoint.vertexIndex > 0) {
					parsedPoint.vertexIndex--;
					parsedPoint.texCoordIndex = (parsedPoint.texCoordIndex > 0) ? parsedPoint.texCoordIndex - 1 : -1;
					parsedPoint.normalIndex = (parsedPoint.normalIndex > 0) ? parsedPoint.normalIndex - 1 : -1;

					memcpy_s(&face->points[face->pointCount], sizeof(meshObjectFacePoint), &parsedPoint, sizeof(meshObjectFacePoint));
					face->pointCount++;
				}
			}

			token = strtok_s(NULL, delimiter, &context);
		}

		if (face->pointCount == 0) {
			free(face->points);
			face->points = NULL;
		}
	}
}

void freeMeshObject(meshObject* object)
{
	if (object != NULL) {
		free(object->vertices);
		free(object->texCoords);
		free(object->normals);

		if (object->faces != NULL) {
			for (int i = 0; i < object->faceCount; i++) {
				free(object->faces[i].points);
			}

			free(object->faces);
		}

		free(object);
	}
}

void drawOval(float width, float length, float x, float y) {
	glPushMatrix();
	glMaterialfv(GL_FRONT, GL_AMBIENT, white);
	glMaterialfv(GL_FRONT, GL_SPECULAR, pondS);
	glMaterialf(GL_FRONT, GL_SHININESS, pondShine);
	glTranslatef(x, y, 0.0f);
	glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
	glRotatef(-26.0f, 0.0f, 0.0f, 1.0f);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, textureWater);

	if (renderFillEnabled == 1) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	else {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	glBegin(GL_TRIANGLE_FAN);
	glNormal3f(0.0f, 0.0f, 1.0f);
	glTexCoord2f(0.5f, 0.5f);
	glVertex2f(x, y);

	for (int i = 0; i <= 30; i++) {
		float angle = (float)((i * 12) * degToRad);
		GLfloat tx = (GLfloat)(cos(angle) * width / imageWidth) + 0.5f;
		GLfloat ty = (GLfloat)(sin(angle) * length / imageHeight) + 0.5f;
		glTexCoord2f(tx, ty);
		glVertex2f((GLfloat)(x + cos(angle) * width), (GLfloat)(y + sin(angle) * length));
	}

	glEnd();

	glDisable(GL_TEXTURE_2D);

	glPopMatrix();
}

void loadImage(FILE* fileID)
{
	int  maxValue;
	int  totalPixels;
	char tempChar;
	int i;
	char headerLine[100];
	float RGBScaling;
	int red, green, blue;
	fscanf(fileID, "%[^\n] ", headerLine);

	if ((headerLine[0] != 'P') || (headerLine[1] != '3'))
	{
		printf("This is not a PPM file!\n");
		exit(0);
	}

	fscanf(fileID, "%c", &tempChar);

	while (tempChar == '#')
	{
		fscanf(fileID, "%[^\n] ", headerLine);

		fscanf(fileID, "%c", &tempChar);
	}

	ungetc(tempChar, fileID);

	fscanf(fileID, "%d %d %d", &imageWidth, &imageHeight, &maxValue);

	totalPixels = imageWidth * imageHeight;

	imageData = malloc(3 * sizeof(GLuint) * totalPixels);

	RGBScaling = (float) 255.0 / maxValue;

	if (maxValue == 255)
	{
		for (i = 0; i < totalPixels; i++)
		{
			fscanf(fileID, "%d %d %d", &red, &green, &blue);

			imageData[3 * totalPixels - 3 * i - 3] = red;
			imageData[3 * totalPixels - 3 * i - 2] = green;
			imageData[3 * totalPixels - 3 * i - 1] = blue;
		}
	}
	else
	{
		for (i = 0; i < totalPixels; i++)
		{
			fscanf(fileID, "%d %d %d", &red, &green, &blue);

			imageData[3 * totalPixels - 3 * i - 3] = (GLubyte)(red * RGBScaling);
			imageData[3 * totalPixels - 3 * i - 2] = (GLubyte)(green * RGBScaling);
			imageData[3 * totalPixels - 3 * i - 1] = (GLubyte)(blue * RGBScaling);
		}
	}

	fclose(fileID);
}

void createWaterTexture(void)
{
	glBindTexture(GL_TEXTURE_2D, textureWater);

	GLubyte* myTexture = (GLubyte*)malloc(seaW * seaH * 3 * sizeof(GLubyte));
	if (myTexture == NULL) {
		printf("Memory allocation failed!\n");
		return;
	}

	int s, t;
	for (s = 0; s < seaW; s++) {
		for (t = 0; t < seaH; t++) {
			int i = t * seaW + s;
			int totalPixels = seaW * seaH;
			myTexture[(s * seaH + t) * 3 + 0] = imageData[3 * totalPixels - 3 * i - 3];
			myTexture[(s * seaH + t) * 3 + 1] = imageData[3 * totalPixels - 3 * i - 2];
			myTexture[(s * seaH + t) * 3 + 2] = imageData[3 * totalPixels - 3 * i - 1];
		}
	}

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	gluBuild2DMipmaps(GL_TEXTURE_2D, 3, seaW, seaH, GL_RGB, GL_UNSIGNED_BYTE, myTexture);

	free(myTexture);
}

void createLeavesTexture(void)
{
	glBindTexture(GL_TEXTURE_2D, textureLeaves);
	GLubyte* myTexture = (GLubyte*)malloc(leafW * leafH * 3 * sizeof(GLubyte));
	if (myTexture == NULL) {
		printf("Memory allocation failed!\n");
		return;
	}

	int s, t;
	for (s = 0; s < leafW; s++) {
		for (t = 0; t < leafH; t++) {
			int i = t * leafW + s;
			int totalPixels = leafW * leafH;
			myTexture[(s * leafH + t) * 3 + 0] = imageData[3 * totalPixels - 3 * i - 3];
			myTexture[(s * leafH + t) * 3 + 1] = imageData[3 * totalPixels - 3 * i - 2];
			myTexture[(s * leafH + t) * 3 + 2] = imageData[3 * totalPixels - 3 * i - 1];
		}
	}

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	gluBuild2DMipmaps(GL_TEXTURE_2D, 3, leafW, leafH, GL_RGB, GL_UNSIGNED_BYTE, myTexture);

	free(myTexture);
}

void createWoodTexture(void)
{
	glBindTexture(GL_TEXTURE_2D, textureWood);

	GLubyte* myTexture = (GLubyte*)malloc(woodW * woodH * 3 * sizeof(GLubyte));
	if (myTexture == NULL) {
		printf("Memory allocation failed!\n");
		return;
	}

	int s, t;
	for (s = 0; s < woodW; s++) {
		for (t = 0; t < woodH; t++) {
			int i = t * woodW + s;
			int totalPixels = woodW * woodH;
			myTexture[(s * woodH + t) * 3 + 0] = imageData[3 * totalPixels - 3 * i - 3];
			myTexture[(s * woodH + t) * 3 + 1] = imageData[3 * totalPixels - 3 * i - 2];
			myTexture[(s * woodH + t) * 3 + 2] = imageData[3 * totalPixels - 3 * i - 1];
		}
	}

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	// mipmaps
	gluBuild2DMipmaps(GL_TEXTURE_2D, 3, woodW, woodH, GL_RGB, GL_UNSIGNED_BYTE, myTexture);

	free(myTexture);
}

void drawEnvironment(void) {
	glPushMatrix();
	drawRocks(rockDL);
	drawLogs(logDL);
	drawDuckAnimation();
	drawForest(treeDL);
	drawWoodBench(30.0f, 0.5f, -13.0f, 7.0f);
	drawOval(10.0f, 5.0f, 15.0f, 0.03f);
	drawLamp();
	glMaterialfv(GL_FRONT, GL_EMISSION, emissionNormal);
	glPopMatrix();
}

void drawLamp(void) {
	glPushMatrix();
	glMaterialfv(GL_FRONT, GL_AMBIENT, lampBaseA);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, lampBaseD);
	glMaterialfv(GL_FRONT, GL_SPECULAR, lampBaseS);
	glMaterialf(GL_FRONT, GL_SHININESS, lampBaseShine);
	renderMeshObject(lampPostMesh, 0.3f, 27.0f, 0.0f, -13.0f, 0.0f);
	glMaterialfv(GL_FRONT, GL_AMBIENT, lampA);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, lampD);
	glMaterialfv(GL_FRONT, GL_SPECULAR, lampS);
	glMaterialf(GL_FRONT, GL_SHININESS, lampShine);
	glMaterialfv(GL_FRONT, GL_EMISSION, lightEmission);
	renderMeshObject(lampMesh, 0.3f, 27.0f, 0.0f, -13.0f, 0.0f);
	glPopMatrix();
}

void drawDuckAnimation(void) {
	glPushMatrix();
	glMaterialfv(GL_FRONT, GL_AMBIENT, duckColourA);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, duckColourD);
	glMaterialfv(GL_FRONT, GL_SPECULAR, duckColourS);
	glMaterialf(GL_FRONT, GL_SHININESS, duckShine);

	renderMeshObject(duckMesh, 0.2f, duckX, duckY, duckZ, duckAngle);
	glPopMatrix();
}

void createTreeDisplayList(void) {
	treeDL = glGenLists(1);
	glNewList(treeDL, GL_COMPILE);
	drawTree();
	glEndList();
}

void drawTree(void) {
	glPushMatrix();
	glMaterialfv(GL_FRONT, GL_AMBIENT, treeTrunkA);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, treeTrunkD);
	glMaterialfv(GL_FRONT, GL_SPECULAR, treeTrunkS);
	glMaterialf(GL_FRONT, GL_SHININESS, treeTrunkShine);
	glNormal3f(0.0f, 1.0f, 0.0f);
	renderMeshObject(treeTrunkMesh, 0.5f, 9.0f, 0.0f, 10.0f, 0.0f);
	glMaterialfv(GL_FRONT, GL_AMBIENT, treeLeavesA);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, treeLeavesD);
	glMaterialfv(GL_FRONT, GL_SPECULAR, treeLeavesS);
	glMaterialf(GL_FRONT, GL_SHININESS, treeLeafShine);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, textureLeaves);
	glNormal3f(0.0f, 1.0f, 0.0f);
	renderMeshObject(treeLeavesMesh, 0.5f, 9.0f, 0.0f, 10.0f, 0.0f);
	glDisable(GL_TEXTURE_2D);
	glPopMatrix();
}

void drawForest(GLuint tree) {
	glPushMatrix();
	glTranslatef(-5.0f, 0.0f, -25.0f);
	glRotatef(45, 0.0f, 1.0f, 0.0f);
	glCallList(tree);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-12.0f, 0.0f, 10.0f);
	glRotatef(30, 0.0f, 1.0f, 0.0f);
	glCallList(tree);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-20.0f, 0.0f, -35.0f);
	glRotatef(60, 0.0f, 1.0f, 0.0f);
	glCallList(tree);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-35.0f, 0.0f, -40.0f);
	glRotatef(300, 0.0f, 1.0f, 0.0f);
	glCallList(tree);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(28.0f, 0.0f, -17.0f);
	glRotatef(120, 0.0f, 1.0f, 0.0f);
	glCallList(tree);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(24.0f, 0.0f, 5.0f);
	glRotatef(150, 0.0f, 1.0f, 0.0f);
	glCallList(tree);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(14.0f, 0.0f, 19.0f);
	glRotatef(10, 0.0f, 1.0f, 0.0f);
	glCallList(tree);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-20.0f, 0.0f, 20.0f);
	glRotatef(234, 0.0f, 1.0f, 0.0f);
	glCallList(tree);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-28.0f, 0.0f, 0.0f);
	glRotatef(84, 0.0f, 1.0f, 0.0f);
	glCallList(tree);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-28.0f, 0.0f, 30.0f);
	glRotatef(10, 0.0f, 1.0f, 0.0f);
	glCallList(tree);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(26.0f, 0.0f, 15.0f);
	glRotatef(67, 0.0f, 1.0f, 0.0f);
	glCallList(tree);
	glPopMatrix();
}

void drawWoodBench(float x, float y, float z, float angle) {
	glPushMatrix();	
	glTranslatef(x, y, z);
	glRotatef(angle, 0.0f, 1.0f, 0.0f);
	glScalef(2.5f, 0.2f, 1.0f);
	drawTexturedCube();

	glScalef(1.0f / 2.5f, 1.0f / 0.2f, 1.0f);

		glPushMatrix();
		glTranslatef(0.0f, 0.5f, -0.5f);
		glRotatef(80.0f, 1.0f, 0.0f, 0.0f);
		glScalef(2.5f, 0.2f, 1.0f);
		drawTexturedCube();
		glPopMatrix();

		glPushMatrix();
		glTranslatef(1.1f, -0.3f, 0.0f);
		glScalef(0.2f, 0.6f, 0.5f);
		drawTexturedCube();
		glPopMatrix();

		glPushMatrix();
		glTranslatef(-1.1f, -0.3f, 0.0f);
		glScalef(0.2f, 0.6f, 0.5f);
		drawTexturedCube();
		glPopMatrix();
	glPopMatrix();

}

void drawTexturedCube(void) {
	if (renderFillEnabled == 1) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	else {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	glPushMatrix();

	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, white);
	glMaterialfv(GL_FRONT, GL_SPECULAR, benchS);
	glMaterialf(GL_FRONT, GL_SHININESS, benchShine);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, textureWood);

	glMatrixMode(GL_TEXTURE);
	glLoadIdentity();
	glRotatef(90.0f, 0.0f, 0.0f, 1.0f);
	glMatrixMode(GL_MODELVIEW);

	glBegin(GL_QUADS);

	// Front face
	GLfloat frontNormal[] = { 0.0f, 0.0f, 1.0f };
	glNormal3fv(frontNormal);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-0.5f, -0.5f, 0.5f);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(0.5f, -0.5f, 0.5f);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(0.5f, 0.5f, 0.5f);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(-0.5f, 0.5f, 0.5f);

	// Back face
	GLfloat backNormal[] = { 0.0f, 0.0f, -1.0f };
	glNormal3fv(backNormal);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-0.5f, -0.5f, -0.5f);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(0.5f, -0.5f, -0.5f);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(0.5f, 0.5f, -0.5f);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(-0.5f, 0.5f, -0.5f);

	// Top face
	GLfloat topNormal[] = { 0.0f, 1.0f, 0.0f };
	glNormal3fv(topNormal);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-0.5f, 0.5f, -0.5f);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(0.5f, 0.5f, -0.5f);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(0.5f, 0.5f, 0.5f);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(-0.5f, 0.5f, 0.5f);

	// Bottom face
	GLfloat bottomNormal[] = { 0.0f, -1.0f, 0.0f };
	glNormal3fv(bottomNormal);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-0.5f, -0.5f, -0.5f);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(0.5f, -0.5f, -0.5f);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(0.5f, -0.5f, 0.5f);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(-0.5f, -0.5f, 0.5f);

	// Right face
	GLfloat rightNormal[] = { 1.0f, 0.0f, 0.0f };
	glNormal3fv(rightNormal);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(0.5f, -0.5f, -0.5f);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(0.5f, -0.5f, 0.5f);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(0.5f, 0.5f, 0.5f);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(0.5f, 0.5f, -0.5f);

	// Left face
	GLfloat leftNormal[] = { -1.0f, 0.0f, 0.0f };
	glNormal3fv(leftNormal);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-0.5f, -0.5f, -0.5f);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(-0.5f, -0.5f, 0.5f);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(-0.5f, 0.5f, 0.5f);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(-0.5f, 0.5f, -0.5f);


	glEnd();

	glMatrixMode(GL_TEXTURE);
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);

	glPopMatrix();
}

void drawRock(float x, float y, float z, float angle, float scale, GLfloat A[], GLfloat D[], GLfloat S[], GLfloat Shine) {
	glPushMatrix();

	glMaterialfv(GL_FRONT, GL_AMBIENT, A);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, D);
	glMaterialfv(GL_FRONT, GL_SPECULAR, S);
	glMaterialf(GL_FRONT, GL_SHININESS, Shine);

	glTranslatef(x, y, z);
	glRotatef(angle, 0.0f, 1.0f, 0.0f);
	glScalef(scale, scale, scale);

	glBegin(GL_QUADS);

	GLfloat frontNormal[] = { 0.0f, 0.0f, 1.0f };
	glNormal3fv(frontNormal);
	// front
	glVertex3f(-0.5f, -0.3f, 0.5f);
	glVertex3f(0.5f, -0.3f, 0.5f);
	glVertex3f(0.4f, 0.3f, 0.4f);
	glVertex3f(-0.4f, 0.2f, 0.4f);

	// back
	GLfloat backNormal[] = { 0.0f, 0.0f, -1.0f };
	glNormal3fv(backNormal);
	glVertex3f(-0.5f, -0.3f, -0.5f);
	glVertex3f(0.5f, -0.3f, -0.5f);
	glVertex3f(0.4f, 0.3f, -0.4f);
	glVertex3f(-0.4f, 0.2f, -0.4f);

	// left
	GLfloat leftNormal[] = { -1.0f, 0.0f, 0.0f };
	glNormal3fv(leftNormal);
	glVertex3f(-0.5f, -0.3f, -0.5f);
	glVertex3f(-0.5f, -0.3f, 0.5f);
	glVertex3f(-0.4f, 0.2f, 0.4f);
	glVertex3f(-0.4f, 0.2f, -0.4f);

	// right
	GLfloat rightNormal[] = { 1.0f, 0.0f, 0.0f };
	glNormal3fv(rightNormal);
	glVertex3f(0.5f, -0.3f, -0.5f);
	glVertex3f(0.5f, -0.3f, 0.5f);
	glVertex3f(0.4f, 0.3f, 0.4f);
	glVertex3f(0.4f, 0.3f, -0.4f);

	// top
	GLfloat topNormal[] = { 0.0f, 1.0f, 0.0f };
	glNormal3fv(topNormal);
	glVertex3f(-0.4f, 0.2f, 0.4f);
	glVertex3f(0.4f, 0.3f, 0.4f);
	glVertex3f(0.4f, 0.3f, -0.4f);
	glVertex3f(-0.4f, 0.2f, -0.4f);

	// bottom
	GLfloat bottomNormal[] = { 0.0f, -1.0f, 0.0f };
	glNormal3fv(bottomNormal);
	glVertex3f(-0.5f, -0.3f, 0.5f);
	glVertex3f(0.5f, -0.3f, 0.5f);
	glVertex3f(0.5f, -0.3f, -0.5f);
	glVertex3f(-0.5f, -0.3f, -0.5f);

	glEnd();

	glPopMatrix();
}

void createRockDisplayList(void) {
	rockDL = glGenLists(1);
	glNewList(rockDL, GL_COMPILE);
	drawRock(0.0f, 0.3f, 0.0f, 0.0f, 1.0f, rock1A, rock1D, rock1S, rock1Shine);
	drawRock(10.0f, 0.3f, -25.0f, 65.0f, 1.2f, rock2A, rock2D, rock2S, rock2Shine);
	glEndList();
}

void drawRocks(GLuint rock) {
	glPushMatrix();
	glTranslatef(-5.0f, 0.0f, 0.0f);
	glRotatef(45, 0.0f, 1.0f, 0.0f);
	glCallList(rock);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(15.0f, 0.0f, 0.0f);
	glRotatef(-30, 0.0f, 1.0f, 0.0f);
	glCallList(rock);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(27.0f, 0.0f, 10.0f);
	glRotatef(180, 0.0f, 1.0f, 0.0f);
	glCallList(rock);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-20.0f, 0.0f, 10.0f);
	glRotatef(150.0, 0.0f, 1.0f, 0.0f);
	glCallList(rock);
	glPopMatrix();
}

void drawLog(void) {
	glPushMatrix();
	glMaterialfv(GL_FRONT, GL_AMBIENT, logColourA);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, logColourD);
	glMaterialfv(GL_FRONT, GL_SPECULAR, logColourS);
	glMaterialf(GL_FRONT, GL_SHININESS, logShine);
	GLfloat normal[] = { 0.0f, 1.0f, 0.0f };
	glNormal3fv(normal);
	renderMeshObject(logMesh, 6.0f, 0.0f, 0.0f, -0.4f, 0.0f);
	glPopMatrix();
}

void createLogDisplayList(void) {
	logDL = glGenLists(1);
	glNewList(logDL, GL_COMPILE);
	drawLog();
	glEndList();
}

void drawLogs(GLuint log) {
	glPushMatrix();
	glTranslatef(10.0f, 0.0f, 6.0f);
	glRotatef(90.0f, 1.0f, 0.0f, 1.0f);
	glCallList(log);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-28.0f, 0.0f, -25.0f);
	glRotatef(90.0f, 1.0f, 0.0f, -1.0f);
	glCallList(log);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-23.0f, 0.0f, 1.0f);
	glRotatef(90.0f, 1.0f, 0.0f, 0.5f);
	glCallList(log);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-20.0f, 0.0f, 24.0f);
	glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
	glCallList(log);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(28.0f, 0.0f, -22.0f);
	glRotatef(90.0f, 1.0f, 0.0f, -0.4f);
	glCallList(log);
	glPopMatrix();
}
/******************************************************************************/
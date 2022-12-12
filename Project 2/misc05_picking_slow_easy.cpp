// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <array>
#include <stack>   
#include <sstream>
// Include GLEW
#include <GL/glew.h>
// Include GLFW
#include <GLFW/glfw3.h>
// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
using namespace glm;
// Include AntTweakBar
#include <AntTweakBar.h>

#include <common/shader.hpp>
#include <common/controls.hpp>
#include <common/objloader.hpp>
#include <common/vboindexer.hpp>

#include <iostream>

#define M_PI 3.14159265358979323846

const int window_width = 1024, window_height = 768;

typedef struct Vertex {
	float Position[4];
	float Color[4];
	float Normal[3];
	void SetPosition(float* coords) {
		Position[0] = coords[0];
		Position[1] = coords[1];
		Position[2] = coords[2];
		Position[3] = 1.0;
	}
	void SetColor(float* color) {
		Color[0] = color[0];
		Color[1] = color[1];
		Color[2] = color[2];
		Color[3] = color[3];
	}
	void SetNormal(float* coords) {
		Normal[0] = coords[0];
		Normal[1] = coords[1];
		Normal[2] = coords[2];
	}
};

// function prototypes
int initWindow(void);
void initOpenGL(void);
void createVAOs(Vertex[], GLushort[], int);
void loadObject(char*, glm::vec4, Vertex*&, GLushort*&, int);
void createObjects(void);
void pickObject(void);
void renderScene(void);
void cleanup(void);
void moveCamera(void);
void moveBase(void);
void moveTop(void);
void moveArm1(void);
void moveArm2(void);
void movePen(void);
void runProj(void);
static void keyCallback(GLFWwindow*, int, int, int, int);
static void mouseCallback(GLFWwindow*, int, int, int);


// GLOBAL VARIABLES
GLFWwindow* window;

glm::mat4 gProjectionMatrix;
glm::mat4 gViewMatrix;
glm::mat4 bMatrix;
glm::mat4 tMatrix;
glm::mat4 a1Matrix;
glm::mat4 a2Matrix;
glm::mat4 pMatrix;

glm::vec4 baseColorO = glm::vec4(1.0, 0.0, 0.0, 1.0);
glm::vec4 baseColorN = glm::vec4(1.0, 0.4, 0.4, 1.0);
glm::vec4 topColorO = glm::vec4(0.0, 0.8, 0.0, 1.0);
glm::vec4 topColorN = glm::vec4(0.4, 0.9, 0.4, 1.0);
glm::vec4 arm1ColorO = glm::vec4(0.0, 0.5, 1.0, 1.0);
glm::vec4 arm1ColorN = glm::vec4(0.4, 0.7, 1.0, 1.0);
glm::vec4 arm2ColorO = glm::vec4(0.0, 1.0, 1.0, 1.0);
glm::vec4 arm2ColorN = glm::vec4(0.4, 1.0, 1.0, 1.0);
glm::vec4 penColorO = glm::vec4(0.7, 0.7, 0.0, 1.0);
glm::vec4 penColorN = glm::vec4(0.8, 0.8, 0.4, 1.0);
glm::vec3 moveDistance = glm::vec3(0.0, 0.0, 0.0);
glm::vec3 moveDistanceA1 = glm::vec3(0.0, 1.55, 0.0);
glm::vec3 moveDistanceA2 = glm::vec3(0.0, 1.58, 2.28);
glm::vec3 moveDistanceP = glm::vec3(0.0, 0.58, 2.96);
glm::vec3 moveDistancePro = glm::vec3(0.0, 0.0, 0.0);

GLuint gPickedIndex = -1;
std::string gMessage;

GLuint programID;
GLuint pickingProgramID;

const GLuint NumObjects = 100;	// ATTN: THIS NEEDS TO CHANGE AS YOU ADD NEW OBJECTS
GLuint VertexArrayId[NumObjects];
GLuint VertexBufferId[NumObjects];
GLuint IndexBufferId[NumObjects];

bool keyUpAlreadyPressed = false;
bool keyDownAlreadyPressed = false;
bool keyLeftAlreadyPressed = false;
bool keyRightAlreadyPressed = false;
bool keyShiftAlreadyPressed = false;
bool cameraSelect = false;
bool penSelect = false;
bool topSelect = false;
bool baseSelect = false;
bool arm1Select = false;
bool arm2Select = false;
bool penRotate = false;
bool baseSelected = false;
bool topSelected = false;
bool arm1Selected = false;
bool arm2Selected = false;
bool penSelected = false;
bool projectileRun = false;
bool moveCameraLeft = false;
bool moveCameraRight = false;
bool moveCameraUp = false;
bool moveCameraDown = false;
bool movePenLeft = false;
bool movePenRight = false;
bool movePenUp = false;
bool movePenDown = false;
bool rotatePenLeft = false;
bool rotatePenRight = false;
bool rotateTopLeft = false;
bool rotateTopRight = false;
bool moveBaseLeft = false;
bool moveBaseRight = false;
bool moveBaseForward = false;
bool moveBaseBackward = false;
bool moveArm1Up = false;
bool moveArm1Down = false;
bool moveArm2Up = false;
bool moveArm2Down = false;


GLfloat cameraAngleT = M_PI / 4;
GLfloat cameraAngleP = asin(1 / (sqrt(3)));
GLfloat cameraSphereR = sqrt(300);
GLfloat tAngle = 0.0;
GLfloat a1Angle = 0.0;
GLfloat a2Angle = 0.0;
GLfloat pAngleH = 0.0;
GLfloat pAngleV = 0.0;
GLfloat pAngleS = 0.0;
Vertex* Base_Verts;
GLushort* Base_Idcs;
Vertex* Top_Verts;
GLushort* Top_Idcs;
Vertex* Arm1_Verts;
GLushort* Arm1_Idcs;
Vertex* Joint_Verts;
GLushort* Joint_Idcs;
Vertex* Arm2_Verts;
GLushort* Arm2_Idcs;
Vertex* Pen_Verts;
GLushort* Pen_Idcs;
Vertex* Button_Verts;
GLushort* Button_Idcs;
Vertex* Proj_Verts;
GLushort* Proj_Idcs;

// TL
size_t VertexBufferSize[NumObjects];
size_t IndexBufferSize[NumObjects];
size_t NumIdcs[NumObjects];
size_t NumVerts[NumObjects];

GLuint MatrixID;
GLuint ModelMatrixID;
GLuint ViewMatrixID;
GLuint ProjMatrixID;
GLuint PickingMatrixID;
GLuint pickingColorID;
GLuint LightID;
GLuint Light1ID;
GLuint Light2ID;

// Declare global objects
// TL
const size_t CoordVertsCount = 6;
int basePoint = 18;
int topPoint = 60;
int arm1Point = 24;
int jointPoint = 60;
int arm2Point = 192;
int penPoint = 72;
int butPoint = 24;
int projPoint = 2022;
float timer = 0.0;
float pickingColor[10];
Vertex CoordVerts[CoordVertsCount];
Vertex Grid[44];

int initWindow(void) {
	// Initialise GLFW
	if (!glfwInit()) {
		fprintf(stderr, "Failed to initialize GLFW\n");
		return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);	// FOR MAC

	// Open a window and create its OpenGL context
	window = glfwCreateWindow(window_width, window_height, "Haolan,Xu(3432-6768)", NULL, NULL);
	if (window == NULL) {
		fprintf(stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n");
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// Initialize GLEW
	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		return -1;
	}

	// Initialize the GUI
	TwInit(TW_OPENGL_CORE, NULL);
	TwWindowSize(window_width, window_height);
	TwBar* GUI = TwNewBar("Picking");
	TwSetParam(GUI, NULL, "refresh", TW_PARAM_CSTRING, 1, "0.1");
	TwAddVarRW(GUI, "Last picked object", TW_TYPE_STDSTRING, &gMessage, NULL);

	// Set up inputs
	glfwSetCursorPos(window, window_width / 2, window_height / 2);
	glfwSetKeyCallback(window, keyCallback);
	glfwSetMouseButtonCallback(window, mouseCallback);

	return 0;
}

void initOpenGL(void) {
	// Enable depth test
	glEnable(GL_DEPTH_TEST);
	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS);
	// Cull triangles which normal is not towards the camera
	glEnable(GL_CULL_FACE);

	// Projection matrix : 45?Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
	gProjectionMatrix = glm::perspective(45.0f, 4.0f / 3.0f, 0.1f, 100.0f);
	// Or, for an ortho camera :
	//gProjectionMatrix = glm::ortho(-4.0f, 4.0f, -3.0f, 3.0f, 0.0f, 100.0f); // In world coordinates

	// Camera matrix
	gViewMatrix = glm::lookAt(glm::vec3(10.0, 10.0, 10.0f),	// eye
		glm::vec3(0.0, 0.0, 0.0),	// center
		glm::vec3(0.0, 1.0, 0.0));	// up

	// Create and compile our GLSL program from the shaders
	programID = LoadShaders("StandardShading.vertexshader", "StandardShading.fragmentshader");
	pickingProgramID = LoadShaders("Picking.vertexshader", "Picking.fragmentshader");

	// Get a handle for our "MVP" uniform
	MatrixID = glGetUniformLocation(programID, "MVP");
	ModelMatrixID = glGetUniformLocation(programID, "M");
	ViewMatrixID = glGetUniformLocation(programID, "V");
	ProjMatrixID = glGetUniformLocation(programID, "P");

	PickingMatrixID = glGetUniformLocation(pickingProgramID, "MVP");
	// Get a handle for our "pickingColorID" uniform
	pickingColorID = glGetUniformLocation(pickingProgramID, "PickingColor");

	for (int i = 0; i < 10; i++)
	{
		pickingColor[i] = i / 255.0f;
	}

	// Get a handle for our "LightPosition" uniform
	LightID = glGetUniformLocation(programID, "LightPosition_worldspace");
	Light1ID = glGetUniformLocation(programID, "Light1Position_worldspace");
	Light2ID = glGetUniformLocation(programID, "Light2Position_worldspace");

	// TL
	// Define objects
	createObjects();

	// ATTN: create VAOs for each of the newly created objects here:
	VertexBufferSize[0] = sizeof(CoordVerts);
	VertexBufferSize[1] = sizeof(Grid);

	NumVerts[0] = CoordVertsCount;
	NumVerts[1] = 44;
	NumVerts[2] = NumIdcs[2];
	NumVerts[3] = NumIdcs[3];
	NumVerts[4] = NumIdcs[4];
	NumVerts[5] = NumIdcs[5];
	NumVerts[6] = NumIdcs[6];
	NumVerts[7] = NumIdcs[7];
	NumVerts[8] = NumIdcs[8];
	NumVerts[9] = NumIdcs[9];

	createVAOs(CoordVerts, NULL, 0);
	createVAOs(Grid, NULL, 1);

}

void createVAOs(Vertex Vertices[], unsigned short Indices[], int ObjectId) {
	GLenum ErrorCheckValue = glGetError();
	const size_t VertexSize = sizeof(Vertices[0]);
	const size_t RgbOffset = sizeof(Vertices[0].Position);
	const size_t Normaloffset = sizeof(Vertices[0].Color) + RgbOffset;

	// Create Vertex Array Object
	glGenVertexArrays(1, &VertexArrayId[ObjectId]);
	glBindVertexArray(VertexArrayId[ObjectId]);

	// Create Buffer for vertex data
	glGenBuffers(1, &VertexBufferId[ObjectId]);
	glBindBuffer(GL_ARRAY_BUFFER, VertexBufferId[ObjectId]);
	glBufferData(GL_ARRAY_BUFFER, VertexBufferSize[ObjectId], Vertices, GL_STATIC_DRAW);

	// Create Buffer for indices
	if (Indices != NULL) {
		glGenBuffers(1, &IndexBufferId[ObjectId]);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IndexBufferId[ObjectId]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, IndexBufferSize[ObjectId], Indices, GL_STATIC_DRAW);
	}

	// Assign vertex attributes
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, VertexSize, 0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, VertexSize, (GLvoid*)RgbOffset);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, VertexSize, (GLvoid*)Normaloffset);	// TL

	glEnableVertexAttribArray(0);	// position
	glEnableVertexAttribArray(1);	// color
	glEnableVertexAttribArray(2);	// normal

	// Disable our Vertex Buffer Object 
	glBindVertexArray(0);

	ErrorCheckValue = glGetError();
	if (ErrorCheckValue != GL_NO_ERROR)
	{
		fprintf(
			stderr,
			"ERROR: Could not create a VBO: %s \n",
			gluErrorString(ErrorCheckValue)
		);
	}
}

// Ensure your .obj files are in the correct format and properly loaded by looking at the following function
void loadObject(char* file, glm::vec4 color, Vertex*& out_Vertices, GLushort*& out_Indices, int ObjectId) {
	// Read our .obj file
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec2> uvs;
	std::vector<glm::vec3> normals;
	bool res = loadOBJ(file, vertices, normals);

	std::vector<GLushort> indices;
	std::vector<glm::vec3> indexed_vertices;
	std::vector<glm::vec2> indexed_uvs;
	std::vector<glm::vec3> indexed_normals;
	indexVBO(vertices, normals, indices, indexed_vertices, indexed_normals);

	const size_t vertCount = indexed_vertices.size();
	const size_t idxCount = indices.size();
	
	// populate output arrays
	out_Vertices = new Vertex[vertCount];
	for (int i = 0; i < vertCount; i++) {
		out_Vertices[i].SetPosition(&indexed_vertices[i].x);
		out_Vertices[i].SetNormal(&indexed_normals[i].x);
		out_Vertices[i].SetColor(&color[0]);
	}
	out_Indices = new GLushort[idxCount];
	for (int i = 0; i < idxCount; i++) {
		out_Indices[i] = indices[i];
	}

	// set global variables!!
	NumIdcs[ObjectId] = idxCount;
	VertexBufferSize[ObjectId] = sizeof(out_Vertices[0]) * vertCount;
	IndexBufferSize[ObjectId] = sizeof(GLushort) * idxCount;
}

void createObjects(void) {
	//-- COORDINATE AXES --//
	CoordVerts[0] = { { 0.0, 0.0, 0.0, 1.0 }, { 1.0, 0.0, 0.0, 1.0 }, { 0.0, 0.0, 1.0 } };
	CoordVerts[1] = { { 5.0, 0.0, 0.0, 1.0 }, { 1.0, 0.0, 0.0, 1.0 }, { 0.0, 0.0, 1.0 } };
	CoordVerts[2] = { { 0.0, 0.0, 0.0, 1.0 }, { 0.0, 1.0, 0.0, 1.0 }, { 0.0, 0.0, 1.0 } };
	CoordVerts[3] = { { 0.0, 5.0, 0.0, 1.0 }, { 0.0, 1.0, 0.0, 1.0 }, { 0.0, 0.0, 1.0 } };
	CoordVerts[4] = { { 0.0, 0.0, 0.0, 1.0 }, { 0.0, 0.0, 1.0, 1.0 }, { 0.0, 0.0, 1.0 } };
	CoordVerts[5] = { { 0.0, 0.0, 5.0, 1.0 }, { 0.0, 0.0, 1.0, 1.0 }, { 0.0, 0.0, 1.0 } };

	//-- GRID --//

	// ATTN: Create your grid vertices here!
	int k = 0;
	for (int i = -5; i <= 5; i++) {
		Grid[4 * k] = { {(float)i, 0.0, -5.0, 1.0}, {1.0, 1.0, 1.0, 1.0}, {0.0, 0.0, 1.0} };
		Grid[4 * k + 1] = { {(float)i, 0.0, 5.0, 1.0}, {1.0, 1.0, 1.0, 1.0}, {0.0, 0.0, 1.0} };
		Grid[4 * k + 2] = { {-5.0, 0.0, (float)i, 1.0}, {1.0, 1.0, 1.0, 1.0}, {0.0, 0.0, 1.0} };
		Grid[4 * k + 3] = { {5.0, 0.0, (float)i, 1.0}, {1.0, 1.0, 1.0, 1.0}, {0.0, 0.0, 1.0} };
		k++;
	}
	//-- .OBJs --//
	loadObject("Base.obj", glm::vec4(1.0, 0.0, 0.0, 1.0), Base_Verts, Base_Idcs, 2);
	createVAOs(Base_Verts, Base_Idcs, 2);

	loadObject("Top.obj", glm::vec4(0.0, 0.8, 0.0, 1.0), Top_Verts, Top_Idcs, 3);
	createVAOs(Top_Verts, Top_Idcs, 3);

	loadObject("Arm1.obj", glm::vec4(0.0, 0.5, 1.0, 1.0), Arm1_Verts, Arm1_Idcs, 4);
	createVAOs(Arm1_Verts, Arm1_Idcs, 4);

	loadObject("Joint.obj", glm::vec4(1.0, 0.6, 0.6, 1.0), Joint_Verts, Joint_Idcs, 5);
	createVAOs(Joint_Verts, Joint_Idcs, 5);

	loadObject("Arm2.obj", glm::vec4(0.0, 1.0, 1.0, 1.0), Arm2_Verts, Arm2_Idcs, 6);
	createVAOs(Arm2_Verts, Arm2_Idcs, 6);

	loadObject("Pen.obj", glm::vec4(0.7, 0.7, 0.0, 1.0), Pen_Verts, Pen_Idcs, 7);
	createVAOs(Pen_Verts, Pen_Idcs, 7);

	loadObject("Button.obj", glm::vec4(1.0, 0.0, 0.0, 1.0), Button_Verts, Button_Idcs, 8);
	createVAOs(Button_Verts, Button_Idcs, 8);

	loadObject("Projectile.obj", glm::vec4(1.0, 1.0, 1.0, 1.0), Proj_Verts, Proj_Idcs, 9);
	createVAOs(Proj_Verts, Proj_Idcs, 9);

	// ATTN: Load your models here through .obj files -- example of how to do so is as shown
	// Vertex* Verts;
	// GLushort* Idcs;
	// loadObject("models/base.obj", glm::vec4(1.0, 0.0, 0.0, 1.0), Verts, Idcs, ObjectID);
	// createVAOs(Verts, Idcs, ObjectID);
}

void pickObject(void) {
	// Clear the screen in white
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(pickingProgramID);
	{
		glm::mat4 ModelMatrix = glm::mat4(1.0); // TranslationMatrix * RotationMatrix;
		glm::mat4 MVP = gProjectionMatrix * gViewMatrix * ModelMatrix;

		// Send our transformation to the currently bound shader, in the "MVP" uniform
		glUniformMatrix4fv(PickingMatrixID, 1, GL_FALSE, &MVP[0][0]);

		// ATTN: DRAW YOUR PICKING SCENE HERE. REMEMBER TO SEND IN A DIFFERENT PICKING COLOR FOR EACH OBJECT BEFOREHAND
		MVP = gProjectionMatrix * gViewMatrix * bMatrix;
		glUniformMatrix4fv(PickingMatrixID, 1, GL_FALSE, &MVP[0][0]);
		glUniform1f(pickingColorID, pickingColor[2]);
		glBindVertexArray(VertexArrayId[2]);
		glDrawElements(GL_TRIANGLES, NumVerts[2], GL_UNSIGNED_SHORT, (void*)0);

		MVP = gProjectionMatrix * gViewMatrix * tMatrix;
		glUniformMatrix4fv(PickingMatrixID, 1, GL_FALSE, &MVP[0][0]);
		glUniform1f(pickingColorID, pickingColor[3]);
		glBindVertexArray(VertexArrayId[3]);
		glDrawElements(GL_TRIANGLES, NumVerts[3], GL_UNSIGNED_SHORT, (void*)0);

		MVP = gProjectionMatrix * gViewMatrix * a1Matrix;
		glUniformMatrix4fv(PickingMatrixID, 1, GL_FALSE, &MVP[0][0]);
		glUniform1f(pickingColorID, pickingColor[4]);
		glBindVertexArray(VertexArrayId[4]);
		glDrawElements(GL_TRIANGLES, NumVerts[4], GL_UNSIGNED_SHORT, (void*)0);

		MVP = gProjectionMatrix * gViewMatrix * a2Matrix;
		glUniformMatrix4fv(PickingMatrixID, 1, GL_FALSE, &MVP[0][0]);
		glUniform1f(pickingColorID, pickingColor[6]);
		glBindVertexArray(VertexArrayId[6]);
		glDrawElements(GL_TRIANGLES, NumVerts[6], GL_UNSIGNED_SHORT, (void*)0);

		MVP = gProjectionMatrix * gViewMatrix * pMatrix;
		glUniformMatrix4fv(PickingMatrixID, 1, GL_FALSE, &MVP[0][0]);
		glUniform1f(pickingColorID, pickingColor[7]);
		glBindVertexArray(VertexArrayId[7]);
		glDrawElements(GL_TRIANGLES, NumVerts[7], GL_UNSIGNED_SHORT, (void*)0);

	}
	glUseProgram(0);
	// Wait until all the pending drawing commands are really done.
	// Ultra-mega-over slow ! 
	// There are usually a long time between glDrawElements() and
	// all the fragments completely rasterized.
	glFlush();
	glFinish();

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	// Read the pixel at the center of the screen.
	// You can also use glfwGetMousePos().
	// Ultra-mega-over slow too, even for 1 pixel, 
	// because the framebuffer is on the GPU.
	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);
	unsigned char data[4];
	glReadPixels(xpos, window_height - ypos, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, data); // OpenGL renders with (0,0) on bottom, mouse reports with (0,0) on top

	// Convert the color back to an integer ID
	gPickedIndex = int(data[0]);

	if (gPickedIndex == 255) { // Full white, must be the background !
		gMessage = "background";
	}
	else {
		std::ostringstream oss;
		oss << "point " << gPickedIndex;
		gMessage = oss.str();
		if (gPickedIndex == 2) {
			if (baseSelect) {
				baseSelect = false;
			}
			else {
				baseSelect = true;
			}
		}
		if (gPickedIndex == 3) {
			if (topSelect) {
				topSelect = false;
			}
			else {
				topSelect = true;
			}
		}
		if (gPickedIndex == 4) {
			if (arm1Select) {
				arm1Select = false;
			}
			else {
				arm1Select = true;
			}
		}
		if (gPickedIndex == 6) {
			if (arm2Select) {
				arm2Select = false;
			}
			else {
				arm2Select = true;
			}
		}
		if (gPickedIndex == 7) {
			if (penSelect) {
				penSelect = false;
			}
			else {
				penSelect = true;
			}
		}
	}
	// Uncomment these lines to see the picking shader in effect
	//glfwSwapBuffers(window);
	//continue; // skips the normal rendering
}

void renderScene(void) {
	//ATTN: DRAW YOUR SCENE HERE. MODIFY/ADAPT WHERE NECESSARY!

	// Dark blue background
	glClearColor(0.0f, 0.0f, 0.2f, 0.0f);
	// Re-clear the screen for real rendering
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	moveCamera();

	glUseProgram(programID);
	{
		glm::vec3 lightPos = glm::vec3(4, 4, 4);
		glm::vec3 light1Pos = glm::vec3(5, 6, 12);
		glm::vec3 light2Pos = glm::vec3(12, 6, 5);

		glm::mat4x4 ModelMatrix = glm::mat4(1.0);

		glUniform3f(LightID, lightPos.x, lightPos.y, lightPos.z);
		glUniform3f(Light1ID, light1Pos.x, light1Pos.y, light1Pos.z);
		glUniform3f(Light2ID, light2Pos.x, light2Pos.y, light2Pos.z);

		glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &gViewMatrix[0][0]);
		glUniformMatrix4fv(ProjMatrixID, 1, GL_FALSE, &gProjectionMatrix[0][0]);
		glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);

		glBindVertexArray(VertexArrayId[0]);	// Draw CoordAxes
		glDrawArrays(GL_LINES, 0, NumVerts[0]);

		glBindVertexArray(VertexArrayId[1]);	// Draw Grid
		glDrawArrays(GL_LINES, 0, NumVerts[1]);

		// should use the glDrawElements
		moveBase();
		glBindVertexArray(VertexArrayId[2]);	// Draw Base
		glBindBuffer(GL_ARRAY_BUFFER, VertexBufferId[2]);
		glBufferSubData(GL_ARRAY_BUFFER, 0, VertexBufferSize[2], Base_Verts);
		glDrawElements(GL_TRIANGLES, NumVerts[2], GL_UNSIGNED_SHORT, (void*)0);

		moveTop();
		glBindVertexArray(VertexArrayId[3]);	// Draw Top
		glBindBuffer(GL_ARRAY_BUFFER, VertexBufferId[3]);
		glBufferSubData(GL_ARRAY_BUFFER, 0, VertexBufferSize[3], Top_Verts);
		glDrawElements(GL_TRIANGLES, NumVerts[3], GL_UNSIGNED_SHORT, (void*)0);

		moveArm1();
		glBindVertexArray(VertexArrayId[4]);	// Draw Arm1
		glBindBuffer(GL_ARRAY_BUFFER, VertexBufferId[4]);
		glBufferSubData(GL_ARRAY_BUFFER, 0, VertexBufferSize[4], Arm1_Verts);
		glDrawElements(GL_TRIANGLES, NumVerts[4], GL_UNSIGNED_SHORT, (void*)0);

		glBindVertexArray(VertexArrayId[5]);	// Draw Joint
		glBindBuffer(GL_ARRAY_BUFFER, VertexBufferId[5]);
		glBufferSubData(GL_ARRAY_BUFFER, 0, VertexBufferSize[5], Joint_Verts);
		glDrawElements(GL_TRIANGLES, NumVerts[5], GL_UNSIGNED_SHORT, (void*)0);

		moveArm2();
		glBindVertexArray(VertexArrayId[6]);	// Draw Arm2
		glBindBuffer(GL_ARRAY_BUFFER, VertexBufferId[6]);
		glBufferSubData(GL_ARRAY_BUFFER, 0, VertexBufferSize[6], Arm2_Verts);
		glDrawElements(GL_TRIANGLES, NumVerts[6], GL_UNSIGNED_SHORT, (void*)0);

		movePen();
		glBindVertexArray(VertexArrayId[7]);	// Draw Pen
		glBindBuffer(GL_ARRAY_BUFFER, VertexBufferId[7]);
		glBufferSubData(GL_ARRAY_BUFFER, 0, VertexBufferSize[7], Pen_Verts);
		glDrawElements(GL_TRIANGLES, NumVerts[7], GL_UNSIGNED_SHORT, (void*)0);

		glBindVertexArray(VertexArrayId[8]);	// Draw Button
		glBindBuffer(GL_ARRAY_BUFFER, VertexBufferId[8]);
		glBufferSubData(GL_ARRAY_BUFFER, 0, VertexBufferSize[8], Button_Verts);
		glDrawElements(GL_TRIANGLES, NumVerts[8], GL_UNSIGNED_SHORT, (void*)0);

		runProj(); // run Proj


		glBindVertexArray(0);
	}
	glUseProgram(0);
	// Draw GUI
	TwDraw();

	// Swap buffers
	glfwSwapBuffers(window);
	glfwPollEvents();
}

void cleanup(void) {
	// Cleanup VBO and shader
	for (int i = 0; i < NumObjects; i++) {
		glDeleteBuffers(1, &VertexBufferId[i]);
		glDeleteBuffers(1, &IndexBufferId[i]);
		glDeleteVertexArrays(1, &VertexArrayId[i]);
	}
	glDeleteProgram(programID);
	glDeleteProgram(pickingProgramID);

	// Close OpenGL window and terminate GLFW
	glfwTerminate();
}

// Alternative way of triggering functions on keyboard events
static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	// ATTN: MODIFY AS APPROPRIATE
	if (action == GLFW_PRESS) {
		switch (key)
		{
		case GLFW_KEY_1:
			if (arm1Select) {
				arm1Select = false;
			}
			else {
				penSelect = false;
				topSelect = false;
				baseSelect = false;
				arm2Select = false;
				cameraSelect = false;
				arm1Select = true;
			}
			break;
		case GLFW_KEY_2:
			if (arm2Select) {
				arm2Select = false;
			}
			else {
				penSelect = false;
				topSelect = false;
				baseSelect = false;
				arm1Select = false;
				cameraSelect = false;
				arm2Select = true;
			}
			break;
		case GLFW_KEY_B:
			if (baseSelect) {
				baseSelect = false;
			}
			else {
				penSelect = false;
				topSelect = false;
				arm1Select = false;
				arm2Select = false;
				cameraSelect = false;
				baseSelect = true;
			}
			break;
		case GLFW_KEY_T:
			if (topSelect) {
				topSelect = false;
			}
			else {
				penSelect = false;
				arm1Select = false;
				baseSelect = false;
				arm2Select = false;
				cameraSelect = false;
				topSelect = true;
			}
			break;
		case GLFW_KEY_P:
			if (penSelect) {
				penSelect = false;
			}
			else {
				arm1Select = false;
				topSelect = false;
				baseSelect = false;
				arm2Select = false;
				cameraSelect = false;
				penSelect = true;
			}
			break;
		case GLFW_KEY_C:
			if (cameraSelect) {
				cameraSelect = false;
			}
			else {
				arm1Select = false;
				topSelect = false;
				baseSelect = false;
				arm2Select = false;
				penSelect = false;
				cameraSelect = true;
			}
			break;
		case GLFW_KEY_S:
			if (projectileRun) {
				projectileRun = false;
				timer = 0.0;
				moveDistancePro = glm::vec3(0.0, 0.0, 0.0);
			}
			else {
				arm1Select = false;
				topSelect = false;
				baseSelect = false;
				arm2Select = false;
				penSelect = false;
				cameraSelect = false;
				projectileRun = true;
			}
			break;
		default:
			break;
		}
	}
}

// Alternative way of triggering functions on mouse click events
static void mouseCallback(GLFWwindow* window, int button, int action, int mods) {
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		pickObject();
	}
}

void moveCamera(void) {
	if (moveCameraLeft) {
		cameraAngleT -= 0.01f;
	}

	if (moveCameraRight) {
		cameraAngleT += 0.01f;
	}

	if (moveCameraUp) {
		cameraAngleP -= 0.01f;
	}

	if (moveCameraDown) {
		cameraAngleP += 0.01f;
	}

	if (cameraSelect && (moveCameraLeft || moveCameraRight || moveCameraDown || moveCameraUp)) {
		float cameraX = cameraSphereR * cos(cameraAngleP) * sin(cameraAngleT);
		float cameraY = cameraSphereR * sin(cameraAngleP);
		float cameraZ = cameraSphereR * cos(cameraAngleP) * cos(cameraAngleT);
		gViewMatrix = glm::lookAt(glm::vec3(cameraX, cameraY, cameraZ),	// eye
			glm::vec3(0.0, 0.0, 0.0),	// center
			glm::vec3(0.0, 1.0, 0.0));	// up
	}
}

void moveBase(void) {
	if (baseSelect && !baseSelected) {
		for (int i = 0; i < basePoint; i++) {
			Base_Verts[i].SetColor(&baseColorN[0]);
		}
		baseSelected = true;
	}
	else if (!baseSelect && baseSelected) {
		for (int i = 0; i < basePoint; i++) {
			Base_Verts[i].SetColor(&baseColorO[0]);
		}
		baseSelected = false;
	}
	else if (baseSelect && baseSelected) {
		if (moveBaseForward) {
			moveDistance.x += 0.01;
		}
		else if (moveBaseBackward) {
			moveDistance.x -= 0.01;
		}
		else if (moveBaseLeft) {
			moveDistance.z += 0.01;
		}
		else if (moveBaseRight) {
			moveDistance.z -= 0.01;
		}
	}
	bMatrix = glm::mat4(1.0);
	bMatrix = glm::translate(bMatrix, moveDistance);
	glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &bMatrix[0][0]);
}

void moveTop(void) {
	if (topSelect && !topSelected) {
		for (int i = 0; i < topPoint; i++) {
			Top_Verts[i].SetColor(&topColorN[0]);
		}
		topSelected = true;
	}
	else if (!topSelect && topSelected) {
		for (int i = 0; i < topPoint; i++) {
			Top_Verts[i].SetColor(&topColorO[0]);
		}
		topSelected = false;
	}
	else if (topSelect && topSelected) {
		if (rotateTopLeft) {
			tAngle -= 1.0;
			if (tAngle < -360) {
				tAngle += 360;
			}
		}
		else if (rotateTopRight) {
			tAngle += 1.0;
			if (tAngle > 360) {
				tAngle -= 360;
			}
		}
	}
	tMatrix = glm::mat4(1.0);
	tMatrix = glm::translate(tMatrix, moveDistance);
	tMatrix = glm::rotate(tMatrix, glm::radians(tAngle), glm::vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &tMatrix[0][0]);
}

void moveArm1(void) {
	if (arm1Select && !arm1Selected) {
		for (int i = 0; i < arm1Point; i++) {
			Arm1_Verts[i].SetColor(&arm1ColorN[0]);
		}
		arm1Selected = true;
	}
	else if (!arm1Select && arm1Selected) {
		for (int i = 0; i < arm1Point; i++) {
			Arm1_Verts[i].SetColor(&arm1ColorO[0]);
		}
		arm1Selected = false;
	}
	else if (arm1Select && arm1Selected) {
		if (moveArm1Down) {
			a1Angle += 1.0;
			if (a1Angle < -360) {
				a1Angle += 360;
			}
		}
		else if (moveArm1Up) {
			a1Angle -= 1.0;
			if (a1Angle > 360) {
				a1Angle -= 360;
			}
		}
	}
	a1Matrix = glm::mat4(1.0);
	a1Matrix = glm::translate(a1Matrix, moveDistance);
	a1Matrix = glm::rotate(a1Matrix, glm::radians(tAngle), glm::vec3(0.0f, 1.0f, 0.0f));
	a1Matrix = glm::translate(a1Matrix, moveDistanceA1);
	a1Matrix = glm::rotate(a1Matrix, glm::radians(a1Angle), glm::vec3(1.0f, 0.0f, 0.0f));
	a1Matrix = glm::translate(a1Matrix, -moveDistanceA1);
	glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &a1Matrix[0][0]);
}

void moveArm2(void) {
	if (arm2Select && !arm2Selected) {
		for (int i = 0; i < arm2Point; i++) {
			Arm2_Verts[i].SetColor(&arm2ColorN[0]);
		}
		arm2Selected = true;
	}
	else if (!arm2Select && arm2Selected) {
		for (int i = 0; i < arm2Point; i++) {
			Arm2_Verts[i].SetColor(&arm2ColorO[0]);
		}
		arm2Selected = false;
	}
	else if (arm2Select && arm2Selected) {
		if (moveArm2Down) {
			a2Angle += 1.0;
			if (a2Angle < -360) {
				a2Angle += 360;
			}
		}
		else if (moveArm2Up) {
			a2Angle -= 1.0;
			if (a2Angle > 360) {
				a2Angle -= 360;
			}
		}
	}
	a2Matrix = glm::mat4(1.0);
	a2Matrix = glm::translate(a2Matrix, moveDistance);
	a2Matrix = glm::rotate(a2Matrix, glm::radians(tAngle), glm::vec3(0.0f, 1.0f, 0.0f));
	a2Matrix = glm::translate(a2Matrix, moveDistanceA1);
	a2Matrix = glm::rotate(a2Matrix, glm::radians(a1Angle), glm::vec3(1.0f, 0.0f, 0.0f));
	a2Matrix = glm::translate(a2Matrix, -moveDistanceA1);
	a2Matrix = glm::translate(a2Matrix, moveDistanceA2);
	a2Matrix = glm::rotate(a2Matrix, glm::radians(a2Angle), glm::vec3(1.0f, 0.0f, 0.0f));
	a2Matrix = glm::translate(a2Matrix, -moveDistanceA2);
	glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &a2Matrix[0][0]);
}

void movePen(void) {
	if (penSelect && !penSelected) {
		for (int i = 0; i < penPoint; i++) {
			Pen_Verts[i].SetColor(&penColorN[0]);
		}
		penSelected = true;
	}
	else if (!penSelect && penSelected) {
		for (int i = 0; i < penPoint; i++) {
			Pen_Verts[i].SetColor(&penColorO[0]);
		}
		penSelected = false;
	}
	else if (penSelect && penSelected) {
		if (movePenDown) {
			pAngleH += 1.0;
			if (pAngleH > 360) {
				pAngleH -= 360;
			}
		}
		else if (movePenUp) {
			pAngleH -= 1.0;
			if (pAngleH < -360) {
				pAngleH += 360;
			}
		}
		else if (movePenRight) {
			pAngleV += 1.0;
			if (pAngleV > 360) {
				pAngleV -= 360;
			}
		}
		else if (movePenLeft) {
			pAngleV -= 1.0;
			if (pAngleV < -360) {
				pAngleV += 360;
			}
		}
		else if (rotatePenRight) {
			pAngleS += 1.0;
			if (pAngleS > 360) {
				pAngleS -= 360;
			}
		}
		else if (rotatePenLeft) {
			pAngleS -= 1.0;
			if (pAngleS < -360) {
				pAngleS += 360;
			}
		}
	}
	pMatrix = glm::mat4(1.0);
	pMatrix = glm::translate(pMatrix, moveDistance);
	pMatrix = glm::rotate(pMatrix, glm::radians(tAngle), glm::vec3(0.0f, 1.0f, 0.0f));
	pMatrix = glm::translate(pMatrix, moveDistanceA1);
	pMatrix = glm::rotate(pMatrix, glm::radians(a1Angle), glm::vec3(1.0f, 0.0f, 0.0f));
	pMatrix = glm::translate(pMatrix, -moveDistanceA1);
	pMatrix = glm::translate(pMatrix, moveDistanceA2);
	pMatrix = glm::rotate(pMatrix, glm::radians(a2Angle), glm::vec3(1.0f, 0.0f, 0.0f));
	pMatrix = glm::translate(pMatrix, -moveDistanceA2);
	pMatrix = glm::translate(pMatrix, moveDistanceP);
	pMatrix = glm::rotate(pMatrix, glm::radians(pAngleH), glm::vec3(1.0f, 0.0f, 0.0f));
	pMatrix = glm::rotate(pMatrix, glm::radians(pAngleV), glm::vec3(0.0f, 1.0f, 0.0f));
	pMatrix = glm::rotate(pMatrix, glm::radians(pAngleS), glm::vec3(0.0f, 0.0f, 1.0f));
	pMatrix = glm::translate(pMatrix, -moveDistanceP);
	glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &pMatrix[0][0]);
}

void runProj(void) {
	glm::mat4 projMatrix = glm::mat4(1.0);
	projMatrix = glm::translate(projMatrix, moveDistance);
	projMatrix = glm::rotate(projMatrix, glm::radians(tAngle), glm::vec3(0.0f, 1.0f, 0.0f));
	projMatrix = glm::translate(projMatrix, moveDistanceA1);
	projMatrix = glm::rotate(projMatrix, glm::radians(a1Angle), glm::vec3(1.0f, 0.0f, 0.0f));
	projMatrix = glm::translate(projMatrix, -moveDistanceA1);
	projMatrix = glm::translate(projMatrix, moveDistanceA2);
	projMatrix = glm::rotate(projMatrix, glm::radians(a2Angle), glm::vec3(1.0f, 0.0f, 0.0f));
	projMatrix = glm::translate(projMatrix, -moveDistanceA2);
	projMatrix = glm::translate(projMatrix, moveDistanceP);
	projMatrix = glm::rotate(projMatrix, glm::radians(pAngleH), glm::vec3(1.0f, 0.0f, 0.0f));
	projMatrix = glm::rotate(projMatrix, glm::radians(pAngleV), glm::vec3(0.0f, 1.0f, 0.0f));
	projMatrix = glm::rotate(projMatrix, glm::radians(pAngleS), glm::vec3(0.0f, 0.0f, 1.0f));
	projMatrix = glm::translate(projMatrix, -moveDistanceP);

	if (projectileRun) {

		timer += 0.001;
		moveDistancePro.z += 0.1 * timer;
		moveDistancePro.y -= 0.5 * 9.8 * timer * timer;

		if (moveDistancePro.y > -0.58) {
			projMatrix = glm::translate(projMatrix, moveDistancePro);
			glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &projMatrix[0][0]);
			glBindVertexArray(VertexArrayId[9]);	// Draw Projectile
			glBindBuffer(GL_ARRAY_BUFFER, VertexBufferId[9]);
			glBufferSubData(GL_ARRAY_BUFFER, 0, VertexBufferSize[9], Proj_Verts);
			glDrawElements(GL_TRIANGLES, NumVerts[9], GL_UNSIGNED_SHORT, (void*)0);
		}
	}
}

int main(void) {
	// TL
	// ATTN: Refer to https://learnopengl.com/Getting-started/Transformations, https://learnopengl.com/Getting-started/Coordinate-Systems,
	// and https://learnopengl.com/Getting-started/Camera to familiarize yourself with implementing the camera movement

	// ATTN (Project 3 only): Refer to https://learnopengl.com/Getting-started/Textures to familiarize yourself with mapping a texture
	// to a given mesh

	// Initialize window
	int errorCode = initWindow();
	if (errorCode != 0)
		return errorCode;

	// Initialize OpenGL pipeline
	initOpenGL();

	// For speed computation
	double lastTime = glfwGetTime();
	int nbFrames = 0;
	do {
		// Measure speed
		double currentTime = glfwGetTime();
		nbFrames++;
		if (currentTime - lastTime >= 1.0) { // If last prinf() was more than 1sec ago
			printf("%f ms/frame\n", 1000.0 / double(nbFrames));
			nbFrames = 0;
			lastTime += 1.0;
		}

		int keyUpState = glfwGetKey(window, GLFW_KEY_UP);
		if (keyUpState == GLFW_PRESS)
		{
			keyUpAlreadyPressed = true;
			if (cameraSelect) {
				moveCameraUp = true;
			}
			if (penSelect) {
				movePenUp = true;
			}
			if (baseSelect) {
				moveBaseForward = true;
			}
			if (arm1Select) {
				moveArm1Up = true;
			}
			if (arm2Select) {
				moveArm2Up = true;
			}
		}
		if (keyUpState == GLFW_RELEASE) {
			if (keyUpAlreadyPressed) {
				keyUpAlreadyPressed = false;
				moveCameraUp = false;
				movePenUp = false;
				moveBaseForward = false;
				moveArm1Up = false;
				moveArm2Up = false;
			}
		}

		int keyDownState = glfwGetKey(window, GLFW_KEY_DOWN);
		if (keyDownState == GLFW_PRESS)
		{
			keyDownAlreadyPressed = true;
			if (cameraSelect) {
				moveCameraDown = true;
			}
			if (penSelect) {
				movePenDown = true;
			}
			if (baseSelect) {
				moveBaseBackward = true;
			}
			if (arm1Select) {
				moveArm1Down = true;
			}
			if (arm2Select) {
				moveArm2Down = true;
			}
		}
		if (keyDownState == GLFW_RELEASE) {
			if (keyDownAlreadyPressed) {
				keyDownAlreadyPressed = false;
				moveCameraDown = false;
				movePenDown = false;
				moveBaseBackward = false;
				moveArm1Down = false;
				moveArm2Down = false;
			}
		}

		int keyLeftState = glfwGetKey(window, GLFW_KEY_LEFT);
		if (keyLeftState == GLFW_PRESS)
		{
			keyLeftAlreadyPressed = true;
			if (cameraSelect) {
				moveCameraLeft = true;
			}
			if (penSelect && !penRotate) {
				movePenLeft = true;
			}
			if (topSelect) {
				rotateTopLeft = true;
			}
			if (baseSelect) {
				moveBaseLeft = true;
			}
			if (penRotate) {
				rotatePenLeft = true;
			}
		}
		if (keyLeftState == GLFW_RELEASE) {
			if (keyLeftAlreadyPressed) {
				keyLeftAlreadyPressed = false;
				moveCameraLeft = false;
				movePenLeft = false;
				rotateTopLeft = false;
				moveBaseLeft = false;
				rotatePenLeft = false;
			}
		}

		int keyRightState = glfwGetKey(window, GLFW_KEY_RIGHT);
		if (keyRightState == GLFW_PRESS)
		{
			keyRightAlreadyPressed = true;
			if (cameraSelect) {
				moveCameraRight = true;
			}
			if (penSelect && !penRotate) {
				movePenRight = true;
			}
			if (topSelect) {
				rotateTopRight = true;
			}
			if (baseSelect) {
				moveBaseRight = true;
			}
			if (penRotate) {
				rotatePenRight = true;
			}
		}
		if (keyRightState == GLFW_RELEASE) {
			if (keyRightAlreadyPressed) {
				keyRightAlreadyPressed = false;
				moveCameraRight = false;
				movePenRight = false;
				rotateTopRight = false;
				moveBaseRight = false;
				rotatePenRight = false;
			}
		}

		int keyShiftState = glfwGetKey(window, GLFW_KEY_LEFT_SHIFT);
		if (keyShiftState == GLFW_PRESS)
		{
			keyShiftAlreadyPressed = true;
			if (penSelect) {
				penRotate = true;
			}
		}
		if (keyShiftState == GLFW_RELEASE) {
			if (keyShiftAlreadyPressed) {
				keyShiftAlreadyPressed = false;
				penRotate = false;
			}
		}

		// DRAWING POINTS
		renderScene();

	} // Check if the ESC key was pressed or the window was closed
	while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
		glfwWindowShouldClose(window) == 0);

	cleanup();

	return 0;
}
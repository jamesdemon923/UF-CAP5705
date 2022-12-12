// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <array>
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

// ATTN 1A is the general place in the program where you have to change the code base to satisfy a Task of Project 1A.
// ATTN 1B for Project 1B. ATTN 1C for Project 1C. Focus on the ones relevant for the assignment you're working on.

typedef struct Vertex {
	float Position[4];
	float Color[4];
	void SetCoords(float *coords) {
		Position[0] = coords[0];
		Position[1] = coords[1];
		Position[2] = coords[2];
		Position[3] = coords[3];
	}
	void SetColor(float *color) {
		Color[0] = color[0];
		Color[1] = color[1];
		Color[2] = color[2];
		Color[3] = color[3];
	}
};

// ATTN: use POINT structs for cleaner code (POINT is a part of a vertex)
// allows for (1-t)*P_1+t*P_2  avoiding repeat for each coordinate (x,y,z)
typedef struct point {
	float x, y, z;
	point(const float x = 0, const float y = 0, const float z = 0) : x(x), y(y), z(z){};
	point(float *coords) : x(coords[0]), y(coords[1]), z(coords[2]){};
	point operator -(const point& a) const {
		return point(x - a.x, y - a.y, z - a.z);
	}
	point operator +(const point& a) const {
		return point(x + a.x, y + a.y, z + a.z);
	}
	point operator *(const float& a) const {
		return point(x * a, y * a, z * a);
	}
	point operator /(const float& a) const {
		return point(x / a, y / a, z / a);
	}
	float* toArray() {
		float array[] = { x, y, z, 1.0f };
		return array;
	}
};

// Function prototypes
int initWindow(void);
void initOpenGL(void);
void createVAOs(Vertex[], GLushort[], int);
void createObjects(void);
void pickVertex(void);
void moveVertex(void);
void renderScene(void);
void cleanup(void);
void changeSize(void);
void Bspline(int);
void createBspline(void);
void showBspline(void);
void Béziercurves(int);
void createBéziercurves(void);
void showBéziercurves(void);
void CRcurves(void);
void createCRcurves(void);
void showCRcurves(void);
void drawCRcurveLine(void);
void DeCasteljau(void);
void doubleView(void);
void littleTri(void);
static void mouseCallback(GLFWwindow*, int, int, int);
static void keyCallback(GLFWwindow*, int, int, int, int);

// GLOBAL VARIABLES
GLFWwindow* window;
const GLuint window_width = 1024, window_height = 768;

glm::mat4 gProjectionMatrix;
glm::mat4 gViewMatrix;


// Program IDs
GLuint programID;
GLuint pickingProgramID;

// Uniform IDs
GLuint MatrixID;
GLuint ViewMatrixID;
GLuint ModelMatrixID;
GLuint PickingMatrixID;
GLuint pickingColorArrayID;
GLuint pickingColorID;
GLuint pointSizeID;

GLuint gPickedIndex;
std::string gMessage;

// ATTN: INCREASE THIS NUMBER AS YOU CREATE NEW OBJECTS
const GLuint NumObjects = 1; // Number of objects types in the scene

// Keeps track of IDs associated with each object
GLuint VertexArrayId[NumObjects];
GLuint VertexBufferId[NumObjects];
GLuint IndexBufferId[NumObjects];

size_t VertexBufferSize[NumObjects];
size_t IndexBufferSize[NumObjects];
size_t NumVerts[NumObjects];	// Useful for glDrawArrays command
size_t NumIdcs[NumObjects];	// Useful for glDrawElements command

// Initialize ---  global objects -- not elegant but ok for this project
const size_t IndexCount = 864;
Vertex Vertices[IndexCount];
GLushort Indices[IndexCount];

// ATTN: DON'T FORGET TO INCREASE THE ARRAY SIZE IN THE PICKING VERTEX SHADER WHEN YOU ADD MORE PICKING COLORS
float pickingColor[IndexCount];


// The variable we need for the project
int middleState;
int rightState;
int k = 1;
int q1 = 1;
int q2 = 0;
int N = 20;
int M = 30;
int index = 690;
float originalColorR;
float originalColorG;
float originalColorB;
float pointSize = 7.0f;
bool alreadyCreate = false;
bool alreadyCreate2 = false;
bool alreadyCreate3 = false;
bool alreadyCreate4 = false;
bool keyShiAlreadyPressed = false;
bool key5AlreadyPressed = false;
bool isBéziercurves = false;
bool drawCRLine = false;
bool isDoubleView = false;
bool key4ForDoubleView = false;
bool createLittleTri = false;
bool runLittleTri = false;
bool vecPoint = false;
glm::vec3 mouPos;
glm::vec3 next;
glm::vec3 current;
glm::vec3 add;
glm::vec3 minus;
std::vector<GLushort> indices;
std::vector<GLushort> indices2;
std::vector<GLushort> indices3;
std::vector<GLushort> indices4;
std::vector<GLushort> indicesT;
std::vector<GLushort> indicesN;
std::vector<GLushort> indicesB;

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
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // FOR MAC

	// ATTN: Project 1A, Task 0 == Change the name of the window
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

	// Initialize the GUI display
	TwInit(TW_OPENGL_CORE, NULL);
	TwWindowSize(window_width, window_height);
	TwBar * GUI = TwNewBar("Picking");
	TwSetParam(GUI, NULL, "refresh", TW_PARAM_CSTRING, 1, "0.1");
	TwAddVarRW(GUI, "Last picked object", TW_TYPE_STDSTRING, &gMessage, NULL);

	// Set up inputs
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_FALSE);
	glfwSetCursorPos(window, window_width / 2, window_height / 2);
	glfwSetMouseButtonCallback(window, mouseCallback);
	glfwSetKeyCallback(window, keyCallback);
	return 0;
}

void initOpenGL(void) {
	// Dark blue background
	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

	// Enable depth test
	glEnable(GL_DEPTH_TEST);
	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS);
	// Cull triangles which normal is not towards the camera
	glEnable(GL_CULL_FACE);

	// Projection matrix : 45� Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
	//glm::mat4 ProjectionMatrix = glm::perspective(45.0f, 4.0f / 3.0f, 0.1f, 100.0f);
	// Or, for Project 1, use an ortho camera :
	gProjectionMatrix = glm::ortho(-4.0f, 4.0f, -3.0f, 3.0f, 0.0f, 100.0f); // In world coordinates

	// Create and compile our GLSL program from the shaders
	programID = LoadShaders("p1_StandardShading.vertexshader", "p1_StandardShading.fragmentshader");
	pickingProgramID = LoadShaders("p1_Picking.vertexshader", "p1_Picking.fragmentshader");

	// Get a handle for our "pointSize" uniform
	pointSizeID = glGetUniformLocation(programID, "pointSize");

	// Get a handle for our "MVP" uniform
	MatrixID = glGetUniformLocation(programID, "MVP");
	ViewMatrixID = glGetUniformLocation(programID, "V");
	ModelMatrixID = glGetUniformLocation(programID, "M");
	PickingMatrixID = glGetUniformLocation(pickingProgramID, "MVP");
	
	// Get a handle for our "pickingColorID" uniform
	pickingColorArrayID = glGetUniformLocation(pickingProgramID, "PickingColorArray");
	pickingColorID = glGetUniformLocation(pickingProgramID, "PickingColor");

	// Define pickingColor array for picking program
	// use a for-loop here
	for (int i = 0; i < IndexCount; i++)
	{
		pickingColor[i] = i / 255.0f;
	}

	// Define objects
	createObjects();

	// ATTN: create VAOs for each of the newly created objects here:
	// for several objects of the same type use a for-loop
	int obj = 0;  // initially there is only one type of object 
	VertexBufferSize[obj] = sizeof(Vertices);
	IndexBufferSize[obj] = sizeof(Indices);
	NumIdcs[obj] = IndexCount;

	createVAOs(Vertices, Indices, obj);
}

// this actually creates the VAO (structure) and the VBO (vertex data buffer)
void createVAOs(Vertex Vertices[], GLushort Indices[], int ObjectId) {
	GLenum ErrorCheckValue = glGetError();
	const size_t VertexSize = sizeof(Vertices[0]);
	const size_t RgbOffset = sizeof(Vertices[0].Position);

	// Create Vertex Array Object
	glGenVertexArrays(1, &VertexArrayId[ObjectId]);
	glBindVertexArray(VertexArrayId[ObjectId]);

	// Create buffer for vertex data
	glGenBuffers(1, &VertexBufferId[ObjectId]);
	glBindBuffer(GL_ARRAY_BUFFER, VertexBufferId[ObjectId]);
	glBufferData(GL_ARRAY_BUFFER, VertexBufferSize[ObjectId], Vertices, GL_STATIC_DRAW);

	// Create buffer for indices
	if (Indices != NULL) {
		glGenBuffers(1, &IndexBufferId[ObjectId]);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IndexBufferId[ObjectId]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, IndexBufferSize[ObjectId], Indices, GL_STATIC_DRAW);
	}

	// Assign vertex attributes
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, VertexSize, 0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, VertexSize, (GLvoid*)RgbOffset);

	glEnableVertexAttribArray(0);	// position
	glEnableVertexAttribArray(1);	// color

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

void createObjects(void) {
	// ATTN: DERIVE YOUR NEW OBJECTS HERE:  each object has
	// an array of vertices {pos;color} and
	// an array of indices (no picking needed here) (no need for indices)
	// ATTN: Project 1A, Task 1 == Add the points in your scene
	Vertices[0] = { { 0.0f, 0.0f, 0.0f, 1.0f }, { 1.0f, 1.0f, 1.0f, 1.0f } };
	Vertices[1] = { { 0.809f, 0.5878f, 0.0f, 1.0f }, { 1.0f, 1.0f, 1.0f, 1.0f } };
	Vertices[2] = { { 0.5f, 1.538f, 0.0f, 1.0f }, { 1.0f, 1.0f, 1.0f, 1.0f } };
	Vertices[3] = { { -0.5f, 1.538f, 0.0f, 1.0f }, { 1.0f, 1.0f, 1.0f, 1.0f } };
	Vertices[4] = { { -0.809f, 0.5878f, 0.0f, 1.0f }, { 1.0f, 1.0f, 1.0f, 1.0f } };
	Vertices[5] = { { 0.0f, 0.0f, 0.0f, 1.0f }, { 1.0f, 1.0f, 1.0f, 1.0f } };
	Vertices[6] = { { 0.809f, -0.5878f, 0.0f, 1.0f }, { 1.0f, 1.0f, 1.0f, 1.0f } };
	Vertices[7] = { { 0.5f, -1.538f, 0.0f, 1.0f }, { 1.0f, 1.0f, 1.0f, 1.0f } };
	Vertices[8] = { { -0.5f, -1.538f, 0.0f, 1.0f }, { 1.0f, 1.0f, 1.0f, 1.0f } };
	Vertices[9] = { { -0.809f, -0.5878f, 0.0f, 1.0f }, { 1.0f, 1.0f, 1.0f, 1.0f } };

	Indices[0] = 0;
	Indices[1] = 1;
	Indices[2] = 2;
	Indices[3] = 3;
	Indices[4] = 4;
	Indices[5] = 5;
	Indices[6] = 6;
	Indices[7] = 7;
	Indices[8] = 8;
	Indices[9] = 9;


	// ATTN: Project 1B, Task 1 == create line segments to connect the control points

	// ATTN: Project 1B, Task 2 == create the vertices associated to the smoother curve generated by subdivision

	// ATTN: Project 1B, Task 4 == create the BB control points and apply De Casteljau's for their corresponding for each piece

	// ATTN: Project 1C, Task 3 == set coordinates of yellow point based on BB curve and perform calculations to find
	// the tangent, normal, and binormal
}

void Bspline(int j) {
	int a = (j - 10) / 2;
	int b = M - N - 1;
	int c = (j - 11) / 2;
	if (j % 2 == 0) {
		if ((j - 10) / 2 - 1 < M - N - 10 * q1) {
			Vertices[j].Position[0] = (4 * Vertices[b].Position[0] + 4 * Vertices[a].Position[0]) / 8;
			Vertices[j].Position[1] = (4 * Vertices[b].Position[1] + 4 * Vertices[a].Position[1]) / 8;
			Vertices[j].Position[2] = (4 * Vertices[b].Position[2] + 4 * Vertices[a].Position[2]) / 8;
		}
		if ((j - 10) / 2 - 1 >= M - N - 10 * q1) {
			Vertices[j].Position[0] = (4 * Vertices[a - 1].Position[0] + 4 * Vertices[a].Position[0]) / 8;
			Vertices[j].Position[1] = (4 * Vertices[a - 1].Position[1] + 4 * Vertices[a].Position[1]) / 8;
			Vertices[j].Position[2] = (4 * Vertices[a - 1].Position[2] + 4 * Vertices[a].Position[2]) / 8;
		}
	}
	else {
		if ((j - 11) / 2 - 1 < M - N - 10 * q1) {
			Vertices[j].Position[0] = (Vertices[b].Position[0] + 6 * Vertices[c].Position[0] + Vertices[c + 1].Position[0]) / 8;
			Vertices[j].Position[1] = (Vertices[b].Position[1] + 6 * Vertices[c].Position[1] + Vertices[c + 1].Position[1]) / 8;
			Vertices[j].Position[2] = (Vertices[b].Position[2] + 6 * Vertices[c].Position[2] + Vertices[c + 1].Position[2]) / 8;
		}
		else if (j == M - 1) {
			Vertices[j].Position[0] = (Vertices[c - 1].Position[0] + 6 * Vertices[c].Position[0] + Vertices[M - N - 10 * q1].Position[0]) / 8;
			Vertices[j].Position[1] = (Vertices[c - 1].Position[1] + 6 * Vertices[c].Position[1] + Vertices[M - N - 10 * q1].Position[1]) / 8;
			Vertices[j].Position[2] = (Vertices[c - 1].Position[2] + 6 * Vertices[c].Position[2] + Vertices[M - N - 10 * q1].Position[2]) / 8;
		}
		else {
			Vertices[j].Position[0] = (Vertices[c - 1].Position[0] + 6 * Vertices[c].Position[0] + Vertices[c + 1].Position[0]) / 8;
			Vertices[j].Position[1] = (Vertices[c - 1].Position[1] + 6 * Vertices[c].Position[1] + Vertices[c + 1].Position[1]) / 8;
			Vertices[j].Position[2] = (Vertices[c - 1].Position[2] + 6 * Vertices[c].Position[2] + Vertices[c + 1].Position[2]) / 8;
		}
	}
}

void createBspline(void) {
	if (!alreadyCreate) {
		for (int i = 10; i < 630; i++) {
			Vertices[i] = { { 0.0f, 0.0f, 0.0f, 1.0f }, { 0.0f, 1.0f, 1.0f, 1.0f } };
		}
		alreadyCreate = true;
	}
	for (; k < 6; k++) {
		for (int j = M - N; j < M; j++) {
			Bspline(j);
		}
		q1 = q1 * 2;
		N = N * 2;
		M = M + N;
	}
	q1 = 1;
	N = 20;
	M = 30;
	k = 1;
}

void showBspline(void) {
	if (k < 6) {
		for (int i = M - N; i < M; i++) {
			Indices[i] = i;
		}
		q1 = q1 * 2;
		N = N * 2;
		M = M + N;
		printf("k is %d", k);
		k++;
	}
	else {
		q1 = 1;
		N = 20;
		M = 30;
		k = 1;
		for (int j = 10; j < 630; j++) {
			Indices[j] = NULL;
		}
	}
}

void Béziercurves(int j) {
	int a = j - 631 - 2 * q2;
	int b = j - 632 - 2 * q2;
	if (j == 630 || j == 645) {
		Vertices[j].Position[0] = Vertices[0].Position[0];
		Vertices[j].Position[1] = Vertices[0].Position[1];
		Vertices[j].Position[2] = Vertices[0].Position[2];
	}
	else if ((j - 633) % 3 == 0) {
		Vertices[j].Position[0] = Vertices[0].Position[0];
		Vertices[j].Position[1] = Vertices[0].Position[1];
		Vertices[j].Position[2] = Vertices[0].Position[2];
	}
	else if ((j - 631) % 3 == 0) {
		if (j == 658) {
			Vertices[j].Position[0] = (2 * Vertices[a].Position[0] + Vertices[0].Position[0]) / 3;
			Vertices[j].Position[1] = (2 * Vertices[a].Position[1] + Vertices[0].Position[1]) / 3;
			Vertices[j].Position[2] = (2 * Vertices[a].Position[2] + Vertices[0].Position[2]) / 3;
		}
		else {
			Vertices[j].Position[0] = (2 * Vertices[a].Position[0] + Vertices[a + 1].Position[0]) / 3;
			Vertices[j].Position[1] = (2 * Vertices[a].Position[1] + Vertices[a + 1].Position[1]) / 3;
			Vertices[j].Position[2] = (2 * Vertices[a].Position[2] + Vertices[a + 1].Position[2]) / 3;
		}					
	}
	else {
		if (j == 659) {
			Vertices[j].Position[0] = (Vertices[b].Position[0] + 2 * Vertices[0].Position[0]) / 3;
			Vertices[j].Position[1] = (Vertices[b].Position[1] + 2 * Vertices[0].Position[1]) / 3;
			Vertices[j].Position[2] = (Vertices[b].Position[2] + 2 * Vertices[0].Position[2]) / 3;
		}
		else {
			Vertices[j].Position[0] = (Vertices[b].Position[0] + 2 * Vertices[b + 1].Position[0]) / 3;
			Vertices[j].Position[1] = (Vertices[b].Position[1] + 2 * Vertices[b + 1].Position[1]) / 3;
			Vertices[j].Position[2] = (Vertices[b].Position[2] + 2 * Vertices[b + 1].Position[2]) / 3;
		}
		q2++;
	}
}

void createBéziercurves(void) {
	if (!alreadyCreate2) {
		for (int j = 630; j < 660; j++) {
			Vertices[j] = { { 0.0f, 0.0f, 0.0f, 1.0f }, { 1.0f, 1.0f, 0.0f, 1.0f } };
		}
		alreadyCreate2 = true;
	}
	for (int j = 630; j < 660; j++) {
		Béziercurves(j);
	}
	for (int j = 633; j < 660; j = j + 3) {
		Vertices[j].Position[0] = (Vertices[j - 1].Position[0] + Vertices[j + 1].Position[0]) / 2;
		Vertices[j].Position[1] = (Vertices[j - 1].Position[1] + Vertices[j + 1].Position[1]) / 2;
		Vertices[j].Position[2] = (Vertices[j - 1].Position[2] + Vertices[j + 1].Position[2]) / 2;
	}
	q2 = 0;
}

void showBéziercurves(void) {
	if (!isBéziercurves) {
		for (int j = 630; j < 660; j++) {
			Indices[j] = j;
		}
		isBéziercurves = true;
	}
	else {
		for (int j = 630; j < 660; j++) {
			Indices[j] = NULL;
		}
		isBéziercurves = false;
	}
}

void CRcurves(int j) {
	if (j == 660) {
		Vertices[j].Position[0] = Vertices[0].Position[0];
		Vertices[j].Position[1] = Vertices[0].Position[1];
		Vertices[j].Position[2] = Vertices[0].Position[2];
	}
	else if (j == 675) {
		Vertices[j].Position[0] = Vertices[5].Position[0];
		Vertices[j].Position[1] = Vertices[5].Position[1];
		Vertices[j].Position[2] = Vertices[5].Position[2];
	}
	else if ((j - 663) % 3 == 0) {
		Vertices[j].Position[0] = Vertices[(j - 663) / 3 + 1].Position[0];
		Vertices[j].Position[1] = Vertices[(j - 663) / 3 + 1].Position[1];
		Vertices[j].Position[2] = Vertices[(j - 663) / 3 + 1].Position[2];
	}
}

void createCRcurves(void) {
	if (!alreadyCreate3) {
		for (int j = 660; j < 690; j++) {
			Vertices[j] = { { 0.0f, 0.0f, 0.0f, 1.0f }, { 1.0f, 0.0f, 0.0f, 1.0f } };
		}
		alreadyCreate3 = true;
	}
	for (int j = 660; j < 690; j++) {
		CRcurves(j);
	}
	for (int j = 662; j < 690; j = j + 3) {
		if (j == 674) {
			Vertices[j].Position[0] = Vertices[5].Position[0] + (Vertices[4].Position[0] - Vertices[6].Position[0]) / 6;
			Vertices[j].Position[1] = Vertices[5].Position[1] + (Vertices[4].Position[1] - Vertices[6].Position[1]) / 6;
			Vertices[j].Position[2] = Vertices[5].Position[2] + (Vertices[4].Position[2] - Vertices[6].Position[2]) / 6;
		}
		else if (j == 689) {
			Vertices[j].Position[0] = Vertices[0].Position[0] - (Vertices[1].Position[0] - Vertices[9].Position[0]) / 6;
			Vertices[j].Position[1] = Vertices[0].Position[1] - (Vertices[1].Position[1] - Vertices[9].Position[1]) / 6;
			Vertices[j].Position[2] = Vertices[0].Position[2] - (Vertices[1].Position[2] - Vertices[9].Position[2]) / 6;
		}
		else if (j == 686) {
			Vertices[j].Position[0] = Vertices[9].Position[0] - (Vertices[0].Position[0] - Vertices[8].Position[0]) / 5;
			Vertices[j].Position[1] = Vertices[9].Position[1] - (Vertices[0].Position[1] - Vertices[8].Position[1]) / 5;
			Vertices[j].Position[2] = Vertices[9].Position[2] - (Vertices[0].Position[2] - Vertices[8].Position[2]) / 5;
		}
		else {
			Vertices[j].Position[0] = Vertices[j + 1].Position[0] + (Vertices[(j - 662) / 3].Position[0] - Vertices[(j - 662) / 3 + 2].Position[0]) / 5;
			Vertices[j].Position[1] = Vertices[j + 1].Position[1] + (Vertices[(j - 662) / 3].Position[1] - Vertices[(j - 662) / 3 + 2].Position[1]) / 5;
			Vertices[j].Position[2] = Vertices[j + 1].Position[2] + (Vertices[(j - 662) / 3].Position[2] - Vertices[(j - 662) / 3 + 2].Position[2]) / 5;
		}
	}
	for (int j = 661; j < 690; j = j + 3) {
		if (j == 661) {
			Vertices[j].Position[0] = Vertices[0].Position[0] + (Vertices[1].Position[0] - Vertices[9].Position[0]) / 6;
			Vertices[j].Position[1] = Vertices[0].Position[1] + (Vertices[1].Position[1] - Vertices[9].Position[1]) / 6;
			Vertices[j].Position[2] = Vertices[0].Position[2] + (Vertices[1].Position[2] - Vertices[9].Position[2]) / 6;
		}
		else if (j == 676) {
			Vertices[j].Position[0] = Vertices[5].Position[0] - (Vertices[4].Position[0] - Vertices[6].Position[0]) / 6;
			Vertices[j].Position[1] = Vertices[5].Position[1] - (Vertices[4].Position[1] - Vertices[6].Position[1]) / 6;
			Vertices[j].Position[2] = Vertices[5].Position[2] - (Vertices[4].Position[2] - Vertices[6].Position[2]) / 6;
		}
		else if (j == 688) {
			Vertices[j].Position[0] = Vertices[9].Position[0] + (Vertices[0].Position[0] - Vertices[8].Position[0]) / 5;
			Vertices[j].Position[1] = Vertices[9].Position[1] + (Vertices[0].Position[1] - Vertices[8].Position[1]) / 5;
			Vertices[j].Position[2] = Vertices[9].Position[2] + (Vertices[0].Position[2] - Vertices[8].Position[2]) / 5;
		}
		else {
			Vertices[j].Position[0] = Vertices[j - 1].Position[0] - (Vertices[(j - 661) / 3 - 1].Position[0] - Vertices[(j - 661) / 3 + 1].Position[0]) / 5;
			Vertices[j].Position[1] = Vertices[j - 1].Position[1] - (Vertices[(j - 661) / 3 - 1].Position[1] - Vertices[(j - 661) / 3 + 1].Position[1]) / 5;
			Vertices[j].Position[2] = Vertices[j - 1].Position[2] - (Vertices[(j - 661) / 3 - 1].Position[2] - Vertices[(j - 661) / 3 + 1].Position[2]) / 5;
		}
	}
	DeCasteljau();
}

void showCRcurves(void) {
	for (int j = 660; j < 860; j++) {
		Indices[j] = j;
	}
}

void DeCasteljau(void) {
	// Generate curve points (deCasteljau's Algorithm)
	// ref: https://blog.csdn.net/Fioman/article/details/2578895
	if (!alreadyCreate4) {
		for (int i = 690; i < 861; i++) {
			if (i == 860) {
				Vertices[i] = { { 0.0f, 0.0f, 0.0f, 1.0f }, { 1.0f, 1.0f, 0.0f, 1.0f } };
			}
			else {
				Vertices[i] = { { 0.0f, 0.0f, 0.0f, 1.0f }, { 0.0f, 1.0f, 0.0f, 1.0f } };
			}
		}
		alreadyCreate4 = true;
	}
	for (int i = 690; i < 860; i++) {
		int n = 4;
		int m = 660 + (i - 690) / 17 * 3;
		int g = (i - 690) % 17;
		double h = g;
		float t = h * 1 / 16;
		double* xarray = new double[4];
		double* yarray = new double[4];
		double* zarray = new double[4];
		int* array = new int[4];
		for (int k = 0; k < n; k++) {
			array[k] = m;
			m++;
		}

		for (int q = 1; q < n; q++) {
			for (int j = 0; j < n - q; j++) {
				if (q == 1) {
					int o = array[j];
					int p = array[j + 1];
					xarray[j] = Vertices[o].Position[0] * (1 - t) + Vertices[p].Position[0] * t;
					yarray[j] = Vertices[o].Position[1] * (1 - t) + Vertices[p].Position[1] * t;
					zarray[j] = Vertices[o].Position[2] * (1 - t) + Vertices[p].Position[2] * t;
					continue;
				}
				xarray[j] = xarray[j] * (1 - t) + xarray[j + 1] * t;
				yarray[j] = yarray[j] * (1 - t) + yarray[j + 1] * t;
				zarray[j] = zarray[j] * (1 - t) + zarray[j + 1] * t;
			}
		}

		Vertices[i].Position[0] = xarray[0];
		Vertices[i].Position[1] = yarray[0];
		Vertices[i].Position[2] = zarray[0];



		delete[]xarray;
		delete[]yarray;
		delete[]zarray;
		delete[]array;
	}
}

void pickVertex(void) {
	// Clear the screen in white
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(pickingProgramID);
	{
		glm::mat4 ModelMatrix = glm::mat4(1.0); // initialization
		// ModelMatrix == TranslationMatrix * RotationMatrix;
		glm::mat4 MVP = gProjectionMatrix * gViewMatrix * ModelMatrix;
		// MVP should really be PVM...
		// Send the MVP to the shader (that is currently bound)
		// as data type uniform (shared by all shader instances)
		glUniformMatrix4fv(PickingMatrixID, 1, GL_FALSE, &MVP[0][0]);

		// pass in the picking color array to the shader
		glUniform1fv(pickingColorArrayID, IndexCount, pickingColor);

		// --- enter vertices into VBO and draw
		glEnable(GL_PROGRAM_POINT_SIZE);
		glBindVertexArray(VertexArrayId[0]);
		glBufferSubData(GL_ARRAY_BUFFER, 0, VertexBufferSize[0], Vertices);	// update buffer data
		glDrawElements(GL_POINTS, NumIdcs[0], GL_UNSIGNED_SHORT, (void*)0);
		glBindVertexArray(0);
	}
	glUseProgram(0);
	glFlush();
	// --- Wait until all the pending drawing commands are really done.
	// Ultra-mega-over slow ! 
	// There are usually a long time between glDrawElements() and
	// all the fragments completely rasterized.
	glFinish();

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	// --- Read the pixel at the center of the screen.
	// You can also use glfwGetMousePos().
	// Ultra-mega-over slow too, even for 1 pixel, 
	// because the framebuffer is on the GPU.
	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);
	unsigned char data[4];  // 2x2 pixel region
	glReadPixels(xpos, window_height - ypos, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, data);
       	// window_height - ypos;  
	// OpenGL renders with (0,0) on bottom, mouse reports with (0,0) on top

	// Convert the color back to an integer ID
	gPickedIndex = int(data[0]);

	// ATTN: Project 1A, Task 2
	// Find a way to change color of selected vertex and
	// store original color
	if (gPickedIndex < IndexCount) {

		// store original color
		originalColorR = Vertices[gPickedIndex].Color[0];
		originalColorG = Vertices[gPickedIndex].Color[1];
		originalColorB = Vertices[gPickedIndex].Color[2];

		glBindBuffer(GL_ARRAY_BUFFER, VertexBufferId[0]);
		glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

		// change color of selected vertex
		Vertices[gPickedIndex].Color[0] = 0.8f;
		Vertices[gPickedIndex].Color[1] = 0.4f;
		Vertices[gPickedIndex].Color[2] = 0.2f;

	}
	// Uncomment these lines if you wan to see the picking shader in effect
	// glfwSwapBuffers(window);
	// continue; // skips the visible rendering
}

// ATTN: Project 1A, Task 3 == Retrieve your cursor position, get corresponding world coordinate, and move the point accordingly
void changeSize(void) {
	middleState = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE);
	rightState = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT);
	if (middleState == GLFW_PRESS) {
		pointSize += 1.0f;
	}
	if (rightState == GLFW_PRESS) {
		if (pointSize > 7.0f) {
			pointSize -= 1.0f;
		}
	}
}
// ATTN: Project 1C, Task 1 == Keep track of z coordinate for selected point and adjust its value accordingly based on if certain
// buttons are being pressed
void moveVertex(void) {
	glm::mat4 ModelMatrix = glm::mat4(1.0);
	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);
	glm::vec4 vp = glm::vec4(viewport[0], viewport[1], viewport[2], viewport[3]);
	if (gPickedIndex >= IndexCount) { 
		// Any number > vertices-indices is background!
		gMessage = "background";
	}
	else {
		glBindBuffer(GL_ARRAY_BUFFER, VertexBufferId[0]);
		glClearColor(0.0f, 0.0f, 0.4f, 0.0f);
		std::ostringstream oss;
		oss << "point " << gPickedIndex;
		gMessage = oss.str();
		double xpos, ypos;
		// Get the Position in X and Y
		glfwGetCursorPos(window, &xpos, &ypos);
		// Get the Mouse Position
		mouPos = glm::unProject(glm::vec3(xpos, ypos, 0.0), ModelMatrix, gProjectionMatrix, vp);
		// Change the Vertex Position into the Mouse Position
		if (keyShiAlreadyPressed) {
			Vertices[gPickedIndex].Position[2] = mouPos[0] + Vertices[gPickedIndex].Position[0];
		}
		else {
			Vertices[gPickedIndex].Position[0] = -mouPos[0];
			Vertices[gPickedIndex].Position[1] = -mouPos[1];
		}
		createBspline();
		createBéziercurves();
		createCRcurves();
	}
}

void doubleView(void) {
	if (!key4ForDoubleView) {
		isDoubleView = true;
		key4ForDoubleView = true;
	}
	else {
		isDoubleView = false;
		key4ForDoubleView = false;
	}
}

void littleTri(void) {
	Vertices[860].Position[0] = Vertices[index].Position[0];
	Vertices[860].Position[1] = Vertices[index].Position[1];
	Vertices[860].Position[2] = Vertices[index].Position[2];
	if (index == 859) {
		current = glm::vec3(Vertices[index].Position[0], Vertices[index].Position[1], Vertices[index].Position[2]);
		next = glm::vec3(Vertices[690].Position[0], Vertices[690].Position[1], Vertices[690].Position[2]);
	}
	else {
		current = glm::vec3(Vertices[index].Position[0], Vertices[index].Position[1], Vertices[index].Position[2]);
		next = glm::vec3(Vertices[index + 1].Position[0], Vertices[index + 1].Position[1]
			, Vertices[index + 1].Position[2]);
	}

	add = glm::vec3(next.x + current.x, next.y + current.y, next.z + current.z);

	minus = glm::vec3(next.x - current.x, next.y - current.y, next.z - current.z);
	
	vec3 T = normalize(minus);
	vec3 B = normalize(cross(T, add));
	vec3 N = normalize(cross(B, T));

	if (!vecPoint) {
		Vertices[861] = { { 0.0f, 0.0f, 0.0f, 1.0f }, { 1.0f, 0.0f, 0.0f, 1.0f } }; // T
		Vertices[862] = { { 0.0f, 0.0f, 0.0f, 1.0f }, { 0.0f, 1.0f, 0.0f, 1.0f } }; // N
		Vertices[863] = { { 0.0f, 0.0f, 0.0f, 1.0f }, { 0.0f, 0.0f, 1.0f, 1.0f } }; // B
		vecPoint = true;
	}
	Vertices[861].Position[0] = Vertices[index].Position[0] - T.x / 3;
	Vertices[861].Position[1] = Vertices[index].Position[1] - T.y / 3;
	Vertices[861].Position[2] = Vertices[index].Position[2] - T.z / 3;

	Vertices[862].Position[0] = Vertices[index].Position[0] - N.x / 3;
	Vertices[862].Position[1] = Vertices[index].Position[1] - N.y / 3;
	Vertices[862].Position[2] = Vertices[index].Position[2] - N.z / 3;

	Vertices[863].Position[0] = Vertices[index].Position[0] - B.x / 3;
	Vertices[863].Position[1] = Vertices[index].Position[1] - B.y / 3;
	Vertices[863].Position[2] = Vertices[index].Position[2] - B.z / 3;

	index++;
	if (index > 859) {
		index = 690;
	}
}		

void renderScene(void) {    
	// Dark blue background
	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);
	// Re-clear the screen for visible rendering
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (isDoubleView) {
		glViewport(0, window_height / 2, window_width, window_height / 2);
		gViewMatrix = glm::lookAt(
			glm::vec3(0, 0, -5),
			glm::vec3(0, 0, 0),
			glm::vec3(0, 1, 0)
		);

		glUseProgram(programID);
		{
			// see comments in pick
			glm::mat4 ModelMatrix = glm::mat4(1.0);
			glm::mat4 MVP = gProjectionMatrix * gViewMatrix * ModelMatrix;
			glUniform1f(pointSizeID, pointSize);
			glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
			glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
			glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &gViewMatrix[0][0]);

			glEnable(GL_PROGRAM_POINT_SIZE);

			glBindVertexArray(VertexArrayId[0]);	// Draw Vertices
			glBufferSubData(GL_ARRAY_BUFFER, 0, VertexBufferSize[0], Vertices);		// Update buffer data
			glDrawElements(GL_POINTS, 660, GL_UNSIGNED_SHORT, (void*)0);

			//cameraMatrix();

			// Draw the line
			if (drawCRLine) {
				glBindBuffer(GL_ARRAY_BUFFER, VertexBufferId[0]);

				for (int i = 660; i < 690; i++) {
					indices.push_back(i);
				}
				indices.push_back(660);
				glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLushort), indices.data(), GL_STATIC_DRAW);
				glDrawElements(GL_POINTS, indices.size(), GL_UNSIGNED_SHORT, (void*)0);
				glDrawElements(GL_LINE_STRIP, 31, GL_UNSIGNED_SHORT, (void*)0);


				for (int i = 690; i < 860; i++) {
					indices2.push_back(i);
				}
				indices2.push_back(690);
				glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices2.size() * sizeof(GLushort), indices2.data(), GL_STATIC_DRAW);
				glDrawElements(GL_LINE_STRIP, 171, GL_UNSIGNED_SHORT, (void*)0);


				for (int i = 0; i < 10; i++) {
					indices3.push_back(i);
				}
				glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices3.size() * sizeof(GLushort), indices3.data(), GL_STATIC_DRAW);
				glDrawElements(GL_POINTS, indices3.size(), GL_UNSIGNED_SHORT, (void*)0);

			}
			if (runLittleTri) {
				indices4.push_back(860);
				glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices4.size() * sizeof(GLushort), indices4.data(), GL_STATIC_DRAW);
				glDrawElements(GL_POINTS, indices4.size(), GL_UNSIGNED_SHORT, (void*)0);

				indicesT.push_back(860);
				indicesT.push_back(861);
				glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesT.size() * sizeof(GLushort), indicesT.data(), GL_STATIC_DRAW);
				glDrawElements(GL_LINE_STRIP, 2, GL_UNSIGNED_SHORT, (void*)0);

				indicesN.push_back(860);
				indicesN.push_back(862);
				glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesN.size() * sizeof(GLushort), indicesN.data(), GL_STATIC_DRAW);
				glDrawElements(GL_LINE_STRIP, 2, GL_UNSIGNED_SHORT, (void*)0);

				indicesB.push_back(860);
				indicesB.push_back(863);
				glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesB.size() * sizeof(GLushort), indicesB.data(), GL_STATIC_DRAW);
				glDrawElements(GL_LINE_STRIP, 2, GL_UNSIGNED_SHORT, (void*)0);
			}


			// If don't use indices
			// glDrawArrays(GL_POINTS, 0, NumVerts[0]);	

			// ATTN: OTHER BINDING AND DRAWING COMMANDS GO HERE
			// one set per object:
			// glBindVertexArray(VertexArrayId[<x>]); etc etc

			// ATTN: Project 1C, Task 2 == Refer to https://learnopengl.com/Getting-started/Transformations and
			// https://learnopengl.com/Getting-started/Coordinate-Systems - draw all the objects associated with the
			// curve twice in the displayed fashion using the appropriate transformations

			glBindVertexArray(0);
		}
		glUseProgram(0);

		glViewport(0, 0, window_width, window_height / 2);
		gViewMatrix = glm::lookAt(
			glm::vec3(-5, 0, 0),
			glm::vec3(0, 0, 0),
			glm::vec3(0, 1, 0)
		);

	}
	else {
		glViewport(0, 0, window_width, window_height);
		gViewMatrix = glm::lookAt(
			glm::vec3(0, 0, -5),
			glm::vec3(0, 0, 0),
			glm::vec3(0, 1, 0)
		);
	}

	glUseProgram(programID);
	{
		// see comments in pick
		glm::mat4 ModelMatrix = glm::mat4(1.0);
		glm::mat4 MVP = gProjectionMatrix * gViewMatrix * ModelMatrix;
		glUniform1f(pointSizeID, pointSize);
		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
		glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
		glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &gViewMatrix[0][0]);

		glEnable(GL_PROGRAM_POINT_SIZE);

		glBindVertexArray(VertexArrayId[0]);	// Draw Vertices
		glBufferSubData(GL_ARRAY_BUFFER, 0, VertexBufferSize[0], Vertices);		// Update buffer data
		glDrawElements(GL_POINTS, 660, GL_UNSIGNED_SHORT, (void*)0);

		//cameraMatrix();

		// Draw the line
		if (drawCRLine) {
			glBindBuffer(GL_ARRAY_BUFFER, VertexBufferId[0]);

			for (int i = 660; i < 690; i++) {
				indices.push_back(i);
			}
			indices.push_back(660);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLushort), indices.data(), GL_STATIC_DRAW);
			glDrawElements(GL_POINTS, indices.size(), GL_UNSIGNED_SHORT, (void*)0);
			glDrawElements(GL_LINE_STRIP, 31, GL_UNSIGNED_SHORT, (void*)0);


			for (int i = 690; i < 860; i++) {
				indices2.push_back(i);
			}
			indices2.push_back(690);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices2.size() * sizeof(GLushort), indices2.data(), GL_STATIC_DRAW);
			glDrawElements(GL_LINE_STRIP, 171, GL_UNSIGNED_SHORT, (void*)0);


			for (int i = 0; i < 10; i++) {
				indices3.push_back(i);
			}
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices3.size() * sizeof(GLushort), indices3.data(), GL_STATIC_DRAW);
			glDrawElements(GL_POINTS, indices3.size(), GL_UNSIGNED_SHORT, (void*)0);

		}
		if (runLittleTri) {
			indices4.push_back(860);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices4.size() * sizeof(GLushort), indices4.data(), GL_STATIC_DRAW);
			glDrawElements(GL_POINTS, indices4.size(), GL_UNSIGNED_SHORT, (void*)0);

			indicesT.push_back(860);
			indicesT.push_back(861);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesT.size() * sizeof(GLushort), indicesT.data(), GL_STATIC_DRAW);
			glDrawElements(GL_LINE_STRIP, 2, GL_UNSIGNED_SHORT, (void*)0);

			indicesN.push_back(860);
			indicesN.push_back(862);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesN.size() * sizeof(GLushort), indicesN.data(), GL_STATIC_DRAW);
			glDrawElements(GL_LINE_STRIP, 2, GL_UNSIGNED_SHORT, (void*)0);

			indicesB.push_back(860);
			indicesB.push_back(863);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesB.size() * sizeof(GLushort), indicesB.data(), GL_STATIC_DRAW);
			glDrawElements(GL_LINE_STRIP, 2, GL_UNSIGNED_SHORT, (void*)0);
		}


		// If don't use indices
		// glDrawArrays(GL_POINTS, 0, NumVerts[0]);	

		// ATTN: OTHER BINDING AND DRAWING COMMANDS GO HERE
		// one set per object:
		// glBindVertexArray(VertexArrayId[<x>]); etc etc

		// ATTN: Project 1C, Task 2 == Refer to https://learnopengl.com/Getting-started/Transformations and
		// https://learnopengl.com/Getting-started/Coordinate-Systems - draw all the objects associated with the
		// curve twice in the displayed fashion using the appropriate transformations

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

// Alternative way of triggering functions on mouse click and keyboard events
static void mouseCallback(GLFWwindow* window, int button, int action, int mods) {
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		pickVertex();
	}
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
		glBindBuffer(GL_ARRAY_BUFFER, VertexBufferId[0]);
		glClearColor(0.0f, 0.0f, 0.4f, 0.0f);
		Vertices[gPickedIndex].Color[0] = originalColorR;
		Vertices[gPickedIndex].Color[1] = originalColorG;
		Vertices[gPickedIndex].Color[2] = originalColorB;
		Vertices[gPickedIndex].Color[3] = 1.0f;
	}
}

static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (key == GLFW_KEY_1 && action == GLFW_PRESS) {
		drawCRLine = false;
		if (!alreadyCreate) {
			createBspline();
			createVAOs(Vertices, Indices, 0);
		}
		showBspline();
		createVAOs(Vertices, Indices, 0);
	}
	if (key == GLFW_KEY_1 && action == GLFW_RELEASE) {
		for (int i = M - N * 3 / 2; i < M - N; i++) {
			if (i >= 10) {
				Indices[i] = NULL;
			}
		}
	}

	if (key == GLFW_KEY_2 && action == GLFW_PRESS) {
		drawCRLine = false;
		if (!alreadyCreate2) {
			createBéziercurves();
			createVAOs(Vertices, Indices, 0);
		}
		showBéziercurves();
		createVAOs(Vertices, Indices, 0);
	}

	if (key == GLFW_KEY_3 && action == GLFW_PRESS) {
		if (!drawCRLine) {
			if (!alreadyCreate3) {
				createCRcurves();
				createVAOs(Vertices, Indices, 0);
			}
			showCRcurves();
			drawCRLine = true;
			createVAOs(Vertices, Indices, 0);
		}
		else {
			drawCRLine = false;
		}
	}

	if (key == GLFW_KEY_4 && action == GLFW_PRESS) {
		doubleView();
	}

}

int main(void) {
	// ATTN: REFER TO https://learnopengl.com/Getting-started/Creating-a-window
	// AND https://learnopengl.com/Getting-started/Hello-Window to familiarize yourself with the initialization of a window in OpenGL

	// Initialize window
	int errorCode = initWindow();
	if (errorCode != 0)
		return errorCode;

	// ATTN: REFER TO https://learnopengl.com/Getting-started/Hello-Triangle to familiarize yourself with the graphics pipeline
	// from setting up your vertex data in vertex shaders to rendering the data on screen (everything that follows)

	// Initialize OpenGL pipeline
	initOpenGL();

	double lastTime = glfwGetTime();
	int nbFrames = 0;

	createObjects(); // re-evaluate curves in case vertices have been moved

	do {
		// Timing 
		double currentTime = glfwGetTime();
		nbFrames++;
		if (currentTime - lastTime >= 1.0){ // If last prinf() was more than 1sec ago
			printf("%f ms/frame\n", 1000.0 / double(nbFrames));
			nbFrames = 0;
			lastTime += 1.0;
		}

		// Use the middle button to increase point size 
		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE)) {
			changeSize();
		}

		// Use the right button to decrease point size
		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT)) {
			changeSize();
		}

		// DRAGGING: move current (picked) vertex with cursor
		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT)) {
			moveVertex();
		}




		/*int key1State = glfwGetKey(window, GLFW_KEY_1);
		if (key1State == GLFW_PRESS) {
			drawCRLine = false;
			if (!key1AlreadyPressed) {
				key1AlreadyPressed = true;
				if (!alreadyCreate) {
					createBspline();
					createVAOs(Vertices, Indices, 0);
				}
				showBspline();
				createVAOs(Vertices, Indices, 0);
			}
		}
		if (key1State == GLFW_RELEASE) {
			if (key1AlreadyPressed) {
				for (int i = M - N * 3 / 2; i < M - N; i++) {
					if (i >= 10) {
						Indices[i] = NULL;
					}
				}
				key1AlreadyPressed = false;
			}
		}

		int key2State = glfwGetKey(window, GLFW_KEY_2);
		if (key2State == GLFW_PRESS) {
			drawCRLine = false;
			if (!key2AlreadyPressed) {
				key2AlreadyPressed = true;
				if (!alreadyCreate2) {
					createBéziercurves();
					createVAOs(Vertices, Indices, 0);
				}
				showBéziercurves();
				createVAOs(Vertices, Indices, 0);
			}
		}
		if (key2State == GLFW_RELEASE) {
			if (key2AlreadyPressed) {
				key2AlreadyPressed = false;
			}
		}

		int key3State = glfwGetKey(window, GLFW_KEY_3);
		if (key3State == GLFW_PRESS)
		{
			if (!key3AlreadyPressed) {
				key3AlreadyPressed = true;
				if (!alreadyCreate3) {
					createCRcurves();
					createVAOs(Vertices, Indices, 0);
				}
				showCRcurves();
				drawCRLine = true;
				createVAOs(Vertices, Indices, 0);
			}
		}
		if (key3State == GLFW_RELEASE) {
			if (key3AlreadyPressed) {
				key3AlreadyPressed = false;
				drawCRLine = false;
			}
		}
		*/

		// ATTN: Project 1B, Task 2 and 4 == account for key presses to activate subdivision and hiding/showing functionality
		// for respective tasks
		int keyShiState = glfwGetKey(window, GLFW_KEY_LEFT_SHIFT);
		if (keyShiState == GLFW_PRESS)
		{
			keyShiAlreadyPressed = true;
		}
		if (keyShiState == GLFW_RELEASE) {
			if (keyShiAlreadyPressed) {
				keyShiAlreadyPressed = false;
			}
		}

		int key5State = glfwGetKey(window, GLFW_KEY_5);
		if (key5State == GLFW_PRESS)
		{
			key5AlreadyPressed = true;
			runLittleTri = true;
			littleTri();
			createVAOs(Vertices, Indices, 0);
		}
		if (key5State == GLFW_RELEASE) {
			if (key5AlreadyPressed) {
				indices[860] = NULL;
				runLittleTri = false;
				key5AlreadyPressed = false;
			}
		}


		// DRAWING the SCENE
		renderScene();

	} // Check if the ESC key was pressed or the window was closed
	while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
	glfwWindowShouldClose(window) == 0);

	cleanup();

	return 0;
}

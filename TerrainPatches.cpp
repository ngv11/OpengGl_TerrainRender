//  ========================================================================
//  COSC363: Computer Graphics (2022);  University of Canterbury.
//  FILE NAME: TerrainPatches.cpp
//
//	The program generates and loads the mesh data for a terrain floor (100 verts, 81 elems).
//  This program requires the following files:
//         TerrainPatches.vert, TerrainPatches.frag
//         TerrainPatches.cont, TerrainPatches.eval
//  ========================================================================

#include <iostream>
#include <fstream>
#include <sstream>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "loadTGA.h" 
using namespace std;

#define SPACEBAR 32
#define ONE_KEYBOARD 49
#define TWO_KEYBOARD 50
#define a_KEYBOARD 97
#define q_KEYBOARD 113
#define w_KEYBOARD 119
#define s_KEYBOARD 115

bool model_wire = false;
string terrain_choice = "Terrain_hm_01.tga";
string terrain_choice1 = "Terrain_hm_01.tga";
string terrain_choice2 = "MtRuapehu.tga";

GLuint vaoID;
GLuint theProgram;
GLuint mvpMatrixLoc, camLoc, mvMatrixLoc, norMatrixLoc, lgtLoc, tickLoc;

float  eye_x = 0, eye_y  = 20, eye_z = 30;      //Initial camera position
float look_x = 0, look_y = 0, look_z = -20;     //"Look-at" point along -z direction
float toRad  = 3.14159265/180.0;                //Conversion from degrees to rad
float angle = 0;
float waterLevel = 3.3;
float snowLevel = 7.6;

int tick = 0;

// camera 
glm::vec3 cameraPos   = glm::vec3(0.0f, 20.0f, 30.0f); //Initial camera position
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -40.0f); //"Look-at" point along -z direction
glm::vec3 cameraUp    = glm::vec3(0.0f, 1.0f, 0.0f);


// mosue currently unused
bool firstMouse = true;
int lastX       = 0;
int lastY       = 0;
float yaw       = -40.0f;
float pitch     = 0.0f;

float verts[100*3];       //10x10 grid (100 vertices)
GLushort elems[81*4];     //Element array for 9x9 = 81 quad patches

glm::mat4 projView;
glm::vec3 lookin;

//Generate vertex and element data for the terrain floor
void generateData()
{
	int indx, start;

	//verts array (100 vertices on a 10x10 grid)
	for (int j = 0; j < 10; j++)   //z-direction
	{
		for (int i = 0; i < 10; i++)  //x-direction
		{
			indx = 10 * j + i;
			verts[3 * indx] = 10 * i - 45;		//x
			verts[3 * indx + 1] = 0;			//y
			verts[3 * indx + 2] = -10 * j;		//z
		}
	}

	//elems array
	indx = 0;
	for (int j = 0; j < 9; j++)
	{
		for (int i = 0; i < 9; i++)
		{
			start = 10 * j + i;
			elems[indx] = start;
			elems[indx + 1] = start + 1;
			elems[indx + 2] = start + 11;
			elems[indx + 3] = start + 10;
			indx += 4;
		}
	}
}

//Loads height map
void loadTexture()
{
	GLuint texID;
    glGenTextures(1, &texID);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texID);
	loadTGA(terrain_choice);

    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	GLuint grassID;
	glGenTextures(1, &grassID);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, grassID);
	loadTGA("Grass.tga");

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	GLuint waterID;
	glGenTextures(1, &waterID);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, waterID);
	loadTGA("Water.tga");

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	GLuint snowID;
	glGenTextures(1, &snowID);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, snowID);
	loadTGA("Snow.tga");

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	GLuint rockID;
	glGenTextures(1, &rockID);
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, rockID);
	loadTGA("Rock1.tga");

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	// glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	//glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

}

//Loads a shader file and returns the reference to a shader object
GLuint loadShader(GLenum shaderType, string filename)
{
	ifstream shaderFile(filename.c_str());
	if(!shaderFile.good()) cout << "Error opening shader file." << endl;
	stringstream shaderData;
	shaderData << shaderFile.rdbuf();
	shaderFile.close();
	string shaderStr = shaderData.str();
	const char* shaderTxt = shaderStr.c_str();

	GLuint shader = glCreateShader(shaderType);
	glShaderSource(shader, 1, &shaderTxt, NULL);
	glCompileShader(shader);
	GLint status;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE)
	{
		GLint infoLogLength;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLength);
		GLchar *strInfoLog = new GLchar[infoLogLength + 1];
		glGetShaderInfoLog(shader, infoLogLength, NULL, strInfoLog);
		const char *strShaderType = NULL;
		cerr <<  "Compile failure in shader: " << strInfoLog << endl;
		delete[] strInfoLog;
	}
	return shader;
}

//Initialise the shader program, create and load buffer data
void initialise()
{
//--------Load terrain height map-----------
	loadTexture();
//--------Load shaders----------------------
	GLuint shaderv = loadShader(GL_VERTEX_SHADER, "TerrainPatches.vert");
	GLuint shaderf = loadShader(GL_FRAGMENT_SHADER, "TerrainPatches.frag");
	GLuint shaderc = loadShader(GL_TESS_CONTROL_SHADER, "TerrainPatches.cont");
	GLuint shadere = loadShader(GL_TESS_EVALUATION_SHADER, "TerrainPatches.eval");
	GLuint shaderg = loadShader(GL_GEOMETRY_SHADER, "TerrainPatches.geom");

	GLuint program = glCreateProgram();
	glAttachShader(program, shaderv);
	glAttachShader(program, shaderf);
	glAttachShader(program, shaderc);
	glAttachShader(program, shadere);
	glAttachShader(program, shaderg);

	glLinkProgram(program);

	GLint status;
	glGetProgramiv (program, GL_LINK_STATUS, &status);
	if (status == GL_FALSE)
	{
		GLint infoLogLength;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLogLength);
		GLchar *strInfoLog = new GLchar[infoLogLength + 1];
		glGetProgramInfoLog(program, infoLogLength, NULL, strInfoLog);
		fprintf(stderr, "Linker failure: %s\n", strInfoLog);
		delete[] strInfoLog;
	}
	glUseProgram(program);

	mvMatrixLoc = glGetUniformLocation(program, "mvMatrix");
	mvpMatrixLoc = glGetUniformLocation(program, "mvpMatrix");
	norMatrixLoc = glGetUniformLocation(program, "norMatrix");
	lgtLoc = glGetUniformLocation(program, "lightPos");
	tickLoc = glGetUniformLocation(program, "tick");

	camLoc = glGetUniformLocation(program, "cameraPos");

	GLuint waterHeightLoc = glGetUniformLocation(program, "waterHeight");
	glUniform1f(waterHeightLoc, waterLevel);

	GLuint snowHeightLoc = glGetUniformLocation(program, "snowHeight");
	glUniform1f(snowHeightLoc, snowLevel);

	GLuint texLoc = glGetUniformLocation(program, "heightMap");
	glUniform1i(texLoc, 0);

	GLuint texLocGrass = glGetUniformLocation(program, "grassTerrain");
	glUniform1i(texLocGrass, 1);

	GLuint texLocWater = glGetUniformLocation(program, "waterTerrain");
	glUniform1i(texLocWater, 2);

	GLuint texLocSnow = glGetUniformLocation(program, "snowTerrain");
	glUniform1i(texLocSnow, 3);

	GLuint texLocRock = glGetUniformLocation(program, "rockTerrain");
	glUniform1i(texLocRock, 4);

//---------Load buffer data-----------------------
	generateData();

	GLuint vboID[2];
	glGenVertexArrays(1, &vaoID);
    glBindVertexArray(vaoID);

    glGenBuffers(2, vboID);

    glBindBuffer(GL_ARRAY_BUFFER, vboID[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(0);  // Vertex position

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboID[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elems), elems, GL_STATIC_DRAW);

    glBindVertexArray(0);

	glPatchParameteri(GL_PATCH_VERTICES, 4);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	if (model_wire == false)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
	else
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
}

// Handles all the camera movement corresponding to keyboard arrow inputs
void special(int key, int x, int y)
{

	float scale_forward_Back = 0.5;
	float scale_side = 0.05;
	if (key == GLUT_KEY_UP)
	{
		cameraPos += cameraFront * scale_forward_Back;
	} 
	if (key == GLUT_KEY_DOWN)
	{
		cameraPos -= cameraFront * scale_forward_Back;
	}
	if (key == GLUT_KEY_LEFT)
	{	
		angle -= 5;
	}
	if (key == GLUT_KEY_RIGHT)
	{
		angle += 5;
	}
	if (key == GLUT_KEY_F1)
	{
		cameraFront.y += 0.05;
		cameraFront = glm::normalize(cameraFront);
	}
	if (key == GLUT_KEY_F2)
	{	
		cameraFront.y -= 0.05;
		cameraFront = glm::normalize(cameraFront);
	}
	glutPostRedisplay();
}

void animate(int tick)
{
	tick++;
	glUniform1i(tickLoc, tick);
	glutTimerFunc(50, animate, tick);
	glutPostRedisplay();
}

void util(unsigned char key, int x, int y)
{
	if (key == SPACEBAR)
	{
		model_wire = !model_wire;
	}
	if (key == ONE_KEYBOARD)
	{
		terrain_choice = terrain_choice1;
	} 
	if (key == TWO_KEYBOARD)
	{
		terrain_choice = terrain_choice2;
	}
	if (key == q_KEYBOARD)
	{
		waterLevel += 0.1;
		if (waterLevel > 4)
		{
			waterLevel = 4;
		} 
		else if ((snowLevel - waterLevel) < 2.5)
		{
			waterLevel = waterLevel;
		}
	}
	if (key == a_KEYBOARD)
	{
		waterLevel -= 0.1;
	}
	if (key == w_KEYBOARD)
	{
		if (snowLevel > 9)
		{
			snowLevel = snowLevel;
		}
		else
		{
			snowLevel += 0.1;
		}
	}
	if (key == s_KEYBOARD)
	{
		if ((snowLevel - waterLevel) < 2.5)
		{
			snowLevel = snowLevel;
		} 
		else
		{
			snowLevel -= 0.1;
		}
	}

	initialise();
	glutPostRedisplay();
}

//Display function to compute uniform values based on transformation parameters and to draw the scene
void display()
{
	//glm::vec4 cameraPosn = glm::vec4(eye_x, eye_y, eye_z, 1.0);
	glUniform4fv(camLoc, 1, &cameraPos[0]);

	////--------Compute matrices----------------------
	glm::vec4 light = glm::vec4(-50.0, 100.0, 60.0, 1.0);
	glm::mat4 proj = glm::perspective(30.0f * toRad, 1.25f, 20.0f, 500.0f);  //perspective projection matrix
	glm::mat4 view = lookAt(cameraPos, cameraPos + cameraFront, cameraUp); //view matri
	projView = proj * view;  //Product matrix
	glm::mat4 mvpMatrix = glm::rotate(projView, angle * toRad, glm::vec3(0.0, 1.0, 0.0)); 
	glm::mat4 invMatrix = glm::inverse(view);
	glUniformMatrix4fv(mvMatrixLoc, 1, GL_FALSE, &view[0][0]);
	glUniformMatrix4fv(mvpMatrixLoc, 1, GL_FALSE, &mvpMatrix[0][0]);
	glUniformMatrix4fv(norMatrixLoc, 1, GL_TRUE, &invMatrix[0][0]);  //Use transpose matrix here

	glm::vec4 lightEye = view * light;
	glUniform4fv(lgtLoc, 1, &lightEye[0]);

	glUniform4fv(camLoc, 1, &cameraPos[0]);

	////--------Compute matrices----------------------
	//glm::vec4 light = glm::vec4(20.0, 10.0, 20.0, 1.0);

	//glm::mat4 proj = glm::perspective(30.0f * toRad, 1.25f, 20.0f, 500.0f);  //perspective projection matrix
	////glm::mat4 view = lookAt(glm::vec3(eye_x, eye_y, eye_z), glm::vec3(look_x, look_y, look_z), glm::vec3(0.0, 1.0, 0.0)); //view matri
	//
	//glm::mat4 view = lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
	////glm::mat4 view = lookAt(cameraPos, camera_look, cameraUp);

	//glm::mat4 mvMatrix = glm::rotate(view, angle * toRad, glm::vec3(1.0, 0.0, 0.0));  //rotation matrix

	////projView = proj * view;  //Product matrix

	//glm::mat4 mvpMatrix = proj * mvMatrix;   //The model-view-projection matrix
	//// glm::mat4 mvpMatrix = glm::rotate(projView, angle * toRad, glm::vec3(0.0, 1.0, 0.0)); old one before lighting

	//glm::vec4 lightEye = view * light;     //Light position in eye coordinates

	//glm::mat4 invMatrix = glm::inverse(mvMatrix);  //Inverse of model-view matrix for normal transformation

	//glUniformMatrix4fv(mvMatrixLoc, 1, GL_FALSE, &mvMatrix[0][0]);
	//glUniformMatrix4fv(mvpMatrixLoc, 1, GL_FALSE, &mvpMatrix[0][0]);
	//glUniformMatrix4fv(norMatrixLoc, 1, GL_TRUE, &invMatrix[0][0]);  //Use transpose matrix here
	//glUniform4fv(camLoc, 1, &cameraPos[0]);
	//glUniform4fv(lgtLoc, 1, &lightEye[0]);


	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	glBindVertexArray(vaoID);
	glDrawElements(GL_PATCHES, 81 * 4, GL_UNSIGNED_SHORT, NULL);
	glFlush();
}


int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_DEPTH);
	glutInitWindowSize(1000, 800);
	glutCreateWindow("Terrain");
	glutInitContextVersion (4, 2);
	glutInitContextProfile ( GLUT_CORE_PROFILE );

	if(glewInit() == GLEW_OK)
	{
		cout << "GLEW initialization successful! " << endl;
		cout << " Using GLEW version " << glewGetString(GLEW_VERSION) << endl;
	}
	else
	{
		cerr << "Unable to initialize GLEW  ...exiting." << endl;
		exit(EXIT_FAILURE);
	}

	initialise();
	//glutPassiveMotionFunc(look);
	animate(tick);
	glutKeyboardFunc(util);
	glutDisplayFunc(display); 
	glutSpecialFunc(special);
	glutMainLoop();
	return 0;
}

//void look(int x, int y)
//{
//	float xoffset = x - lastX;
//	float yoffset = lastY - y;
//
//	lastX = x;
//	lastY = y;
//
//	float sensitivity = 0.8f;
//	xoffset *= sensitivity;
//	yoffset *= sensitivity;
//
//	yaw += xoffset;
//	pitch += yoffset;
//
//	if (pitch > 89.0f)
//	{
//		pitch = 89.0f;
//	}
//	if (pitch < -89.0f)
//	{
//		pitch = -89.0f;
//	}
//
//	glm::vec3 front;
//	front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
//	front.x = sin(glm::radians(pitch));
//	front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
//	cameraFront = glm::normalize(front);
//}


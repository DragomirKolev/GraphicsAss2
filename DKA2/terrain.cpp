/*
Terrain example
Creates a class to define a terrain with an array of vertices and calculates vertex normalsd
Iain Martin October 2014
*/


/* Link to static libraries, could define these as linker inputs in the project settings instead
if you prefer */
#pragma comment(lib, "glfw3.lib")
#pragma comment(lib, "glloadD.lib")
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "libnoise.lib")

/* Include the header to the GLFW wrapper class which
also includes the OpenGL extension initialisation*/
#include "wrapper_glfw.h"
#include <iostream>

/* Include GLM core and matrix extensions*/
#include <glm/glm.hpp>
#include "glm/gtc/matrix_transform.hpp"
#include <glm/gtc/type_ptr.hpp>
#include "object_ldr.h"
#include "terrain_object.h"
GLdouble tempScaleTerrain, scaleTerrain = 15.0;
GLdouble tempFrequencyForTerrain, FrequencyForTerrain = 1;
int SizeOfTerrain, tempSizeOfTerrain = 1;
/* Define buffer object indices */
GLuint positionBufferObject, colourObject, normalsBufferObject;
GLuint sphereBufferObject, sphereNormals, sphereColours, sphereTexCoords;
GLuint elementbuffer;
object_ldr monkey;

bool keys[1024];

glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 cameraDirection = glm::normalize(cameraPos - cameraTarget);
glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
glm::vec3 cameraRight = glm::normalize(glm::cross(up, cameraDirection));
//glm::vec3 cameraUp = glm::cross(cameraDirection, cameraRight);

glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);



GLuint program;		/* Identifier for the shader prgoram */
GLuint vao;			/* Vertex array (Containor) object. This is the index of the VAO that will be the container for
					our buffer objects */

GLuint colourmode;	/* Index of a uniform to switch the colour mode in the vertex shader
					I've included this to show you how to pass in an unsigned integer into
					your vertex shader. */

/* Position and view globals */
GLfloat angle_x, angle_inc_x, x, scale, z, y,sampleTerrain;
GLfloat angle_y, angle_inc_y, angle_z, angle_inc_z;
GLuint drawmode;			// Defines drawing mode of sphere as points, lines or filled polygons
GLuint numlats, numlongs;	//Define the resolution of the sphere object

/* Uniforms*/
GLuint modelID, viewID, projectionID;
GLuint colourmodeID;

GLfloat aspect_ratio;		/* Aspect ratio of the window defined in the reshape callback*/
GLuint numspherevertices;



terrain_object *heightfield;
int octaves;
GLfloat perlin_scale, perlin_frequency;
GLfloat land_size;


/*
This function is called before entering the main rendering loop.
Use it for all your initialisation stuff
*/
void init(GLWrapper *glw)
{
	/* Set the object transformation controls to their initial values */
	x = -0.55f;
	y = 0;
	z = 0;
	angle_x = 295.f;
	angle_y = angle_z = 0;
	angle_inc_x = angle_inc_y = angle_inc_z = 0;
	scale = 0.15f;
	aspect_ratio = 1.3333f;
	colourmode = 0;
	numlats = 20;		// Number of latitudes in our sphere
	numlongs = 20;		// Number of longitudes in our sphere

	// Generate index (name) for one vertex array object
	glGenVertexArrays(1, &vao);

	// Create the vertex array object and make it current
	glBindVertexArray(vao);

	/* Define vertices for a cube in 12 triangles */
	GLfloat vertexPositions[] =
	{
		-0.25f, 0.25f, -0.25f,
		-0.25f, -0.25f, -0.25f,
		0.25f, -0.25f, -0.25f,

		0.25f, -0.25f, -0.25f,
		0.25f, 0.25f, -0.25f,
		-0.25f, 0.25f, -0.25f,

		0.25f, -0.25f, -0.25f,
		0.25f, -0.25f, 0.25f,
		0.25f, 0.25f, -0.25f,

		0.25f, -0.25f, 0.25f,
		0.25f, 0.25f, 0.25f,
		0.25f, 0.25f, -0.25f,

		0.25f, -0.25f, 0.25f,
		-0.25f, -0.25f, 0.25f,
		0.25f, 0.25f, 0.25f,

		-0.25f, -0.25f, 0.25f,
		-0.25f, 0.25f, 0.25f,
		0.25f, 0.25f, 0.25f,

		-0.25f, -0.25f, 0.25f,
		-0.25f, -0.25f, -0.25f,
		-0.25f, 0.25f, 0.25f,

		-0.25f, -0.25f, -0.25f,
		-0.25f, 0.25f, -0.25f,
		-0.25f, 0.25f, 0.25f,

		-0.25f, -0.25f, 0.25f,
		0.25f, -0.25f, 0.25f,
		0.25f, -0.25f, -0.25f,

		0.25f, -0.25f, -0.25f,
		-0.25f, -0.25f, -0.25f,
		-0.25f, -0.25f, 0.25f,

		-0.25f, 0.25f, -0.25f,
		0.25f, 0.25f, -0.25f,
		0.25f, 0.25f, 0.25f,

		0.25f, 0.25f, 0.25f,
		-0.25f, 0.25f, 0.25f,
		-0.25f, 0.25f, -0.25f,
	};

	/* Manually specified colours for our cube */
	float vertexColours[] = {
		0.0f, 0.0f, 1.0f, 1.0f,
		0.0f, 0.0f, 1.0f, 1.0f,
		0.0f, 0.0f, 1.0f, 1.0f,
		0.0f, 0.0f, 1.0f, 1.0f,
		0.0f, 0.0f, 1.0f, 1.0f,
		0.0f, 0.0f, 1.0f, 1.0f,

		0.0f, 1.0f, 0.0f, 1.0f,
		0.0f, 1.0f, 0.0f, 1.0f,
		0.0f, 1.0f, 0.0f, 1.0f,
		0.0f, 1.0f, 0.0f, 1.0f,
		0.0f, 1.0f, 0.0f, 1.0f,
		0.0f, 1.0f, 0.0f, 1.0f,

		1.0f, 1.0f, 0.0f, 1.0f,
		1.0f, 1.0f, 0.0f, 1.0f,
		1.0f, 1.0f, 0.0f, 1.0f,
		1.0f, 1.0f, 0.0f, 1.0f,
		1.0f, 1.0f, 0.0f, 1.0f,
		1.0f, 1.0f, 0.0f, 1.0f,

		1.0f, 0.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 0.0f, 1.0f,

		1.0f, 0.0f, 1.0f, 1.0f,
		1.0f, 0.0f, 1.0f, 1.0f,
		1.0f, 0.0f, 1.0f, 1.0f,
		1.0f, 0.0f, 1.0f, 1.0f,
		1.0f, 0.0f, 1.0f, 1.0f,
		1.0f, 0.0f, 1.0f, 1.0f,

		0.0f, 1.0f, 1.0f, 1.0f,
		0.0f, 1.0f, 1.0f, 1.0f,
		0.0f, 1.0f, 1.0f, 1.0f,
		0.0f, 1.0f, 1.0f, 1.0f,
		0.0f, 1.0f, 1.0f, 1.0f,
		0.0f, 1.0f, 1.0f, 1.0f,
	};

	/* Manually specified normals for our cube */
	GLfloat normals[] =
	{
		0, 0, -1.f, 0, 0, -1.f, 0, 0, -1.f,
		0, 0, -1.f, 0, 0, -1.f, 0, 0, -1.f,
		1.f, 0, 0, 1.f, 0, 0, 1.f, 0, 0,
		1.f, 0, 0, 1.f, 0, 0, 1.f, 0, 0,
		0, 0, 1.f, 0, 0, 1.f, 0, 0, 1.f,
		0, 0, 1.f, 0, 0, 1.f, 0, 0, 1.f,
		-1.f, 0, 0, -1.f, 0, 0, -1.f, 0, 0,
		-1.f, 0, 0, -1.f, 0, 0, -1.f, 0, 0,
		0, -1.f, 0, 0, -1.f, 0, 0, -1.f, 0,
		0, -1.f, 0, 0, -1.f, 0, 0, -1.f, 0,
		0, 1.f, 0, 0, 1.f, 0, 0, 1.f, 0,
		0, 1.f, 0, 0, 1.f, 0, 0, 1.f, 0,
	};

	GLfloat texcoords[] =
	{
		0, 1.f, 0, 0, 1.f, 0,
		1.f, 0, 1.f, 1.f, 0, 1.f,
		1.f, 0, 1.f, 0, 1.f, 1.f,
		0, 0, 0, 1.f, 1.f, 0,
		0, 1.f, 1.f, 1.f, 1.f, 0,
		1.f, 0, 0, 0, 1.f, 1.f,
		0, 0, 0, 1.f, 1.f, 1.f,
		0, 1.f, 0, 0, 1.f, 1.f,
		0, 0, 1.f, 0, 1.f, 1.f,
		0, 1.f, 1.f, 1.f, 1.f, 0,
		1.f, 0, 0, 0, 0, 1.f,
		0, 0, 1.f, 0, 1.f, 1.f,
		1.f, 1.f, 0, 1.f, 0, 0
	};


	/* Create the vertex buffer for the cube */
	glGenBuffers(1, &positionBufferObject);
	glBindBuffer(GL_ARRAY_BUFFER, positionBufferObject);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexPositions), vertexPositions, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	/* Create the colours buffer for the cube */
	glGenBuffers(1, &colourObject);
	glBindBuffer(GL_ARRAY_BUFFER, colourObject);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexColours), vertexColours, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	/* Create the normals  buffer for the cube */
	glGenBuffers(1, &normalsBufferObject);
	glBindBuffer(GL_ARRAY_BUFFER, normalsBufferObject);
	glBufferData(GL_ARRAY_BUFFER, 36 * sizeof(glm::vec3), normals, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	/* Load and create our monkey object*/
	monkey.load_obj("Monkey.obj");

	/* Calculate vertex normals using cross products from the surrounding faces*/
	/* A better way to do this would be to generate the vertex normals in Blender and
	/* then extract the vertex normals from the face definitions and use that to create an
	/* accurate array of veretx normals */
	monkey.smoothNormals();
	monkey.createObject();


	/* Create the heightfield object */
	octaves = 1;
	perlin_scale = 2.f;
	perlin_frequency = 1.f;
	land_size = 50.f;
	heightfield = new terrain_object(octaves, perlin_frequency, perlin_scale);
	heightfield->createTerrain(200, 200, land_size, land_size);
	heightfield->createObject();

	/* Load and build the vertex and fragment shaders */
	try
	{
		program = glw->LoadShader("terrain.vert", "terrain.frag");
	}
	catch (std::exception &e)
	{
		std::cout << "Caught exception: " << e.what() << std::endl;
		std::cin.ignore();
		exit(0);
	}

	/* Define uniforms to send to vertex shader */
	modelID = glGetUniformLocation(program, "model");
	colourmodeID = glGetUniformLocation(program, "colourmode");
	viewID = glGetUniformLocation(program, "view");
	projectionID = glGetUniformLocation(program, "projection");



	//display controls in the cosole
	std::cout << "Move in and out of the terrain with A - S" << std::endl;
	std::cout << "Change the frequency with Q - W (if the frequency is 0, you will not get any terrain!)" << std::endl;
	std::cout << "Press M to put color on and off on the objects " << std::endl;
	std::cout << "Press N to show the vertecies on the objects " << std::endl;



}

/* Called to update the display. Note that this function is called in the event loop in the wrapper
class because we registered display as a callback function */
void display()
{
	/* Define the background colour */
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	/* Clear the colour and frame buffers */
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	/* Enable depth test  */
	glEnable(GL_DEPTH_TEST);

	/* Make the compiled shader program current */
	glUseProgram(program);

	/* Bind cube vertices. Note that this is in attribute index 0 */
	glBindBuffer(GL_ARRAY_BUFFER, positionBufferObject);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	/* Bind cube colours. Note that this is in attribute index 1 */
	glBindBuffer(GL_ARRAY_BUFFER, colourObject);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, 0);

	/* Bind cube normals. Note that this is in attribute index 2 */
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, normalsBufferObject);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	// Define the model transformations for the monkey
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(x + 2.3, y +1.2, z));
	model = glm::scale(model, glm::vec3(0.1, 0.1, 0.1));//scale equally in all axis
	model = glm::rotate(model, 20.0f, glm::vec3(1, 0, 0)); //rotating in clockwise direction around x-axis
	model = glm::rotate(model, -30.0f, glm::vec3(0, 1, 0)); //rotating in clockwise direction around y-axis
	model = glm::rotate(model, 90.0f, glm::vec3(0, 0, 1)); //rotating in clockwise direction around z-axis

	// Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
	glm::mat4 Projection = glm::perspective(30.0f, aspect_ratio, 0.1f, 100.0f);



	GLfloat radius = 10.0f;
	GLfloat camX = sin(glfwGetTime()) * radius;
	GLfloat camZ = cos(glfwGetTime()) * radius;



	// Camera matrix
	glm::mat4 View = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);



	// Send our uniforms variables to the currently bound shader,
	glUniformMatrix4fv(modelID, 1, GL_FALSE, &model[0][0]);
	glUniform1ui(colourmodeID, colourmode);
	glUniformMatrix4fv(viewID, 1, GL_FALSE, &View[0][0]);
	glUniformMatrix4fv(projectionID, 1, GL_FALSE, &Projection[0][0]);

	/* Draw our Blender Monkey object */
	monkey.drawObject();

	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(-x - 0.5, 0, 0));
	model = glm::scale(model, glm::vec3(scale / 3.f, scale / 3.f, scale / 3.f));//scale equally in all axis
	model = glm::rotate(model, -angle_x - 20.f, glm::vec3(1, 0, 0)); //rotating in clockwise direction around x-axis
	model = glm::rotate(model, -angle_y, glm::vec3(0, 1, 0)); //rotating in clockwise direction around y-axis
	model = glm::rotate(model, -angle_z, glm::vec3(0, 0, 1)); //rotating in clockwise direction around z-axis
	glUniformMatrix4fv(modelID, 1, GL_FALSE, &model[0][0]);

	//this function passes values to the terrain_object class so that it knows when something is modified, what values to use
	heightfield->getValuesFromTerrainClass(scaleTerrain,FrequencyForTerrain, SizeOfTerrain);
	// doesn't do anything if the scale is not changed 
	if (tempScaleTerrain != scaleTerrain){
		//if the scale is changed it recreates the new heightfield with the parameters passed
		delete heightfield;
		heightfield = new terrain_object(octaves, perlin_frequency, perlin_scale);
		//this is one of the more imporntant lines of code in this assignement ( SizeOfTerrain value is only being added to the current terrain, by doing that it increases the detail in the terrain ) 
		
		heightfield->createTerrain(200 + SizeOfTerrain, 200 + SizeOfTerrain, land_size, land_size);
		// the next like of code that is commented out, if it is used in place of the current one it will make the terrain more and more detaild everytime you zoom in.
		//(  CAUTION DO NOT ZOOM BACK OR IT WILL BREAK ! array goes out of bounds, this peace of code is there only to show how the detail can be increased by a lot!)
		//heightfield->createTerrain(200 + SizeOfTerrain*10, 200 + SizeOfTerrain*10, land_size, land_size);
		// Obviously the previous line of code is not optimal because it rerenders the whole terrain, ideally it would only render the verticies in the terrain that is in the field of view, which would make it faster.
		// Also having this in the tessalation shader would increase the speed by a lot since it would run in the GPU and not the CPU, but for this assignement there was not enough time to explore that option
		heightfield->createObject();
		std::cout << "scale";
		std::cout << scaleTerrain;
		
	}
	// This if statement runs only if the frequency of the terrain is changed, if it s then it recreates the terrain with the new frequency
	
	if (tempFrequencyForTerrain != FrequencyForTerrain){
		delete heightfield;
		heightfield = new terrain_object(octaves, perlin_frequency, perlin_scale);
		heightfield->createTerrain(200 + SizeOfTerrain, 200 + SizeOfTerrain, land_size, land_size);
		heightfield->createObject();
		std::cout << "frequency";
		std::cout << FrequencyForTerrain;
	}
	
	heightfield->drawObject(drawmode);
	tempScaleTerrain = scaleTerrain;
	tempFrequencyForTerrain = FrequencyForTerrain;
	tempSizeOfTerrain = SizeOfTerrain;

	glDisableVertexAttribArray(0);
	glUseProgram(0);

	/* Modify our animation variables */
	angle_x += angle_inc_x;
	angle_y += angle_inc_y;
	angle_z += angle_inc_z;
}

/* Called whenever the window is resized. The new window size is given, in pixels. */
static void reshape(GLFWwindow* window, int w, int h)
{
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	aspect_ratio = ((float)w / 640.f*4.f) / ((float)h / 480.f*3.f);
}

/* change view angle, exit upon ESC */
static void keyCallback(GLFWwindow* window, int key, int s, int action, int mods)
{
	
	if (action == GLFW_PRESS)
		keys[key] = true;
	else if (action == GLFW_RELEASE)
		keys[key] = false;

	// Camera controls
	GLfloat cameraSpeed = 0.1f;
	if (keys[GLFW_KEY_W])
		cameraPos += cameraSpeed * cameraFront;
	if (keys[GLFW_KEY_S])
		cameraPos -= cameraSpeed * cameraFront;
	if (keys[GLFW_KEY_A])
		cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	if (keys[GLFW_KEY_D])
		cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;

	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	//change the frequency
	//if (key == 'Q' && action != GLFW_PRESS){
	//	FrequencyForTerrain += 1;
	//}
	//if (key == 'W' && action != GLFW_PRESS){

	//	FrequencyForTerrain -= 1;
	//}
	//change the scale of the terrain and the detail of the mesh
	//if (key == 'A' && action != GLFW_PRESS){

//		scale -= 0.02f;
//		scaleTerrain -= 1.0;
	///	SizeOfTerrain -= 10;
//	}
//	if (key == 'S' && action != GLFW_PRESS){

//		scale += 0.02f;
//		scaleTerrain += 1.0;
//		SizeOfTerrain += 10;
//		
//	}
	//disable and enable colour 
	if (key == 'M' && action != GLFW_PRESS)
	{
		colourmode = !colourmode;
		std::cout << "colourmode=" << colourmode << std::endl;
	}

	/* Cycle between drawing vertices, mesh and filled polygons */
	if (key == 'N' && action != GLFW_PRESS)
	{
		drawmode++;
		if (drawmode > 2) drawmode = 0;
	}

	

}



/* Entry point of program */
int main(int argc, char* argv[])
{
	GLWrapper *glw = new GLWrapper(1024, 768, "Colourful Noise");;

	if (!ogl_LoadFunctions())
	{
		fprintf(stderr, "ogl_LoadFunctions() failed. Exiting\n");
		return 0;
	}

	glw->setRenderer(display);
	glw->setKeyCallback(keyCallback);
	glw->setReshapeCallback(reshape);

	init(glw);

	glw->eventLoop();


	delete(glw);
	return 0;
}




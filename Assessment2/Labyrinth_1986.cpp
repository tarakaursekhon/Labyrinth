#define _USE_MATH_DEFINES

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <vector>
#include <map>

#include "error.h"
#include "file.h"
#include "shader.h"
#include "camera.h"
#include "shadow.h"
#include "texture.h"
#include "load_obj.h"
#include "casteljau.h"
#include "point.h"


FTCamera Camera;
glm::vec3 lightDirection = Camera.Front;
glm::vec3 lightPos = Camera.Position;

#define WIDTH 640
#define HEIGHT 480
#define SH_MAP_WIDTH 2048
#define SH_MAP_HEIGHT 2048
#define DEG2RAD(n)	(n)*(M_PI/180)

unsigned int VAO[7];
unsigned int VBO[7];

float deltaTime = 0.0f;
float lastFrame = 0.0f;


float skyboxVertices[] = {
	// positions          
	-1.0f,  1.0f, -1.0f,
	-1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f,
	 1.0f,  1.0f, -1.0f,
	-1.0f,  1.0f, -1.0f,

	-1.0f, -1.0f,  1.0f,
	-1.0f, -1.0f, -1.0f,
	-1.0f,  1.0f, -1.0f,
	-1.0f,  1.0f, -1.0f,
	-1.0f,  1.0f,  1.0f,
	-1.0f, -1.0f,  1.0f,

	 1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f,

	-1.0f, -1.0f,  1.0f,
	-1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f, -1.0f,  1.0f,
	-1.0f, -1.0f,  1.0f,

	-1.0f,  1.0f, -1.0f,
	 1.0f,  1.0f, -1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	-1.0f,  1.0f,  1.0f,
	-1.0f,  1.0f, -1.0f,

	-1.0f, -1.0f, -1.0f,
	-1.0f, -1.0f,  1.0f,
	 1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f,
	-1.0f, -1.0f,  1.0f,
	 1.0f, -1.0f,  1.0f
};


float cubeVertices[] = {
	// positions          // col				// alpha	// normal
	-0.5f, -0.5f, -0.5f,  0.4f, 0.88f, 2.0f,	0.4f,		0.f, 0.f, -1.f,
	 0.5f, -0.5f, -0.5f,  0.4f, 0.88f, 2.0f,	0.4f,		0.f, 0.f, -1.f,
	 0.5f,  0.5f, -0.5f,  0.4f, 0.88f, 2.0f,	0.4f,		0.f, 0.f, -1.f,
	 0.5f,  0.5f, -0.5f,  0.4f, 0.88f, 2.0f,	0.4f,		0.f, 0.f, -1.f,
	-0.5f,  0.5f, -0.5f,  0.4f, 0.88f, 2.0f,	0.4f,		0.f, 0.f, -1.f,
	-0.5f, -0.5f, -0.5f,  0.4f, 0.88f, 2.0f,	0.4f,		0.f, 0.f, -1.f,

	-0.5f, -0.5f,  0.5f,  0.4f, 0.88f, 2.0f,	0.4f,		0.f, 0.f, 1.f,
	 0.5f, -0.5f,  0.5f,  0.4f, 0.88f, 2.0f,	0.4f,		0.f, 0.f, 1.f,
	 0.5f,  0.5f,  0.5f,  0.4f, 0.88f, 2.0f,	0.4f,		0.f, 0.f, 1.f,
	 0.5f,  0.5f,  0.5f,  0.4f, 0.88f, 2.0f,	0.4f,		0.f, 0.f, 1.f,
	-0.5f,  0.5f,  0.5f,  0.4f, 0.88f, 2.0f,	0.4f,		0.f, 0.f, 1.f,
	-0.5f, -0.5f,  0.5f,  0.4f, 0.88f, 2.0f,	0.4f,		0.f, 0.f, 1.f,

	-0.5f,  0.5f,  0.5f,  0.4f, 0.88f, 2.0f,	0.4f,		-1.f, 0.f, 0.f,
	-0.5f,  0.5f, -0.5f,  0.4f, 0.88f, 2.0f,	0.4f,		-1.f, 0.f, 0.f,
	-0.5f, -0.5f, -0.5f,  0.4f, 0.88f, 2.0f,	0.4f,		-1.f, 0.f, 0.f,
	-0.5f, -0.5f, -0.5f,  0.4f, 0.88f, 2.0f,	0.4f,		-1.f, 0.f, 0.f,
	-0.5f, -0.5f,  0.5f,  0.4f, 0.88f, 2.0f,	0.4f,		-1.f, 0.f, 0.f,
	-0.5f,  0.5f,  0.5f,  0.4f, 0.88f, 2.0f,	0.4f,		-1.f, 0.f, 0.f,
															
	 0.5f,  0.5f,  0.5f,  0.4f, 0.88f, 2.0f,	0.4f,		1.f, 0.f, 0.f,
	 0.5f,  0.5f, -0.5f,  0.4f, 0.88f, 2.0f,	0.4f,		1.f, 0.f, 0.f,
	 0.5f, -0.5f, -0.5f,  0.4f, 0.88f, 2.0f,	0.4f,		1.f, 0.f, 0.f,
	 0.5f, -0.5f, -0.5f,  0.4f, 0.88f, 2.0f,	0.4f,		1.f, 0.f, 0.f,
	 0.5f, -0.5f,  0.5f,  0.4f, 0.88f, 2.0f,	0.4f,		1.f, 0.f, 0.f,
	 0.5f,  0.5f,  0.5f,  0.4f, 0.88f, 2.0f,	0.4f,		1.f, 0.f, 0.f,
															
	-0.5f, -0.5f, -0.5f,  0.4f, 0.88f, 2.0f,	0.4f,		0.f, -1.f, 0.f,
	 0.5f, -0.5f, -0.5f,  0.4f, 0.88f, 2.0f,	0.4f,		0.f, -1.f, 0.f,
	 0.5f, -0.5f,  0.5f,  0.4f, 0.88f, 2.0f,	0.4f,		0.f, -1.f, 0.f,
	 0.5f, -0.5f,  0.5f,  0.4f, 0.88f, 2.0f,	0.4f,		0.f, -1.f, 0.f,
	-0.5f, -0.5f,  0.5f,  0.4f, 0.88f, 2.0f,	0.4f,		0.f, -1.f, 0.f,
	-0.5f, -0.5f, -0.5f,  0.4f, 0.88f, 2.0f,	0.4f,		0.f, -1.f, 0.f,
															
	-0.5f,  0.5f, -0.5f,  0.4f, 0.88f, 2.0f,	0.4f,		0.f, 1.f, 0.f,
	 0.5f,  0.5f, -0.5f,  0.4f, 0.88f, 2.0f,	0.4f,		0.f, 1.f, 0.f,
	 0.5f,  0.5f,  0.5f,  0.4f, 0.88f, 2.0f,	0.4f,		0.f, 1.f, 0.f,
	 0.5f,  0.5f,  0.5f,  0.4f, 0.88f, 2.0f,	0.4f,		0.f, 1.f, 0.f,
	-0.5f,  0.5f,  0.5f,  0.4f, 0.88f, 2.0f,	0.4f,		0.f, 1.f, 0.f,
	-0.5f,  0.5f, -0.5f,  0.4f, 0.88f, 2.0f,	0.4f,		0.f, 1.f, 0.f
};

float redCubeVertices[] = {
	// positions          // col				// alpha	// normal
	-0.5f, -0.5f, -0.5f,  0.8f, 0.f, 0.f,	0.4f,		0.f, 0.f, -1.f,
	 0.5f, -0.5f, -0.5f,  0.8f, 0.f, 0.f,	0.4f,		0.f, 0.f, -1.f,
	 0.5f,  0.5f, -0.5f,  0.8f, 0.f, 0.f,	0.4f,		0.f, 0.f, -1.f,
	 0.5f,  0.5f, -0.5f,  0.8f, 0.f, 0.f,	0.4f,		0.f, 0.f, -1.f,
	-0.5f,  0.5f, -0.5f,  0.8f, 0.f, 0.f,	0.4f,		0.f, 0.f, -1.f,
	-0.5f, -0.5f, -0.5f,  0.8f, 0.f, 0.f,	0.4f,		0.f, 0.f, -1.f,
									 
	-0.5f, -0.5f,  0.5f,  0.8f, 0.f, 0.f,	0.4f,		0.f, 0.f, 1.f,
	 0.5f, -0.5f,  0.5f,  0.8f, 0.f, 0.f,	0.4f,		0.f, 0.f, 1.f,
	 0.5f,  0.5f,  0.5f,  0.8f, 0.f, 0.f,	0.4f,		0.f, 0.f, 1.f,
	 0.5f,  0.5f,  0.5f,  0.8f, 0.f, 0.f,	0.4f,		0.f, 0.f, 1.f,
	-0.5f,  0.5f,  0.5f,  0.8f, 0.f, 0.f,	0.4f,		0.f, 0.f, 1.f,
	-0.5f, -0.5f,  0.5f,  0.8f, 0.f, 0.f,	0.4f,		0.f, 0.f, 1.f,
									 
	-0.5f,  0.5f,  0.5f,  0.8f, 0.f, 0.f,	0.4f,		-1.f, 0.f, 0.f,
	-0.5f,  0.5f, -0.5f,  0.8f, 0.f, 0.f,	0.4f,		-1.f, 0.f, 0.f,
	-0.5f, -0.5f, -0.5f,  0.8f, 0.f, 0.f,	0.4f,		-1.f, 0.f, 0.f,
	-0.5f, -0.5f, -0.5f,  0.8f, 0.f, 0.f,	0.4f,		-1.f, 0.f, 0.f,
	-0.5f, -0.5f,  0.5f,  0.8f, 0.f, 0.f,	0.4f,		-1.f, 0.f, 0.f,
	-0.5f,  0.5f,  0.5f,  0.8f, 0.f, 0.f,	0.4f,		-1.f, 0.f, 0.f,
									 
	 0.5f,  0.5f,  0.5f,  0.8f, 0.f, 0.f,	0.4f,		1.f, 0.f, 0.f,
	 0.5f,  0.5f, -0.5f,  0.8f, 0.f, 0.f,	0.4f,		1.f, 0.f, 0.f,
	 0.5f, -0.5f, -0.5f,  0.8f, 0.f, 0.f,	0.4f,		1.f, 0.f, 0.f,
	 0.5f, -0.5f, -0.5f,  0.8f, 0.f, 0.f,	0.4f,		1.f, 0.f, 0.f,
	 0.5f, -0.5f,  0.5f,  0.8f, 0.f, 0.f,	0.4f,		1.f, 0.f, 0.f,
	 0.5f,  0.5f,  0.5f,  0.8f, 0.f, 0.f,	0.4f,		1.f, 0.f, 0.f,
									 
	-0.5f, -0.5f, -0.5f,  0.8f, 0.f, 0.f,	0.4f,		0.f, -1.f, 0.f,
	 0.5f, -0.5f, -0.5f,  0.8f, 0.f, 0.f,	0.4f,		0.f, -1.f, 0.f,
	 0.5f, -0.5f,  0.5f,  0.8f, 0.f, 0.f,	0.4f,		0.f, -1.f, 0.f,
	 0.5f, -0.5f,  0.5f,  0.8f, 0.f, 0.f,	0.4f,		0.f, -1.f, 0.f,
	-0.5f, -0.5f,  0.5f,  0.8f, 0.f, 0.f,	0.4f,		0.f, -1.f, 0.f,
	-0.5f, -0.5f, -0.5f,  0.8f, 0.f, 0.f,	0.4f,		0.f, -1.f, 0.f,
									 
	-0.5f,  0.5f, -0.5f,  0.8f, 0.f, 0.f,	0.4f,		0.f, 1.f, 0.f,
	 0.5f,  0.5f, -0.5f,  0.8f, 0.f, 0.f,	0.4f,		0.f, 1.f, 0.f,
	 0.5f,  0.5f,  0.5f,  0.8f, 0.f, 0.f,	0.4f,		0.f, 1.f, 0.f,
	 0.5f,  0.5f,  0.5f,  0.8f, 0.f, 0.f,	0.4f,		0.f, 1.f, 0.f,
	-0.5f,  0.5f,  0.5f,  0.8f, 0.f, 0.f,	0.4f,		0.f, 1.f, 0.f,
	-0.5f,  0.5f, -0.5f,  0.8f, 0.f, 0.f,	0.4f,		0.f, 1.f, 0.f
};

float planeVertices[] = {
	// positions			// col					// alpha	// normal
	 10.0f, -0.5f,  10.0f,  0.2f, 0.1f, 0.f,		1.f,		0.f, 1.f, 0.f,
	-10.0f, -0.5f,  10.0f,  0.2f, 0.1f, 0.f,		1.f,		0.f, 1.f, 0.f,
	-10.0f, -0.5f, -10.0f,  0.2f, 0.1f, 0.f,		1.f,		0.f, 1.f, 0.f,
																	 	  
	 10.0f, -0.5f,  10.0f,	0.2f, 0.1f, 0.f,		1.f,		0.f, 1.f, 0.f,
	-10.0f, -0.5f, -10.0f,	0.2f, 0.1f, 0.f,		1.f,		0.f, 1.f, 0.f,
	 10.0f, -0.5f, -10.0f,	0.2f, 0.1f, 0.f,		1.f,		0.f, 1.f, 0.f
};

float wallVertices[] = {
	// positions				// normal			// texture
	-0.5f,  -0.5f,  0.0f,		0.f, 0.f, 1.f,		0.f, 0.f,
	-0.5f, 0.5f,  0.0f,			0.f, 0.f, 1.f,		0.f, 1.f,
	0.5f, 0.5f,  0.0f,			0.f, 0.f, 1.f,		1.f, 1.f,
										  
	-0.5f, -0.5f, 0.0f,			0.f, 0.f, 1.f,		0.f, 0.f,
	0.5f, 0.5f, 0.0f,			0.f, 0.f, 1.f,		1.f, 1.f,
	0.5f, -0.5f, 0.0f,			0.f, 0.f, 1.f,		1.f, 0.f
};


std::vector<glm::vec3> positionsx;
std::vector<glm::vec3> positionsz;
std::vector<float> anglesx;
std::vector<float> anglesz;


float* CreateCircle(int num_segments, float radius)
{
	float* vertices = (float*)malloc(sizeof(float) * 24 * num_segments);
	float offset = 360.0 / num_segments;
	float angle = 0;

	int texCtr = 0;

	for (int i = 0; i < num_segments * 24; i += 24)
	{
		// positions
		vertices[i] = 0.0;
		vertices[i + 1] = 0.0;
		vertices[i + 2] = 0.0;

		vertices[i + 8] = sin(DEG2RAD(angle)) * radius;
		vertices[i + 9] = cos(DEG2RAD(angle)) * radius;
		vertices[i + 10] = 0.0;

		vertices[i + 16] = sin(DEG2RAD(angle + offset)) * radius;
		vertices[i + 17] = cos(DEG2RAD(angle + offset)) * radius;
		vertices[i + 18] = 0.0;

		angle += offset;

		// normal
		vertices[i + 3] = 0.0;
		vertices[i + 4] = 0.0;
		vertices[i + 5] = -1.0;

		vertices[i + 11] = 0.0;
		vertices[i + 12] = 0.0;
		vertices[i + 13] = -1.0;

		vertices[i + 19] = 0.0;
		vertices[i + 20] = 0.0;
		vertices[i + 21] = -1.0;

		// texture
		if (texCtr == 0)
		{
			vertices[i + 6] = 0.0;
			vertices[i + 7] = 0.0;

			vertices[i + 14] = 0.0;
			vertices[i + 15] = 1.0;

			vertices[i + 22] = 1.0;
			vertices[i + 23] = 1.0;
		}
		else
		{
			vertices[i + 6] = 0.0;
			vertices[i + 7] = 0.0;

			vertices[i + 14] = 1.0;
			vertices[i + 15] = 1.0;

			vertices[i + 22] = 1.0;
			vertices[i + 23] = 0.0;
		}

		texCtr = (texCtr + 1) % 2;
	}

	return vertices;
}


float* CreateEye(int num_segments, float radius)
{
	float* vertices = (float*)malloc(sizeof(float) * 30 * num_segments);
	float offset = 360.0 / num_segments;
	float angle = 0;

	for (int i = 0; i < num_segments * 30; i += 30)
	{
		// positions
		vertices[i] = 0.0;
		vertices[i + 1] = 0.0;
		vertices[i + 2] = 0.0;

		vertices[i + 10] = sin(DEG2RAD(angle)) * radius;
		vertices[i + 11] = cos(DEG2RAD(angle)) * radius;
		vertices[i + 12] = 0.0;

		vertices[i + 20] = sin(DEG2RAD(angle + offset)) * radius;
		vertices[i + 21] = cos(DEG2RAD(angle + offset)) * radius;
		vertices[i + 22] = 0.0;

		angle += offset;

		// colour
		vertices[i + 3] = 0.0;
		vertices[i + 4] = 0.0;
		vertices[i + 5] = 0.0;
		vertices[i + 6] = 1.0;

		vertices[i + 13] = 0.0;
		vertices[i + 14] = 0.0;
		vertices[i + 15] = 0.0;
		vertices[i + 16] = 1.0;

		vertices[i + 23] = 0.0;
		vertices[i + 24] = 0.0;
		vertices[i + 25] = 0.0;
		vertices[i + 26] = 1.0;

		// normal
		vertices[i + 7] = 0.0;
		vertices[i + 8] = 0.0;
		vertices[i + 9] = 1.0;

		vertices[i + 17] = 0.0;
		vertices[i + 18] = 0.0;
		vertices[i + 19] = 1.0;

		vertices[i + 27] = 0.0;
		vertices[i + 28] = 0.0;
		vertices[i + 29] = 1.0;
	}

	return vertices;
}


void processKeyboard(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	float x_offset = 0.f;
	float y_offset = 0.f;
	bool cam_changed = false;

	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
	{
		x_offset = -1.5f;
		y_offset = 0.f;
		cam_changed = true;
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
	{
		x_offset = 1.5f;
		y_offset = 0.f;
		cam_changed = true;
	}
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
	{
		x_offset = 0.f;
		y_offset = -1.5f;
		cam_changed = true;
	}
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
	{
		x_offset = 0.f;
		y_offset = 1.5f;
		cam_changed = true;
	}

	if (cam_changed)
	{
		MoveAndOrientCamera(Camera, x_offset, y_offset, -1, deltaTime);
	}

	float currentFrame = glfwGetTime();
	deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;
	float cameraSpeed = 1.f * deltaTime;

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		Camera.Position += cameraSpeed * Camera.Front;
		MoveAndOrientCamera(Camera, x_offset, y_offset, 0, deltaTime);
	}

	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		Camera.Position -= cameraSpeed * Camera.Front;
		MoveAndOrientCamera(Camera, x_offset, y_offset, 1, deltaTime);
	}

	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		Camera.Position += glm::normalize(glm::cross(Camera.Front, Camera.Up)) * cameraSpeed;
		MoveAndOrientCamera(Camera, x_offset, y_offset, 3, deltaTime);
	}

	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		Camera.Position -= glm::normalize(glm::cross(Camera.Front, Camera.Up)) * cameraSpeed;
		MoveAndOrientCamera(Camera, x_offset, y_offset, 2, deltaTime);
	}

	lightDirection = Camera.Front;
	lightPos = Camera.Position;
}


void SizeCallback(GLFWwindow* window, int w, int h)
{
	glViewport(0, 0, w, h);
}


void setPlane(unsigned int program)
{
	glUseProgram(program);

	glBindVertexArray(VAO[1]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), planeVertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 10 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 10 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 10 * sizeof(float), (void*)(7 * sizeof(float)));
	glEnableVertexAttribArray(2);
}


void setCube(unsigned int program)
{
	glUseProgram(program);

	glBindVertexArray(VAO[0]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 10 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 10 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 10 * sizeof(float), (void*)(7 * sizeof(float)));
	glEnableVertexAttribArray(2);
}


void setCubeRed(unsigned int program)
{
	glUseProgram(program);

	glBindVertexArray(VAO[0]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(redCubeVertices), redCubeVertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 10 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 10 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 10 * sizeof(float), (void*)(7 * sizeof(float)));
	glEnableVertexAttribArray(2);
}


void drawPlane(unsigned int program)
{
	glUseProgram(program);

	glm::mat4 model = glm::mat4(1.f);
	glBindVertexArray(VAO[1]);
	model = glm::mat4(1.0f);
	glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_FALSE, glm::value_ptr(model));
	glDrawArrays(GL_TRIANGLES, 0, 6);
}


void drawCube(unsigned int program)
{
	glUseProgram(program);

	glm::mat4 model = glm::mat4(1.f);
	glBindVertexArray(VAO[0]);
	model = glm::translate(model, glm::vec3(4.0f, 0.0f, 0.0f));
	model = glm::rotate(model, (float)glfwGetTime() / 2.f, glm::vec3(0.0f, 1.0f, .0f));
	model = glm::scale(model, glm::vec3(.5, .5, .5));
	glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_FALSE, glm::value_ptr(model));
	glDrawArrays(GL_TRIANGLES, 0, 36);
}


void setWalls(unsigned int program)
{
	glUseProgram(program);

	glBindVertexArray(VAO[2]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(wallVertices), wallVertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);
}


void drawWalls(unsigned int program)
{
	glUseProgram(program);

	glm::mat4 model = glm::mat4(1.f);

	glBindVertexArray(VAO[2]);
	for (int i = 0; i < positionsx.size(); i++)
	{
		model = glm::mat4(1.0f);
		model = glm::translate(model, positionsx[i]);
		model = glm::rotate(model, glm::radians(anglesx[i]), glm::vec3(0.f, 1.f, 0.f));
		glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_FALSE, glm::value_ptr(model));
		glDrawArrays(GL_TRIANGLES, 0, 6);
	}

	glBindVertexArray(VAO[2]);
	for (int i = 0; i < positionsz.size(); i++)
	{
		model = glm::mat4(1.0f);
		model = glm::translate(model, positionsz[i]);
		model = glm::rotate(model, glm::radians(anglesz[i]), glm::vec3(0.f, 1.f, 0.f));
		glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_FALSE, glm::value_ptr(model));
		glDrawArrays(GL_TRIANGLES, 0, 6);
	}
}


void setSnake(unsigned int program, int num_segs, float* verts)
{
	glUseProgram(program);

	glBindVertexArray(VAO[3]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[3]);
	glBufferData(GL_ARRAY_BUFFER, (sizeof(float) * 8 * 3 * num_segs), verts, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);
}


void drawSnake(unsigned int program, int num_segs, float* verts, std::vector<point> curve)
{
	glUseProgram(program);

	for (point p : curve)
	{
		glBindVertexArray(VAO[3]);
		glm::mat4 model = glm::mat4(1.f);
		model = glm::translate(model, glm::vec3(1.f, 0.f, 0.99f));
		model = glm::scale(model, glm::vec3(.5f, .5f, .5f));
		model = glm::translate(model, glm::vec3(p.x, p.y, p.z));
		glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_FALSE, glm::value_ptr(model));
		glDrawArrays(GL_TRIANGLES, 0, 3 * num_segs);
	}
}


void setSnakeEyes(unsigned int program, int num_segs, float* verts)
{
	glUseProgram(program);

	glBindVertexArray(VAO[4]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[4]);
	glBufferData(GL_ARRAY_BUFFER, (sizeof(float) * 10 * 3 * num_segs), verts, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 10 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 10 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 10 * sizeof(float), (void*)(7 * sizeof(float)));
	glEnableVertexAttribArray(2);
}


void drawSnakeEyes(unsigned int program, int num_segs, float* verts)
{
	glUseProgram(program);

	glBindVertexArray(VAO[4]);

	glm::mat4 model = glm::mat4(1.f);
	model = glm::translate(model, glm::vec3(1.23f, 0.2f, 0.98f));
	model = glm::scale(model, glm::vec3(.1f, .1f, .1f));
	glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_FALSE, glm::value_ptr(model));
	glDrawArrays(GL_TRIANGLES, 0, 3 * num_segs);

	model = glm::mat4(1.f);
	model = glm::translate(model, glm::vec3(1.27f, 0.2f, 0.98f));
	model = glm::scale(model, glm::vec3(.1f, .1f, .1f));
	glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_FALSE, glm::value_ptr(model));
	glDrawArrays(GL_TRIANGLES, 0, 3 * num_segs);
}


void generateDepthMap(unsigned int program, int num_segs, float* verts, float* verts_eyes, std::vector<point> curve, ShadowStruct shadow, glm::mat4 projectedLightSpaceMatrix)
{
	glViewport(0, 0, SH_MAP_WIDTH, SH_MAP_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, shadow.FBO);
	glClear(GL_DEPTH_BUFFER_BIT);
	glUseProgram(program);
	glUniformMatrix4fv(glGetUniformLocation(program, "projectedLightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(projectedLightSpaceMatrix));
	drawPlane(program);
	drawCube(program);
	drawWalls(program);
	drawSnake(program, num_segs, verts, curve);
	drawSnakeEyes(program, num_segs, verts_eyes);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


void renderWithShadow(unsigned int program, ShadowStruct shadow, glm::mat4 projectedLightSpaceMatrix)
{
	glViewport(0, 0, WIDTH, HEIGHT);

	static const GLfloat bgd[] = { 1.f, 1.f, 1.f, 1.f };
	glClearBufferfv(GL_COLOR, 0, bgd);
	glClear(GL_DEPTH_BUFFER_BIT);

	glUseProgram(program);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glUniformMatrix4fv(glGetUniformLocation(program, "projectedLightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(projectedLightSpaceMatrix));
	glUniform3f(glGetUniformLocation(program, "lightDirection"), lightDirection.x, lightDirection.y, lightDirection.z);
	glUniform3f(glGetUniformLocation(program, "lightColour"), 1.f, 1.f, 1.f);
	glUniform3f(glGetUniformLocation(program, "lightPos"), lightPos.x, lightPos.y, lightPos.z);
	glUniform3f(glGetUniformLocation(program, "camPos"), Camera.Position.x, Camera.Position.y, Camera.Position.z);

	glm::mat4 view = glm::mat4(1.f);
	view = glm::lookAt(Camera.Position, Camera.Position + Camera.Front, Camera.Up);
	glUniformMatrix4fv(glGetUniformLocation(program, "view"), 1, GL_FALSE, glm::value_ptr(view));

	glm::mat4 projection = glm::mat4(1.f);
	projection = glm::perspective(glm::radians(45.f), (float)WIDTH / (float)HEIGHT, .001f, 100.f);
	glUniformMatrix4fv(glGetUniformLocation(program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
}


void drawObj(unsigned int program, int tris_size)
{
	glUseProgram(program);

	glBindVertexArray(VAO[5]);
	glm::mat4 model = glm::mat4(1.f);
	model = glm::translate(model, glm::vec3(-2.f, -0.5f, 2.5f));
	model = glm::scale(model, glm::vec3(.2f, .2f, .2f));
	model = glm::rotate(model, glm::radians(90.f), glm::vec3(0.f, 1.f, 0.f));
	glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_FALSE, glm::value_ptr(model));
	glDrawArrays(GL_TRIANGLES, 0, tris_size * 3);
}


void moveObj(unsigned int program, int tris_size)
{
	glUseProgram(program);

	for (int i = 0; i < 100; i++)
	{
		glBindVertexArray(VAO[5]);
		glm::mat4 model = glm::mat4(1.f);
		model = glm::translate(model, glm::vec3(-2.f + (i * 0.001 * (float)glfwGetTime()), -0.5f, 2.5f));
		model = glm::scale(model, glm::vec3(.2f, .2f, .2f));
		model = glm::rotate(model, glm::radians(90.f), glm::vec3(0.f, 1.f, 0.f));
		glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_FALSE, glm::value_ptr(model));
		glDrawArrays(GL_TRIANGLES, 0, tris_size * 3);
	}
}


unsigned int loadCubemap(vector<std::string> faces)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	int width, height, nrChannels;
	for (unsigned int i = 0; i < faces.size(); i++)
	{
		unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
				0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
			);
			stbi_image_free(data);
		}
		else
		{
			std::cout << "Cubemap tex failed to load at path: " << faces[i] << std::endl;
			stbi_image_free(data);
		}
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	return textureID;
}


void setSky(unsigned int program)
{
	glUseProgram(program);

	glBindVertexArray(VAO[6]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[6]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), skyboxVertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
}


void drawSky(unsigned int program, unsigned int skyTexture)
{
	glDisable(GL_DEPTH_TEST);
	glUseProgram(program);
	glm::mat4 view = glm::mat4(1.f);
	view = glm::mat3(glm::lookAt(Camera.Position, Camera.Position + Camera.Front, Camera.Up));
	glUniformMatrix4fv(glGetUniformLocation(program, "view"), 1, GL_FALSE, glm::value_ptr(view));
	glm::mat4 projection = glm::mat4(1.f);
	projection = glm::perspective(glm::radians(45.f), (float)WIDTH / (float)HEIGHT, .001f, 100.f);
	glUniformMatrix4fv(glGetUniformLocation(program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
	glBindVertexArray(VAO[6]);
	glBindTexture(GL_TEXTURE_CUBE_MAP, skyTexture);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glEnable(GL_DEPTH_TEST);
}


int main(int argc, char** argv)
{
	glfwInit();
	glfwWindowHint(GLFW_SAMPLES, 32);
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Labyrinth_1986", NULL, NULL);
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, SizeCallback);

	gl3wInit();

	glEnable(GL_DEBUG_OUTPUT);
	glDebugMessageCallback(DebugCallback, 0);

	ShadowStruct shadow = setup_shadowmap(SH_MAP_WIDTH, SH_MAP_HEIGHT);
	GLuint program = CompileShader("triangle.vert", "triangle.frag");
	GLuint shadowProgram = CompileShader("shadow.vert", "shadow.frag");
	GLuint textureProgram = CompileShader("texture.vert", "texture.frag");
	GLuint objProgram = CompileShader("obj.vert", "obj.frag");
	GLuint skyProgram = CompileShader("skybox.vert", "skybox.frag");

	InitCamera(Camera);
	MoveAndOrientCamera(Camera, 0.f, 0.f, 0, deltaTime);
	GLuint wallTex = setup_texture("wallTexture.bmp");
	GLuint wallTexRed = setup_texture("wallTextureRed.bmp");
	GLuint snakeTex = setup_texture("snakeTexture.bmp");

	vector<std::string> faces
	{
		"skybox/right.jpg",
		"skybox/left.jpg",
		"skybox/top.jpg",
		"skybox/bottom.jpg",
		"skybox/back.jpg",
		"skybox/front.jpg"
	};
	unsigned int skyTex = loadCubemap(faces);

	int num_segs = 64;
	float* verts = CreateCircle(num_segs, 0.1f);
	float* verts_eyes = CreateEye(num_segs, 0.1f);

	glGenVertexArrays(7, VAO);
	glGenBuffers(7, VBO);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glPointSize(8);

	setSky(skyProgram);
	setPlane(program);
	setCube(program);
	setWalls(textureProgram);
	setSnake(textureProgram, num_segs, verts);
	setSnakeEyes(program, num_segs, verts_eyes);

	std::vector<point> ctrl;
	ctrl.push_back(point(-0.5f, -0.4f, 0.f));
	ctrl.push_back(point(-0.5f, 0.4f, 0.f));
	ctrl.push_back(point(0.5f, -0.4f, 0.f));
	ctrl.push_back(point(0.5f, 0.4f, 0.f));

	int num_evals = 128;
	std::vector<point> curve = EvaluateBezierCurve(ctrl, num_evals);

	int num_curve_verts = 0;
	int num_curve_floats = 0;
	float* curve_vertices = MakeFloatsFromVector(curve, num_curve_verts, num_curve_floats, 0.f, 0.f, 0.f);

	int num_ctrl_verts = 0;
	int num_ctrl_floats = 0;
	float* ctrl_vertices = MakeFloatsFromVector(ctrl, num_ctrl_verts, num_ctrl_floats, 1.f, 0.f, 0.f);

	positionsx.push_back(glm::vec3(4.f, 0.f, 4.f));
	positionsx.push_back(glm::vec3(0.f, 0.f, 3.f));
	positionsx.push_back(glm::vec3(1.f, 0.f, 3.f));
	positionsx.push_back(glm::vec3(2.f, 0.f, 3.f));
	positionsx.push_back(glm::vec3(3.f, 0.f, 3.f));
	positionsx.push_back(glm::vec3(0.f, 0.f, 2.f));
	positionsx.push_back(glm::vec3(1.f, 0.f, 2.f));
	positionsx.push_back(glm::vec3(2.f, 0.f, 2.f));
	positionsx.push_back(glm::vec3(3.f, 0.f, 2.f));
	positionsx.push_back(glm::vec3(1.f, 0.f, 1.f));
	positionsx.push_back(glm::vec3(5.f, 0.f, -1.f));
	positionsx.push_back(glm::vec3(6.f, 0.f, -1.f));
	positionsx.push_back(glm::vec3(3.f, 0.f, -1.f));
	positionsx.push_back(glm::vec3(2.f, 0.f, -1.f));
	positionsx.push_back(glm::vec3(0.f, 0.f, -1.f));
	positionsx.push_back(glm::vec3(-1.f, 0.f, -1.f));
	positionsx.push_back(glm::vec3(-2.f, 0.f, -1.f));
	positionsx.push_back(glm::vec3(-3.f, 0.f, -1.f));
	positionsx.push_back(glm::vec3(-4.f, 0.f, -1.f));
	positionsx.push_back(glm::vec3(-4.f, 0.f, -2.f));
	positionsx.push_back(glm::vec3(-3.f, 0.f, -2.f));
	positionsx.push_back(glm::vec3(-2.f, 0.f, -2.f));
	positionsx.push_back(glm::vec3(-1.f, 0.f, -2.f));
	positionsx.push_back(glm::vec3(0.f, 0.f, -2.f));
	positionsx.push_back(glm::vec3(2.f, 0.f, -2.f));
	positionsx.push_back(glm::vec3(3.f, 0.f, -2.f));
	positionsx.push_back(glm::vec3(4.f, 0.f, -2.f));
	positionsx.push_back(glm::vec3(5.f, 0.f, -2.f));
	positionsx.push_back(glm::vec3(6.f, 0.f, -2.f));

	positionsz.push_back(glm::vec3(6.5f, 0.f, -1.5f));
	positionsz.push_back(glm::vec3(4.5f, 0.f, -0.5f));
	positionsz.push_back(glm::vec3(4.5f, 0.f, 0.5f));
	positionsz.push_back(glm::vec3(4.5f, 0.f, 1.5f));
	positionsz.push_back(glm::vec3(4.5f, 0.f, 2.5f));
	positionsz.push_back(glm::vec3(4.5f, 0.f, 3.5f));
	positionsz.push_back(glm::vec3(3.5f, 0.f, 3.5f));
	positionsz.push_back(glm::vec3(3.5f, 0.f, 1.5f));
	positionsz.push_back(glm::vec3(3.5f, 0.f, 0.5f));
	positionsz.push_back(glm::vec3(3.5f, 0.f, -0.5f));
	positionsz.push_back(glm::vec3(1.5f, 0.f, -0.5f));
	positionsz.push_back(glm::vec3(1.5f, 0.f, 0.5f));
	positionsz.push_back(glm::vec3(0.5f, 0.f, -0.5f));
	positionsz.push_back(glm::vec3(0.5f, 0.f, 0.5f));
	positionsz.push_back(glm::vec3(-4.5f, 0.f, -1.5f));

	anglesx.push_back(180.f);
	anglesx.push_back(180.f);
	anglesx.push_back(180.f);
	anglesx.push_back(180.f);
	anglesx.push_back(180.f);
	anglesx.push_back(0.f);
	anglesx.push_back(0.f);
	anglesx.push_back(0.f);
	anglesx.push_back(0.f);
	anglesx.push_back(180.f);
	anglesx.push_back(180.f);
	anglesx.push_back(180.f);
	anglesx.push_back(180.f);
	anglesx.push_back(180.f);
	anglesx.push_back(180.f);
	anglesx.push_back(180.f);
	anglesx.push_back(180.f);
	anglesx.push_back(180.f);
	anglesx.push_back(180.f);
	anglesx.push_back(0.f);
	anglesx.push_back(0.f);
	anglesx.push_back(0.f);
	anglesx.push_back(0.f);
	anglesx.push_back(0.f);
	anglesx.push_back(0.f);
	anglesx.push_back(0.f);
	anglesx.push_back(0.f);
	anglesx.push_back(0.f);
	anglesx.push_back(0.f);

	anglesz.push_back(270.f);
	anglesz.push_back(270.f);
	anglesz.push_back(270.f);
	anglesz.push_back(270.f);
	anglesz.push_back(270.f);
	anglesz.push_back(270.f);
	anglesz.push_back(90.f);
	anglesz.push_back(90.f);
	anglesz.push_back(90.f);
	anglesz.push_back(90.f);
	anglesz.push_back(270.f);
	anglesz.push_back(270.f);
	anglesz.push_back(90.f);
	anglesz.push_back(90.f);
	anglesz.push_back(90.f);

	const std::string objPath = "objs/Goblin/Goblin.obj";
	const std::string mtlPath = "objs/Goblin";
	std::vector<triangle> tris;
	obj_parse(objPath.c_str(), mtlPath.c_str(), &tris);
	float* objVerts = trisToVertices(tris);

	glUseProgram(objProgram);

	glBindVertexArray(VAO[5]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[5]);
	glBufferData(GL_ARRAY_BUFFER, tris.size() * 30 * sizeof(float), objVerts, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 10 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 10 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 10 * sizeof(float), (void*)(7 * sizeof(float)));
	glEnableVertexAttribArray(2);

	bool redMode = false;

	while (!glfwWindowShouldClose(window))
	{

		glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 1.0f, 70.5f);
		glm::mat4 lightView = glm::lookAt(lightPos, lightPos + lightDirection, glm::vec3(0.0f, 1.0f, 0.0f));
		glm::mat4 projectedLightSpaceMatrix = lightProjection * lightView;

		generateDepthMap(shadowProgram, num_segs, verts, verts_eyes, curve, shadow, projectedLightSpaceMatrix);
		renderWithShadow(program, shadow, projectedLightSpaceMatrix);
		renderWithShadow(textureProgram, shadow, projectedLightSpaceMatrix);

		drawSky(skyProgram, skyTex);

		glUseProgram(textureProgram);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, wallTex);
		glUniform1i(glGetUniformLocation(textureProgram, "texture1"), 0);
		glBindVertexArray(VAO[2]);

		drawPlane(program);

		if (!redMode)
		{
			glUseProgram(textureProgram);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, wallTex);
			glUniform1i(glGetUniformLocation(textureProgram, "texture1"), 0);
			glBindVertexArray(VAO[2]);

			drawWalls(textureProgram);
			drawCube(program);
		}
		else
		{
			glUseProgram(textureProgram);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, wallTexRed);
			glUniform1i(glGetUniformLocation(textureProgram, "texture1"), 0);
			glBindVertexArray(VAO[2]);

			drawWalls(textureProgram);
			setCubeRed(program);
			drawCube(program);
		}

		glUseProgram(textureProgram);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, snakeTex);
		glUniform1i(glGetUniformLocation(textureProgram, "texture1"), 0);
		glBindVertexArray(VAO[3]);

		drawSnake(textureProgram, num_segs, verts, curve);
		drawSnakeEyes(program, num_segs, verts_eyes);

		if (Camera.Position.x > 4 && Camera.Position.x < 5
			&& Camera.Position.y > -1 && Camera.Position.y < 1
			&& Camera.Position.z > 2 && Camera.Position.z < 3
			&& Camera.Yaw > 150 && Camera.Yaw < 220
			&& Camera.Pitch > -30 && Camera.Pitch < 30)
		{
			moveObj(program, tris.size());
		}
		else
		{
			drawObj(program, tris.size());
		}

		if (Camera.Position.x > 4 && Camera.Position.x < 5
			&& Camera.Position.y > -1 && Camera.Position.y < 1
			&& Camera.Position.z > -1 && Camera.Position.z < 1
			&& glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
		{
			redMode = true;
		}
		
		glfwSwapBuffers(window);
		glfwPollEvents();
		processKeyboard(window);
	}

	free(verts);
	free(verts_eyes);
	free(objVerts);
	glDeleteVertexArrays(7, VAO);
	glDeleteBuffers(7, VBO);

	glfwTerminate();

	return 0;
}
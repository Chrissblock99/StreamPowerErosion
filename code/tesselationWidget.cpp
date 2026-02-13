#include <math.h>

#include "tesselationWidget.h"
#include "shader-api.h"
#include "window.h"

/*!
\class TesselationWidget tesselationWidget.h
*/

/*!
\brief Default constructor. No call to OpenGL here.
*/
TesselationWidget::TesselationWidget()
{
	parent = nullptr;
	raytraceVAO = shaderProgram = 0;
	x0 = y0 = 0;
	psi = 3.1415926/4;
	theta = 3.1415926/4;
	r = 300.0;
}

/*!
\brief Destructor. Release shader program, vao and textures.
*/
TesselationWidget::~TesselationWidget()
{
	release_program(shaderProgram);
	glDeleteVertexArrays(1, &raytraceVAO);
}

/*!
\brief Initialize OpenGL, shaders, texture buffers, and a camera centered at origin.
*/
void TesselationWidget::initializeGL()
{
	glPatchParameteri(GL_PATCH_VERTICES, 4);
	glEnable(GL_CULL_FACE);

	glClearColor(0.49f, 0.67f, 1.0f, 1.0f);

	ReloadShaders();

	glGenVertexArrays(1, &raytraceVAO);
}

/*!
\brief Renders the scene.
*/
void TesselationWidget::paintGL()
{
	// Clear
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Draw
	glUseProgram(shaderProgram);

	double x = r * sin(theta) * cos(psi);
	double y = r * cos(theta);
	double z = r * sin(theta) * sin(psi);

	// Uniforms - Camera
	glUniform3f(glGetUniformLocation(shaderProgram, "cameraPos"), float(x), float(y), float(z));

	// Draw heightfield
	glBindVertexArray(raytraceVAO);
	glDrawArrays(GL_PATCHES, 0, 64);
	glBindVertexArray(0);
	glUseProgram(0);
}

/*!
\brief Custom event callback for mouse scrolling.
\param w raw window pointer
\param x, y scroll values
*/
void TesselationWidget::ScrollCallback(GLFWwindow* w, double x, double y)
{
	r -= y * 10;
}

/*!
\brief Internal update functions for camera movements.
*/
void TesselationWidget::Update()
{
	double mousePosX;
	double mousePosY;
	parent->GetMousePosition(&mousePosX, &mousePosY);

	if (parent->GetMousePressed(GLFW_MOUSE_BUTTON_LEFT) && !parent->GetKey(GLFW_KEY_LEFT_CONTROL))
	{
		psi -= (x0 - mousePosX) * 0.01;
		theta = std::min(std::max(0.0, theta + (y0 - mousePosY) * 0.005), 3.1415926535);
	}
	x0 = mousePosX;
	y0 = mousePosY;
}

/*!
\brief Reload the shaders of the widget. Useful for realtime editing and fine tuning of the rendering.
*/
void TesselationWidget::ReloadShaders()
{
	shaderProgram = read_program("./data/shaders/tesselation.glsl");
}
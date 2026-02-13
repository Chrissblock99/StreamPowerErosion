#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <vector>

class Window;
class Texture2D;

class TesselationWidget
{
protected:
	// Scene
	double x0, y0;					//!< Reference mouse coordinates.
	double psi, theta, r;

	// GL data
	GLuint shaderProgram;			//!< GL program for shader.
	GLuint raytraceVAO;				//!< Raytracer VAO.

	// Parent window
	Window* parent;					//!< Parent %Window class.

public:
	TesselationWidget();
	~TesselationWidget();

	int width() const;
	int height() const;
	void SetWindowPtr(Window* ptr);
	void ScrollCallback(GLFWwindow* w, double x, double y);
	void Update();

	virtual void initializeGL();
	virtual void paintGL();
	virtual void ReloadShaders();
};

/*!
\brief Set the parent window pointer.
*/
inline void TesselationWidget::SetWindowPtr(Window* ptr)
{
	parent = ptr;
}
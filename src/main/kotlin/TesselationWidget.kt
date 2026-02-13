package me.chriss99

import org.lwjgl.opengl.GL45.*
import org.lwjgl.glfw.GLFW.*
import kotlin.math.*

class TesselationWidget {
	lateinit var parent: Window
	var raytraceVAO = 0
	var shaderProgram= 0
	var x0 = 0.0
	var y0 = 0.0
	var psi = 3.1415926 / 4
	var theta = 3.1415926 / 4
	var r = 300.0

	fun delete() {
		releaseProgram(shaderProgram)
		glDeleteVertexArrays(raytraceVAO)
	}

	fun initializeGL() {
		glPatchParameteri(GL_PATCH_VERTICES, 4)
		glEnable(GL_CULL_FACE)

		glClearColor(0.49f, 0.67f, 1.0f, 1.0f)

		reloadShaders()

		raytraceVAO = glGenVertexArrays()
	}

	fun paintGL()
	{
		// Clear
		glClear(GL_COLOR_BUFFER_BIT or GL_DEPTH_BUFFER_BIT)

		// Draw
		glUseProgram(shaderProgram)

		val x = r * sin (theta) * cos(psi)
		val y = r * cos (theta)
		val z = r * sin (theta) * sin(psi)

		// Uniforms - Camera
		glUniform3f(glGetUniformLocation(shaderProgram, "cameraPos"), x.toFloat(), y.toFloat(), z.toFloat())

		// Draw heightfield
		glBindVertexArray(raytraceVAO)
		glDrawArrays(GL_PATCHES, 0, 64)
		glBindVertexArray(0)
		glUseProgram(0)
	}

	fun scrollCallback(x: Double, y: Double) {
		r -= y * 10
	}

	fun update() {
		val (mousePosX, mousePosY) = parent.getMousePosition()

		if (parent.getMousePressed(GLFW_MOUSE_BUTTON_LEFT) && !parent.getKey(GLFW_KEY_LEFT_CONTROL)){
			psi -= (x0 - mousePosX) * 0.01
			theta = min(max(0.0, theta + (y0 - mousePosY) * 0.005), 3.1415926535)
		}
		x0 = mousePosX
		y0 = mousePosY
	}

	fun reloadShaders() {
		shaderProgram = readProgram("./src/main/glsl/tesselation.glsl")
	}
}
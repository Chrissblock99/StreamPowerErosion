package me.chriss99

import org.lwjgl.glfw.GLFW.*
import org.lwjgl.glfw.GLFWScrollCallback
import org.lwjgl.opengl.GL
import org.lwjgl.opengl.GL45.*

class Window(windowName: String, var width: Int, var height: Int) {
	var widget: TesselationWidget? = null
		set(value) {
			field = value;
			field!!.parent = this;
			field!!.initializeGL();
			/*
			glfwSetWindowUserPointer(windowPtr, widget);
			glfwSetScrollCallback(windowPtr, [](GLFWwindow* win, double x, double y) {
				TesselationWidget* ptr = (TesselationWidget*) glfwGetWindowUserPointer(win);
				ptr->ScrollCallback(win, x, y);
			});
			*/
		}
	//void (*uiUserFunPtr)();

	init {
		// Window
		if (!glfwInit()) {
			glfwTerminate();
			throw RuntimeException("GLFW failed to initialize")
		}

		val monitor = glfwGetPrimaryMonitor();
		if (monitor == 0L) {
			glfwTerminate();
			throw RuntimeException("GLFW failed to get primary monitor")
		}

		val mode = glfwGetVideoMode(monitor)
		if (mode == null) {
			glfwTerminate();
			throw RuntimeException("GLFW failed to get video mode")
		}
		width = if (width <= 0) mode.width() else width
		height = if (height <= 0) mode.height() else height

		glfwDefaultWindowHints();
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_MAXIMIZED, GLFW_TRUE);
		glfwWindowHint(GLFW_SAMPLES, 4);
	}

	val windowPtr = glfwCreateWindow(width, height, windowName, 0, 0);
	init {
		if (windowPtr == 0L) {
			glfwTerminate();
			throw RuntimeException("GLFW failed to create window")
		}
		glfwMakeContextCurrent(windowPtr);
		glfwSetInputMode(windowPtr, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

		// OpenGL
		if (!GL.createCapabilities().OpenGL45) {
			glfwTerminate();
			throw RuntimeException("OpenGL failed to initialize")
		}

		glEnable(GL_DEPTH_TEST);

		val widthArr = intArrayOf(0)
		val heightArr = intArrayOf(0)
		glfwGetFramebufferSize(windowPtr, widthArr, heightArr);
		width = widthArr[0]
		height = heightArr[0]
		glViewport(0, 0, width, height);

		// vsync
		glfwSwapInterval(1);

		// TODO(Axel): log this into the console.
		//spdlog::info("All systems were correctly initialized");
		//spdlog::info("OpenGL device information: Vendor: {}", (const char*)glGetString(GL_VENDOR));
		//spdlog::info("OpenGL device information: Renderer: {}", (const char*)glGetString(GL_RENDERER));
		//spdlog::info("OpenGL device information: Version: {}", (const char*)glGetString(GL_VERSION));
		//spdlog::info("OpenGL device information: GLSL: {}", (const char*)glGetString(GL_SHADING_LANGUAGE_VERSION));
		//spdlog::info("Framebuffer size: {}x{}", width, height);
		//spdlog::info("Dear ImGui: {}", ImGui::GetVersion());
	}

	fun delete() {
		widget!!.delete();
		glfwTerminate();
	}

	fun update() {
		// Widget rendering
		widget!!.update();
		widget!!.paintGL();

		// Swap buffers
		glfwSwapBuffers(windowPtr);
		glfwPollEvents();
	}

	fun getKey(key: Int) = glfwGetKey(windowPtr, key) == GLFW_PRESS

	fun getMousePressed(mouse: Int) = glfwGetMouseButton(windowPtr, mouse) == GLFW_PRESS

	fun getMousePosition(): Pair<Double, Double> {
		val xArr = doubleArrayOf(0.0)
		val yArr = doubleArrayOf(0.0)
		glfwGetCursorPos(windowPtr, xArr, yArr)
		return xArr[0] to yArr[0]
	}

	fun exit() = glfwWindowShouldClose(windowPtr)
}
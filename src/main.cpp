#include <iostream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

const GLint WIDTH = 1280, HEIGHT = 720;

int main()
{
	// Init GLFW
	if (!glfwInit())
	{
		std::cout << "GLFW Init Failed!";
		glfwTerminate();
		return 1;
	}

	// Setup GLFW window properties
	// OpenGL Version
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	// Core profile = No Backwards Compability
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	// Allow forward compability
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	GLFWwindow* mainWindow = glfwCreateWindow(WIDTH, HEIGHT, "Genix-OpenGL", nullptr, nullptr);
	if (!mainWindow)
	{
		std::cout << "GLFW Window Creation Failed!";
		glfwTerminate();
		return 1;
	}

	// Get buffer size information
	int buffWidth, buffHeight;
	glfwGetFramebufferSize(mainWindow, &buffWidth, &buffHeight);

	// Set context for GLEW to use
	glfwMakeContextCurrent(mainWindow);

	// Allow modern extension features
	glewExperimental = GL_TRUE;

	if (glewInit() != GLEW_OK)
	{
		std::cout << "GLEW init Failed!";
		glfwDestroyWindow(mainWindow);
		glfwTerminate();
		return 1;
	}

	// Setup viewportsize
	glViewport(0,0,buffWidth,buffHeight);

	// Render Loop
	while (!glfwWindowShouldClose(mainWindow))
	{
		glfwPollEvents();

		// Clear window
		glClearColor(1.0, 0.0, 0.0, 1.0);
		glClear(GL_COLOR_BUFFER_BIT);

		glfwSwapBuffers(mainWindow);
	}

	glfwTerminate();

	return 0;
}
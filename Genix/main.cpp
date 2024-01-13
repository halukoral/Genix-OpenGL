#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "Shader.h"

constexpr GLint WIDTH = 1920;
constexpr GLint HEIGHT = 1080;

int main()
{
	// Init GLFW
	if (!glfwInit())
	{
		std::cout << "GLFW initialization failed!" << std::endl;
		glfwTerminate();
		return 1;
	}

	// Setup GLFW window properties
	// OpenGL version
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	// Core profile = No backwards compatibility
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	GLFWwindow* MainWindow = glfwCreateWindow(WIDTH, HEIGHT, "Genix", nullptr, nullptr);
	if (MainWindow == nullptr)
	{
		std::cout << "GLFW window creation failed!" << std::endl;
		glfwTerminate();
		return 1;
	}

	// Get buffer size information
	int BufferWidth, BufferHeight;
	glfwGetFramebufferSize(MainWindow, &BufferWidth, &BufferHeight);

	// Set context for GLEW to use
	glfwMakeContextCurrent(MainWindow);

	// Allow modern extension features
	glewExperimental = GL_TRUE;

	if (glewInit() != GLEW_OK)
	{
		std::cout << "GLEW initialization failed!" << std::endl;
		glfwDestroyWindow(MainWindow);
		glfwTerminate();
		return 1;
	}

	// Setup viewport size
	glViewport(0, 0, BufferWidth, BufferHeight);

	// build and compile our shader program
	// ------------------------------------
	Shader Shader("Shaders/shader.vert", "Shaders/shader.frag");

	// set up vertex data (and buffer(s)) and configure vertex attributes
	// ------------------------------------------------------------------
	constexpr GLfloat Vertices[] = {
		// positions         // colors
		0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,  // bottom right
	   -0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,  // bottom left
		0.0f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f   // top 
   };
	
	// constexpr unsigned int Indices[] = {  // note that we start from 0!
	// 	0, 1, 3,  // first Triangle
	// 	1, 2, 3   // second Triangle
	// };

	GLuint VAO, VBO, EBO;
	glGenVertexArrays(1, &VAO);
	// bind the Vertex Array Object first,
	// then bind and set vertex buffer(s),
	// and then configure vertex attributes(s).
	glBindVertexArray(VAO);

	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);

	// glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	// glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices), Indices, GL_STATIC_DRAW);
	
	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	
	// color attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	// Note that this is allowed, the call to glVertexAttribPointer registered VBO
	// as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	// Remember: do NOT unbind the EBO while a VAO is active as the bound element buffer object IS stored in the VAO; keep the EBO bound.

	// You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
	// VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
	glBindVertexArray(0);

	// uncomment this call to draw in wireframe polygons.
	// glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	
	// Loop until window closed
	while (!glfwWindowShouldClose(MainWindow))
	{
		// Clear window
		glClearColor(0.f, 0.f, 0.f, 1.f);
		glClear(GL_COLOR_BUFFER_BIT);

		// render the triangle
		Shader.Use();
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 3);
		//glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		
		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(MainWindow);
		glfwPollEvents();
	}

	// optional: de-allocate all resources once they've outlived their purpose:
	// ------------------------------------------------------------------------
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);

	// glfw: terminate, clearing all previously allocated GLFW resources.
	// ------------------------------------------------------------------
	glfwTerminate();
	
	return 0;
}

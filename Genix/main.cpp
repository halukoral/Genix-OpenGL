#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

const GLint WIDTH = 1920;
const GLint HEIGHT = 1080;

GLuint VAO, VBO, Shader;

// Vertex Shader
static const char* vShader = "						\n\
#version 330										\n\
													\n\
layout (location = 0) in ve3 pos;					\n\
													\n\
void main()											\n\
{													\n\
	gl_Position = vec4(pos.x, pos.y, pos.z, 1.f);	\n\
}";

// Fragment Shader
static const char* fShader = "						\n\
#version 330										\n\
													\n\
out vec4 color;										\n\
													\n\
void main()											\n\
{													\n\
	color = vec4(1.f, 0.f, 0.f, 1.f);				\n\
}";

void CreateTriangle()
{
	constexpr GLfloat Vertices[] = {
		-1.f, -1.f, 0.f,
		 1.f, -1.f, 0.f,
		 0.f,  1.f, 0.f
	};

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void AddShader(GLuint inProgram, const char* inShaderFile, GLenum inShaderType)
{
	GLuint Shader = glCreateShader(inShaderType);

	const GLchar* Code[1];
	Code[0] = inShaderFile;

	glShaderSource(Shader, 1, Code, nullptr);
	glCompileShader(Shader);

	GLint Result = 0;
	GLchar ErrorLog[1024];
	glGetShaderiv(Shader, GL_COMPILE_STATUS, &Result);
	if (Result == false)
	{
		glGetShaderInfoLog(Shader, sizeof(ErrorLog), nullptr, ErrorLog);
		std::cout << "Error compiling program: " << inShaderType << " " << ErrorLog << std::endl;
		return;
	}

	glAttachShader(inProgram,Shader);
}

void CompileShaders()
{
	Shader = glCreateProgram();
	if (!Shader)
	{
		std::cout << "Error Creating Shaders!" << std::endl;
		return;
	}

	AddShader(Shader, vShader, GL_VERTEX_SHADER);
	AddShader(Shader, fShader, GL_FRAGMENT_SHADER);

	GLint Result = 0;
	GLchar ErrorLog[1024];

	glLinkProgram(Shader);
	glGetProgramiv(Shader, GL_LINK_STATUS, &Result);
	if (Result == false)
	{
		glGetProgramInfoLog(Shader, sizeof(ErrorLog), nullptr, ErrorLog);
		std::cout << "Error linking program: " << ErrorLog << std::endl;
		return;
	}

	glValidateProgram(Shader);
	glGetProgramiv(Shader, GL_VALIDATE_STATUS, &Result);
	if (Result == false)
	{
		glGetProgramInfoLog(Shader, sizeof(ErrorLog), nullptr, ErrorLog);
		std::cout << "Error validating program: " << ErrorLog << std::endl;
		return;
	}
}

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

	CreateTriangle();
	CompileShaders();
	
	// Loop until window closed
	while (!glfwWindowShouldClose(MainWindow))
	{
		// Clear window
		glClearColor(0.f, 0.f, 0.f, 1.f);
		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(Shader);
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 3);

		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(MainWindow);
		glfwPollEvents();
	}

	// optional: de-allocate all resources once they've outlived their purpose:
	// ------------------------------------------------------------------------
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteProgram(Shader);

	// glfw: terminate, clearing all previously allocated GLFW resources.
	// ------------------------------------------------------------------
	glfwTerminate();
	
	return 0;
}

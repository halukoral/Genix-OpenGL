#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.h"
#include "Camera.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

void FramebufferSizeCallback(GLFWwindow* Window, int Width, int Height);
void MouseCallback(GLFWwindow* Window, double Xpos, double Ypos);
void ScrollCallback(GLFWwindow* Window, double Xoffset, double Yoffset);
void ProcessInput(GLFWwindow* Window);

constexpr GLint WIDTH = 1920;
constexpr GLint HEIGHT = 1080;

// Camera
Camera Camera(glm::vec3(0.0f, 0.0f, 3.0f));
float LastX = WIDTH / 2.0f;
float LastY = HEIGHT / 2.0f;
bool FirstMouse = true;

// Timing
float DeltaTime = 0.0f;	// Current - Last
float LastFrame = 0.0f;

// Lighting
glm::vec3 LightPos(1.2f, 1.0f, 2.0f);

int main()
{
	// --------------------------------Initialization Phase---------------------------------------
	// -------------------------------------------------------------------------------------------

	if (!glfwInit())
	{
		std::cout << "GLFW initialization failed!" << std::endl;
		glfwTerminate();
		return 1;
	}

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

	glfwMakeContextCurrent(MainWindow);
	glfwSetFramebufferSizeCallback(MainWindow, FramebufferSizeCallback);
	glfwSetCursorPosCallback(MainWindow, MouseCallback);
	glfwSetScrollCallback(MainWindow, ScrollCallback);

	// Tell GLFW to capture our mouse
	glfwSetInputMode(MainWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// Setup viewport size
	glViewport(0, 0, BufferWidth, BufferHeight);

	// Configure global opengl state
	glEnable(GL_DEPTH_TEST);

	// Initialize ImGUI
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	const ImGuiIO& Io = ImGui::GetIO(); (void)Io;
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(MainWindow, true);
	ImGui_ImplOpenGL3_Init("#version 330");
	
	// --------------------------------End Of Initialization Phase--------------------------------
	// -------------------------------------------------------------------------------------------


	// Build and compile our shader program
	// ------------------------------------
	const Shader LightShader("Shaders/LightShader.vert", "Shaders/LightShader.frag");
	const Shader CubeShader("Shaders/CubeShader.vert", "Shaders/CubeShader.frag");
	
	// Set up vertex data (and buffer(s)) and configure vertex attributes
	// ------------------------------------------------------------------
	constexpr GLfloat Vertices[] = {
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
		 0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,

		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
	};

	/* --------------------------------- Initialise VAO & VBO --------------------------------- */
	/* ---------------------------------------------------------------------------------------- */

	/* Create variables to hold the VBO and the VAO identifiers */
	GLuint CubeVAO, LightVAO, VBO; //EBO;

	/* Create a new VBO and use the VBO to store the id */
	glGenBuffers(1, &VBO);

	// Cube
	// ----
	glGenVertexArrays(1, &CubeVAO);
	glBindVertexArray(CubeVAO);
	
	/* Make the new VBO active */
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	/* Upload vertex data to the video device */
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// normal attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	// Light
	// -----
	glGenVertexArrays(1, &LightVAO);
	glBindVertexArray(LightVAO);

	/* Make the new VBO active */
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	/* Upload vertex data to the video device */
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);

	// Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// uncomment this call to draw in wireframe polygons.
	// glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	// Loop until window closed
	while (!glfwWindowShouldClose(MainWindow))
	{
		// Delta-time
		const float CurrentFrame = static_cast<float>(glfwGetTime());
		DeltaTime = CurrentFrame - LastFrame;
		LastFrame = CurrentFrame;

		ProcessInput(MainWindow);

		// Tell OpenGL a new frame is about to begin
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		
		// Clear window
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // also clear the depth buffer now!

		// Be sure to activate shader when setting uniforms/drawing objects
		CubeShader.Use();
		CubeShader.SetVec3("objectColor", 1.0f, 0.5f, 0.31f);
		CubeShader.SetVec3("lightColor",  1.0f, 1.0f, 1.0f);
		CubeShader.SetVec3("lightPos", LightPos);
		CubeShader.SetVec3("viewPos", Camera.Position);
		
		// view/projection transformations
		glm::mat4 Projection = glm::perspective(glm::radians(Camera.Zoom), (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f);
		glm::mat4 View = Camera.GetViewMatrix();
		CubeShader.SetMat4("projection", Projection);
		CubeShader.SetMat4("view", View);

		// world transformation
		glm::mat4 Model = glm::mat4(1.0f);
		CubeShader.SetMat4("model", Model);

		// render the cube
		glBindVertexArray(CubeVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		// also draw the lamp object
		LightShader.Use();
		LightShader.SetMat4("projection", Projection);
		LightShader.SetMat4("view", View);
		Model = glm::mat4(1.0f);
		Model = glm::translate(Model, LightPos);
		Model = glm::scale(Model, glm::vec3(0.2f)); // a smaller cube
		LightShader.SetMat4("model", Model);

		glBindVertexArray(LightVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		// ImGUI window creation
		ImGui::Begin("My name is window, ImGUI window");
		// Text that appears in the window
		ImGui::Text("Hello there adventurer!");
		// Ends the window
		ImGui::End();

		
		// Renders the ImGUI elements
		// --------------------------
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		
		// Glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(MainWindow);
		glfwPollEvents();
	}

	// Deletes all ImGUI instances
	// ---------------------------
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	
	// Optional: de-allocate all resources once they've outlived their purpose:
	// ------------------------------------------------------------------------
	glDeleteVertexArrays(1, &CubeVAO);
	glDeleteVertexArrays(1, &LightVAO);
	glDeleteBuffers(1, &VBO);

	// Glfw: terminate, clearing all previously allocated GLFW resources.
	// ------------------------------------------------------------------
	glfwTerminate();

	return 0;
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void FramebufferSizeCallback(GLFWwindow* Window, int Width, int Height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, Width, Height);
}

// GlFW: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void MouseCallback(GLFWwindow* Window, double Xpos, double Ypos)
{
	const float X = static_cast<float>(Xpos);
	const float Y = static_cast<float>(Ypos);

	if (FirstMouse)
	{
		LastX = X;
		LastY = Y;
		FirstMouse = false;
	}

	const float Xoffset = X - LastX;
	const float Yoffset = LastY - Y; // reversed since y-coordinates go from bottom to top

	LastX = X;
	LastY = Y;

	Camera.ProcessMouseMovement(Xoffset, Yoffset);
}

// GlFW: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void ScrollCallback(GLFWwindow* Window, double Xoffset, double Yoffset)
{
	Camera.ProcessMouseScroll(static_cast<float>(Yoffset));

}

// Process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void ProcessInput(GLFWwindow* Window)
{
	if (glfwGetKey(Window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(Window, true);
	}

	if (glfwGetKey(Window, GLFW_KEY_W) == GLFW_PRESS)
	{
		Camera.ProcessKeyboard(FORWARD, DeltaTime);
	}
	if (glfwGetKey(Window, GLFW_KEY_S) == GLFW_PRESS)
	{
		Camera.ProcessKeyboard(BACKWARD, DeltaTime);
	}
	if (glfwGetKey(Window, GLFW_KEY_A) == GLFW_PRESS)
	{
		Camera.ProcessKeyboard(LEFT, DeltaTime);
	}
	if (glfwGetKey(Window, GLFW_KEY_D) == GLFW_PRESS)
	{
		Camera.ProcessKeyboard(RIGHT, DeltaTime);
	}
}

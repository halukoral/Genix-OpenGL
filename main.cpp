#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.h"
#include "Camera.h"

#define STB_IMAGE_IMPLEMENTATION
#include "Model.h"
#include "stb_image.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

void FramebufferSizeCallback(GLFWwindow* Window, int Width, int Height);
void MouseCallback(GLFWwindow* Window, double Xpos, double Ypos);
void ScrollCallback(GLFWwindow* Window, double Xoffset, double Yoffset);
void ProcessInput(GLFWwindow* Window);
unsigned int LoadTexture(char const * path);

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
	glDepthFunc(GL_LESS);
	glEnable(GL_STENCIL_TEST);
	glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

	// tell stb_image.h to flip loaded texture's on the y-axis (before loading model).
	stbi_set_flip_vertically_on_load(true);

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
	Shader OurShader("Shaders/Material.vert", "Shaders/Material.frag");
	Shader OutlineShader("Shaders/OutlineShader.vert", "Shaders/OutlineShader.frag");
	
	// load models
	// -----------
	Model OurModel("Resources/Models/Backpack/backpack.obj");
	
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
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		// view/projection transformations
		glm::mat4 Projection = glm::perspective(glm::radians(Camera.Zoom), (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f);
		glm::mat4 View = Camera.GetViewMatrix();
		
		OutlineShader.Use();
		OutlineShader.SetMat4("projection", Projection);
		OutlineShader.SetMat4("view", View);

		OurShader.Use();
		OurShader.SetMat4("projection", Projection);
		OurShader.SetMat4("view", View);
		
		// 1st. render pass, draw objects as normal, writing to the stencil buffer
		// --------------------------------------------------------------------
		glStencilFunc(GL_ALWAYS, 1, 0xFF);
		glStencilMask(0xFF);
		
		// render the loaded model
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));	// it's a bit too big for our scene, so scale it down
		OurShader.SetMat4("model", model);
		OurModel.Draw(OurShader);

		// 2nd. render pass: now draw slightly scaled versions of the objects, this time disabling stencil writing.
		// Because the stencil buffer is now filled with several 1s. The parts of the buffer that are 1 are not drawn, thus only drawing 
		// the objects' size differences, making it look like borders.
		// -----------------------------------------------------------------------------------------------------------------------------
		glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
		glStencilMask(0x00);
		glDisable(GL_DEPTH_TEST);

		OutlineShader.Use();
		constexpr float Scale = 1.1f;
		// render the loaded model
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
		model = glm::scale(model, glm::vec3(Scale, Scale, Scale));	// it's a bit too big for our scene, so scale it down
		OutlineShader.SetMat4("model", model);
		OurModel.Draw(OutlineShader);

		glStencilMask(0xFF);
		glStencilFunc(GL_ALWAYS, 0, 0xFF);
		glEnable(GL_DEPTH_TEST);

		
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

// utility function for loading a 2D texture from file
// ---------------------------------------------------
unsigned int LoadTexture(char const * path)
{
	unsigned int TextureId;
	glGenTextures(1, &TextureId);

	int Width, Height, NrComponents;
	unsigned char *Data = stbi_load(path, &Width, &Height, &NrComponents, 0);
	if (Data)
	{
		GLenum Format;
		if (NrComponents == 1)
		{
			Format = GL_RED;
		}
		else if (NrComponents == 3)
		{
			Format = GL_RGB;
		}
		else if (NrComponents == 4)
		{
			Format = GL_RGBA;
		}

		glBindTexture(GL_TEXTURE_2D, TextureId);
		glTexImage2D(GL_TEXTURE_2D, 0, Format, Width, Height, 0, Format, GL_UNSIGNED_BYTE, Data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(Data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(Data);
	}

	return TextureId;
}

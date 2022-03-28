// Quick note: GLAD needs to be included first before GLFW.
// Otherwise, GLAD will complain about gl.h being already included.
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <fstream>
#include <iostream>
#include <string>

// Include stb_image for loading images
// Remember to define STB_IMAGE_IMPLEMENTATION first before including
// to avoid linker errors
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

// We include glm to give us access to vectors (glm::vec3) and matrices (glm::mat4)
#include <glm/glm.hpp>
// This gives us access to convenience functions for constructing transformation matrices
#include <glm/gtc/matrix_transform.hpp>
// This gives us access to the glm::value_ptr() function, which converts a vector/matrix to a pointer that OpenGL accepts
#include <glm/gtc/type_ptr.hpp>

// ---------------
// Function declarations
// ---------------

/**
 * @brief Creates a shader program based on the provided file paths for the vertex and fragment shaders.
 * @param[in] vertexShaderFilePath Vertex shader file path
 * @param[in] fragmentShaderFilePath Fragment shader file path
 * @return OpenGL handle to the created shader program
 */
GLuint CreateShaderProgram(const std::string& vertexShaderFilePath, const std::string& fragmentShaderFilePath);

/**
 * @brief Creates a shader based on the provided shader type and the path to the file containing the shader source.
 * @param[in] shaderType Shader type
 * @param[in] shaderFilePath Path to the file containing the shader source
 * @return OpenGL handle to the created shader
 */
GLuint CreateShaderFromFile(const GLuint& shaderType, const std::string& shaderFilePath);

/**
 * @brief Creates a shader based on the provided shader type and the string containing the shader source.
 * @param[in] shaderType Shader type
 * @param[in] shaderSource Shader source string
 * @return OpenGL handle to the created shader
 */
GLuint CreateShaderFromSource(const GLuint& shaderType, const std::string& shaderSource);

/**
 * @brief Function for handling the event when the size of the framebuffer changed.
 * @param[in] window Reference to the window
 * @param[in] width New width
 * @param[in] height New height
 */
void FramebufferSizeChangedCallback(GLFWwindow* window, int width, int height);

/**
 * Struct containing data about a vertex
 */
struct Vertex
{
	GLfloat x, y, z;	// Position
	GLubyte r, g, b;	// Color
	GLfloat u, v;		// UV-coordinates
	GLfloat nx, ny, nz; //normal vector
};

void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void processInput(GLFWwindow *window);

glm::vec3 cameraPosition = { 0.0f, 0.0f, 0.0f };//for wasd

glm::vec3 PointlightPos = { 0.0f, 1.0f, 0.0f };
glm::vec3 SpotlightPos = { 3.0f, 3.0f, -1.0f };

float ambientComponent = 0.1f;
float diffuseComponent = 1.0f;
float specularComponent = 1.0f;
glm::vec3 specularIntensity = { 1.0f, 1.0f, 1.0f };
float shine = 32.0f;

glm::vec3 target = { 0.0f, 0.0f, -1.0f }; // Target is a specific point that the camera is looking at
glm::vec3 up = { 0.0f, 0.1f, 0.0f }; // Global up vector (which will be used by the lookAt function to calculate the camera's right and up vectors)

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

bool firstMouse = true;
float yaw   = -90.0f;	// yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right so we initially rotate a bit to the left.
float pitch =  0.0f;
float lastX =  800.0f / 2.0;
float lastY =  600.0 / 2.0;

//round off function from: https://www.geeksforgeeks.org/rounding-floating-point-number-two-decimal-places-c-c/
float roundOff(float var)
{
	float value = (int)(var * 100 + .5);
	return (float)value / 100;
}

/**
 * @brief Main function
 * @return An integer indicating whether the program ended successfully or not.
 * A value of 0 indicates the program ended succesfully, while a non-zero value indicates
 * something wrong happened during execution.
 */
int main()
{
	// Initialize GLFW
	int glfwInitStatus = glfwInit();
	if (glfwInitStatus == GLFW_FALSE)
	{
		std::cerr << "Failed to initialize GLFW!" << std::endl;
		return 1;
	}

	// Tell GLFW that we prefer to use OpenGL 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

	// Tell GLFW that we prefer to use the modern OpenGL
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Tell GLFW to create a window
	int windowWidth = 800;
	int windowHeight = 600;
	GLFWwindow* window = glfwCreateWindow(windowWidth, windowHeight, "Hello Triangle", nullptr, nullptr);
	if (window == nullptr)
	{
		std::cerr << "Failed to create GLFW window!" << std::endl;
		glfwTerminate();
		return 1;
	}

	// Tell GLFW to use the OpenGL context that was assigned to the window that we just created
	glfwMakeContextCurrent(window);

	// Register the callback function that handles when the framebuffer size has changed
	glfwSetFramebufferSizeCallback(window, FramebufferSizeChangedCallback);

	// Tell GLAD to load the OpenGL function pointers
	if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)))
	{
		std::cerr << "Failed to initialize GLAD!" << std::endl;
		return 1;
	}

	// --- Vertex specification ---
	
	// Set up the data for each vertex of the quad
	// These vertices are in LOCAL SPACE
	Vertex vertices[78];

	//frontmost face to view space
		//bottom left (in relation to own normal vector)
		vertices[0].x = 1.0f;	vertices[0].y = -1.0f;	vertices[0].z = -1.0f;
		vertices[0].r = 255;	vertices[0].g = 255;		vertices[0].b = 255;
		vertices[0].u = 0.0f;	vertices[0].v = 0.0f;
		vertices[0].nx = 0.0f;	vertices[0].ny = 0.0f;	vertices[0].nz = -1.0f;

		//bottom right (in relation to own normal vector)
		vertices[1].x = -1.0f;	vertices[1].y = -1.0f;	vertices[1].z = -1.0f;
		vertices[1].r = 255;		vertices[1].g = 255;	vertices[1].b = 255;
		vertices[1].u = 1.0f;	vertices[1].v = 0.0f;
		vertices[1].nx = 0.0f;	vertices[1].ny = 0.0f;	vertices[1].nz = -1.0f;


		//top right (in relation to own normal vector)
		vertices[2].x = 1.0f;	vertices[2].y = 1.0f;	vertices[2].z = -1.0f;
		vertices[2].r = 255;	vertices[2].g = 255;		vertices[2].b = 255;
		vertices[2].u = 0.0f;	vertices[2].v = 1.0f;
		vertices[2].nx = 0.0f;	vertices[2].ny = 0.0f;	vertices[2].nz = -1.0f;

		//bottom left (in relation to own normal vector)
		vertices[3].x = -1.0f;	vertices[3].y = -1.0f;	vertices[3].z = -1.0f;
		vertices[3].r = 255;	vertices[3].g = 255;		vertices[3].b = 255;
		vertices[3].u = 1.0f;	vertices[3].v = 0.0f;
		vertices[3].nx = 0.0f;	vertices[3].ny = 0.0f;	vertices[3].nz = -1.0f;

		//top left (in relation to own normal vector)
		vertices[4].x = -1.0f;	vertices[4].y = 1.0f;	vertices[4].z = -1.0f;
		vertices[4].r = 255;		vertices[4].g = 255;	vertices[4].b = 255;
		vertices[4].u = 1.0f;	vertices[4].v = 1.0f;
		vertices[4].nx = 0.0f;	vertices[4].ny = 0.0f;	vertices[4].nz = -1.0f;

		//top right (in relation to own normal vector)
		vertices[5].x = 1.0f;	vertices[5].y = 1.0f;	vertices[5].z = -1.0f;
		vertices[5].r = 255;		vertices[5].g = 255;		vertices[5].b = 255;
		vertices[5].u = 0.0f;	vertices[5].v = 1.0f;
		vertices[5].nx = 0.0f;	vertices[5].ny = 0.0f;	vertices[5].nz = -1.0f;


	//backmost face to view space
		//bottom right (in relation to own normal vector)
		vertices[6].x = -1.0f;	vertices[6].y = -1.0f;	vertices[6].z = 1.0f;
		vertices[6].r = 255;	vertices[6].g = 255;		vertices[6].b = 255;
		vertices[6].u = 0.0f;	vertices[6].v = 0.0f;
		vertices[6].nx = 0.0f;	vertices[6].ny = 0.0f;	vertices[6].nz = 1.0f;

		//bottom left (in relation to own normal vector)
		vertices[7].x = 1.0f;	vertices[7].y = -1.0f;	vertices[7].z = 1.0f;
		vertices[7].r = 255;	vertices[7].g = 255;	vertices[7].b = 255;
		vertices[7].u = 1.0f;	vertices[7].v = 0.0f;
		vertices[7].nx = 0.0f;	vertices[7].ny = 0.0f;	vertices[7].nz = 1.0f;

		//top left (in relation to own normal vector)
		vertices[8].x = 1.0f;	vertices[8].y = 1.0f;	vertices[8].z = 1.0f;
		vertices[8].r = 255;	vertices[8].g = 255;		vertices[8].b = 255;
		vertices[8].u = 1.0f;	vertices[8].v = 1.0f;
		vertices[8].nx = 0.0f;	vertices[8].ny = 0.0f;	vertices[8].nz = 1.0f;

		//bottom right (in relation to own normal vector)
		vertices[9].x = -1.0f;	vertices[9].y = -1.0f;	vertices[9].z = 1.0f;
		vertices[9].r = 255;	vertices[9].g = 255;		vertices[9].b = 255;
		vertices[9].u = 0.0f;	vertices[9].v = 0.0f;
		vertices[9].nx = 0.0f;	vertices[9].ny = 0.0f;	vertices[9].nz = 1.0f;

		//top right (in relation to own normal vector)
		vertices[10].x = -1.0f;	vertices[10].y = 1.0f;	vertices[10].z = 1.0f;
		vertices[10].r = 255;	vertices[10].g = 255;	vertices[10].b = 255;
		vertices[10].u = 0.0f;	vertices[10].v = 1.0f;
		vertices[10].nx = 0.0f;	vertices[10].ny = 0.0f;	vertices[10].nz = 1.0f;

		//top left (in relation to own normal vector)
		vertices[11].x = 1.0f;	vertices[11].y = 1.0f;	vertices[11].z = 1.0f;
		vertices[11].r = 255;	vertices[11].g = 255;		vertices[11].b = 255;
		vertices[11].u = 1.0f;	vertices[11].v = 1.0f;
		vertices[11].nx = 0.0f;	vertices[11].ny = 0.0f;	vertices[11].nz = 1.0f;

	//leftmost face to view space
		//bottom left (in relation to own normal vector)
		vertices[12].x = -1.0f;	vertices[12].y = -1.0f;	vertices[12].z = -1.0f;
		vertices[12].r = 255;	vertices[12].g = 255;		vertices[12].b = 255;
		vertices[12].u = 0.0f;	vertices[12].v = 0.0f;
		vertices[12].nx = -1.0f; vertices[12].ny = 0.0f;	vertices[12].nz = 0.0f;

		//bottom right (in relation to own normal vector)
		vertices[13].x = -1.0f;	vertices[13].y = -1.0f;	vertices[13].z = 1.0f;
		vertices[13].r = 255;	vertices[13].g = 255;	vertices[13].b = 255;
		vertices[13].u = 1.0f;	vertices[13].v = 0.0f;
		vertices[13].nx = -1.0f; vertices[13].ny = 0.0f;	vertices[13].nz = 0.0f;

		//top right (in relation to own normal vector)
		vertices[14].x = -1.0f;	vertices[14].y = 1.0f;	vertices[14].z = 1.0f;
		vertices[14].r = 255;	vertices[14].g = 255;		vertices[14].b = 255;
		vertices[14].u = 1.0f;	vertices[14].v = 1.0f;
		vertices[14].nx = -1.0f; vertices[14].ny = 0.0f;	vertices[14].nz = 0.0f;

		//bottom left (in relation to own normal vector)
		vertices[15].x = -1.0f;	vertices[15].y = -1.0f;	vertices[15].z = -1.0f;
		vertices[15].r = 255;	vertices[15].g = 255;		vertices[15].b = 255;
		vertices[15].u = 0.0f;	vertices[15].v = 0.0f;
		vertices[15].nx = -1.0f; vertices[15].ny = 0.0f;	vertices[15].nz = 0.0f;

		//top left (in relation to own normal vector)
		vertices[16].x = -1.0f;	vertices[16].y = 1.0f;	vertices[16].z = -1.0f;
		vertices[16].r = 255;	vertices[16].g = 255;	vertices[16].b = 255;
		vertices[16].u = 0.0f;	vertices[16].v = 1.0f;
		vertices[16].nx = -1.0f; vertices[16].ny = 0.0f;	vertices[16].nz = 0.0f;

		//top right (in relation to own normal vector)
		vertices[17].x = -1.0f;	vertices[17].y = 1.0f;	vertices[17].z = 1.0f;
		vertices[17].r = 255;	vertices[17].g = 255;		vertices[17].b = 255;
		vertices[17].u = 1.0f;	vertices[17].v = 1.0f;
		vertices[17].nx = -1.0f; vertices[17].ny = 0.0f;	vertices[17].nz = 0.0f;


	//rightmost face to view space
		//bottom left (in relation to own normal vector)
		vertices[18].x = 1.0f;	vertices[18].y = -1.0f;	vertices[18].z = 1.0f;
		vertices[18].r = 255;	vertices[18].g = 255;		vertices[18].b = 255;
		vertices[18].u = 0.0f;	vertices[18].v = 0.0f;
		vertices[18].nx = 1.0f; vertices[18].ny = 0.0f;	vertices[18].nz = 0.0f;

		//bottom right (in relation to own normal vector)
		vertices[19].x = 1.0f;	vertices[19].y = -1.0f;	vertices[19].z = -1.0f;
		vertices[19].r = 255;		vertices[19].g = 255;	vertices[19].b = 255;
		vertices[19].u = 1.0f;	vertices[19].v = 0.0f;
		vertices[19].nx = 1.0f; vertices[19].ny = 0.0f;	vertices[19].nz = 0.0f;

		//top right (in relation to own normal vector)
		vertices[20].x = 1.0f;	vertices[20].y = 1.0f;	vertices[20].z = -1.0f;
		vertices[20].r = 255;	vertices[20].g = 255;		vertices[20].b = 255;
		vertices[20].u = 1.0f;	vertices[20].v = 1.0f;
		vertices[20].nx = 1.0f; vertices[20].ny = 0.0f;	vertices[20].nz = 0.0f;

		//bottom left (in relation to own normal vector)
		vertices[21].x = 1.0f;	vertices[21].y = -1.0f;	vertices[21].z = 1.0f;
		vertices[21].r = 255;	vertices[21].g = 255;		vertices[21].b = 255;
		vertices[21].u = 0.0f;	vertices[21].v = 0.0f;
		vertices[21].nx = 1.0f; vertices[21].ny = 0.0f;	vertices[21].nz = 0.0f;

		//top left (in relation to own normal vector)
		vertices[22].x = 1.0f;	vertices[22].y = 1.0f;	vertices[22].z = 1.0f;
		vertices[22].r = 255;	vertices[22].g = 255;	vertices[22].b = 255;
		vertices[22].u = 0.0f;	vertices[22].v = 1.0f;
		vertices[22].nx = 1.0f; vertices[22].ny = 0.0f;	vertices[22].nz = 0.0f;

		//top right (in relation to own normal vector)
		vertices[23].x = 1.0f;	vertices[23].y = 1.0f;	vertices[23].z = -1.0f;
		vertices[23].r = 255;		vertices[23].g = 255;		vertices[23].b = 255;
		vertices[23].u = 1.0f;	vertices[23].v = 1.0f;
		vertices[23].nx = 1.0f; vertices[23].ny = 0.0f;	vertices[23].nz = 0.0f;


	//topmost face to view space
		//bottom left (in relation to own normal vector)
		vertices[24].x = -1.0f;	vertices[24].y = 1.0f;	vertices[24].z = 1.0f;
		vertices[24].r = 255;	vertices[24].g = 255;		vertices[24].b = 255;
		vertices[24].u = 0.0f;	vertices[24].v = 0.0f;
		vertices[24].nx = 0.0f; vertices[24].ny = 1.0f;	vertices[24].nz = 0.0f;

		//bottom right (in relation to own normal vector)
		vertices[25].x = 1.0f;	vertices[25].y = 1.0f;	vertices[25].z = 1.0f;
		vertices[25].r = 255;		vertices[25].g = 255;	vertices[25].b = 255;
		vertices[25].u = 1.0f;	vertices[25].v = 0.0f;
		vertices[25].nx = 0.0f; vertices[25].ny = 1.0f;	vertices[25].nz = 0.0f;

		//top right (in relation to own normal vector)
		vertices[26].x = 1.0f;	vertices[26].y = 1.0f;	vertices[26].z = -1.0f;
		vertices[26].r = 255;	vertices[26].g = 255;		vertices[26].b = 255;
		vertices[26].u = 1.0f;	vertices[26].v = 1.0f;
		vertices[26].nx = 0.0f; vertices[26].ny = 1.0f;	vertices[26].nz = 0.0f;

		//bottom left (in relation to own normal vector)
		vertices[27].x = -1.0f;	vertices[27].y = 1.0f;	vertices[27].z = 1.0f;
		vertices[27].r = 255;	vertices[27].g = 255;		vertices[27].b = 255;
		vertices[27].u = 0.0f;	vertices[27].v = 0.0f;
		vertices[27].nx = 0.0f; vertices[27].ny = 1.0f;	vertices[27].nz = 0.0f;

		//top left (in relation to own normal vector)
		vertices[28].x = -1.0f;	vertices[28].y = 1.0f;	vertices[28].z = -1.0f;
		vertices[28].r = 255;		vertices[28].g = 255;	vertices[28].b = 255;
		vertices[28].u = 0.0f;	vertices[28].v = 1.0f;
		vertices[28].nx = 0.0f; vertices[28].ny = 1.0f;	vertices[28].nz = 0.0f;

		//top right (in relation to own normal vector)
		vertices[29].x = 1.0f;	vertices[29].y = 1.0f;	vertices[29].z = -1.0f;
		vertices[29].r = 255;		vertices[29].g = 255;		vertices[29].b = 255;
		vertices[29].u = 1.0f;	vertices[29].v = 1.0f;
		vertices[29].nx = 0.0f; vertices[29].ny = 1.0f;	vertices[29].nz = 0.0f;


	//bottommost face to view space
		//bottom left (in relation to own normal vector)
	vertices[30].x = 1.0f;	vertices[30].y = -1.0f;	vertices[30].z = 1.0f;
	vertices[30].r = 255;	vertices[30].g = 255;		vertices[30].b = 255;
	vertices[30].u = 1.0f;	vertices[30].v = 1.0f;
	vertices[30].nx = 0.0f; vertices[30].ny = -1.0f;	vertices[30].nz = 0.0f;

	//bottom right (in relation to own normal vector)
	vertices[31].x = -1.0f;	vertices[31].y = -1.0f;	vertices[31].z = 1.0f;
	vertices[31].r = 255;		vertices[31].g = 255;	vertices[31].b = 255;
	vertices[31].u = 0.0f;	vertices[31].v = 1.0f;
	vertices[31].nx = 0.0f; vertices[31].ny = -1.0f; vertices[31].nz = 0.0f;

	//top right (in relation to own normal vector)
	vertices[32].x = -1.0f;	vertices[32].y = -1.0f;	vertices[32].z = -1.0f;
	vertices[32].r = 255;		vertices[32].g = 255;		vertices[32].b = 255;
	vertices[32].u = 0.0f;	vertices[32].v = 0.0f;
	vertices[32].nx = 0.0f; vertices[32].ny = -1.0f; vertices[32].nz = 0.0f;

	//bottom left (in relation to own normal vector)
	vertices[33].x = 1.0f;	vertices[33].y = -1.0f;	vertices[33].z = 1.0f;
	vertices[33].r = 255;	vertices[33].g = 255;		vertices[33].b = 255;
	vertices[33].u = 1.0f;	vertices[33].v = 1.0f;
	vertices[33].nx = 0.0f; vertices[33].ny = -1.0f; vertices[33].nz = 0.0f;

	//top left (in relation to own normal vector)
	vertices[34].x = 1.0f;	vertices[34].y = -1.0f;	vertices[34].z = -1.0f;
	vertices[34].r = 255;		vertices[34].g = 255;	vertices[34].b = 255;
	vertices[34].u = 1.0f;	vertices[34].v = 0.0f;
	vertices[34].nx = 0.0f; vertices[34].ny = -1.0f; vertices[34].nz = 0.0f;

	//top right (in relation to own normal vector)
	vertices[35].x = -1.0f;	vertices[35].y = -1.0f;	vertices[35].z = -1.0f;
	vertices[35].r = 255;		vertices[35].g = 255;		vertices[35].b = 255;
	vertices[35].u = 0.0f;	vertices[35].v = 0.0f;
	vertices[35].nx = 0.0f; vertices[35].ny = -1.0f; vertices[35].nz = 0.0f;


	//for quad
		//bottom left (in relation to own normal vector)
	vertices[36].x = -0.5f;	vertices[36].y = -1.0f;	vertices[36].z = 0.0f;
	vertices[36].r = 255;	vertices[36].g = 255;	vertices[36].b = 255;
	vertices[36].u = 0.0f;	vertices[36].v = 0.0f;
	vertices[36].nx = 0.0f; vertices[36].ny = 0.0f; vertices[36].nz = 1.0f;

	//bottom right (in relation to own normal vector)
	vertices[37].x = 0.5f;	vertices[37].y = -1.0f;	vertices[37].z = 0.0f;
	vertices[37].r = 255;	vertices[37].g = 255;	vertices[37].b = 255;
	vertices[37].u = 1.0f;	vertices[37].v = 0.0f;
	vertices[37].nx = 0.0f; vertices[37].ny = 0.0f; vertices[37].nz = 1.0f;

	//top right (in relation to own normal vector)
	vertices[38].x = 0.3f;	vertices[38].y = 0.0f;	vertices[38].z = 0.0f;
	vertices[38].r = 255;	vertices[38].g = 255;	vertices[38].b = 255;
	vertices[38].u = 1.0f;	vertices[38].v = 1.0f;
	vertices[38].nx = 0.0f; vertices[38].ny = 0.0f; vertices[38].nz = 1.0f;

	//bottom left (in relation to own normal vector)
	vertices[39].x = -0.5f;	vertices[39].y = -1.0f;	vertices[39].z = 0.0f;
	vertices[39].r = 255;	vertices[39].g = 255;	vertices[39].b = 255;
	vertices[39].u = 0.0f;	vertices[39].v = 0.0f;
	vertices[39].nx = 0.0f; vertices[39].ny = 0.0f; vertices[39].nz = 1.0f;

	//top left (in relation to own normal vector)
	vertices[40].x = -0.3f;	vertices[40].y = 0.0f;	vertices[40].z = 0.0f;
	vertices[40].r = 255;	vertices[40].g = 255;	vertices[40].b = 255;
	vertices[40].u = 0.0f;	vertices[40].v = 1.0f;
	vertices[40].nx = 0.0f; vertices[40].ny = 0.0f; vertices[40].nz = 1.0f;

	//top right (in relation to own normal vector)
	vertices[41].x = 0.3f;	vertices[41].y = 0.0f;	vertices[41].z = 0.0f;
	vertices[41].r = 255;	vertices[41].g = 255;	vertices[41].b = 255;
	vertices[41].u = 1.0f;	vertices[41].v = 1.0f;
	vertices[41].nx = 0.0f; vertices[41].ny = 0.0f; vertices[41].nz = 1.0f;

	//box
		//+z face
	vertices[42].x = -0.5f;	vertices[42].y = -0.5f;	vertices[42].z = 0.5f;
	vertices[42].r = 255;	vertices[42].g = 255;		vertices[42].b = 255;
	vertices[42].u = 0.0f;	vertices[42].v = 0.0f;
	vertices[42].nx = 0.0f; vertices[42].ny = 0.0f; vertices[42].nz = 1.0f;

	vertices[43].x = 0.5f;	vertices[43].y = -0.5f;	vertices[43].z = 0.5f;
	vertices[43].r = 255;		vertices[43].g = 255;	vertices[43].b = 255;
	vertices[43].u = 1.0f;	vertices[43].v = 0.0f;
	vertices[42].nx = 0.0f; vertices[42].ny = 0.0f; vertices[42].nz = 1.0f;

	vertices[44].x = 0.5f;	vertices[44].y = 0.5f;	vertices[44].z = 0.5f;
	vertices[44].r = 255;		vertices[44].g = 255;		vertices[44].b = 255;
	vertices[44].u = 1.0f;	vertices[44].v = 1.0f;
	vertices[44].nx = 0.0f; vertices[44].ny = 0.0f; vertices[44].nz = 1.0f;

	vertices[45].x = -0.5f;	vertices[45].y = 0.5f;	vertices[45].z = 0.5f;
	vertices[45].r = 255;		vertices[45].g = 255;		vertices[45].b = 255;
	vertices[45].u = 0.0f;	vertices[45].v = 1.0f;
	vertices[45].nx = 0.0f; vertices[45].ny = 0.0f; vertices[45].nz = 1.0f;

	//+x face
	vertices[46].x = 0.5f;	vertices[46].y = -0.5f;	vertices[46].z = 0.5f;
	vertices[46].r = 255;	vertices[46].g = 255;		vertices[46].b = 255;
	vertices[46].u = 0.0f;	vertices[46].v = 0.0f;
	vertices[46].nx = 1.0f; vertices[46].ny = 0.0f; vertices[46].nz = 0.0f;

	vertices[47].x = 0.5f;	vertices[47].y = -0.5f;	vertices[47].z = -0.5f;
	vertices[47].r = 255;		vertices[47].g = 255;		vertices[47].b = 255;
	vertices[47].u = 1.0f;	vertices[47].v = 0.0f;
	vertices[47].nx = 1.0f; vertices[47].ny = 0.0f; vertices[47].nz = 0.0f;

	vertices[48].x = 0.5f;	vertices[48].y = 0.5f;	vertices[48].z = -0.5f;
	vertices[48].r = 255;		vertices[48].g = 255;	vertices[48].b = 255;
	vertices[48].u = 1.0f;	vertices[48].v = 1.0f;
	vertices[48].nx = 1.0f; vertices[48].ny = 0.0f; vertices[48].nz = 0.0f;

	vertices[49].x = 0.5f;	vertices[49].y = 0.5f;	vertices[49].z = 0.5f;
	vertices[49].r = 255;		vertices[49].g = 255;		vertices[49].b = 255;
	vertices[49].u = 0.0f;	vertices[49].v = 1.0f;
	vertices[49].nx = 1.0f; vertices[49].ny = 0.0f; vertices[49].nz = 0.0f;

	//-z face
	vertices[50].x = 0.5f;	vertices[50].y = -0.5f;	vertices[50].z = -0.5f;
	vertices[50].r = 255;		vertices[50].g = 255;		vertices[50].b = 255;
	vertices[50].u = 0.0f;	vertices[50].v = 0.0f;
	vertices[50].nx = 0.0f; vertices[50].ny = 0.0f; vertices[50].nz = -1.0f;

	vertices[51].x = -0.5f;	vertices[51].y = -0.5f;	vertices[51].z = -0.5f;
	vertices[51].r = 255;		vertices[51].g = 255;		vertices[51].b = 255;
	vertices[51].u = 1.0f;	vertices[51].v = 0.0f;
	vertices[51].nx = 0.0f; vertices[51].ny = 0.0f; vertices[51].nz = -1.0f;

	vertices[52].x = -0.5f;	vertices[52].y = 0.5f;	vertices[52].z = -0.5f;
	vertices[52].r = 255;		vertices[52].g = 255;		vertices[52].b = 255;
	vertices[52].u = 1.0f;	vertices[52].v = 1.0f;
	vertices[52].nx = 0.0f; vertices[52].ny = 0.0f; vertices[52].nz = -1.0f;

	vertices[53].x = 0.5f;	vertices[53].y = 0.5f;	vertices[53].z = -0.5f;
	vertices[53].r = 255;		vertices[53].g = 255;		vertices[53].b = 255;
	vertices[53].u = 0.0f;	vertices[53].v = 1.0f;
	vertices[53].nx = 0.0f; vertices[53].ny = 0.0f; vertices[53].nz = -1.0f;

	//-x face
	vertices[54].x = -0.5f;	vertices[54].y = -0.5f;	vertices[54].z = -0.5f;
	vertices[54].r = 255;		vertices[54].g = 255;		vertices[54].b = 255;
	vertices[54].u = 0.0f;	vertices[54].v = 0.0f;
	vertices[54].nx = -1.0f; vertices[54].ny = 0.0f; vertices[54].nz = 0.0f;

	vertices[55].x = -0.5f;	vertices[55].y = -0.5f;	vertices[55].z = 0.5f;
	vertices[55].r = 255;		vertices[55].g = 255;		vertices[55].b = 255;
	vertices[55].u = 1.0f;	vertices[55].v = 0.0f;
	vertices[55].nx = -1.0f; vertices[55].ny = 0.0f; vertices[55].nz = 0.0f;

	vertices[56].x = -0.5f;	vertices[56].y = 0.5f;	vertices[56].z = 0.5f;
	vertices[56].r = 255;		vertices[56].g = 255;		vertices[56].b = 255;
	vertices[56].u = 1.0f;	vertices[56].v = 1.0f;
	vertices[56].nx = -1.0f; vertices[56].ny = 0.0f; vertices[56].nz = 0.0f;

	vertices[57].x = -0.5f;	vertices[57].y = 0.5f;	vertices[57].z = -0.5f;
	vertices[57].r = 255;		vertices[57].g = 255;		vertices[57].b = 255;
	vertices[57].u = 0.0f;	vertices[57].v = 1.0f;
	vertices[57].nx = -1.0f; vertices[57].ny = 0.0f; vertices[57].nz = 0.0f;

	//+y face
	vertices[58].x = -0.5f;	vertices[58].y = 0.5f;	vertices[58].z = 0.5f;
	vertices[58].r = 255;		vertices[58].g = 255;		vertices[58].b = 255;
	vertices[58].u = 0.0f;	vertices[58].v = 0.0f;
	vertices[58].nx = 0.0f; vertices[58].ny = 1.0f; vertices[58].nz = 0.0f;

	vertices[59].x = 0.5f;	vertices[59].y = 0.5f;	vertices[59].z = 0.5f;
	vertices[59].r = 255;		vertices[59].g = 255;		vertices[59].b = 255;
	vertices[59].u = 1.0f;	vertices[59].v = 0.0f;
	vertices[59].nx = 0.0f; vertices[59].ny = 1.0f; vertices[59].nz = 0.0f;

	vertices[60].x = 0.5f;	vertices[60].y = 0.5f;	vertices[60].z = -0.5f;
	vertices[60].r = 255;		vertices[60].g = 255;		vertices[60].b = 255;
	vertices[60].u = 1.0f;	vertices[60].v = 1.0f;
	vertices[60].nx = 0.0f; vertices[60].ny = 1.0f; vertices[60].nz = 0.0f;

	vertices[61].x = -0.5f;	vertices[61].y = 0.5f;	vertices[61].z = -0.5f;
	vertices[61].r = 255;		vertices[61].g = 255;		vertices[61].b = 255;
	vertices[61].u = 0.0f;	vertices[61].v = 1.0f;
	vertices[61].nx = 0.0f; vertices[61].ny = 1.0f; vertices[61].nz = 0.0f;

	//-y
	vertices[62].x = -0.5f;	vertices[62].y = -0.5f;	vertices[62].z = -0.5f;
	vertices[62].r = 255;		vertices[62].g = 255;		vertices[62].b = 255;
	vertices[62].u = 0.0f;	vertices[62].v = 0.0f;
	vertices[62].nx = 0.0f; vertices[62].ny = -1.0f; vertices[62].nz = 0.0f;

	vertices[63].x = 0.5f;	vertices[63].y = -0.5f;	vertices[63].z = -0.5f;
	vertices[63].r = 255;		vertices[63].g = 255;		vertices[63].b = 255;
	vertices[63].u = 1.0f;	vertices[63].v = 0.0f;
	vertices[63].nx = 0.0f; vertices[63].ny = -1.0f; vertices[63].nz = 0.0f;

	vertices[64].x = 0.5f;	vertices[64].y = -0.5f;	vertices[64].z = 0.5f;
	vertices[64].r = 255;		vertices[64].g = 255;		vertices[64].b = 255;
	vertices[64].u = 1.0f;	vertices[64].v = 1.0f;
	vertices[64].nx = 0.0f; vertices[64].ny = -1.0f; vertices[64].nz = 0.0f;

	vertices[65].x = -0.5f;	vertices[65].y = -0.5f;	vertices[65].z = 0.5f;
	vertices[65].r = 255;		vertices[65].g = 255;		vertices[65].b = 255;
	vertices[65].u = 0.0f;	vertices[65].v = 1.0f;
	vertices[65].nx = 0.0f; vertices[65].ny = -1.0f; vertices[65].nz = 0.0f;

	//hat
		//+z face
	vertices[66].x = -0.5f;	vertices[66].y = 0.5f;	vertices[66].z = 0.5f;
	vertices[66].r = 255;		vertices[66].g = 255;		vertices[66].b = 255;
	vertices[66].u = 0.0f;	vertices[66].v = 0.0f;
	vertices[66].nx = 0.0f; vertices[66].ny = 0.5f; vertices[66].nz = 0.5f;

	vertices[67].x = 0.5f;	vertices[67].y = 0.5f;	vertices[67].z = 0.5f;
	vertices[67].r = 255;		vertices[67].g = 255;		vertices[67].b = 255;
	vertices[67].u = 0.0f;	vertices[67].v = 1.0f;
	vertices[67].nx = 0.0f; vertices[67].ny = 0.5f; vertices[67].nz = 0.5f;

	vertices[68].x = 0.0f;	vertices[68].y = 1.0f;	vertices[68].z = 0.0f;
	vertices[68].r = 255;		vertices[68].g = 255;		vertices[68].b = 255;
	vertices[68].u = 0.5f;	vertices[68].v = 1.0f;
	vertices[68].nx = 0.0f; vertices[68].ny = 0.5f; vertices[68].nz = 0.5f;

	//+x face
	vertices[69].x = 0.5f;	vertices[69].y = 0.5f;	vertices[69].z = 0.5f;
	vertices[69].r = 255;		vertices[69].g = 255;		vertices[69].b = 255;
	vertices[69].u = 0.0f;	vertices[69].v = 0.0f;
	vertices[69].nx = 0.5f; vertices[69].ny = 0.5f; vertices[69].nz = 0.0f;

	vertices[70].x = 0.5f;	vertices[70].y = 0.5f;	vertices[70].z = -0.5f;
	vertices[70].r = 255;		vertices[70].g = 255;		vertices[70].b = 255;
	vertices[70].u = 0.0f;	vertices[70].v = 1.0f;
	vertices[70].nx = 0.5f; vertices[70].ny = 0.5f; vertices[70].nz = 0.0f;

	vertices[71].x = 0.0f;	vertices[71].y = 1.0f;	vertices[71].z = 0.0f;
	vertices[71].r = 255;		vertices[71].g = 255;		vertices[71].b = 255;
	vertices[71].u = 0.5f;	vertices[71].v = 1.0f;
	vertices[71].nx = 0.5f; vertices[71].ny = 0.5f; vertices[71].nz = 0.0f;

	//-z face
	vertices[72].x = 0.5f;	vertices[72].y = 0.5f;	vertices[72].z = -0.5f;
	vertices[72].r = 255;		vertices[72].g = 255;		vertices[72].b = 255;
	vertices[72].u = 0.0f;	vertices[72].v = 0.0f;
	vertices[72].nx = 0.0f; vertices[72].ny = 0.5f; vertices[72].nz = -0.5f;

	vertices[73].x = -0.5f;	vertices[73].y = 0.5f;	vertices[73].z = -0.5f;
	vertices[73].r = 255;		vertices[73].g = 255;		vertices[73].b = 255;
	vertices[73].u = 0.0f;	vertices[73].v = 1.0f;
	vertices[73].nx = 0.0f; vertices[73].ny = 0.5f; vertices[73].nz = -0.5f;

	vertices[74].x = 0.0f;	vertices[74].y = 1.0f;	vertices[74].z = 0.0f;
	vertices[74].r = 255;		vertices[74].g = 255;		vertices[74].b = 255;
	vertices[74].u = 0.5f;	vertices[74].v = 1.0f;
	vertices[74].nx = 0.0f; vertices[74].ny = 0.5f; vertices[74].nz = -0.5f;

	//-x face
	vertices[75].x = -0.5f;	vertices[75].y = 0.5f;	vertices[75].z = -0.5f;
	vertices[75].r = 255;		vertices[75].g = 255;		vertices[75].b = 255;
	vertices[75].u = 0.0f;	vertices[75].v = 0.0f;
	vertices[75].nx = -0.5f; vertices[75].ny = 0.5f; vertices[75].nz = 0.0f;

	vertices[76].x = -0.5f;	vertices[76].y = 0.5f;	vertices[76].z = 0.5f;
	vertices[76].r = 255;		vertices[76].g = 255;		vertices[76].b = 255;
	vertices[76].u = 0.0f;	vertices[76].v = 1.0f;
	vertices[76].nx = -0.5f; vertices[76].ny = 0.5f; vertices[76].nz = 0.0f;

	vertices[77].x = 0.0f;	vertices[77].y = 1.0f;	vertices[77].z = 0.0f;
	vertices[77].r = 255;		vertices[77].g = 255;		vertices[77].b = 255;
	vertices[77].u = 0.5f;	vertices[77].v = 1.0f;
	vertices[77].nx = -0.5f; vertices[77].ny = 0.5f; vertices[77].nz = 0.0f;

	// Create a vertex buffer object (VBO), and upload our vertices data to the VBO
	GLuint vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Create a vertex array object that contains data on how to map vertex attributes
	// (e.g., position, color) to vertex shader properties.
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	
	// Vertex attribute 0 - Position
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

	// Vertex attribute 1 - Color
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Vertex), (void*)(offsetof(Vertex, r)));

	// Vertex attribute 2 - UV-coordinates
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, u)));

	// Vertex attribute 3 - normal coordinates
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, nx)));

	glBindVertexArray(0);

	// Create a variable that will contain the ID of our first texture (pepe.jpg),
	// and use glGenTextures() to generate the texture itself
	GLuint tex0;
	glGenTextures(1, &tex0);

	// --- Load our pepe.jpg image using stb_image ---

	// Im image-space (pixels), (0, 0) is the upper-left corner of the image
	// However, in u-v coordinates, (0, 0) is the lower-left corner of the image
	// This means that the image will appear upside-down when we use the image data as is
	// This function tells stbi to flip the image vertically so that it is not upside-down when we use it
	stbi_set_flip_vertically_on_load(true);

	// 'imageWidth' and imageHeight will contain the width and height of the loaded image respectively
	int imageWidth, imageHeight, numChannels;

	// Read the image data of our pepe.jpg image, and store it in an unsigned char array
	unsigned char* imageData = stbi_load("pepe.jpg", &imageWidth, &imageHeight, &numChannels, 0);

	// Make sure that we actually loaded the image before uploading the data to the GPU
	if (imageData != nullptr)
	{
		// Our texture is 2D, so we bind our texture to the GL_TEXTURE_2D target
		glBindTexture(GL_TEXTURE_2D, tex0);

		// Set the filtering methods for magnification and minification
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

		// Set the wrapping method for the s-axis (x-axis) and t-axis (y-axis)
		// Try experimenting with different wrapping methods introduced in the textures slide set
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		// Upload the image data to GPU memory
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imageWidth, imageHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, imageData);

		// If we set minification to use mipmaps, we can tell OpenGL to generate the mipmaps for us
		//glGenerateMipmap(GL_TEXTURE_2D);

		// Once we have copied the data over to the GPU, we can delete
		// the data on the CPU side, since we won't be using it anymore
		stbi_image_free(imageData);
		imageData = nullptr;
	}
	else
	{
		std::cerr << "Failed to load pepe.jpg" << std::endl;
	}

	// Create our other texture (bioshock.jpg)
	GLuint tex1;
	glGenTextures(2, &tex1);

	// Read the image data of our bioshock.jpg image
	imageData = stbi_load("bioshock.jpg", &imageWidth, &imageHeight, &numChannels, 0);

	// Make sure that we actually loaded the image before uploading the data to the GPU
	if (imageData != nullptr)
	{
		// Our texture is 2D, so we bind our texture to the GL_TEXTURE_2D target
		glBindTexture(GL_TEXTURE_2D, tex1);

		// Set the filtering methods for magnification and minification
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

		// Set the wrapping method for the s-axis (x-axis) and t-axis (y-axis)
		// Try experimenting with different wrapping methods introduced in the textures slide set
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		// Upload the image data to GPU memory
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imageWidth, imageHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, imageData);

		// If we set minification to use mipmaps, we can tell OpenGL to generate the mipmaps for us
		//glGenerateMipmap(GL_TEXTURE_2D);

		// Once we have copied the data over to the GPU, we can delete
		// the data on the CPU side, since we won't be using it anymore
		stbi_image_free(imageData);
		imageData = nullptr;
	}
	else
	{
		std::cerr << "Failed to load bioshock.jpg" << std::endl;
	}

	// Create our other texture (bioshock.jpg)
	GLuint tex2;
	glGenTextures(3, &tex2);

	// Read the image data of our bioshock.jpg image
	imageData = stbi_load("color.jpg", &imageWidth, &imageHeight, &numChannels, 0);

	// Make sure that we actually loaded the image before uploading the data to the GPU
	if (imageData != nullptr)
	{
		// Our texture is 2D, so we bind our texture to the GL_TEXTURE_2D target
		glBindTexture(GL_TEXTURE_2D, tex2);

		// Set the filtering methods for magnification and minification
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

		// Set the wrapping method for the s-axis (x-axis) and t-axis (y-axis)
		// Try experimenting with different wrapping methods introduced in the textures slide set
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		// Upload the image data to GPU memory
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imageWidth, imageHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, imageData);

		// If we set minification to use mipmaps, we can tell OpenGL to generate the mipmaps for us
		//glGenerateMipmap(GL_TEXTURE_2D);

		// Once we have copied the data over to the GPU, we can delete
		// the data on the CPU side, since we won't be using it anymore
		stbi_image_free(imageData);
		imageData = nullptr;
	}
	else
	{
		std::cerr << "Failed to load color.jpg" << std::endl;
	}

	// Create a shader program
	GLuint program = CreateShaderProgram("main.vsh", "main.fsh");

	// Tell OpenGL the dimensions of the region where stuff will be drawn.
	// For now, tell OpenGL to use the whole screen
	glViewport(0, 0, windowWidth, windowHeight);

	// We enable depth testing so that we use the z-axis to determine
	// which objects goes in front of which object (when overlapping geometry is drawn)
	glEnable(GL_DEPTH_TEST);

	bool toggleBody = false;
	bool toggleHead = false;
	bool hasToggledBody = false;
	bool hasToggledHead = false;
	float timer = 0.0f;
	float offsetTime = 0.0f;
	// Render loop
	while (!glfwWindowShouldClose(window))
	{
		if (timer > 1.5f)
		{
			offsetTime += timer;
			timer = 0;
		}

		timer = roundOff(glfwGetTime()) - offsetTime;

		if (timer == 0.25f)
		{
			if (!hasToggledBody)
			{
				toggleBody = true;
				hasToggledBody = true;
			}
		}
		else if (timer == 1.0f)
		{
			if (!hasToggledBody)
			{
				toggleBody = false;
				hasToggledBody = true;
			}
		}
		else if (timer == 0.5f)
		{
			if (!hasToggledHead)
			{
				toggleHead = true;
				hasToggledHead = true;
			}
		}
		else if (timer == 0.75f)
		{
			if (!hasToggledHead)
			{
				toggleHead = false;
				hasToggledHead = true;
			}
		}
		else
		{
			hasToggledBody = false;
			hasToggledHead = false;
		}

		// per-frame time logic
		// --------------------
		float currentFrame = static_cast<float>(glfwGetTime());
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// input
		// -----
		processInput(window);

		// Clear the colors and depth values (since we enabled depth testing) in our off-screen framebuffer
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Use the shader program that we created
		glUseProgram(program);

		// Use the vertex array object that we created
		glBindVertexArray(vao);

		// Construct our view matrix (for the "camera")
		// Let's say we want to position our camera to be at (2, 1, 4) and looking down at the origin (0, 0, 0).
		// For the position, remember that having our camera at (2, 1, 4) is the same as moving the entire world in the opposite direction (-2, -1, -4)
		// As for the orientation of the camera, we can use the lookAt function, which glm kindly provides us
		glm::mat4 viewMatrix(1.0f);
		viewMatrix = glm::translate(viewMatrix, -cameraPosition); // Note the negative translation

		glm::vec3 eye = cameraPosition; // Eye is where our camera is
		glm::mat4 lookAtMatrix = glm::lookAt(eye, target, up);

		viewMatrix = viewMatrix * lookAtMatrix;
		GLint viewMatrixUniform = glGetUniformLocation(program, "viewMatrix");
		glUniformMatrix4fv(viewMatrixUniform, 1, GL_FALSE, glm::value_ptr(viewMatrix));

		//uniform for camera/eye position
		GLint cameraPositionUniform = glGetUniformLocation(program, "cameraPosition");
		glUniform3fv(cameraPositionUniform, 1, glm::value_ptr(cameraPosition));

		//Phong Lighting Model uniforms
		GLint ambientComponentUniform = glGetUniformLocation(program, "ambientComponent");
		GLint diffuseComponentUniform = glGetUniformLocation(program, "diffuseComponent");
		GLint specularComponentUniform = glGetUniformLocation(program, "specularComponent");
		GLint specularIntensityUniform = glGetUniformLocation(program, "specularIntensity");
		GLint shineUniform = glGetUniformLocation(program, "shine");

		// Passing the light uniforms
		glUniform1f(ambientComponentUniform, ambientComponent);
		glUniform1f(diffuseComponentUniform, diffuseComponent);
		glUniform1f(specularComponentUniform, specularComponent);
		glUniform3fv(specularIntensityUniform, 1, glm::value_ptr(specularIntensity));
		glUniform1f(shineUniform, shine);

		//uniform for light positions
		GLint PointlightPosUniform = glGetUniformLocation(program, "PointlightPos");
		glUniform3fv(PointlightPosUniform, 1, glm::value_ptr(PointlightPos));
		GLint SpotlightPosUniform = glGetUniformLocation(program, "SpotlightPos");
		glUniform3fv(SpotlightPosUniform, 1, glm::value_ptr(SpotlightPos));

		// Construct our view frustrum (projection matrix) using the following parameters
		float fieldOfViewY = glm::radians(45.0f); // Field of view
		float aspectRatio = windowWidth * 1.0f / windowHeight; // Aspect ratio, which is the ratio between width and height
		float nearPlane = 0.1f; // Near plane, minimum distance from the camera where things will be rendered
		float farPlane = 30.0f; // Far plane, maximum distance from the camera where things will be rendered
		glm::mat4 projectionMatrix = glm::perspective(fieldOfViewY, aspectRatio, nearPlane, farPlane);

		GLint projectionMatrixUniform = glGetUniformLocation(program, "projectionMatrix");
		glUniformMatrix4fv(projectionMatrixUniform, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

		// We retrieve our 'modelMatrix' uniform variable from the vertex shader,
		GLint modelMatrixUniform = glGetUniformLocation(program, "modelMatrix");

		// Create a 4x4 matrix that will be our model matrix,
		// and initialize it to be the identity matrix.
		// The model matrix is a series of affine transformations that will place our object
		// in the world (local space -> world space)
		glm::mat4 modelMatrix(1.0f);

		// For the first quad, let's scale it by half the size, and move it to the right and down via translation
		// The matrix multiplication chain should look like: (Identity) * Translation * Scale
		// glm::translate() is a function that takes an existing matrix, and appends a translation matrix
		// to the RIGHT given the tx, ty, tz values.
		glm::vec3 translationVector = glm::vec3(5.0f, 3.0f, -9.9f);
		modelMatrix = glm::translate(modelMatrix, translationVector);

		// At this point, we now have: Identity * Translation * Rotate
		//rotate on z-axis by 30 degrees
		glm::vec3 rotationAxis(1.0f, 0.0f, 0.0f);
		modelMatrix = glm::rotate(modelMatrix, glm::radians(0.0f), rotationAxis);

		// glm::scale() is a function that takes an existing matrix, and appends a scale matrix to the RIGHT given the sx, sy, sz values.
		// Let's scale the quad on all axes by 2.0.
		glm::vec3 scaleVector(3.0f, 3.0f, 3.0f);
		modelMatrix = glm::scale(modelMatrix, scaleVector);
		// At this point, we now have: Identity * Translation * Rotate * Scale

		// Set the value of our transformationMatrix uniform variable in the vertex shader to our matrix here
		// The first parameter is the uniform location of the uniform we want to set the value of.
		// The second parameter is the number of matrices to set the uniform with (we only have 1 matrix, so we give it a value of 1)
		// The third parameter is a boolean flag to indicate whether to transpose the matrix or not. In our case, we do not need to since glm makes matrix that are column-major (same with OpenGL)
		// The fourth parameter is a pointer to the matrices that we will set the uniform (actual data)
		glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, glm::value_ptr(modelMatrix));

		ambientComponent = 0.1f;
		diffuseComponent = 0.1f;
		specularComponent = 1.0f;
		shine = 0.5f;

		// Passing the light uniforms
		glUniform1f(ambientComponentUniform, ambientComponent);
		glUniform1f(diffuseComponentUniform, diffuseComponent);
		glUniform1f(specularComponentUniform, specularComponent);
		glUniform3fv(specularIntensityUniform, 1, glm::value_ptr(specularIntensity));
		glUniform1f(shineUniform, shine);

		// Draw a quad (4 vertices) using a triangle-fan
		// Draw the cube (36 vertices) using a triangle
		//glDrawArrays(GL_TRIANGLES, 0, 36);

		// Bind our bioshock.jpg texture to texture unit 0
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, 4);

		//Drawing the QUAD
		glDrawArrays(GL_TRIANGLES, 36, 6);

		// Now for the second quad (wall.jpg), let's scale it by 1.5, rotate it by 45 degrees along the z-axis,
		// and then move it to the right and up.
		modelMatrix = glm::mat4(1.0f);
		// (Identity) * Translation
		translationVector = glm::vec3(0.0f, 9.0f, 0.0f);
		modelMatrix = glm::translate(modelMatrix, translationVector);

		// (Identity) * Translation * Rotation
		//modelMatrix = glm::rotate(modelMatrix, (float)glfwGetTime() * glm::radians(50.0f), rotationAxis);

		// (Identity) * Translation * (Rotation) * Scale
		scaleVector = glm::vec3(10.0f, 10.0f, 10.0f);
		modelMatrix = glm::scale(modelMatrix, scaleVector);

		// We now update our moelMatrix uniform to have the new model matrix
		glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, glm::value_ptr(modelMatrix));

		ambientComponent = 0.1f;
		diffuseComponent = 5.0f;
		specularComponent = 1.0f;
		shine = 1.0f;

		// Passing the light uniforms
		glUniform1f(ambientComponentUniform, ambientComponent);
		glUniform1f(diffuseComponentUniform, diffuseComponent);
		glUniform1f(specularComponentUniform, specularComponent);
		glUniform3fv(specularIntensityUniform, 1, glm::value_ptr(specularIntensity));
		glUniform1f(shineUniform, shine);

		// Bind our bioshock.jpg texture to texture unit 0
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, 1);

		// Drawing the ROOM
		glDrawArrays(GL_TRIANGLES, 0, 36);
		
		// Create a 4x4 matrix that will be our model matrix,
		// and initialize it to be the identity matrix.
		// The model matrix is a series of affine transformations that will place our object
		// in the world (local space -> world space)
		modelMatrix = glm::mat4(1.0f);

		// For the first quad (pepe.jpg texture), let's scale it by half the size, and move it to the left via translation
		// The matrix multiplication chain should look like: (Identity) * Translation * Scale
		// glm::translate() is a function that takes an existing matrix, and appends a translation matrix
		// to the RIGHT given the tx, ty, tz values.
		translationVector = glm::vec3(-3.0f, -0.5f, -5.0f);

		if (toggleBody)
		{
			translationVector += glm::vec3(0.0f, 0.25f, 0.0f);
		}
		else
		{
			translationVector += glm::vec3(0.0f, 0.0f, 0.0f);
		}

		modelMatrix = glm::translate(modelMatrix, translationVector);
		// At this point, we now have: Identity * Translation

		//(Identity) * Translation * Rotation
		glm::vec3 rotationAxis1(0.0f, 1.0f, 0.0f);

		modelMatrix = glm::rotate(modelMatrix, glm::radians(90.0f), rotationAxis1);

		// glm::scale() is a function that takes an existing matrix, and appends a scale matrix to the RIGHT given the sx, sy, sz values.
		// Let's scale the quad on all axes by 2.0.
		scaleVector = glm::vec3(0.25f, 0.5f, 0.25f);
		modelMatrix = glm::scale(modelMatrix, scaleVector);
		// At this point, we now have: Identity * Translation * Scale

		// Set the value of our transformationMatrix uniform variable in the vertex shader to our matrix here
		// The first parameter is the uniform location of the uniform we want to set the value of.
		// The second parameter is the number of matrices to set the uniform with (we only have 1 matrix, so we give it a value of 1)
		// The third parameter is a boolean flag to indicate whether to transpose the matrix or not. In our case, we do not need to since glm makes matrix that are column-major (same with OpenGL)
		// The fourth parameter is a pointer to the matrices that we will set the uniform (actual data)
		glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, glm::value_ptr(modelMatrix));

		ambientComponent = 0.1f;
		diffuseComponent = 0.1f;
		specularComponent = 1.0f;
		shine = 1.0f;

		// Passing the light uniforms
		glUniform1f(ambientComponentUniform, ambientComponent);
		glUniform1f(diffuseComponentUniform, diffuseComponent);
		glUniform1f(specularComponentUniform, specularComponent);
		glUniform3fv(specularIntensityUniform, 1, glm::value_ptr(specularIntensity));
		glUniform1f(shineUniform, shine);

		// Bind our pepe.jpg texture to texture unit 0
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, tex1);

		// Drawing the BODY
		glDrawArrays(GL_TRIANGLES, 0, 36);

		// Now for the second quad (bioshock.jpg), let's scale it by 1.5, rotate it by 45 degrees along the z-axis,
		// and then move it to the right and up.
		modelMatrix = glm::mat4(1.0f);
		// (Identity) * Translation
		translationVector = glm::vec3(-3.0f, 0.5f, -5.0f);

		if (toggleBody)
		{
			translationVector += glm::vec3(0.0f, 0.25f, 0.0f);
		}
		else
		{
			translationVector += glm::vec3(0.0f, 0.0f, 0.0f);
		}

		if (toggleHead)
		{
			translationVector += glm::vec3(0.0f, 0.25f, 0.0f);
		}
		modelMatrix = glm::translate(modelMatrix, translationVector);

		//(Identity) * Translation * Rotation
		/*glm::vec3 rotationAxis2(0.0f, 1.0f, 0.0f);
		modelMatrix = glm::rotate(modelMatrix, glm::radians(90.0f), rotationAxis2);*/

		// (Identity) * Translation * Rotation * Scale
		scaleVector = glm::vec3(0.5f, 0.5f, 0.5f);
		modelMatrix = glm::scale(modelMatrix, scaleVector);

		// We now update our moelMatrix uniform to have the new model matrix
		glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, glm::value_ptr(modelMatrix));

		ambientComponent = 0.1f;
		diffuseComponent = 0.1f;
		specularComponent = 1.0f;
		shine = 0.5f;

		// Passing the light uniforms
		glUniform1f(ambientComponentUniform, ambientComponent);
		glUniform1f(diffuseComponentUniform, diffuseComponent);
		glUniform1f(specularComponentUniform, specularComponent);
		glUniform3fv(specularIntensityUniform, 1, glm::value_ptr(specularIntensity));
		glUniform1f(shineUniform, shine);

		// Bind our bioshock.jpg texture to texture unit 0
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, tex2);

		// Drawing the HEAD
		glDrawArrays(GL_TRIANGLES, 6, 6);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, tex1);

		glDrawArrays(GL_TRIANGLES, 0, 6);
		glDrawArrays(GL_TRIANGLES, 12, 24);

		// Now for the second quad (bioshock.jpg), let's scale it by 1.5, rotate it by 45 degrees along the z-axis,
		// and then move it to the right and up.
		modelMatrix = glm::mat4(1.0f);
		// (Identity) * Translation
		translationVector = glm::vec3(-3.0f, 0.5f, -5.0f);

		if (toggleBody)
		{
			translationVector += glm::vec3(0.0f, 0.25f, 0.0f);
		}
		else
		{
			translationVector += glm::vec3(0.0f, 0.0f, 0.0f);
		}

		if (toggleHead)
		{
			translationVector += glm::vec3(0.0f, 0.25f, 0.0f);
		}
		modelMatrix = glm::translate(modelMatrix, translationVector);

		//(Identity) * Translation * Rotation
		/*glm::vec3 rotationAxis2(0.0f, 1.0f, 0.0f);
		modelMatrix = glm::rotate(modelMatrix, glm::radians(90.0f), rotationAxis2);*/

		// (Identity) * Translation * Rotation * Scale
		scaleVector = glm::vec3(1.0f, 1.0f, 1.0f);
		modelMatrix = glm::scale(modelMatrix, scaleVector);

		// We now update our moelMatrix uniform to have the new model matrix
		glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, glm::value_ptr(modelMatrix));

		// Bind our bioshock.jpg texture to texture unit 0
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, tex1);

		// Drawing the HAT
		glDrawArrays(GL_TRIANGLES, 66, 12);

		// "Unuse" the vertex array object
		glBindVertexArray(0);

		// Tell GLFW to swap the screen buffer with the offscreen buffer
		glfwSwapBuffers(window);

		// Tell GLFW to process window events (e.g., input events, window closed events, etc.)
		glfwPollEvents();
	}

	// --- Cleanup ---

	// Make sure to delete the shader program
	glDeleteProgram(program);

	// Delete the VBO that contains our vertices
	glDeleteBuffers(1, &vbo);

	// Delete the vertex array object
	glDeleteVertexArrays(1, &vao);

	// Delete our textures
	glDeleteTextures(1, &tex0);
	glDeleteTextures(1, &tex1);

	// Remember to tell GLFW to clean itself up before exiting the application
	glfwTerminate();

	return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	float cameraSpeed = static_cast<float>(2.5 * deltaTime);
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		cameraPosition += target * cameraSpeed;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		cameraPosition -= target * cameraSpeed;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		cameraPosition -= glm::normalize(glm::cross(target, up)) * cameraSpeed;
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		cameraPosition += glm::normalize(glm::cross(target, up)) * cameraSpeed;
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
		cameraPosition += up * cameraSpeed;
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
		cameraPosition -= up * cameraSpeed;
}

/**
 * @brief Creates a shader program based on the provided file paths for the vertex and fragment shaders.
 * @param[in] vertexShaderFilePath Vertex shader file path
 * @param[in] fragmentShaderFilePath Fragment shader file path
 * @return OpenGL handle to the created shader program
 */
GLuint CreateShaderProgram(const std::string& vertexShaderFilePath, const std::string& fragmentShaderFilePath)
{
	GLuint vertexShader = CreateShaderFromFile(GL_VERTEX_SHADER, vertexShaderFilePath);
	GLuint fragmentShader = CreateShaderFromFile(GL_FRAGMENT_SHADER, fragmentShaderFilePath);

	GLuint program = glCreateProgram();
	glAttachShader(program, vertexShader);
	glAttachShader(program, fragmentShader);

	glLinkProgram(program);

	glDetachShader(program, vertexShader);
	glDeleteShader(vertexShader);
	glDetachShader(program, fragmentShader);
	glDeleteShader(fragmentShader);

	// Check shader program link status
	GLint linkStatus;
	glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);
	if (linkStatus != GL_TRUE) {
		char infoLog[512];
		GLsizei infoLogLen = sizeof(infoLog);
		glGetProgramInfoLog(program, infoLogLen, &infoLogLen, infoLog);
		std::cerr << "program link error: " << infoLog << std::endl;
	}

	return program;
}

/**
 * @brief Creates a shader based on the provided shader type and the path to the file containing the shader source.
 * @param[in] shaderType Shader type
 * @param[in] shaderFilePath Path to the file containing the shader source
 * @return OpenGL handle to the created shader
 */
GLuint CreateShaderFromFile(const GLuint& shaderType, const std::string& shaderFilePath)
{
	std::ifstream shaderFile(shaderFilePath);
	if (shaderFile.fail())
	{
		std::cerr << "Unable to open shader file: " << shaderFilePath << std::endl;
		return 0;
	}

	std::string shaderSource;
	std::string temp;
	while (std::getline(shaderFile, temp))
	{
		shaderSource += temp + "\n";
	}
	shaderFile.close();

	return CreateShaderFromSource(shaderType, shaderSource);
}

/**
 * @brief Creates a shader based on the provided shader type and the string containing the shader source.
 * @param[in] shaderType Shader type
 * @param[in] shaderSource Shader source string
 * @return OpenGL handle to the created shader
 */
GLuint CreateShaderFromSource(const GLuint& shaderType, const std::string& shaderSource)
{
	GLuint shader = glCreateShader(shaderType);

	const char* shaderSourceCStr = shaderSource.c_str();
	GLint shaderSourceLen = static_cast<GLint>(shaderSource.length());
	glShaderSource(shader, 1, &shaderSourceCStr, &shaderSourceLen);
	glCompileShader(shader);

	// Check compilation status
	GLint compileStatus;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &compileStatus);
	if (compileStatus == GL_FALSE)
	{
		char infoLog[512];
		GLsizei infoLogLen = sizeof(infoLog);
		glGetShaderInfoLog(shader, infoLogLen, &infoLogLen, infoLog);
		std::cerr << "shader compilation error: " << infoLog << std::endl;
	}

	return shader;
}

/**
 * @brief Function for handling the event when the size of the framebuffer changed.
 * @param[in] window Reference to the window
 * @param[in] width New width
 * @param[in] height New height
 */
void FramebufferSizeChangedCallback(GLFWwindow* window, int width, int height)
{
	// Whenever the size of the framebuffer changed (due to window resizing, etc.),
	// update the dimensions of the region to the new size
	glViewport(0, 0, width, height);
}

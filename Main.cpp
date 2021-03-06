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
};

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
	vertices[0].x = -1.0f;	vertices[0].y = -1.0f;	vertices[0].z = -1.0f;
	vertices[0].r = 255;	vertices[0].g = 0;		vertices[0].b = 0;

	//bottom right (in relation to own normal vector)
	vertices[1].x = 1.0f;	vertices[1].y = -1.0f;	vertices[1].z = -1.0f;
	vertices[1].r = 0;		vertices[1].g = 255;	vertices[1].b = 0;

	//top right (in relation to own normal vector)
	vertices[2].x = 1.0f;	vertices[2].y = 1.0f;	vertices[2].z = -1.0f;
	vertices[2].r = 0;		vertices[2].g = 0;		vertices[2].b = 255;

	//bottom left (in relation to own normal vector)
	vertices[3].x = -1.0f;	vertices[3].y = -1.0f;	vertices[3].z = -1.0f;
	vertices[3].r = 255;	vertices[3].g = 0;		vertices[3].b = 0;

	//top left (in relation to own normal vector)
	vertices[4].x = -1.0f;	vertices[4].y = 1.0f;	vertices[4].z = -1.0f;
	vertices[4].r = 0;		vertices[4].g = 255;	vertices[4].b = 0;

	//top right (in relation to own normal vector)
	vertices[5].x = 1.0f;	vertices[5].y = 1.0f;	vertices[5].z = -1.0f;
	vertices[5].r = 0;		vertices[5].g = 0;		vertices[5].b = 255;


	//backmost face to view space
		//bottom right (in relation to own normal vector)
	vertices[6].x = -1.0f;	vertices[6].y = -1.0f;	vertices[6].z = 1.0f;
	vertices[6].r = 255;	vertices[6].g = 0;		vertices[6].b = 0;

	//bottom left (in relation to own normal vector)
	vertices[7].x = 1.0f;	vertices[7].y = -1.0f;	vertices[7].z = 1.0f;
	vertices[7].r = 0;		vertices[7].g = 255;	vertices[7].b = 0;

	//top left (in relation to own normal vector)
	vertices[8].x = 1.0f;	vertices[8].y = 1.0f;	vertices[8].z = 1.0f;
	vertices[8].r = 0;		vertices[8].g = 0;		vertices[8].b = 255;

	//bottom right (in relation to own normal vector)
	vertices[9].x = -1.0f;	vertices[9].y = -1.0f;	vertices[9].z = 1.0f;
	vertices[9].r = 255;	vertices[9].g = 0;		vertices[9].b = 0;

	//top right (in relation to own normal vector)
	vertices[10].x = -1.0f;	vertices[10].y = 1.0f;	vertices[10].z = 1.0f;
	vertices[10].r = 0;		vertices[10].g = 255;	vertices[10].b = 0;

	//top left (in relation to own normal vector)
	vertices[11].x = 1.0f;	vertices[11].y = 1.0f;	vertices[11].z = 1.0f;
	vertices[11].r = 0;		vertices[11].g = 0;		vertices[11].b = 255;


	//leftmost face to view space
		//bottom left (in relation to own normal vector)
	vertices[12].x = -1.0f;	vertices[12].y = -1.0f;	vertices[12].z = -1.0f;
	vertices[12].r = 255;	vertices[12].g = 0;		vertices[12].b = 0;

	//bottom right (in relation to own normal vector)
	vertices[13].x = -1.0f;	vertices[13].y = -1.0f;	vertices[13].z = 1.0f;
	vertices[13].r = 0;		vertices[13].g = 255;	vertices[13].b = 0;

	//top right (in relation to own normal vector)
	vertices[14].x = -1.0f;	vertices[14].y = 1.0f;	vertices[14].z = 1.0f;
	vertices[14].r = 0;		vertices[14].g = 0;		vertices[14].b = 255;

	//bottom left (in relation to own normal vector)
	vertices[15].x = -1.0f;	vertices[15].y = -1.0f;	vertices[15].z = -1.0f;
	vertices[15].r = 255;	vertices[15].g = 0;		vertices[15].b = 0;

	//top left (in relation to own normal vector)
	vertices[16].x = -1.0f;	vertices[16].y = 1.0f;	vertices[16].z = -1.0f;
	vertices[16].r = 0;		vertices[16].g = 255;	vertices[16].b = 0;

	//top right (in relation to own normal vector)
	vertices[17].x = -1.0f;	vertices[17].y = 1.0f;	vertices[17].z = 1.0f;
	vertices[17].r = 0;		vertices[17].g = 0;		vertices[17].b = 255;


	//rightmost face to view space
		//bottom left (in relation to own normal vector)
	vertices[18].x = 1.0f;	vertices[18].y = -1.0f;	vertices[18].z = 1.0f;
	vertices[18].r = 255;	vertices[18].g = 0;		vertices[18].b = 0;

	//bottom right (in relation to own normal vector)
	vertices[19].x = 1.0f;	vertices[19].y = -1.0f;	vertices[19].z = -1.0f;
	vertices[19].r = 0;		vertices[19].g = 255;	vertices[19].b = 0;

	//top right (in relation to own normal vector)
	vertices[20].x = 1.0f;	vertices[20].y = 1.0f;	vertices[20].z = -1.0f;
	vertices[20].r = 0;		vertices[20].g = 0;		vertices[20].b = 255;

	//bottom left (in relation to own normal vector)
	vertices[21].x = 1.0f;	vertices[21].y = -1.0f;	vertices[21].z = 1.0f;
	vertices[21].r = 255;	vertices[21].g = 0;		vertices[21].b = 0;

	//top left (in relation to own normal vector)
	vertices[22].x = 1.0f;	vertices[22].y = 1.0f;	vertices[22].z = 1.0f;
	vertices[22].r = 0;		vertices[22].g = 255;	vertices[22].b = 0;

	//top right (in relation to own normal vector)
	vertices[23].x = 1.0f;	vertices[23].y = 1.0f;	vertices[23].z = -1.0f;
	vertices[23].r = 0;		vertices[23].g = 0;		vertices[23].b = 255;


	//topmost face to view space
		//bottom left (in relation to own normal vector)
	vertices[24].x = -1.0f;	vertices[24].y = 1.0f;	vertices[24].z = 1.0f;
	vertices[24].r = 255;	vertices[24].g = 0;		vertices[24].b = 0;

	//bottom right (in relation to own normal vector)
	vertices[25].x = 1.0f;	vertices[25].y = 1.0f;	vertices[25].z = 1.0f;
	vertices[25].r = 0;		vertices[25].g = 255;	vertices[25].b = 0;

	//top right (in relation to own normal vector)
	vertices[26].x = 1.0f;	vertices[26].y = 1.0f;	vertices[26].z = -1.0f;
	vertices[26].r = 0;		vertices[26].g = 0;		vertices[26].b = 255;

	//bottom left (in relation to own normal vector)
	vertices[27].x = -1.0f;	vertices[27].y = 1.0f;	vertices[27].z = 1.0f;
	vertices[27].r = 255;	vertices[27].g = 0;		vertices[27].b = 0;

	//top left (in relation to own normal vector)
	vertices[28].x = -1.0f;	vertices[28].y = 1.0f;	vertices[28].z = -1.0f;
	vertices[28].r = 0;		vertices[28].g = 255;	vertices[28].b = 0;

	//top right (in relation to own normal vector)
	vertices[29].x = 1.0f;	vertices[29].y = 1.0f;	vertices[29].z = -1.0f;
	vertices[29].r = 0;		vertices[29].g = 0;		vertices[29].b = 255;


	//bottommost face to view space
		//bottom left (in relation to own normal vector)
	vertices[30].x = 1.0f;	vertices[30].y = -1.0f;	vertices[30].z = 1.0f;
	vertices[30].r = 255;	vertices[30].g = 0;		vertices[30].b = 0;

	//bottom right (in relation to own normal vector)
	vertices[31].x = -1.0f;	vertices[31].y = -1.0f;	vertices[31].z = 1.0f;
	vertices[31].r = 0;		vertices[31].g = 255;	vertices[31].b = 0;

	//top right (in relation to own normal vector)
	vertices[32].x = -1.0f;	vertices[32].y = -1.0f;	vertices[32].z = -1.0f;
	vertices[32].r = 0;		vertices[32].g = 0;		vertices[32].b = 255;

	//bottom left (in relation to own normal vector)
	vertices[33].x = 1.0f;	vertices[33].y = -1.0f;	vertices[33].z = 1.0f;
	vertices[33].r = 255;	vertices[33].g = 0;		vertices[33].b = 0;

	//top left (in relation to own normal vector)
	vertices[34].x = 1.0f;	vertices[34].y = -1.0f;	vertices[34].z = -1.0f;
	vertices[34].r = 0;		vertices[34].g = 255;	vertices[34].b = 0;

	//top right (in relation to own normal vector)
	vertices[35].x = -1.0f;	vertices[35].y = -1.0f;	vertices[35].z = -1.0f;
	vertices[35].r = 0;		vertices[35].g = 0;		vertices[35].b = 255;


	//for quad
		//bottom left (in relation to own normal vector)
	vertices[36].x = -0.5f;	vertices[36].y = -1.0f;	vertices[36].z = 0.0f;
	vertices[36].r = 128;	vertices[36].g = 128;	vertices[36].b = 128;

	//bottom right (in relation to own normal vector)
	vertices[37].x = 0.5f;	vertices[37].y = -1.0f;	vertices[37].z = 0.0f;
	vertices[37].r = 128;	vertices[37].g = 128;	vertices[37].b = 128;

	//top right (in relation to own normal vector)
	vertices[38].x = 0.3f;	vertices[38].y = 0.0f;	vertices[38].z = 0.0f;
	vertices[38].r = 128;	vertices[38].g = 128;	vertices[38].b = 128;

	//bottom left (in relation to own normal vector)
	vertices[39].x = -0.5f;	vertices[39].y = -1.0f;	vertices[39].z = 0.0f;
	vertices[39].r = 128;	vertices[39].g = 128;	vertices[39].b = 128;

	//top left (in relation to own normal vector)
	vertices[40].x = -0.3f;	vertices[40].y = 0.0f;	vertices[40].z = 0.0f;
	vertices[40].r = 128;	vertices[40].g = 128;	vertices[40].b = 128;

	//top right (in relation to own normal vector)
	vertices[41].x = 0.3f;	vertices[41].y = 0.0f;	vertices[41].z = 0.0f;
	vertices[41].r = 128;	vertices[41].g = 128;	vertices[41].b = 128;
	
	//box
	vertices[42].x = -0.5f;	vertices[42].y = -0.5f;	vertices[42].z = 0.5f;
	vertices[42].r = 255;	vertices[42].g = 0;		vertices[42].b = 255;
	vertices[42].u = 0.0f;	vertices[42].v = 0.0f;

	vertices[43].x = 0.5f;	vertices[43].y = -0.5f;	vertices[43].z = 0.5f;
	vertices[43].r = 0;		vertices[43].g = 255;	vertices[43].b = 0;
	vertices[43].u = 1.0f;	vertices[43].v = 0.0f;

	vertices[44].x = 0.5f;	vertices[44].y = 0.5f;	vertices[44].z = 0.5f;
	vertices[44].r = 0;		vertices[44].g = 0;		vertices[44].b = 255;
	vertices[44].u = 1.0f;	vertices[44].v = 1.0f;

	vertices[45].x = -0.5f;	vertices[45].y = 0.5f;	vertices[45].z = 0.5f;
	vertices[45].r = 0;		vertices[45].g = 0;		vertices[45].b = 0;
	vertices[45].u = 0.0f;	vertices[45].v = 1.0f;

	vertices[46].x = 0.5f;	vertices[46].y = -0.5f;	vertices[46].z = 0.5f;
	vertices[46].r = 255;	vertices[46].g = 0;		vertices[46].b = 0;
	vertices[46].u = 0.0f;	vertices[46].v = 0.0f;

	vertices[47].x = 0.5f;	vertices[47].y = -0.5f;	vertices[47].z = -0.5f;
	vertices[47].r = 0;		vertices[47].g = 0;		vertices[47].b = 255;
	vertices[47].u = 1.0f;	vertices[47].v = 0.0f;

	vertices[48].x = 0.5f;	vertices[48].y = 0.5f;	vertices[48].z = -0.5f;
	vertices[48].r = 0;		vertices[48].g = 255;	vertices[48].b = 0;
	vertices[48].u = 1.0f;	vertices[48].v = 1.0f;

	vertices[49].x = 0.5f;	vertices[49].y = 0.5f;	vertices[49].z = 0.5f;
	vertices[49].r = 0;		vertices[49].g = 0;		vertices[49].b = 0;
	vertices[49].u = 0.0f;	vertices[49].v = 1.0f;

	vertices[50].x = 0.5f;	vertices[50].y = -0.5f;	vertices[50].z = -0.5f;
	vertices[50].r = 0;		vertices[50].g = 0;		vertices[50].b = 255;
	vertices[50].u = 0.0f;	vertices[50].v = 0.0f;

	vertices[51].x = -0.5f;	vertices[51].y = -0.5f;	vertices[51].z = -0.5f;
	vertices[51].r = 0;		vertices[51].g = 0;		vertices[51].b = 0;
	vertices[51].u = 1.0f;	vertices[51].v = 0.0f;

	vertices[52].x = -0.5f;	vertices[52].y = 0.5f;	vertices[52].z = -0.5f;
	vertices[52].r = 0;		vertices[52].g = 0;		vertices[52].b = 255;
	vertices[52].u = 1.0f;	vertices[52].v = 1.0f;

	vertices[53].x = 0.5f;	vertices[53].y = 0.5f;	vertices[53].z = -0.5f;
	vertices[53].r = 0;		vertices[53].g = 0;		vertices[53].b = 0;
	vertices[53].u = 0.0f;	vertices[53].v = 1.0f;

	vertices[54].x = -0.5f;	vertices[54].y = -0.5f;	vertices[54].z = -0.5f;
	vertices[54].r = 0;		vertices[54].g = 0;		vertices[54].b = 255;
	vertices[54].u = 0.0f;	vertices[54].v = 0.0f;

	vertices[55].x = -0.5f;	vertices[55].y = -0.5f;	vertices[55].z = 0.5f;
	vertices[55].r = 0;		vertices[55].g = 0;		vertices[55].b = 0;
	vertices[55].u = 1.0f;	vertices[55].v = 0.0f;

	vertices[56].x = -0.5f;	vertices[56].y = 0.5f;	vertices[56].z = 0.5f;
	vertices[56].r = 0;		vertices[56].g = 0;		vertices[56].b = 255;
	vertices[56].u = 1.0f;	vertices[56].v = 1.0f;

	vertices[57].x = -0.5f;	vertices[57].y = 0.5f;	vertices[57].z = -0.5f;
	vertices[57].r = 0;		vertices[57].g = 0;		vertices[57].b = 0;
	vertices[57].u = 0.0f;	vertices[57].v = 1.0f;

	vertices[58].x = -0.5f;	vertices[58].y = 0.5f;	vertices[58].z = 0.5f;
	vertices[58].r = 0;		vertices[58].g = 0;		vertices[58].b = 255;
	vertices[58].u = 0.0f;	vertices[58].v = 0.0f;

	vertices[59].x = 0.5f;	vertices[59].y = 0.5f;	vertices[59].z = 0.5f;
	vertices[59].r = 0;		vertices[59].g = 0;		vertices[59].b = 0;
	vertices[59].u = 1.0f;	vertices[59].v = 0.0f;

	vertices[60].x = 0.5f;	vertices[60].y = 0.5f;	vertices[60].z = -0.5f;
	vertices[60].r = 0;		vertices[60].g = 0;		vertices[60].b = 255;
	vertices[60].u = 1.0f;	vertices[60].v = 1.0f;

	vertices[61].x = -0.5f;	vertices[61].y = 0.5f;	vertices[61].z = -0.5f;
	vertices[61].r = 0;		vertices[61].g = 0;		vertices[61].b = 0;
	vertices[61].u = 0.0f;	vertices[61].v = 1.0f;

	vertices[62].x = -0.5f;	vertices[62].y = -0.5f;	vertices[62].z = -0.5f;
	vertices[62].r = 0;		vertices[62].g = 0;		vertices[62].b = 255;
	vertices[62].u = 0.0f;	vertices[62].v = 0.0f;

	vertices[63].x = 0.5f;	vertices[63].y = -0.5f;	vertices[63].z = -0.5f;
	vertices[63].r = 0;		vertices[63].g = 0;		vertices[63].b = 0;
	vertices[63].u = 1.0f;	vertices[63].v = 0.0f;

	vertices[64].x = 0.5f;	vertices[64].y = -0.5f;	vertices[64].z = 0.5f;
	vertices[64].r = 0;		vertices[64].g = 0;		vertices[64].b = 255;
	vertices[64].u = 1.0f;	vertices[64].v = 1.0f;

	vertices[65].x = -0.5f;	vertices[65].y = -0.5f;	vertices[65].z = 0.5f;
	vertices[65].r = 0;		vertices[65].g = 0;		vertices[65].b = 0;
	vertices[65].u = 0.0f;	vertices[65].v = 1.0f;

	//hat
	vertices[66].x = -0.5f;	vertices[66].y = 0.5f;	vertices[66].z = 0.5f;
	vertices[66].r = 0;		vertices[66].g = 0;		vertices[66].b = 0;
	vertices[66].u = 0.0f;	vertices[66].v = 0.0f;

	vertices[67].x = 0.5f;	vertices[67].y = 0.5f;	vertices[67].z = 0.5f;
	vertices[67].r = 0;		vertices[67].g = 0;		vertices[67].b = 0;
	vertices[67].u = 0.0f;	vertices[67].v = 1.0f;

	vertices[68].x = 0.0f;	vertices[68].y = 1.0f;	vertices[68].z = 0.0f;
	vertices[68].r = 0;		vertices[68].g = 0;		vertices[68].b = 255;
	vertices[68].u = 0.5f;	vertices[68].v = 1.0f;

	vertices[69].x = 0.5f;	vertices[69].y = 0.5f;	vertices[69].z = 0.5f;
	vertices[69].r = 0;		vertices[69].g = 0;		vertices[69].b = 0;
	vertices[69].u = 0.0f;	vertices[69].v = 0.0f;

	vertices[70].x = 0.5f;	vertices[70].y = 0.5f;	vertices[70].z = -0.5f;
	vertices[70].r = 0;		vertices[70].g = 0;		vertices[70].b = 0;
	vertices[70].u = 0.0f;	vertices[70].v = 1.0f;

	vertices[71].x = 0.0f;	vertices[71].y = 1.0f;	vertices[71].z = 0.0f;
	vertices[71].r = 0;		vertices[71].g = 0;		vertices[71].b = 255;
	vertices[71].u = 0.5f;	vertices[71].v = 1.0f;

	vertices[72].x = 0.5f;	vertices[72].y = 0.5f;	vertices[72].z = -0.5f;
	vertices[72].r = 0;		vertices[72].g = 0;		vertices[72].b = 0;
	vertices[72].u = 0.0f;	vertices[72].v = 0.0f;

	vertices[73].x = -0.5f;	vertices[73].y = 0.5f;	vertices[73].z = -0.5f;
	vertices[73].r = 0;		vertices[73].g = 0;		vertices[73].b = 0;
	vertices[73].u = 0.0f;	vertices[73].v = 1.0f;

	vertices[74].x = 0.0f;	vertices[74].y = 1.0f;	vertices[74].z = 0.0f;
	vertices[74].r = 0;		vertices[74].g = 0;		vertices[74].b = 255;
	vertices[74].u = 0.5f;	vertices[74].v = 1.0f;

	vertices[75].x = -0.5f;	vertices[75].y = 0.5f;	vertices[75].z = -0.5f;
	vertices[75].r = 0;		vertices[75].g = 0;		vertices[75].b = 0;
	vertices[75].u = 0.0f;	vertices[75].v = 0.0f;

	vertices[76].x = -0.5f;	vertices[76].y = 0.5f;	vertices[76].z = 0.5f;
	vertices[76].r = 0;		vertices[76].g = 0;		vertices[76].b = 0;
	vertices[76].u = 0.0f;	vertices[76].v = 1.0f;

	vertices[77].x = 0.0f;	vertices[77].y = 1.0f;	vertices[77].z = 0.0f;
	vertices[77].r = 0;		vertices[77].g = 0;		vertices[77].b = 255;
	vertices[77].u = 0.5f;	vertices[77].v = 1.0f;

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
	glGenTextures(1, &tex1);

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
	glGenTextures(2, &tex2);

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
		glm::vec3 cameraPosition = { 2.0f, 1.0f, 4.0f };
		glm::mat4 viewMatrix(1.0f);
		viewMatrix = glm::translate(viewMatrix, -cameraPosition); // Note the negative translation

		glm::vec3 eye = cameraPosition; // Eye is where our camera is
		glm::vec3 target = { 0.0f, 0.0f, 0.0f }; // Target is a specific point that the camera is looking at
		glm::vec3 up = { 0.0f, 1.0f, 0.0f }; // Global up vector (which will be used by the lookAt function to calculate the camera's right and up vectors)
		glm::mat4 lookAtMatrix = glm::lookAt(eye, target, up);

		viewMatrix = viewMatrix * lookAtMatrix;
		GLint viewMatrixUniform = glGetUniformLocation(program, "viewMatrix");
		glUniformMatrix4fv(viewMatrixUniform, 1, GL_FALSE, glm::value_ptr(viewMatrix));

		// Construct our view frustrum (projection matrix) using the following parameters
		float fieldOfViewY = glm::radians(45.0f); // Field of view
		float aspectRatio = windowWidth * 1.0f / windowHeight; // Aspect ratio, which is the ratio between width and height
		float nearPlane = 0.1f; // Near plane, minimum distance from the camera where things will be rendered
		float farPlane = 10.0f; // Far plane, maximum distance from the camera where things will be rendered
		glm::mat4 projectionMatrix = glm::perspective(fieldOfViewY, aspectRatio, nearPlane, farPlane);

		GLint projectionMatrixUniform = glGetUniformLocation(program, "projectionMatrix");
		glUniformMatrix4fv(projectionMatrixUniform, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

		// We retrieve our 'tex' uniform variable from the fragment shader,
		// and set its value to 0 to indicate that we are using texture unit 0
		GLint texUniform = glGetUniformLocation(program, "tex");
		glUniform1i(texUniform, 0);

		// We retrieve our 'modelMatrix' uniform variable from the vertex shader,
		GLint modelMatrixUniform = glGetUniformLocation(program, "modelMatrix");

		// Create a 4x4 matrix that will be our model matrix,
		// and initialize it to be the identity matrix.
		// The model matrix is a series of affine transformations that will place our object
		// in the world (local space -> world space)
		glm::mat4 modelMatrix(1.0f);

		// For the first quad (pepe.jpg texture), let's scale it by half the size, and move it to the left via translation
		// The matrix multiplication chain should look like: (Identity) * Translation * Scale
		// glm::translate() is a function that takes an existing matrix, and appends a translation matrix
		// to the RIGHT given the tx, ty, tz values.
		glm::vec3 translationVector = glm::vec3(0.0f, 0.0f, 0.0f);

		if (toggleBody)
		{
			translationVector = glm::vec3(0.0f, 0.25f, 0.0f);
		}
		else
		{
			translationVector = glm::vec3(0.0f, 0.0f, 0.0f);
		}

		modelMatrix = glm::translate(modelMatrix, translationVector);
		// At this point, we now have: Identity * Translation

		//(Identity) * Translation * Rotation
		glm::vec3 rotationAxis(0.0f, 1.0f, 0.0f);

		modelMatrix = glm::rotate(modelMatrix, glm::radians(90.0f), rotationAxis);

		// glm::scale() is a function that takes an existing matrix, and appends a scale matrix to the RIGHT given the sx, sy, sz values.
		// Let's scale the quad on all axes by 2.0.
		glm::vec3 scaleVector(0.5f, 1.0f, 0.5f);
		modelMatrix = glm::scale(modelMatrix, scaleVector);
		// At this point, we now have: Identity * Translation * Scale

		// Set the value of our transformationMatrix uniform variable in the vertex shader to our matrix here
		// The first parameter is the uniform location of the uniform we want to set the value of.
		// The second parameter is the number of matrices to set the uniform with (we only have 1 matrix, so we give it a value of 1)
		// The third parameter is a boolean flag to indicate whether to transpose the matrix or not. In our case, we do not need to since glm makes matrix that are column-major (same with OpenGL)
		// The fourth parameter is a pointer to the matrices that we will set the uniform (actual data)
		glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, glm::value_ptr(modelMatrix));

		// Bind our pepe.jpg texture to texture unit 0
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, tex0);

		// Draw a quad (4 vertices) using a triangle-fan
		glDrawArrays(GL_TRIANGLE_FAN, 42, 4);
		glDrawArrays(GL_TRIANGLE_FAN, 46, 4);
		glDrawArrays(GL_TRIANGLE_FAN, 50, 4);
		glDrawArrays(GL_TRIANGLE_FAN, 54, 4);
		glDrawArrays(GL_TRIANGLE_FAN, 58, 4);
		glDrawArrays(GL_TRIANGLE_FAN, 62, 4);

		// Now for the second quad (bioshock.jpg), let's scale it by 1.5, rotate it by 45 degrees along the z-axis,
		// and then move it to the right and up.
		modelMatrix = glm::mat4(1.0f);
		// (Identity) * Translation
		translationVector = glm::vec3(0.0f, 1.0f, 0.0f);

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
		glm::vec3 rotationAxis1(0.0f, 1.0f, 0.0f);
		modelMatrix = glm::rotate(modelMatrix, glm::radians(90.0f), rotationAxis1);

		// (Identity) * Translation * Rotation * Scale
		scaleVector = glm::vec3(1.0f, 1.0f, 1.0f);
		modelMatrix = glm::scale(modelMatrix, scaleVector);

		// We now update our moelMatrix uniform to have the new model matrix
		glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, glm::value_ptr(modelMatrix));

		// Bind our bioshock.jpg texture to texture unit 0
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, tex2);

		// Draw the same quad again, but this time with the matrices and texture updated
		glDrawArrays(GL_TRIANGLE_FAN, 42, 4);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, tex1);

		glDrawArrays(GL_TRIANGLE_FAN, 46, 4);
		glDrawArrays(GL_TRIANGLE_FAN, 50, 4);
		glDrawArrays(GL_TRIANGLE_FAN, 54, 4);
		glDrawArrays(GL_TRIANGLE_FAN, 58, 4);
		glDrawArrays(GL_TRIANGLE_FAN, 62, 4);
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

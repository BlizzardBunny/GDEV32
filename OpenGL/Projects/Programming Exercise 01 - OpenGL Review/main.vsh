#version 330

// Vertex attributes as inputs
layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec3 vertexColor;
layout(location = 2) in vec2 vertexUV;

// Output color
out vec3 outColor;
// Output UV-coordinates
out vec2 outUV;

uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;
// 4x4 matrix uniform variable to contain the transformation to be applied to of our vertex position
uniform mat4 modelMatrix;

void main()
{
	// Transform our vertex position to homogeneous coordinates.
	// Remember that w = 1.0 means that the vector is a position.
	vec4 finalPosition = vec4(vertexPosition, 1.0);

	// Apply the transformation to our vertex position by multiplying
	// our transformation matrix
	finalPosition = projectionMatrix * viewMatrix * modelMatrix * finalPosition;

	// gl_Position is a built-in shader variable that we need to set
	gl_Position = finalPosition;

	// We pass the color of the current vertex to our output variable
	outColor = vertexColor;

	// We pass the UV-coordinates of the current vertex to our output variable
	outUV = vertexUV;
}

#version 330

// Take the 'outColor' output from the vertex shader as input of our fragment shader
in vec3 outColor;

// Take the 'outUV' output from the vertex shader as input of our fragment shader
in vec2 outUV;

// Final color of the fragment, which we are required to output
out vec4 fragColor;

// Uniform variable that will hold the texture unit of the texture that we want to use
uniform sampler2D tex;

// If we want to simultaneously use another texture at a different texture unit,
// we can create another uniform for it.
// Example:
// uniform sampler2D tex1;
// We can then set the value of this uniform to 1 (for texture unit 1) in our Main.cpp

void main()
{
	// Sample the color of the texture at the specified UV-coordinates
	vec4 sampledColor = texture(tex, outUV);

	// Pass the sampled color from the texture to our fragColor output variable
	fragColor = sampledColor; 
}

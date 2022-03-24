#version 330

// Take the 'outColor' output from the vertex shader as input of our fragment shader
in vec3 outColor;

// Take the 'outUV' output from the vertex shader as input of our fragment shader
in vec2 outUV;

// Taking 'fragvertexNormal' from vertex shader as input to fragment shader
in vec3 fragvertexNormal;

//Input frag position from vsh
in vec3 fragPosition;

// Final color of the fragment, which we are required to output
out vec4 fragColor;

// Uniform variable that will hold the texture unit of the texture that we want to use
uniform sampler2D tex;

// light color
uniform vec3 lightColor;

//from main.cpp
uniform vec3 cameraPosition;

// light position vector
uniform vec3 lightPos;

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

	//Set value of vertex normal to fragNormal and normalize
	vec3 fragNormal = normalize(fragvertexNormal);

	//ambient
	float ambientStrength = .1f;
	vec3 ambient = ambientStrength * lightColor;

	//diffuse lighting
	vec3 lightDir = normalize(lightPos - fragPosition);
	float diff = max(dot(fragNormal, lightDir), 0.f);
	vec3 diffuse = diff * lightColor;

	//specular lighting
	vec3 viewDir = normalize(cameraPosition - fragPosition);
	vec3 reflectDir = reflect(-lightDir, fragNormal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
	vec3 specular = spec * lightColor;

	// add all lighting stuff
	vec3 finalColor = (ambient + diffuse + specular) * vec3(fragColor);
	fragColor = vec4(finalColor, 1.f);

}

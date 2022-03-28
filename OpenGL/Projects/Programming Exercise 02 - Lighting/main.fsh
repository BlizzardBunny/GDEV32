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

//from main.cpp
uniform vec3 cameraPosition;

// light position vector
uniform vec3 lightPos;

uniform float ambientComponent, diffuseComponent, specularComponent;
uniform vec3 specularIntensity;

// shininess of material
uniform float shine;

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
	vec3 textureColor = vec3(sampledColor); 

	//Set value of vertex normal to fragNormal and normalize
	vec3 fragNormal = normalize(fragvertexNormal);	
	vec3 lightDir = normalize(lightPos - fragPosition);

	//ambient
	float ambientStrength = 0.1f;
	vec3 ambient = ambientComponent * textureColor;

	//diffuse lighting
	float diff = max(dot(fragNormal, lightDir), 0.0f);
	vec3 diffuse = diff * diffuseComponent * textureColor;

	//specular lighting
	vec3 viewDir = normalize(cameraPosition - fragPosition);
	vec3 reflectDir = reflect(-lightDir, fragNormal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0f), shine);
	vec3 specular = spec * specularComponent * specularIntensity;

	// add all lighting stuff
	vec3 finalColor = (ambient + diffuse + specular) * outColor;
	//vec3 finalColor = (ambient) * textureColor;
	fragColor = vec4(finalColor, 1.0f) * sampledColor;
}

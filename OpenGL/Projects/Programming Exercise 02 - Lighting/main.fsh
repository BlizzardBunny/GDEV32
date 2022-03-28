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

	//ambient light
	vec3 ambient = ambientComponent * textureColor;

	// =======ATTENUATION===========
	//For attenuations
	float a, b, c;
	float d = sqrt(pow(lightPos.x - fragPosition.x, 2) + pow(lightPos.y - fragPosition.y, 2));
	float atten = 1 / (a + (b * d) + (c * pow(d, 2)));

	// =======POINT LIGHT===========
	//diffuse lighting for point light
	float diff = max(dot(fragNormal, lightDir), 0.0f);
	vec3 Pointdiffuse = diff * diffuseComponent * textureColor;

	//specular lighting for point light
	vec3 viewDir = normalize(cameraPosition - fragPosition);
	vec3 reflectDir = reflect(-lightDir, fragNormal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0f), shine);
	vec3 Pointspecular = spec * specularComponent * specularIntensity;

	//Attenuation for point
	a = 0.1f;
	b = 0.1f;
	c = 0.1f;
	float attenPoint = 1 / (a + (b * d) + (c * pow(d, 2)));

	vec3 PointComponent = (Pointdiffuse + Pointspecular) * attenPoint;

	// =======DIRECTIONAL LIGHT===========
	vec3 directionColor = vec3(0.1f, 0.1f, 0.0f);
	
	//light direction
	lightDir = vec3(10.0f, 10.0f, 10.0f);

	//diffuse lighting for directional light
	diff = max(dot(fragNormal, lightDir), 0.0f);
	vec3 Dirdiffuse = diff * diffuseComponent * textureColor;

	//specular lighting for directional light
	reflectDir = reflect(-lightDir, fragNormal);
	spec = pow(max(dot(viewDir, reflectDir), 0.0f), shine);
	vec3 Dirspecular = spec * specularComponent * specularIntensity;

	//Attenuation for directional
	a = 0.05f;
	b = 0.05f;
	c = 0.05f;
	float attenDir = 1 / (a + (b * d) + (c * pow(d, 2)));
	
	vec3 DirComponent = ((Dirdiffuse + Dirspecular) * attenDir) * directionColor;

	// =======PHONG LIGHTING MODEL EQUATION===========
	// add all lighting stuff
	vec3 finalColor = (ambient + (PointComponent + DirComponent)) * outColor;
	fragColor = vec4(finalColor, 1.0f) * sampledColor;
}

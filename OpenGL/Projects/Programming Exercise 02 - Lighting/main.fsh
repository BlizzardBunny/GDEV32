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

// light position vectors
uniform vec3 PointlightPos;

uniform vec3 SpotlightPos;
uniform vec3 SpotlightDir;
uniform float SpotlightAngle;

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

	//ambient
	float ambientStrength = 0.1f;
	vec3 ambient = ambientComponent * textureColor;
	
	// =======POINT LIGHT===========
	vec3 lightDir = normalize(PointlightPos - fragPosition);

	//diffuse lighting
	float diff = max(dot(fragNormal, lightDir), 0.0f);
	vec3 Pointdiffuse = diff * diffuseComponent * textureColor;

	//specular lighting
	vec3 viewDir = normalize(cameraPosition - fragPosition);
	vec3 reflectDir = reflect(-lightDir, fragNormal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0f), shine);
	vec3 Pointspecular = spec * specularComponent * specularIntensity;
		
	vec3 PointlightColor = vec3(1.0f, 1.0f, 1.0f);
	vec3 PointComponents = (Pointdiffuse + Pointspecular) * PointlightColor;
	
	// =======SPOT LIGHT===========
	// check if in shadow
	
	vec3 dirLighttoFrag = normalize(SpotlightPos - fragPosition);
	float dotProduct = dot(normalize(SpotlightDir), dirLighttoFrag);
	
	vec3 SpotComponents;
	if (dotProduct > SpotlightAngle)
	{
		SpotComponents = vec3(0.0f, 0.0f, 0.0f);
	}
	else
	{
		lightDir = normalize(SpotlightPos - fragPosition);	
	
		//diffuse lighting
		diff = max(dot(fragNormal, lightDir), 0.0f);
		vec3 Spotdiffuse = diff * diffuseComponent * textureColor;

		//specular lighting
		viewDir = normalize(cameraPosition - fragPosition);
		reflectDir = reflect(-lightDir, fragNormal);
		spec = pow(max(dot(viewDir, reflectDir), 0.0f), shine);
		vec3 Spotspecular = spec * specularComponent * specularIntensity;
		
		vec3 spotlightColor = vec3(0.0f, 1.0f, 0.0f);
		SpotComponents = (Spotdiffuse + Spotspecular) * spotlightColor;
	}	

	// add all lighting stuff
	vec3 finalColor = (ambient + PointComponents + SpotComponents) * outColor;
	//vec3 finalColor = (ambient) * textureColor;
	fragColor = vec4(finalColor, 1.0f) * sampledColor;
}

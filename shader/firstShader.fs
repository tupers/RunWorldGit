#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

// texture samplers
uniform sampler2D texture_1;
uniform sampler2D texture_2;

void main()
{
	// linearly interpolate between both textures (80% container, 20% awesomeface)
	FragColor = mix(texture(texture_1, TexCoord), texture(texture_2, TexCoord), 0.2);
}
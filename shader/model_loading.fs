#version 330 core
out vec4 FragColor;

in vec2 TexCoords;
in vec3 Normal;
in vec3 FragPos;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;

void main()
{
    // ambient
    vec3 ambient = vec3(0.2f, 0.2f, 0.2f) * texture(texture_diffuse1, TexCoords).rgb;
  	
    // diffuse 
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(vec3(1.0f, 1.0f, 4.0f) - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = vec3(0.5f, 0.5f, 0.5f) * diff * texture(texture_diffuse1, TexCoords).rgb;  

    // specular
    vec3 viewDir = normalize(vec3(0.0f, 0.0f, 3.0f) - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 16);
    vec3 specular = vec3(1.0f, 1.0f, 1.0f) * spec * texture(texture_specular1, TexCoords).rgb;  

    vec3 result = ambient + diffuse + specular;
    FragColor = vec4(result, 1.0);
//    FragColor = texture(texture_diffuse1, TexCoords);
}
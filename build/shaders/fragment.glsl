#version 330 core

out vec4 FragColor;

in vec3 Normal;  
in vec3 FragPos;  

// Hardcoded lighting values
const vec3 lightPos = vec3(1.2f, 1.0f, 0.0f); // Example position
const vec3 lightColor = vec3(1.0f, 1.0f, 1.0f); // White light
const vec3 objectColor = vec3(1.0f, 0.5f, 0.31f); // Example object color

void main()
{
    // Ambient
    float ambientStrength = 0.5;
    vec3 ambient = ambientStrength * lightColor;
  	
    // Diffuse 
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;
            
    vec3 result = (ambient + diffuse) * objectColor;
    FragColor = vec4(result, 1.0);
} 


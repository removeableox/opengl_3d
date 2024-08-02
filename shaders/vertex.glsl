#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

out vec3 FragPos;
out vec3 Normal;

// Hardcoded transformation matrices
const mat4 model = mat4(2.0);  // Identity matrix as an example
const mat4 view = mat4(1.0);   // Identity matrix as an example
const mat4 projection = mat4(1.0); // Identity matrix as an example

void main()
{
    FragPos = vec3(model * vec4(aPos, 1.0));
    Normal = aNormal;  

    gl_Position = projection * view * vec4(FragPos, 1.0);
}


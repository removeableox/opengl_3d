#include <shaders.h>

unsigned int initShaders() {
	// build and compile our shader program
	// ------------------------------------
	// vertex shader

	const char* vertexShaderSource = loadShaderFromFile("shaders/vertex.glsl"); 
	const char* fragmentShaderSource = loadShaderFromFile("shaders/fragment.glsl"); 

	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);

	int success;
	char infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		printf("ERROR::SHADER::VERTEX::COMPILATION_FAILED\n%s\n", infoLog);
	}

	unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);

	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		printf("ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n%s\n", infoLog);
	}

	// link shaders
	unsigned int shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	return shaderProgram;
}

char* loadShaderFromFile(char* path) {
	FILE *fptr;
	char ch = 0;
	char* content = NULL;
	size_t size = 0;

	fptr = fopen(path, "r");
	if (fptr == NULL) {
		puts("ERROR: Unable to open shader source file.");
		return content;
	}

	while ((ch = fgetc(fptr)) != EOF) {
		size++;
		content = (char*)realloc(content, (size + 1) * sizeof(char));
		if (content == NULL) {
			puts("ERROR: Memory allocation failed.");
			fclose(fptr);
			return NULL;
		}
		content[size - 1] = ch;
	}
	content[size] = '\0'; // Null-terminate the string
	fclose(fptr);

	return content;
}


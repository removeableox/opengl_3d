#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <fast_obj.h>

#include <stdio.h>
#include <math.h>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

unsigned int VBO, VAO, EBO;

const char *vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"void main()\n"
"{\n"
"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
"}\0";
const char *fragmentShaderSource = "#version 330 core\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"   FragColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);\n"
"}\n\0";

struct Model {
	float x, y, z;
	fastObjMesh* mesh;
};

struct Model room; 

unsigned int initShaders() {
	// build and compile our shader program
	// ------------------------------------
	// vertex shader
	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);

	// fragment shader
	unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);

	// link shaders
	unsigned int shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	return shaderProgram;
}

void perspective (float *vertices, size_t size) {
	for (int i = 0; i < size; i += 3) {
		float distance = vertices[i+2] + 1;
	       	float scale = 1/distance;
		vertices[i] *= scale;
		vertices[i+1] *= scale;
		vertices[i+2] *= scale;
	}
}

void rotateVertices (struct Model *model, double xtheta, double ytheta, double ztheta) {
    xtheta *= (M_PI / 180.0);
    ytheta *= (M_PI / 180.0);
    ztheta *= (M_PI / 180.0);

    double xRotMatrix[] = {
        1, 0, 0,
        0, cos(xtheta), -sin(xtheta),
        0, sin(xtheta), cos(xtheta)
    };

    double yRotMatrix[] = {
        cos(ytheta), 0, sin(ytheta),
        0, 1, 0,
        -sin(ytheta), 0, cos(ytheta)
    };

    double zRotMatrix[] = {
        cos(ztheta), -sin(ztheta), 0,
        sin(ztheta), cos(ztheta), 0,
        0, 0, 1
    };

    for (int i = 0; i < model->size; i += 3) {
        // Translate vertices to the origin
        double x = model->vertices[i] - model->x;
        double y = model->vertices[i+1] - model->y;
        double z = model->vertices[i+2] - model->z;

        // Apply x rotation
        double xNew = x * xRotMatrix[0] + y * xRotMatrix[1] + z * xRotMatrix[2];
        double yNew = x * xRotMatrix[3] + y * xRotMatrix[4] + z * xRotMatrix[5];
        double zNew = x * xRotMatrix[6] + y * xRotMatrix[7] + z * xRotMatrix[8];
        
        x = xNew;
        y = yNew;
        z = zNew;

        // Apply y rotation
        xNew = x * yRotMatrix[0] + y * yRotMatrix[1] + z * yRotMatrix[2];
        yNew = x * yRotMatrix[3] + y * yRotMatrix[4] + z * yRotMatrix[5];
        zNew = x * yRotMatrix[6] + y * yRotMatrix[7] + z * yRotMatrix[8];
        
        x = xNew;
        y = yNew;
        z = zNew;

        // Apply z rotation
        xNew = x * zRotMatrix[0] + y * zRotMatrix[1] + z * zRotMatrix[2];
        yNew = x * zRotMatrix[3] + y * zRotMatrix[4] + z * zRotMatrix[5];
        zNew = x * zRotMatrix[6] + y * zRotMatrix[7] + z * zRotMatrix[8];

        // Translate back
        model->vertices[i] = xNew + model->x;
        model->vertices[i+1] = yNew + model->y;
        model->vertices[i+2] = zNew + model->z;
    }
}

void zoom (struct Model *model, double units) {
	model->x *= units;
	model->y *= units;
	model->z *= units;
	for (int i = 0; i < model->size; i++) {
		model->vertices[i] *= units;
	}
}

void prepareForRendering (struct Model *model) {
	float perspectiveVertices[9999];
	for (int i = 0; i < model->size; i++) {
		perspectiveVertices[i] = model->vertices[i];
	}
	perspective(perspectiveVertices, model->size);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, model->size*sizeof(float), perspectiveVertices, GL_STATIC_DRAW);
}

void move(struct Model *model, double xunits, double yunits, double zunits) {
	model->x += xunits;
	model->y += yunits;
	model->z += zunits;

	for (int i = 0; i < model->size; i += 3) {
		model->vertices[i] += xunits;
		model->vertices[i+1] += yunits;
		model->vertices[i+2] += zunits;
	}
}

double oldx = -1, oldy = -1;
float sensitivity = 0.1f;

static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos) {
    if (oldx == -1 || oldy == -1) {
        oldx = xpos;
        oldy = ypos;
    }

    float xdiff = oldx - xpos;
    float ydiff = oldy - ypos;

    xdiff *= sensitivity;
    ydiff *= sensitivity;

    oldx = xpos;
    oldy = ypos;

    float tempx = model.x;
    float tempy = model.y;
    float tempz = model.z;

    model.x = 0.0f;
    model.y = 0.0f;
    model.z = -1.0f;

    rotateVertices(&model, 0, xdiff, 0);
    rotateVertices(&model, ydiff, 0, 0);

    model.x = tempx;
    model.y = tempy;
    model.z = tempz;
}

int main () {
	// glfw: initialize and configure
	// ------------------------------
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	// glfw window creation
	// --------------------
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
	if (window == NULL)
	{
		puts("Error initializing window");
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, cursor_position_callback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		puts("Error initializing GLAD");
		return -1;
	}

	unsigned int shaderProgram = initShaders(); 

	model.x = 0.0f;
	model.y = 0.0f;
	model.z = 0.0f;

	model.mesh = fast_obj_read("assets/models/room.obj");

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	// bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, model.mesh->position_count*sizeof(float), model.mesh->positions, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
	glBindBuffer(GL_ARRAY_BUFFER, 0); 

	// remember: do NOT unbind the EBO while a VAO is active as the bound element buffer object IS stored in the VAO; keep the EBO bound.
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	// You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
	// VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
	glBindVertexArray(0); 


	// uncomment this call to draw in wireframe polygons.
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	// render loop
	// -----------
	while (!glfwWindowShouldClose(window))
	{
		processInput(window);

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		prepareForRendering(&model);

		glUseProgram(shaderProgram);
		glBindVertexArray(VAO); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
		glDrawElements(GL_TRIANGLES, sizeof(indices)/sizeof(unsigned int), GL_UNSIGNED_INT, 0);

		// glfw: swap buffers and poll IO events (keys pressed/released, mouse zoomd etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// optional: de-allocate all resources once they've outlived their purpose:
	// ------------------------------------------------------------------------
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
	glDeleteProgram(shaderProgram);

	fast_obj_destroy(mesh);

	// glfw: terminate, clearing all previously allocated GLFW resources.
	// ------------------------------------------------------------------
	glfwTerminate();
	return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, 1);
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		zoom(&model, 1.01);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		zoom(&model, 0.99);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		move(&model, 0.01, 0, 0);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		move(&model, -0.01, 0, 0);
	
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}

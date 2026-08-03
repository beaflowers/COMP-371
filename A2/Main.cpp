#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

//bea hoekstra / student id: 40285118 / Section CX
//omar ghazaly / student id: 40280795 / Section CY

//global variables for transformation states - to be modified by keyboard input
glm::vec3 translation = glm::vec3(0.0f);
float rotationAngle = 0.0f;
glm::vec3 scale = glm::vec3(1.0f);
//scale and position variables
const float d = 0.01f;
const float s = 1.01f;

//input tracking function
void processInput(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

//better input tracking function for interactivity 
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {

	if (action != GLFW_PRESS) return; { //fires once per key press

		switch (key) {
		case GLFW_KEY_Q: rotationAngle += 30.0f;
			break;
		case GLFW_KEY_E: rotationAngle -= 30.0f;
			break;
		case GLFW_KEY_W: translation.y += d; break;
		case GLFW_KEY_S: translation.y -= d; break;
		case GLFW_KEY_A: translation.x -= d; break;
		case GLFW_KEY_D: translation.x += d; break;
		case GLFW_KEY_R: scale.z *= s; break;
		case GLFW_KEY_F: scale.z /= s; break;
		default:
			break;
		}
	}
}


int main() {
	//initialize GLFW
	glfwInit();
	if (!glfwInit()) {
		std::cout << "Failed to init GLFW\n";
		return -1;
	}

	GLFWwindow* window = glfwCreateWindow(800, 600, "OpenGL Window", nullptr, nullptr);

	if (!window) {
		std::cout << "Failed to create window\n";
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);

	//set key callback for input
	glfwSetKeyCallback(window, keyCallback);

	//initialize GLEW
	glewInit();

	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK) {
		std::cout << "Failed to init GLEW\n";
		return -1;
	}

	//allow for 3D
	glEnable(GL_DEPTH_TEST);

	//basic vertex shader
	const char* vertexShaderSource = R"glsl(
		#version 330 core
		layout (location = 0) in vec3 aPos;
		uniform mat4 transform;
		void main() {
			gl_Position = transform * vec4(aPos, 1.0); //converts to vec4 for matrix multiplication
		}
		)glsl";

	//basic fragment shader
	const char* fragmentShaderSource = R"glsl(
		#version 330 core
		out vec4 FragColor;
		void main() {
			FragColor = vec4(0.6f, 0.2f, 0.8f, 1.0f); //shape color
		}
		)glsl";

	//vertex shader link
	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
	glCompileShader(vertexShader);

	//fragment shader
	unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
	glCompileShader(fragmentShader);

	//link shader program together
	unsigned int shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);

	////set transform matrix to identity matrix [DEFAULT, changed with transform functions later] 
	//glUseProgram(shaderProgram);
	//unsigned int TransformLoc = glGetUniformLocation(shaderProgram, "transform");
	//glm::mat4 identity = glm::mat4(1.0f);
	//glUniformMatrix4fv(TransformLoc, 1, GL_FALSE, glm::value_ptr(identity));


	//pyramid
	float verticesPyramid[] = {
		// base (square) - 4 corners
		-0.5f, 0.0f, -0.5f,   // 0: back-left
		 0.5f, 0.0f, -0.5f,   // 1: back-right
		 0.5f, 0.0f,  0.5f,   // 2: front-right
		-0.5f, 0.0f,  0.5f,   // 3: front-left
		// apex
		 0.0f, 0.8f,  0.0f    // 4: top
	};

	unsigned int indicesPyramid[] = {
		// base [2 triangles]
		0, 1, 2,
		0, 2, 3,
		// sides
		0, 1, 4,
		1, 2, 4,
		2, 3, 4,
		3, 0, 4
	};

	//VAO, VBO and EBO
	unsigned int VBO[2], VAO[2], EBO;
	glGenVertexArrays(2, VAO);
	glGenBuffers(2, VBO);
	glGenBuffers(1, &EBO);

	//bind VAO and VBO to array object
	glBindVertexArray(VAO[0]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verticesPyramid), verticesPyramid, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	//uploads data to GPU
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indicesPyramid), indicesPyramid, GL_STATIC_DRAW);

	//to allow actually viewing pyramid as 3d shape
	//camera matrix
	glm::mat4 view = glm::lookAt(
		glm::vec3(0.0f, 0.5f, 2.0f),
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 1.0f, 0.0f)
	);

	//projection (perspective) matrix
	glm::mat4 projection = glm::perspective(
		glm::radians(45.0f),
		800.0f / 600.0f,
		0.1f, 100.0f
	);


	//render loop
	while (!glfwWindowShouldClose(window)) {
		processInput(window);

		//background color
		glClearColor(0.0f, 0.6f, 0.3f, .2f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUseProgram(shaderProgram); // needs to come before buffers 

		// transformation
		glm::mat4 transform = glm::mat4(1.0f);
		transform = glm::translate(transform, translation); //builds the 4x4 matrix for translation
		transform = glm::rotate(transform, glm::radians(rotationAngle), glm::vec3(0.0f, 0.0f, 1.0f)); //determines what axis rotating around
		transform = glm::scale(transform, scale); //scaling


		//combine model, view, projection
		glm::mat4 mvp = projection * view * transform;

		//send mvp matrix to shader
		unsigned int transformLoc = glGetUniformLocation(shaderProgram, "transform");
		glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(mvp));

		//actually draws shape
		glBindVertexArray(VAO[0]);
		glDrawElements(GL_TRIANGLES, sizeof(indicesPyramid) / sizeof(unsigned int), GL_UNSIGNED_INT, 0);
		//18 bc 6 trianlges with 3 indices each

		glfwSwapBuffers(window);
		glfwPollEvents();

	}


	//clean up
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	glfwTerminate();
	return 0;

}
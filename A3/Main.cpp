#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

//bea hoekstra / student id: 40285118 / Section CX
//omar ghazaly / student id: 40280795 / Section CY

void processInput(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

//global variables for transformation states - to be modified by keyboard input
glm::vec3 translation = glm::vec3(0.0f);
float rotationAngle = 0.0f;
glm::vec3 scale = glm::vec3(1.0f);
//scale and position variables
const float d = 0.01f;
const float s = 1.51f;

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
	// Initialize GLFW
	if (!glfwInit()) {
		std::cerr << "Failed to initialize GLFW" << std::endl;
		return -1;
	}
	// Create a windowed mode window and its OpenGL context
	GLFWwindow* window = glfwCreateWindow(800, 600, "OpenGL Window", NULL, NULL);
	if (!window) {
		std::cerr << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	// Make the window's context current
	glfwMakeContextCurrent(window);

	//set key callback for input
	glfwSetKeyCallback(window, keyCallback);

	// Initialize GLEW
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK) {
		std::cerr << "Failed to initialize GLEW" << std::endl;
		return -1;
	}

	//allow for 3D
	glEnable(GL_DEPTH_TEST);

	//tinyobjloader function to load obj file
	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::string warn, err;

	bool success = tinyobj::LoadObj(&attrib, &shapes, &materials, &err, "chair.obj");

	if (!err.empty()) std::cerr << "ERR: " << err << std::endl;
	if (!success) {
		std::cerr << "Failed to load OBJ" << std::endl;
		return -1;
	}

	//extract obj data into arrays
	std::vector<float> vertices; 
	std::vector<unsigned int> indices;

	for (const auto& shape : shapes) {
		for (const auto& index : shape.mesh.indices) {
			// position
			float vx = attrib.vertices[3 * index.vertex_index + 0];
			float vy = attrib.vertices[3 * index.vertex_index + 1];
			float vz = attrib.vertices[3 * index.vertex_index + 2];

			vertices.push_back(vx);
			vertices.push_back(vy);
			vertices.push_back(vz);

			indices.push_back(indices.size());
		}
	}

	//vertex shader
	const char* vertexShaderSource = R"glsl(
		#version 330 core
		layout (location = 0) in vec3 aPos;
		uniform mat4 transform;
		void main() {
			gl_Position = transform * vec4(aPos, 1.0);
		}
	)glsl";
	
	//fragment shader
	const char* fragmentShaderSource = R"glsl(
		#version 330 core
		out vec4 FragColor;
		void main() {
			FragColor = vec4(0.6f, 0.2f, 0.8f, 1.0f);
		}
	)glsl";

	//vertex shader link
	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);

	//fragment shader link
	unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);

	//shader program
	unsigned int shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	//VAO, VBO and EBO
	unsigned int VBO[2], VAO[2], EBO;
	glGenVertexArrays(2, VAO);
	glGenBuffers(2, VBO);
	glGenBuffers(1, &EBO);

	//bindd VAO and VBO to array object
	glBindVertexArray(VAO[0]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	//uploads to GPU
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

	//camera matrix
	glm::mat4 view = glm::lookAt(
		glm::vec3(0.0f, 0.5f, 2.0f),
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 1.0f, 0.0f)
	);

	//perspective matrix
	glm::mat4 projection = glm::perspective(
		glm::radians(45.0f),
		800.0f / 600.0f,
		0.1f, 100.0f
	);


	while (!glfwWindowShouldClose(window)) {
		processInput(window);

		//background color
		glClearColor(0.0f, 0.6f, 0.3f, .2f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUseProgram(shaderProgram); // needs to come before buffers 

		glm::mat4 model = glm::mat4(1.0f); // identity matrix for now 
		model = glm::scale(model, glm::vec3(0.15f)); //scale
		model = glm::translate(model, translation); //translation
		model = glm::rotate(model, glm::radians(rotationAngle), glm::vec3(0.0f, 1.0f, 0.0f)); //rotation around y-axis

		glm::mat4 mvp = projection * view * model;

		// Pass the transformation matrix to the shader
		unsigned int transformLoc = glGetUniformLocation(shaderProgram, "transform");
		glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(mvp));

		// Draw the object
		glBindVertexArray(VAO[0]);
		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
		
		// Swap buffers and poll events
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}
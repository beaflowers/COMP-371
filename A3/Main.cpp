#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

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
const float wasd = 0.1f; 
const float scalar = 1.5f;

//better input tracking function for interactivity 
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {

	if (action != GLFW_PRESS) return; { //fires once per key press

		switch (key) {
		case GLFW_KEY_Q: rotationAngle += 30.0f;
			break;
		case GLFW_KEY_E: rotationAngle -= 30.0f;
			break;
		case GLFW_KEY_W: translation.y += wasd; 
			break;
		case GLFW_KEY_S: translation.y -= wasd; 
			break;
		case GLFW_KEY_A: translation.x -= wasd; 
			break;
		case GLFW_KEY_D: translation.x += wasd; 
			break;
		case GLFW_KEY_R: scale *= scalar; 
			break;
		case GLFW_KEY_F: scale /= scalar; 
			break;
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

	//load texture
	int width, height, channels;
	unsigned char* data = stbi_load("texture.png", &width, &height, &channels, 0);

	if (!data) {
		std::cerr << "Failed to load texture: " << stbi_failure_reason() << std::endl;
	}

	//creates and activates texture object 
	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);

	// texture wrapping/filtering settings
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	if (data) {
		//accounts for coolor data
		GLenum format = (channels == 4) ? GL_RGBA : GL_RGB;
		//uploads pixel data to GPU
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else {
		std::cerr << "Failed to load texture: " << stbi_failure_reason() << std::endl;
	}

	stbi_image_free(data); // free the CPU-side copy

	//extract obj data into arrays
	std::vector<float> vertices; 
	std::vector<unsigned int> indices;

	//pulls position and texture coordinates from obj file into arrays for rendering
	for (const auto& shape : shapes) {
		for (const auto& index : shape.mesh.indices) {
			// position
			float vx = attrib.vertices[3 * index.vertex_index + 0];
			float vy = attrib.vertices[3 * index.vertex_index + 1];
			float vz = attrib.vertices[3 * index.vertex_index + 2];

			// texture coordinates (UV) - default to (0,0) if missing
			float u = 0.0f, v = 0.0f;
			if (index.texcoord_index >= 0) {
				u = attrib.texcoords[2 * index.texcoord_index + 0];
				v = attrib.texcoords[2 * index.texcoord_index + 1];
			}

			// temporary debug print - remove once confirmed
			static int debugCount = 0;
			if (debugCount < 10) {
				std::cout << "texcoord_index: " << index.texcoord_index << " -> u=" << u << " v=" << v << std::endl;
				debugCount++;
			}

			vertices.push_back(vx);
			vertices.push_back(vy);
			vertices.push_back(vz);
			vertices.push_back(u);
			vertices.push_back(v);

			indices.push_back(indices.size());
		}
	}

	//vertex shader
	const char* vertexShaderSource = R"glsl(
		#version 330 core
		layout (location = 0) in vec3 aPos;
		layout (location = 1) in vec2 aTexCoord;
		uniform mat4 transform;
		out vec2 TexCoord;
		void main() {
			gl_Position = transform * vec4(aPos, 1.0);
			TexCoord = aTexCoord;
		}
	)glsl";
	
	//fragment shader
	const char* fragmentShaderSource = R"glsl(
		#version 330 core
		in vec2 TexCoord;
		out vec4 FragColor;
		uniform sampler2D texture1;
		void main() {
			FragColor = texture(texture1, TexCoord);
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

	//position attribute (location 0)
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// UV attribute (location 1)
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

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
		model = glm::translate(model, translation); //translation
		model = glm::rotate(model, glm::radians(rotationAngle), glm::vec3(0.0f, 1.0f, 0.0f)); //rotation around y-axis
		model = glm::scale(model, glm::vec3(0.15f) * scale); //scale
		
		
		//combine for rendering
		glm::mat4 mvp = projection * view * model;

		// Pass the transformation matrix to the shader
		unsigned int transformLoc = glGetUniformLocation(shaderProgram, "transform");
		glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(mvp));

		//bind texture
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureID);
		glUniform1i(glGetUniformLocation(shaderProgram, "texture1"), 0);

		// Draw the object
		glBindVertexArray(VAO[0]);
		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
		
		// Swap buffers and poll events
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	glfwTerminate();
	return 0;
}
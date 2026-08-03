#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include <iostream>

const char* vertexShaderSource = R"glsl(
	#version 330 core
	layout (location = 0) in vec3 aPos;
	uniform mat4 transform;
	void main() {
		gl_Position = transform * vec4(aPos, 1.0);
	}
)glsl";

const char* fragmentShaderSource = R"glsl(
	#version 330 core
	out vec4 FragColor;
	void main() {
		FragColor = vec4(1.0, 1.0, 1.0, 1.0);
	}
)glsl";

void processInput(GLFWwindow* window, glm::vec3& translation, float& rotation, float& zScale) {
	const float d = 0.01f;
	const float s = 1.01f;

	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) glfwSetWindowShouldClose(window, true);
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) translation.y += d;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) translation.y -= d;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) translation.x -= d;
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) translation.x += d;
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) rotation += 30.0f;
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) rotation -= 30.0f;
	if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) zScale *= s;
	if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS) zScale /= s;
}

int main() {
	// Init GLFW
	if (!glfwInit()) {
		std::cout << "Failed to init GLFW\n";
		return -1;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(800, 600, "OpenGL Test", nullptr, NULL);


	if (!window) {
		std::cout << "Failed to create window\n";
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);

	// Init GLEW (must be after making context current)
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK) {
		std::cout << "Failed to init GLEW\n";
		return -1;
	}

	//Vertex Shader
	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);

	//Fragment Shader
	unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

	//Compile Vertex Shader
	glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
	glCompileShader(vertexShader);

	int success;
	char infoLog[512];

	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);

	if (!success) {
		glGetShaderInfoLog(vertexShader, 512, nullptr, infoLog);
		std::cout << infoLog << std::endl;
	}

	//Compile Fragment Shader
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
	glCompileShader(fragmentShader);

	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);

	if (!success) {
		glGetShaderInfoLog(fragmentShader, 512, nullptr, infoLog);
		std::cout << infoLog << std::endl;
	}


	glm::vec3 translation(0.0f, 0.0f, 0.0f);
	float rotation = 0.0f;
	float zScale = 1.0f;


	//Link Program
	unsigned int shaderProgram = glCreateProgram();

	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);

	glLinkProgram(shaderProgram);

	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);

	if (!success) {
		glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
		std::cout << infoLog << std::endl;
	}

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	unsigned int VBO[2], VAO[2], EBO;
	glGenVertexArrays(2, VAO);
	glGenBuffers(2, VBO);
	glGenBuffers(1, &EBO);

	// Bind the Vertex Array Object for the tetrahedron
	glBindVertexArray(VAO[0]);

	// Define vertices for a tetrahedron
	float verticesTetrahedron[] = {
		 0.3f,  0.3f,  0.3f,
		 0.3f, -0.3f, -0.3f,
		-0.3f,  0.3f, -0.3f,
		-0.3f, -0.3f,  0.3f
	};

	// Coordinates for each triangle
	unsigned int indices[] = {
		0, 1, 2,
		0, 1, 3,
		0, 2, 3,
		1, 2, 3
	};

	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verticesTetrahedron), verticesTetrahedron, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

	glEnableVertexAttribArray(0);

	float transform[] =
	{
		1,0,0,0,
		0,1,0,0,
		0,0,1,0,
		0,0,0,1
	};

	glUseProgram(shaderProgram);

	unsigned int transformLoc = glGetUniformLocation(shaderProgram, "transform");

	glUniformMatrix4fv(transformLoc, 1, GL_FALSE, transform);

	// Print OpenGL version - good sanity check
	std::cout << "OpenGL version: " << glGetString(GL_VERSION) << std::endl;

	// Render loop
	while (!glfwWindowShouldClose(window)) {
		processInput(window, translation, rotation, zScale);

		glClearColor(0.2f, 0.5f, 0.8f, 1.0f); // nice blue
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glUseProgram(shaderProgram);

		glBindVertexArray(VAO[0]);

		glm::mat4 transform = glm::mat4(1.0f);

		transform = glm::translate(transform, translation);
		transform = glm::rotate( transform, glm::radians(rotation), glm::vec3(0.0f, 0.0f, 1.0f));
		transform = glm::scale( transform, glm::vec3(1.0f, 1.0f, zScale));

		glUniformMatrix4fv( transformLoc, 1, GL_FALSE, glm::value_ptr(transform));

		glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, nullptr);

		glfwSwapBuffers(window);
		glfwPollEvents();

	}

	glfwTerminate();
	return 0;
}
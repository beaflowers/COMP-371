#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>

int main() {
    // Init GLFW
    if (!glfwInit()) {
        std::cout << "Failed to init GLFW\n";
        return -1;
    }

    GLFWwindow* window = glfwCreateWindow(800, 600, "OpenGL Test", NULL, NULL);
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

    // Print OpenGL version - good sanity check
    std::cout << "OpenGL version: " << glGetString(GL_VERSION) << std::endl;

    while (!glfwWindowShouldClose(window)) {
        glClearColor(0.2f, 0.5f, 0.8f, 1.0f); // nice blue
        glClear(GL_COLOR_BUFFER_BIT);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
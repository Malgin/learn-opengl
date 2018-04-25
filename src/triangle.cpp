#define GLEW_STATIC

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <cmath>

const char *vertexShaderSource = R"glsl(
    #version 150 core

    in vec2 position;
    in vec3 color;

    out vec3 Color;

    void main() {
        gl_Position = vec4(position, 0.0, 1.0);
        Color = color;
    }
)glsl";

const char *fragmentShaderSource = R"glsl(
    #version 150 core

    in vec3 Color;

    out vec4 outColor;

    void main() {
         outColor = vec4(Color, 1.0);
    }
)glsl";

int main () {
  glfwInit();

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

  glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

  GLFWwindow *window = glfwCreateWindow(800, 600, "OpenGL", nullptr, nullptr);

  glfwMakeContextCurrent(window);

  glewExperimental = GL_TRUE;
  glewInit();

  GLuint vao; // Vertex Array Object
  glGenVertexArrays(1, &vao);

  // As soon as you've bound a certain VAO, every time you call
  // glVertexAttribPointer, that information will be stored in that VAO.
  //
  // Since only calls after binding a VAO stick to it, make sure
  // that you've created and bound the VAO at the start of your
  // program. Any vertex buffers and element buffers bound
  // before it will be ignored.
  glBindVertexArray(vao);

  // Defining vertex data
  float vertices[] = {
      -0.5f, 0.5f, 1.0f, 0.0f, 0.0f, // Top-left
      0.5f, 0.5f, 0.0f, 1.0f, 0.0f, // Top-right
      0.5f, -0.5f, 0.0f, 0.0f, 1.0f, // Bottom-right
      -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, // Bottom-left
  };

  GLuint elements[] = {
      0, 1, 2,
      2, 3, 0
  };

  GLuint vbo; // Vertex Buffer Object
  glGenBuffers(1, &vbo); // Generate 1 buffer

  // Bind Vertex Buffer Object
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  // Load vertex data to binded buffer
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  GLuint ebo; // Element Buffer Object
  glGenBuffers(1, &ebo);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);


  GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
  glCompileShader(vertexShader);

  GLint status;
  glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &status);
  if (status != GL_TRUE) {
    // Shader failed to compile, retrieving log...
    char buffer[512];
    glGetShaderInfoLog(vertexShader, 512, nullptr, buffer);
    std::cout << "Vertex Shader Issues!" << std::endl;
    std::cout << buffer << std::endl;
  }

  GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
  glCompileShader(fragmentShader);

  glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &status);
  if (status != GL_TRUE) {
    // Shader failed to compile, retrieving log...
    char buffer[512];
    glGetShaderInfoLog(fragmentShader, 512, nullptr, buffer);
    std::cout << "Fragment Shader Issues!" << std::endl;
    std::cout << buffer << std::endl;
  }

  GLuint shaderProgram = glCreateProgram();
  glAttachShader(shaderProgram, vertexShader);
  glAttachShader(shaderProgram, fragmentShader);

  // Since a fragment shader is allowed to write to multiple buffers,
  // you need to explicitly specify which output is written to which buffer.
  // This needs to happen before linking the program. However, since this is
  // 0 by default and there's only one output right now, the following line
  // of code is not necessary:
  glBindFragDataLocation(shaderProgram, 0,
                         "outColor"); // Use glDrawBuffers when rendering to multiple buffers, because only the first output will be enabled by default.

  glLinkProgram(shaderProgram);
  glUseProgram(shaderProgram); // Just like a vertex buffer, only one program can be active at a time.

  // A shader object can be deleted with glDeleteShader, but it will not
  // actually be removed before it has been detached from all programs
  // with glDetachShader

  // The location is a number depending on the order of the input definitions.
  // The first and only input position in this example will always have location 0.
  GLint posAttrib = glGetAttribLocation(shaderProgram, "position");
  glEnableVertexAttribArray(posAttrib);

  // This function also stores VBO currently bound to GL_ARRAY_BUFFER.
  // This means that with next invocation we can bind different buffer
  glVertexAttribPointer(
      posAttrib, // reference to input
      2,         // number of values for input (number of components of vec)
      GL_FLOAT,  // type of each component
      GL_FALSE,  // whether imput values should be normalized between -1.0 and 1.0
      5 * sizeof(float), // stride (0 - no data between data attributes)
      nullptr            // offset (how many bytes from the start of the array the attributes occur)
  );

  GLint colAttrib = glGetAttribLocation(shaderProgram, "color");
  glEnableVertexAttribArray(colAttrib);
  glVertexAttribPointer(colAttrib, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *) (2 * sizeof(float)));

  while (!glfwWindowShouldClose(window)) {
    glfwSwapBuffers(window);
    glfwPollEvents();

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) glfwSetWindowShouldClose(window, GL_TRUE);
  }

  glfwTerminate();

  return 0;
}

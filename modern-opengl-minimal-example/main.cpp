#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>

#define GLSL330(shader) "#version 330 core\n" #shader

static const int WINDOW_WIDTH = 300;
static const int WINDOW_HEIGHT = 300;

const GLchar * const VertexShaderCode = GLSL330(
  in vec3 Position;
  in vec3 Color;

  out vec3 ForwaredColor;

  void main()
  {
    ForwaredColor = Color;
    gl_Position = vec4(Position, 1.0);
  }
);

const GLchar * const FragmentShaderCode = GLSL330(
  in vec3 ForwaredColor;

  out vec4 OutColor;

  void main()
  {
    OutColor = vec4(ForwaredColor, 1.0f);
  }
);

static bool
IsShaderCompiled(GLuint Shader)
{
  GLint Status;

  glGetShaderiv(Shader, GL_COMPILE_STATUS, &Status);
  if (!Status) {
    GLchar ShaderInfoLog[512];
    glGetShaderInfoLog(Shader, sizeof(ShaderInfoLog), nullptr, ShaderInfoLog);
    std::cerr << "GLSL compilation error: " << ShaderInfoLog << std::endl;
    return false;
  }
  return true;
}

struct Vertex {
  GLfloat Position[3];
  GLfloat Color[3];
};

int
main(int argc, char* argv[])
{
  (void) argc;
  (void) argv;

  static_assert(sizeof(Vertex) == sizeof(GLfloat) * 6, "Invalid assumption");

  // GLFW initialization
  glfwInit();

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
  glfwWindowHint(GLFW_SAMPLES, 4);

  GLFWwindow*Window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "OpenGL Hello world!", nullptr, nullptr);

  glfwMakeContextCurrent(Window);
  glfwSwapInterval(1);

  // GLEW initialization
  GLenum GlewError;
  glewExperimental = GL_TRUE;
  if ((GlewError = glewInit()) != GLEW_OK) {
    fprintf(stderr, "Error : %s\n", glewGetErrorString(GlewError));
    std::cerr << "Error: " << glewGetErrorString(GlewError) << std::endl;
    return -1;
  }

  glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
  glEnable(GL_MULTISAMPLE);

  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // Shader compilation
  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  // Crate shader program
  GLuint ShaderProgram = glCreateProgram();
  {
    // Compile vertex shader
    GLuint VertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(VertexShader, 1, &VertexShaderCode, nullptr);
    glCompileShader(VertexShader);
    if (!IsShaderCompiled(VertexShader))
      return -1;

    // Compile fragment shader
    GLuint FragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(FragmentShader, 1, &FragmentShaderCode, nullptr);
    glCompileShader(FragmentShader);
    if (!IsShaderCompiled(FragmentShader))
      return -1;

    // Attach and link the compiled shaders against the shader program
    glAttachShader(ShaderProgram, VertexShader);
    glAttachShader(ShaderProgram, FragmentShader);
    glBindFragDataLocation(ShaderProgram, 0, "OutColor");
    glLinkProgram(ShaderProgram);

    // Delete the shader as we have done compiling and liking he shaders.
    glDeleteShader(VertexShader);
    glDeleteShader(FragmentShader);
  }


  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // Vertex setup
  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // Vertex data
  static const Vertex TriangleVertices[] = {
    {-0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f},
    { 0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f},
    { 0.0f,  0.5f, 0.0f, 0.0f, 0.0f, 1.0f}
  };

  // Create vertex array object
  GLuint Vao;
  glGenVertexArrays(1, &Vao);
  // Create vertex buffer object
  GLuint Vbo;
  glGenBuffers(1, &Vbo);

  glBindVertexArray(Vao);
  {
    glBindBuffer(GL_ARRAY_BUFFER, Vbo);
    {
      // Copy the data
      glBufferData(GL_ARRAY_BUFFER, sizeof(TriangleVertices), TriangleVertices, GL_STATIC_DRAW);
      // Set the vertex shader Position attribute layout
      GLuint PositionAttr = glGetAttribLocation(ShaderProgram, "Position");
      glEnableVertexAttribArray(PositionAttr);
      glVertexAttribPointer(PositionAttr, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                            reinterpret_cast<GLvoid *>(offsetof(Vertex, Position)));
      // Set the vertex shader Color attribute layout
      GLuint ColorAttr = glGetAttribLocation(ShaderProgram, "Color");
      glEnableVertexAttribArray(ColorAttr);
      glVertexAttribPointer(ColorAttr, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                            reinterpret_cast<GLvoid *>(offsetof(Vertex, Color)));
    }
    glBindBuffer(GL_ARRAY_BUFFER, 0);
  }
  glBindVertexArray(0);


  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // Begin rendering
  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  while (!glfwWindowShouldClose(Window)) {
    // Poll and check events
    glfwPollEvents();
    if (glfwGetKey(Window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
      glfwSetWindowShouldClose(Window, GL_TRUE);

    // Clear the color buffer
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // Render our shape using the previous program
    glUseProgram(ShaderProgram);

    glBindVertexArray(Vao);
    {
      glDrawArrays(GL_TRIANGLES, 0, 3);
    }
    glBindVertexArray(0);

    // Swap the OpenGL buffer into the window
    glfwSwapBuffers(Window);
  }

  // Delete the shader program, vertex array and vertex buffer
  glDeleteProgram(ShaderProgram);
  glDeleteVertexArrays(1, &Vao);
  glDeleteBuffers(1, &Vbo);

  // Destroy GLFW window and terminate the program
  glfwDestroyWindow(Window);
  glfwTerminate();
  return 0;
}

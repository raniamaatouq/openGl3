#include <iostream>

// لازم GLEW قبل GLFW
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

// إعدادات النافذة
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// ================== Shaders ==================

// Vertex Shader
const char* vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"uniform vec3 offset;\n"
"void main()\n"
"{\n"
"   gl_Position = vec4(aPos + offset, 1.0);\n"
"}\0";

// Fragment Shader (لون + شفافية)
const char* fragmentShaderSource = "#version 330 core\n"
"out vec4 FragColor;\n"
"uniform vec4 ourColor;\n"
"void main()\n"
"{\n"
"   FragColor = ourColor;\n"
"}\n\0";

// ================== متغيرات التحكم ==================
float offsetX = 0.0f;
float offsetY = 0.0f;
float alpha = 0.6f;

float colorR = 1.0f;
float colorG = 0.5f;
float colorB = 0.2f;

// ================== دوال ==================
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    // الحركة باستخدام الأسهم
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        offsetY += 0.001f;

    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        offsetY -= 0.001f;

    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        offsetX -= 0.001f;

    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        offsetX += 0.001f;

    // التحكم بالشفافية
    if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS)
        alpha = 0.3f;
    else
        alpha = 0.6f;

    // تغيير اللون
    if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS)
    {
        colorR = 0.2f;
        colorG = 0.8f;
        colorB = 0.4f;
    }

    // زر إضافي للتحكم بالعمق (مثلاً W/S لتحريك المثلثين على Z-axis)
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        ; // ممكن تضيف حركة للأمام على Z لو بدك
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        ; // ممكن تضيف حركة للخلف على Z لو بدك
}

int main()
{
    // ================== GLFW ==================
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "OpenGL Project", NULL, NULL);
    if (!window)
    {
        std::cout << "Failed to create window\n";
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // ================== GLEW ==================
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK)
    {
        std::cout << "Failed to initialize GLEW\n";
        return -1;
    }

    // تفعيل الشفافية
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // ================== تفعيل Depth Test ==================
    glEnable(GL_DEPTH_TEST);

    // ================== Shaders ==================
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // ================== بيانات المثلث ==================
    float vertices[] = {
        -0.3f, -0.3f, 0.0f,
         0.3f, -0.3f, 0.0f,
         0.0f,  0.3f, 0.0f
    };

    unsigned int VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // ================== Render Loop ==================
    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        // مسح اللون + depth buffer
        glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);

        // المثلث الأول
        glUniform3f(glGetUniformLocation(shaderProgram, "offset"),
            offsetX, offsetY, -0.3f);
        glUniform4f(glGetUniformLocation(shaderProgram, "ourColor"),
            colorR, colorG, colorB, alpha);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        // المثلث الثاني (Z مختلف)
        glUniform3f(glGetUniformLocation(shaderProgram, "offset"),
            -offsetX, -offsetY, -0.7f);
        glUniform4f(glGetUniformLocation(shaderProgram, "ourColor"),
            0.8f, 0.2f, 0.4f, alpha);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // ================== Cleanup ==================
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);

    glfwTerminate();
    return 0;
}
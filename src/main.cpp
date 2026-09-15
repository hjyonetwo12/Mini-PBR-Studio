#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include<numbers>
#include <iostream>

#include "Shader.h"
#include "Camera.h"
#include "stb_image.h"
#include "SphereMesh.h"

// ----------------------------------------------------------------------------------
// 1. 常量与全局变量定义
// ----------------------------------------------------------------------------------
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// 相机对象
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// 帧率/时间差控制
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// ----------------------------------------------------------------------------------
// 工业标准：单张 2D 纹理加载通用辅助函数
// ----------------------------------------------------------------------------------
unsigned int loadTexture(char const* path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    stbi_set_flip_vertically_on_load(true); // 翻转 Y 轴匹配 OpenGL 纹理坐标系
    unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);

    if (data)
    {
        GLenum format = GL_RGB;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        // 设置纹理环绕与过滤参数
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}

// ----------------------------------------------------------------------------------
// 2. GLFW 回调函数与输入响应
// ----------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
}

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // 屏幕坐标 Y 轴翻转

    lastX = xpos;
    lastY = ypos;

    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
    {
        camera.ProcessMouseMovement(xoffset, yoffset);
    }
    else
    {
        firstMouse = true;
    }
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

// ----------------------------------------------------------------------------------
// 3. 主函数 main
// ----------------------------------------------------------------------------------
int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "HJY'S FIRST WORK", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
    glEnable(GL_DEPTH_TEST);

    Shader ourShader("shaders/shader.vert", "shaders/shader.frag");

    SphereMesh mesh;
    mesh.Generate();
    // ----------------------------------------------------
    // 加载漫反射贴图与高光贴图
    // ----------------------------------------------------
    unsigned int diffuseMap = loadTexture("assets/container.jpg");
    unsigned int specularMap = loadTexture("assets/container2_specular.png");

    // 静态配置采样器绑定的纹理单元槽位
    ourShader.use();
    ourShader.setInt("material.diffuse", 0);  // 对应 GL_TEXTURE0
    ourShader.setInt("material.specular", 1); // 对应 GL_TEXTURE1

    // ====================================================
    // 渲染主循环 (Render Loop)
    // ====================================================
    while (!glfwWindowShouldClose(window))
    {
        // 1. 计算 deltaTime
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // 2. 输入响应
        processInput(window);

        // 3. 清屏
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // 4. 激活 Shader 与矩阵变换
        ourShader.use();
        ourShader.setVec3("viewPos", camera.Position);

        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::rotate(model, (float)glfwGetTime() * 0.5f, glm::vec3(0.5f, 1.0f, 0.0f)); // 稍微减慢旋转速度便于观察

        ourShader.setMat4("projection", projection);
        ourShader.setMat4("view", view);
        ourShader.setMat4("model", model);

        // 5. 传递 Light 结构体属性
        glm::vec3 lightPos(1.2f, 1.0f, 2.0f);
        ourShader.setVec3("light.position", lightPos);
        ourShader.setVec3("light.ambient", glm::vec3(0.2f, 0.2f, 0.2f));
        ourShader.setVec3("light.diffuse", glm::vec3(0.6f, 0.6f, 0.6f));
        ourShader.setVec3("light.specular", glm::vec3(1.0f, 1.0f, 1.0f));

        // 6. 传递 Material 结构体属性 (仅设置反光度，specular 贴图通过纹理单元读取)
        ourShader.setFloat("material.shininess", 64.0f);

        // 7. 激活并绑定两个纹理单元
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, diffuseMap);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, specularMap);

        // 8. 绘制几何体
        mesh.Draw();

        // 9. 双缓冲交换与轮询
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    mesh.Delete();
    

    glfwTerminate();
    return 0;
}
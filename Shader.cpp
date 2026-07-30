#include "Shader.h"
#include <glad/glad.h> // 包含 glad 来获取所有的必需 OpenGL 头文件
#include <string>
#include <sstream>
#include <fstream>
Shader::Shader(const char* vertexPath, const char* fragmentPath) {
    // ----------------------------------------------------
    // 步骤 1：从文件路径中读取 Vertex/Fragment Shader 的源码
    // ----------------------------------------------------
    std::string vertexCode;
    std::string fragmentCode;
    std::ifstream vShaderFile;
    std::ifstream fShaderFile;

    // 保证 ifstream 对象可以抛出异常：
    vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    try {
        // TODO: 【手写练习 1】打开文件，使用 stringstream 读入 string 中
        vShaderFile.open(vertexPath);
        std::stringstream vShaderStream;
        vShaderStream << vShaderFile.rdbuf();
        vShaderFile.close();
        vertexCode = vShaderStream.str();
        fShaderFile.open(fragmentPath);
        std::stringstream fShaderStream;
        fShaderStream << fShaderFile.rdbuf();
        fShaderFile.close();
        fragmentCode = fShaderStream.str();
    }
    catch (std::ifstream::failure& e) {
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ: " << e.what() << std::endl;
    }

    const char* vShaderCode = vertexCode.c_str();
    const char* fShaderCode = fragmentCode.c_str();

    // ----------------------------------------------------
    // 步骤 2：编译 Shader (GPU 侧)
    // ----------------------------------------------------
    unsigned int vertex, fragment;

    // TODO: 【手写练习 2】调用 glCreateShader, glShaderSource, glCompileShader 编译顶点着色器
    // 并调用 checkCompileErrors(vertex, "VERTEX"); 检查错误
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, NULL);
    glCompileShader(vertex);
    checkCompileErrors(vertex, "VERTEX");
    // TODO: 【手写练习 3】编译片段着色器 (Fragment Shader)
    // 并调用 checkCompileErrors(fragment, "FRAGMENT"); 检查错误
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, NULL);
    glCompileShader(fragment);
    checkCompileErrors(fragment, "FRAGMENT");
    // ----------------------------------------------------
    // 步骤 3：链接 Shader Program
    // ---------------------------------------------------- 
    //
    // TODO: 【手写练习 4】创建 ID (glCreateProgram)，Attach 两个 Shader，Link 并检查错误
    ID = glCreateProgram();
    glAttachShader(ID, vertex);
    glAttachShader(ID, fragment);
    glLinkProgram(ID);
    checkCompileErrors(ID, "PROGRAM");
    // ----------------------------------------------------
    // 步骤 4：删除已链接的 Shader (释放 GPU 编译临时资源)
    // ----------------------------------------------------
    glDeleteShader(vertex);
    glDeleteShader(fragment);
}
void Shader::use() {
    // TODO: 调用 glUseProgram 激活该程序
    glUseProgram(ID);
}

void Shader::setBool(const std::string& name, bool value) const {
    glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
}
void Shader::setInt(const std::string& name, int value) const {
    glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}
void Shader::setFloat(const std::string& name, float value) const {
    glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::checkCompileErrors(unsigned int shader, std::string type) {
    int success;
    char infoLog[1024];
    if (type != "PROGRAM") {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(shader, 1024, NULL, infoLog);
            std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
        }
    }
    else {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(shader, 1024, NULL, infoLog);
            std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
        }
    }
}
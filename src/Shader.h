#pragma once

#include <glad/glad.h> // 包含 glad 来获取所有的必需 OpenGL 头文件
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class Shader {
public:
    // Shader Program 的唯一 ID 标识符
    unsigned int ID;

    // 构造函数：读取并构建顶点与片段着色器
    Shader(const char* vertexPath, const char* fragmentPath);

    // 激活/使用该 Shader 程序
    void use();

    // 实用 Uniform 设定函数（后续 MVP 矩阵和 PBR 材质参数传值用）
    void setBool(const std::string& name, bool value) const;
    void setInt(const std::string& name, int value) const;
    void setFloat(const std::string& name, float value) const;

private:
    // 检查 Shader 编译与链接错误的辅助函数
    void checkCompileErrors(unsigned int shader, std::string type);
};
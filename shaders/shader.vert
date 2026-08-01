#version 330 core
layout (location = 0) in vec3 aPos;

uniform mat4 u_MVP; // 接收 C++ 算好的 Model-View-Projection 连乘矩阵

void main()
{
    gl_Position = u_MVP * vec4(aPos, 1.0); // 矩阵 * 3D 坐标
}
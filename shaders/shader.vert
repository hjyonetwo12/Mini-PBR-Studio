#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec3 aNormal; // 接收法线

out vec2 TexCoord;
out vec3 FragPos;  // 顶点的世界坐标
out vec3 Normal;   // 变换后的法线

uniform mat4 model; // 需要单独传入 model 矩阵，算 FragPos
uniform mat4 view;
uniform mat4 projection;

void main()
{
    // 计算顶点的世界空间坐标
    FragPos = vec3(model * vec4(aPos, 1.0));
    
    // 法线矩阵变换（防止非等比缩放导致法线扭曲）
    Normal = mat3(transpose(inverse(model))) * aNormal; 
    
    TexCoord = aTexCoord;
    gl_Position = projection * view * vec4(FragPos, 1.0);
}
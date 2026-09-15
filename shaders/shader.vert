#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec3 aNormal;

// 输出给片段着色器的插值变量
out vec2 TexCoord;
out vec3 FragPos;  // 片段在世界空间下的坐标
out vec3 Normal;   // 变换到世界空间的法线向量

// MVP 独立拆分矩阵
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    // 1. 计算当前顶点的世界空间绝对坐标 (用于片段着色器计算光照距离与方向)
    FragPos = vec3(model * vec4(aPos, 1.0));
    
    // 2. 利用法线矩阵变换法线 (防止模型发生非等比缩放时法线失真)
    Normal = mat3(transpose(inverse(model))) * aNormal;  
    
    // 3. 原样传递 UV 纹理坐标 (GPU 将在多边形内部进行双线性插值)
    TexCoord = aTexCoord;
    
    // 4. 顶点最终投射至屏幕裁剪空间
    gl_Position = projection * view * vec4(FragPos, 1.0);
}
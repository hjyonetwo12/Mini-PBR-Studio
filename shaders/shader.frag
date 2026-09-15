#version 330 core
out vec4 FragColor;

// 1. 材质结构体（双贴图模式）
struct Material {
    sampler2D diffuse;   // 漫反射贴图 (GL_TEXTURE0)
    sampler2D specular;  // 高光贴图 (GL_TEXTURE1)
    float shininess;     // 高光散射反光度
};

// 2. 光源结构体
struct Light {
    vec3 position;       // 光源在世界空间的位置

    vec3 ambient;        // 环境光分量强度
    vec3 diffuse;        // 漫反射分量强度
    vec3 specular;       // 镜面高光分量强度
};

// 3. 来自顶点着色器的输入属性 (名称必须与 shader.vert 的 out 完全一致)
in vec2 TexCoord;
in vec3 FragPos;
in vec3 Normal;

// 4. Uniform 传值
uniform vec3 viewPos;    // 摄像机在世界空间的位置
uniform Material material;
uniform Light light;

void main()
{
    // ----------------------------------------------------
    // 1. Ambient (环境光)
    // ----------------------------------------------------
    vec3 ambient = light.ambient * texture(material.diffuse, TexCoord).rgb;

    // ----------------------------------------------------
    // 2. Diffuse (漫反射：朗伯余弦定律)
    // ----------------------------------------------------
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light.position - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * texture(material.diffuse, TexCoord).rgb;

    // ----------------------------------------------------
    // 3. Specular (镜面高光：Phong 反射模型 + 高光贴图控制)
    // ----------------------------------------------------
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // 高光贴图采样：黑色区域(0.0)不反光，白色铁边与铁钉(1.0)产生强反光
    vec3 specular = light.specular * spec * texture(material.specular, TexCoord).rgb;

    // ----------------------------------------------------
    // 4. 三项叠加合成最终像素颜色
    // ----------------------------------------------------
    vec3 result = ambient + diffuse + specular;
    FragColor = vec4(result, 1.0);
}
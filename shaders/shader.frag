#version 330 core
out vec4 FragColor;

in vec2 TexCoord;
in vec3 FragPos;
in vec3 Normal;

uniform sampler2D texture1;
uniform vec3 lightPos;   // 光源位置
uniform vec3 viewPos;    // 摄像机位置
uniform vec3 lightColor; // 光源颜色 (如 vec3(1.0, 1.0, 1.0))

void main()
{
    // 1. 采样木箱基础颜色
    vec3 objectColor = texture(texture1, TexCoord).rgb;

    // 2. Ambient (环境光)
    float ambientStrength = 0.15;
    vec3 ambient = ambientStrength * lightColor;

    // 3. Diffuse (漫反射)
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos); // 光线方向
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    // 4. Specular (镜面高光)
    float specularStrength = 0.5;
    vec3 viewDir = normalize(viewPos - FragPos); // 视角方向
    vec3 reflectDir = reflect(-lightDir, norm);  // 反射光线方向
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32); // 32 为反光度 Shininess
    vec3 specular = specularStrength * spec * lightColor;

    // 5. 三项叠加并乘以基础颜色
    vec3 result = (ambient + diffuse + specular) * objectColor;
    FragColor = vec4(result, 1.0);
}
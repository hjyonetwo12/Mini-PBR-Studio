#include "SphereMesh.h"

#include <vector>
#include <numbers>
#include <cmath>
using namespace std;


    void SphereMesh::Generate(unsigned int xSegments , unsigned int ySegments ) {
        std::vector<float> data;
        std::vector<unsigned int> indices;

        // ====================================================
        // 1. 生成顶点属性数据 (Position + UV + Normal)
        // ====================================================
        for (unsigned int y = 0; y <= ySegments; ++y) {
            for (unsigned int x = 0; x <= xSegments; ++x) {
                float xSegment = (float)x / (float)xSegments;
                float ySegment = (float)y / (float)ySegments;

                // TODO: 计算角度 theta (0 ~ PI) 与 phi (0 ~ 2*PI)
                // TODO: 根据球坐标公式计算 xPos, yPos, zPos
                // TODO: 将 Position (3), UV (2), Normal (3) 依次 push_back 存入 data 容器
                float phi = xSegment * 2 * numbers::pi;
                float theta = ySegment * numbers::pi;
                float xPos = sin(theta) * cos(phi);
                float yPos = cos(theta);
                float zPos = sin(theta) * sin(phi);
                float u = xSegment;
                float v = ySegment;
                float Normal_x = xPos;
                float Normal_y = yPos;
                float Normal_z = zPos;
                data.insert(data.end(), { xPos,yPos,zPos,u,v,Normal_x,Normal_y,Normal_z });
            }
        }

        // ====================================================
        // 2. 生成拓扑索引数据 (EBO Indices)
        // ====================================================
        for (unsigned int y = 0; y < ySegments; ++y) {
            for (unsigned int x = 0; x < xSegments; ++x) {
                // 当前四边形对应的四个顶点在一维数组中的索引
                unsigned int k1 = y * (xSegments + 1) + x;
                unsigned int k2 = k1 + xSegments + 1;
                // TODO: 用 k1, k2, k1 + 1, k2 + 1 拼出两个逆时针三角形
                // 三角形 1: k1 -> k2 -> k1 + 1
                // 三角形 2: k1 + 1 -> k2 -> k2 + 1
                indices.push_back(k1);
                indices.push_back(k2);
                indices.push_back(k1 + 1);
                indices.push_back(k1 + 1);
                indices.push_back(k2);
                indices.push_back(k2 + 1);
            }
        }
        indexCount = static_cast<unsigned int>(indices.size());

        // ====================================================
        // 3. 配置 OpenGL 缓冲 (VAO / VBO / EBO)
        // ====================================================
        // TODO: glGenVertexArrays, glGenBuffers
        // TODO: glBindVertexArray
        // TODO: 绑定并填充 GL_ARRAY_BUFFER (VBO)
        // TODO: 绑定并填充 GL_ELEMENT_ARRAY_BUFFER (EBO)
        // TODO: 配置 3 个 glVertexAttribPointer (位置 0, UV 1, 法线 2)
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1,&VBO);
        glGenBuffers(1, &EBO);
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(
            GL_ARRAY_BUFFER,
            data.size() * sizeof(float),
            data.data(),
            GL_STATIC_DRAW
        );
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(
            GL_ELEMENT_ARRAY_BUFFER,
            indices.size() * sizeof(unsigned int),
            indices.data(),
            GL_STATIC_DRAW
        );
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float)));
        glEnableVertexAttribArray(2);
    }

    void SphereMesh::Draw() const {
        // TODO: 绑定 VAO 并执行 glDrawElements
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
    }
    void SphereMesh::Delete() const {
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &EBO);
        glDeleteVertexArrays(1, &VAO);
    }

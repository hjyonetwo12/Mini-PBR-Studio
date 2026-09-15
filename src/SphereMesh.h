#pragma once

#include <glad/glad.h>

class SphereMesh
{
public:
    unsigned int VAO, VBO, EBO;
    unsigned int indexCount;

    void Generate(unsigned int xSegments = 64, unsigned int ySegments = 64);
    void Draw() const;
    void Delete() const;
};
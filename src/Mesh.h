#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <vector>

class Mesh
{
public:
    Mesh() = default;
    ~Mesh();

    bool LoadFromFile(const char* filepath);
    void Draw() const;

    int NumTriangles() const { return m_numTriangles; }
    
private:
    GLuint m_vertexArrayId;
    GLuint m_vertexBufferId;
    int m_numTriangles;

};

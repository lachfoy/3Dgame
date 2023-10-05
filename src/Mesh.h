#ifndef MESH_H_
#define MESH_H_

#include <glad/glad.h> // gluint
#include <glm/glm.hpp> // mat4

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

#endif
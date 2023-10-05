#include "Mesh.h"

#include <cstdio> // printf
#include <iostream> // cerr
#include <cassert> // assert

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

Mesh::~Mesh()
{
    glDeleteVertexArrays(1, &m_vertexArrayId);
    glDeleteBuffers(1, &m_vertexBufferId);
}

bool Mesh::LoadFromFile(const char *filepath)
{
    tinyobj::attrib_t inattrib;
    std::vector<tinyobj::shape_t> inshapes;

    std::string err;

    bool ret = tinyobj::LoadObj(&inattrib, &inshapes, nullptr, &err, filepath);

    if (!ret)
    {
        std::cerr << "Failed to load " << filepath << std::endl;
        return false;
    }

    printf("# of vertices = %d\n", (int)(inattrib.vertices.size()) / 3);
    printf("# of normals = %d\n", (int)(inattrib.normals.size()) / 3);
    printf("# of texcoords = %d\n", (int)(inattrib.texcoords.size()) / 2);

    // inshapes is a collection of meshes in the OBJ. we only care about loading a single mesh so just index 0
    assert(inshapes.size() == 1);
    tinyobj::shape_t shape = inshapes[0];

    std::vector<float> buffer; // pos(3float), normal(3float), texcoords(2float)
    for (size_t f = 0; f < shape.mesh.indices.size() / 3; f++)
    {
        tinyobj::index_t idx0 = shape.mesh.indices[3 * f + 0];
        tinyobj::index_t idx1 = shape.mesh.indices[3 * f + 1];
        tinyobj::index_t idx2 = shape.mesh.indices[3 * f + 2];

        float tc[3][2];
        if (inattrib.texcoords.size() > 0)
        {
            if ((idx0.texcoord_index < 0) || (idx1.texcoord_index < 0) || (idx2.texcoord_index < 0))
            {
                // face does not contain valid uv index.
                tc[0][0] = 0.0f;
                tc[0][1] = 0.0f;
                tc[1][0] = 0.0f;
                tc[1][1] = 0.0f;
                tc[2][0] = 0.0f;
                tc[2][1] = 0.0f;
            } else {
                assert(inattrib.texcoords.size() >
                    size_t(2 * idx0.texcoord_index + 1));
                assert(inattrib.texcoords.size() >
                    size_t(2 * idx1.texcoord_index + 1));
                assert(inattrib.texcoords.size() >
                    size_t(2 * idx2.texcoord_index + 1));

                // Flip Y coord.
                tc[0][0] = inattrib.texcoords[2 * idx0.texcoord_index];
                tc[0][1] = 1.0f - inattrib.texcoords[2 * idx0.texcoord_index + 1];
                tc[1][0] = inattrib.texcoords[2 * idx1.texcoord_index];
                tc[1][1] = 1.0f - inattrib.texcoords[2 * idx1.texcoord_index + 1];
                tc[2][0] = inattrib.texcoords[2 * idx2.texcoord_index];
                tc[2][1] = 1.0f - inattrib.texcoords[2 * idx2.texcoord_index + 1];
            }
        }
        else
        {
            tc[0][0] = 0.0f;
            tc[0][1] = 0.0f;
            tc[1][0] = 0.0f;
            tc[1][1] = 0.0f;
            tc[2][0] = 0.0f;
            tc[2][1] = 0.0f;
        }

        float v[3][3];
        for (int k = 0; k < 3; k++)
        {
            int f0 = idx0.vertex_index;
            int f1 = idx1.vertex_index;
            int f2 = idx2.vertex_index;
            assert(f0 >= 0);
            assert(f1 >= 0);
            assert(f2 >= 0);    
            v[0][k] = inattrib.vertices[3 * f0 + k];
            v[1][k] = inattrib.vertices[3 * f1 + k];
            v[2][k] = inattrib.vertices[3 * f2 + k];
        }

        float n[3][3];
        {
            if (inattrib.normals.size() > 0)
            {
                int nf0 = idx0.normal_index;
                int nf1 = idx1.normal_index;
                int nf2 = idx2.normal_index;  
                if ((nf0 < 0) || (nf1 < 0) || (nf2 < 0))
                {
                    // normal index is missing from this face.
                    //invalid_normal_index = true;
                    assert(false);
                }
                else
                {
                    for (int k = 0; k < 3; k++)
                    {
                        assert(size_t(3 * nf0 + k) < inattrib.normals.size());
                        assert(size_t(3 * nf1 + k) < inattrib.normals.size());
                        assert(size_t(3 * nf2 + k) < inattrib.normals.size());
                        n[0][k] = inattrib.normals[3 * nf0 + k];
                        n[1][k] = inattrib.normals[3 * nf1 + k];
                        n[2][k] = inattrib.normals[3 * nf2 + k];
                    }
                }
            }
        }

        for (int k = 0; k < 3; k++)
        {
            // vertices
            buffer.push_back(v[k][0]);
            buffer.push_back(v[k][1]);
            buffer.push_back(v[k][2]);

            // normals
            buffer.push_back(n[k][0]);
            buffer.push_back(n[k][1]);
            buffer.push_back(n[k][2]);

            // texcoords
            buffer.push_back(tc[k][0]);
            buffer.push_back(tc[k][1]);
        }
    }

    m_numTriangles = buffer.size() / (3 + 3 + 2) / 3; // 3:vtx, 3:normal, 2:texcoord
    printf("# of triangles = %d\n", m_numTriangles);

    // create the actual mesh
    m_vertexArrayId = 0;
    m_vertexBufferId = 0;
    glGenVertexArrays(1, &m_vertexArrayId);

    // bind the mesh data
    glBindVertexArray(m_vertexArrayId);
    glGenBuffers(1, &m_vertexBufferId);
    glBindBuffer(GL_ARRAY_BUFFER, m_vertexBufferId);
    glBufferData(GL_ARRAY_BUFFER, buffer.size() * sizeof(float), buffer.data(), GL_STATIC_DRAW);

    GLsizei stride = (3 + 3 + 2) * sizeof(float); // temp kinda
    // positions
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (const void *)0);

    // normals
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (const void *)(sizeof(float) * 3));

    // tex coords
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (const void *)(sizeof(float) * 6));

    // unbind
    glBindVertexArray(0);

    return true;
}

void Mesh::Draw() const
{
    glBindVertexArray(m_vertexArrayId);
    glDrawArrays(GL_TRIANGLES, 0, 3 * m_numTriangles);
    glBindVertexArray(0);
}

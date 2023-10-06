#pragma once

// interface for 3D render objects
// should contain
// - geometry: vertices, normals etc.
// - transformation matrices
// - material/texture properties

#include "Entity3D.h"
#include <glm/glm.hpp>

class Mesh;
class Texture;

class iRenderable : public Entity3D
{
public:
    iRenderable() = default;
    ~iRenderable() {}

protected:
    glm::mat4 m_transform;
    Mesh* m_mesh;
    Texture* m_texture;

};

#ifndef RENDERABLE_H_
#define RENDERABLE_H_

// interface for 3D render objects
// should contain
// - geometry: vertices, normals etc.
// - transformation matrices
// - material/texture properties

#include <glm/glm.hpp>

class Mesh;
class Texture;
class Renderer3D;

class iRenderable
{
friend class Renderer3D;
public:
    iRenderable() = default;
    ~iRenderable() {}

protected:
    glm::mat4 m_transform;
    Mesh* m_mesh;
    Texture* m_texture;

};

#endif
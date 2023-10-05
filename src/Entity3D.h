#ifndef ENTITY_3D_H_
#define ENTITY_3D_H_

#include <glm/glm.hpp>

#include "Renderable.h"

class Entity3D : public iRenderable
{
public:
    Entity3D();
    ~Entity3D() {}

    void UpdateTransform();

protected:
    glm::vec3 m_worldPosition;
    glm::vec3 m_scale;
    glm::vec3 m_rotation;

};

#endif
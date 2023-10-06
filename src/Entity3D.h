#pragma once

#include <glm/glm.hpp>

class Entity3D
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

#include "Entity3D.h"

#include <glm/gtc/matrix_transform.hpp>

Entity3D::Entity3D()
    : m_worldPosition(glm::vec3(0.0f, 0.0f, 0.0f)), m_scale(glm::vec3(1.0f, 1.0f, 1.0f)), m_rotation(glm::vec3(0.0f, 0.0f, 0.0f))
{

}

void Entity3D::UpdateTransform()
{
    glm::mat4 transform = glm::mat4(1.0f); // identity

    transform = glm::scale(transform, m_scale);

    transform = glm::rotate(transform, m_rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
    transform = glm::rotate(transform, m_rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
    transform = glm::rotate(transform, m_rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));

    transform = glm::translate(transform, m_worldPosition);

    //m_transform = transform;
}
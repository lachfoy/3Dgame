#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "Entity.h"
#include <vector>
#include "Vertex.h"

class Renderer;

class RenderObject : public Entity
{
friend class Renderer;
public:
	RenderObject(std::vector<Vertex> vertices, unsigned int* indices, GLuint texture)
	: m_vertices(vertices), m_indices(indices), m_texture(texture) {}
	~RenderObject() {}

private:
	Renderer* m_renderer;
	std::vector<Vertex> m_vertices;
	unsigned int* m_indices;
	GLuint m_texture;

};

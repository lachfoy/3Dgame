#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "Vertex.h"
#include "Texture.h"

#include <vector>

class SpriteEntity;

typedef std::vector<Vertex> tVertexVec;
typedef std::vector<unsigned int> tIndexVec;

class RenderObject
{
public:
	RenderObject(tVertexVec* vertexVec, tIndexVec* indexVec, glm::vec2* position, Texture* texture)
		: m_vertexVec(vertexVec), m_indexVec(indexVec), m_position(position), m_texture(texture) {}

	tVertexVec* GetVertexVec() const { return m_vertexVec; }
	tIndexVec* GetIndexVec() const { return m_indexVec; }
	Texture* GetTexture() const { return m_texture; }
	glm::vec2* GetPosition() const { return m_position; }

private:
	tVertexVec* m_vertexVec;
	tIndexVec* m_indexVec;
	Texture* m_texture;
	glm::vec2* m_position;

};

class RenderObjectCompare
{
public:
	bool operator()(const RenderObject& obj1, const RenderObject& obj2) const
	{
		return obj1.GetTexture() < obj2.GetTexture();
	}
};

class Renderer
{
public:
	Renderer() = default;
	~Renderer() {}

	void Init();
	void SetProjection(unsigned int screenWidth, unsigned int screenHeight);
	void Dispose();

	void AddRenderObject(const RenderObject& renderObject);
	
	void RenderObjects();

	void AddDebugLine(const glm::vec2& p1, const glm::vec2& p2);
	void RenderDebugLines();

	void FlushBatch();
	void ClearBatch();

private:
	void CreateShaderProgram();
	void CreateRenderData();

	void CheckError();

	GLuint m_shaderProgram;

	tVertexVec m_vertexBuffer;
	tIndexVec m_indexBuffer;

	GLuint m_vao;
	GLuint m_vbo;
	GLuint m_ebo;

	std::vector<RenderObject> m_renderObjects;

	// Debug lines
	GLuint m_debugShaderProgram;
	std::vector<glm::vec2> m_linePoints;
	GLuint m_lineVao;
	GLuint m_lineVbo;

};
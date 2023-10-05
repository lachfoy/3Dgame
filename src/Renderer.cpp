#include "Renderer.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <cassert>
#include <algorithm>

static const unsigned int kMaxSprites = 2000;
static const unsigned int kMaxLines = 100;

void Renderer::Init()
{
	CreateShaderProgram();
	CreateRenderData();
}

void Renderer::SetProjection(unsigned int screenWidth, unsigned int screenHeight)
{
	glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(screenWidth), static_cast<float>(screenHeight), 0.0f, -1.0f, 1.0f);
	
	glUseProgram(m_shaderProgram);
	glUniformMatrix4fv(glGetUniformLocation(m_shaderProgram, "u_projection"), 1, false, glm::value_ptr(projection));

	glUseProgram(m_debugShaderProgram);
	glUniformMatrix4fv(glGetUniformLocation(m_debugShaderProgram, "u_projection"), 1, false, glm::value_ptr(projection));
}

void Renderer::Dispose()
{
	glDeleteBuffers(1, &m_ebo);
	glDeleteBuffers(1, &m_vbo);
	glDeleteVertexArrays(1, &m_vao);
	glDeleteBuffers(1, &m_lineVbo);
	glDeleteVertexArrays(1, &m_lineVao);

	glDeleteProgram(m_shaderProgram);
	glDeleteProgram(m_debugShaderProgram);
}

void Renderer::AddRenderObject(const RenderObject& renderObject)
{
	m_renderObjects.push_back(renderObject);
}

void Renderer::RenderObjects()
{
	//std::sort(m_renderObjects.begin(), m_renderObjects.end(), RenderObjectCompare());

	glUseProgram(m_shaderProgram);
	glBindVertexArray(m_vao);

	Texture* currentTexture = nullptr;

	for (const RenderObject& obj : m_renderObjects)
	{
		if (obj.GetTexture() != currentTexture)
		{
			// If a different texture is encountered, start a new batch
			if (!m_vertexBuffer.empty())
			{
				currentTexture->Bind();
				FlushBatch();
				ClearBatch();
			}
			currentTexture = obj.GetTexture();
		}

		for (Vertex vertex : *(obj.GetVertexVec())) {
			if (obj.GetPosition() != nullptr)
				vertex.position += *(obj.GetPosition());

			m_vertexBuffer.push_back(vertex);
		}

		// Append the indices from the current RenderObject to the current index batch
		unsigned int vertexOffset = m_vertexBuffer.size() - obj.GetVertexVec()->size();
		for (unsigned int index : *(obj.GetIndexVec()))
		{
			m_indexBuffer.push_back(index + vertexOffset);
		}
	}

	// Add the last batch (if any) to the result
	if (!m_vertexBuffer.empty())
	{
		currentTexture->Bind();
		FlushBatch();
		ClearBatch();
	}

	glBindVertexArray(0);
	m_renderObjects.clear();
}

void Renderer::AddDebugLine(const glm::vec2& p1, const glm::vec2& p2)
{
	m_linePoints.push_back(p1);
	m_linePoints.push_back(p2);
}

void Renderer::RenderDebugLines()
{
	glUseProgram(m_debugShaderProgram);
	glBindVertexArray(m_lineVao);

	glBindBuffer(GL_ARRAY_BUFFER, m_lineVbo);
	glBufferSubData(GL_ARRAY_BUFFER, 0, m_linePoints.size() * sizeof(glm::vec2), m_linePoints.data());

	glDrawArrays(GL_LINES, 0, m_linePoints.size());

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	m_linePoints.clear();

	glBindVertexArray(0);
}

void Renderer::FlushBatch()
{
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferSubData(GL_ARRAY_BUFFER, 0, m_vertexBuffer.size() * sizeof(Vertex), m_vertexBuffer.data());
	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
	glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, m_indexBuffer.size() * sizeof(unsigned int), m_indexBuffer.data());
	
	glDrawElements(GL_TRIANGLES, m_indexBuffer.size(), GL_UNSIGNED_INT, 0);
	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Renderer::ClearBatch()
{
	m_vertexBuffer.clear();
	m_indexBuffer.clear();
}

void Renderer::CreateShaderProgram()
{
	// Create the vertex shader
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	{
		const GLchar* vertexSource = R"(
			#version 330 core

			layout (location = 0) in vec2 a_position;
			layout (location = 1) in vec2 a_texcoord;

			varying vec2 v_texcoord;

			uniform mat4 u_projection;

			void main()
			{
				gl_Position = u_projection * vec4(a_position, 0.0, 1.0);
				v_texcoord = a_texcoord;
			}
		)";

		glShaderSource(vertexShader, 1, &vertexSource, 0);
		glCompileShader(vertexShader);

		int success;
		char infoLog[512];
		glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
		if (!success) {
			glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
			printf("Failed to compile vertex shader:\n%s\n", infoLog);
		}
	}

	// Create the fragment shader
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	{
		const GLchar* fragmentSource = R"(
			#version 330 core
			out vec4 out_color;

			in vec2 v_texcoord;

			uniform vec4 u_color;
			uniform sampler2D u_sampler;

			void main()
			{
				out_color = texture(u_sampler, v_texcoord);
				//out_color = vec4(1, 0, 1, 1);
			}
		)";
		
		glShaderSource(fragmentShader, 1, &fragmentSource, 0);
		glCompileShader(fragmentShader);

		int success;
		char infoLog[512];
		glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
		if (!success) {
			glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
			printf("Failed to compile fragment shader:\n%s\n", infoLog);
		}
	}

	// Link the final shader program
	m_shaderProgram = glCreateProgram();
	glAttachShader(m_shaderProgram, vertexShader);
	glAttachShader(m_shaderProgram, fragmentShader);
	glLinkProgram(m_shaderProgram);

	int success;
	char info_log[512];
	glGetProgramiv(m_shaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(m_shaderProgram, 512, NULL, info_log);
		printf("Failed to link shader:\n%s\n", info_log);
	}

	// No longer need these
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	// Set up sampler ...just one for now
	glUseProgram(m_shaderProgram);
	GLint textureUniformLocation = glGetUniformLocation(m_shaderProgram, "u_sampler");
	assert(textureUniformLocation >= 0 && "Sampler does not exist");
	glUniform1i(textureUniformLocation, 0);


	// DEBUG SHADER -- MOVE THIS
	{
		// Create the vertex shader
		GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
		{
			const GLchar* vertexSource = R"(
			#version 330 core

			layout (location = 0) in vec2 a_position;

			uniform mat4 u_projection;

			void main()
			{
				gl_Position = u_projection * vec4(a_position, 0.0, 1.0);
			}
		)";

			glShaderSource(vertexShader, 1, &vertexSource, 0);
			glCompileShader(vertexShader);

			int success;
			char infoLog[512];
			glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
			if (!success) {
				glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
				printf("Failed to compile vertex shader:\n%s\n", infoLog);
			}
		}

		// Create the fragment shader
		GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
		{
			const GLchar* fragmentSource = R"(
			#version 330 core
			out vec4 out_color;

			uniform vec4 u_color;

			void main()
			{
				out_color = vec4(1, 0, 1, 1);
			}
		)";

			glShaderSource(fragmentShader, 1, &fragmentSource, 0);
			glCompileShader(fragmentShader);

			int success;
			char infoLog[512];
			glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
			if (!success) {
				glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
				printf("Failed to compile fragment shader:\n%s\n", infoLog);
			}
		}

		// Link the final shader program
		m_debugShaderProgram = glCreateProgram();
		glAttachShader(m_debugShaderProgram, vertexShader);
		glAttachShader(m_debugShaderProgram, fragmentShader);
		glLinkProgram(m_debugShaderProgram);

		int success;
		char info_log[512];
		glGetProgramiv(m_debugShaderProgram, GL_LINK_STATUS, &success);
		if (!success) {
			glGetProgramInfoLog(m_debugShaderProgram, 512, NULL, info_log);
			printf("Failed to link shader:\n%s\n", info_log);
		}

		// No longer need these
		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);
	}
}

void Renderer::CreateRenderData()
{
	// VAO
	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);

	// VBO
	glGenBuffers(1, &m_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, kMaxSprites * 4 * sizeof(Vertex), NULL, GL_DYNAMIC_DRAW);

	// EBO
	glGenBuffers(1, &m_ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, kMaxSprites * 6 * sizeof(unsigned int), NULL, GL_DYNAMIC_DRAW);

	// Enable the vertex attribute arrays for position and texcoords
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoord));

	// Unbind
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	// FOR DEBUG LINES
	// VAO
	glGenVertexArrays(1, &m_lineVao);
	glBindVertexArray(m_lineVao);

	// VBO
	glGenBuffers(1, &m_lineVbo);
	glBindBuffer(GL_ARRAY_BUFFER, m_lineVbo);
	glBufferData(GL_ARRAY_BUFFER, kMaxLines * 2 * sizeof(glm::vec2), NULL, GL_DYNAMIC_DRAW);

	// Enable the vertex attribute arrays for position and texcoords
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (void*)0);

	// Unbind
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Renderer::CheckError()
{
	GLenum error = glGetError();
	if (error > 0) {
		switch (error)
		{
		case GL_INVALID_ENUM:
			printf("GL_INVALID_ENUM");
			break;
		case GL_INVALID_OPERATION:
			printf("GL_INVALID_OPERATION");
			break;
		case GL_INVALID_VALUE:
			printf("GL_INVALID_VALUE");
			break;
		default:
			break;
		}
		//assert(false);
	}
}

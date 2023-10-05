#pragma once

#include <glad/glad.h>

class Texture
{
public:
	Texture() = default;
	Texture(const char* path, bool useMipMaps = false);
	~Texture();

	bool LoadFromFile(const char* path, bool useMipMaps = false);
	void Bind() const;

private:
	GLuint m_texture;

};

#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

class Shader
{
public:
    Shader() = default;
    ~Shader();

    void LoadFromFile(const char* vertex_path, const char* fragment_path);

    void Use() const;
    GLint GetUniformLocation(const char* name) const;
    void SetMat4f(const char* name, glm::mat4 value);
    void SetVec4f(const char* name, glm::vec4 value);
    void SetVec3f(const char* name, glm::vec3 value);
    void Set1i(const char* name, int value);
    void Set1f(const char* name, float value);

private:
    GLuint m_programId = 0;

};

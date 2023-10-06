#include "Shader.h"

#include <string>
#include <fstream>
#include <cassert>
#include <iostream>
#include <cstdio>

#include <glm/gtc/type_ptr.hpp>

Shader::~Shader()
{
    // delete shader
    glDeleteProgram(m_programId);
}

void Shader::LoadFromFile(const char *vertex_path, const char *fragment_path)
{
    GLuint vertex_id = 0, fragment_id = 0;
    
    // read in the vertex file
    {
        std::string vertex_source;
        std::ifstream vertex_file(vertex_path, std::ios::ate);
        if (vertex_file.is_open()) 
        {
            std::streamsize file_size = vertex_file.tellg(); // get the size from the end of file
            vertex_file.seekg(0, std::ios::beg); // set the cursor back to the begining of file

            vertex_source.resize(static_cast<size_t>(file_size));
            vertex_file.read((char*)vertex_source.data(), file_size);
            vertex_file.close();

            const char* src = vertex_source.c_str();
            vertex_id = glCreateShader(GL_VERTEX_SHADER);
            glShaderSource(vertex_id, 1, &src, 0);
            glCompileShader(vertex_id);

            int success;
            char infoLog[512];
            glGetShaderiv(vertex_id, GL_COMPILE_STATUS, &success);
            if (!success) {
                glGetShaderInfoLog(vertex_id, 512, NULL, infoLog);
                printf("Failed to compile vertex shader:\n%s\n", infoLog);
                assert(false);
            }
        }
        else
        {
            assert(false && "vertex_file could not be opened");
        }
    }

    // read in the fragment file
    {
        std::string fragment_source;
        std::ifstream fragment_file(fragment_path, std::ios::ate);
        if (fragment_file.is_open()) 
        {
            std::streamsize file_size = fragment_file.tellg(); // get the size from the end of file
            fragment_file.seekg(0, std::ios::beg); // set the cursor back to the begining of file

            fragment_source.resize(static_cast<size_t>(file_size));
            fragment_file.read((char*)fragment_source.data(), file_size);
            fragment_file.close();

            const char* src = fragment_source.c_str();
            fragment_id = glCreateShader(GL_FRAGMENT_SHADER);
            glShaderSource(fragment_id, 1, &src, 0);
            glCompileShader(fragment_id);

            int success;
            char infoLog[512];
            glGetShaderiv(fragment_id, GL_COMPILE_STATUS, &success);
            if (!success) {
                glGetShaderInfoLog(fragment_id, 512, NULL, infoLog);
                printf("Failed to compile fragment shader:\n%s\n", infoLog);
            }
        }
        else
        {
            assert(false && "fragment_file could not be opened");
        }
    }

    // link to shader program
    m_programId = glCreateProgram();
    glAttachShader(m_programId, vertex_id);
    glAttachShader(m_programId, fragment_id);
    glLinkProgram(m_programId);
    
    // error check linking
    int success;
    char info_log[512];
    glGetProgramiv(m_programId, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(m_programId, 512, NULL, info_log);
        printf("Failed to link shader:\n%s\n", info_log);
    }

    // the vertex and fragment shader are no longer used after they have been linked into the shader program
    glDeleteShader(vertex_id);
    glDeleteShader(fragment_id);

    // hardcode image location -- reaaaallly bad to do here
    glUniform1i(glGetUniformLocation(m_programId, "gSampler"), 0);
}

void Shader::Use() const
{
    glUseProgram(m_programId);
}

GLint Shader::GetUniformLocation(const char* name) const
{
    return glGetUniformLocation(m_programId, name);
}

void Shader::SetMat4f(const char* name, glm::mat4 value)
{
    glUniformMatrix4fv(glGetUniformLocation(m_programId, name), 1, false, glm::value_ptr(value));
}

void Shader::SetVec4f(const char *name, glm::vec4 value)
{
    glUniform4f(glGetUniformLocation(m_programId, name), value.x, value.y, value.z, value.w);
}

void Shader::SetVec3f(const char *name, glm::vec3 value)
{
    glUniform3f(glGetUniformLocation(m_programId, name), value.x, value.y, value.z);
}

void Shader::Set1i(const char *name, int value)
{
    glUniform1i(glGetUniformLocation(m_programId, name), value);
}

void Shader::Set1f(const char *name, float value)
{
    glUniform1f(glGetUniformLocation(m_programId, name), value);
}

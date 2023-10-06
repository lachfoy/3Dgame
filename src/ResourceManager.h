#pragma once

#include <map>
#include <string>

class Mesh;
class Texture;

typedef std::map<std::string, Mesh*> tMeshMap;
typedef std::map<std::string, Texture*> tTextureMap;

class ResourceManager
{
public:
    ResourceManager() {}
    ~ResourceManager() {}

    void UnloadResources();

    bool LoadMesh(std::string name);
    bool LoadTexture(std::string name);

    Mesh* GetMesh(std::string name);
    Texture* GetTexture(std::string name);

private:
    tMeshMap m_meshMap;
    tTextureMap m_textureMap;
    static const std::string s_meshDirectoryPath;
    static const std::string s_textureDirectoryPath;

};

#include "ResourceManager.h"

#include <iostream>

#include "Mesh.h"
#include "Texture.h"

const std::string ResourceManager::s_meshDirectoryPath = "../data/models/";
const std::string ResourceManager::s_textureDirectoryPath = "../data/images/";

void ResourceManager::UnloadResources()
{
    std::cout << "Unloading resources\n";
    std::cout << "--------------------------------------------------------\n";
    for (auto& it : m_meshMap)
    {
        delete it.second;
        std::cout << "Unloaded mesh: " << it.first << "\n";
    }
    for (auto& it : m_textureMap)
    {
        delete it.second;
        std::cout << "Unloaded texture: " << it.first << "\n";
    }
    std::cout << "--------------------------------------------------------\n\n";
}

bool ResourceManager::LoadMesh(std::string name)
{
    std::string path = s_meshDirectoryPath + name + ".obj";

    Mesh *mesh = new Mesh();
    bool ret = mesh->LoadFromFile(path.c_str());

    m_meshMap[name] = mesh;

    return ret;
}

bool ResourceManager::LoadTexture(std::string name)
{
    std::string path = s_textureDirectoryPath + name + ".png";

    Texture *texture = new Texture();
    bool ret = texture->LoadFromFile(path.c_str());

    m_textureMap[name] = texture;

    return ret;
}

Mesh *ResourceManager::GetMesh(std::string name)
{
    Mesh* temp = m_meshMap[name];
    //assert(temp != nullptr && "couldn't find mesh");
    if (!temp)
    {
        std::cerr << "ERROR: missing mesh: " << name << "\n";
        abort();
    }

    return temp;
}

Texture *ResourceManager::GetTexture(std::string name)
{
    Texture* temp = m_textureMap[name];
    if (!temp)
    {
        std::cerr << "ERROR: missing texture: " << name << "\n";
        abort();
    }

    return temp;
}

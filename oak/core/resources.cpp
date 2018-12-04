#include "resources.h"
#include "../oak_def.h"


using namespace oak;



std::vector<Shader*> Resources::shaders;
std::vector<Texture*> Resources::textures;

void Resources::addShader(std::string shaderName)
{
  if (!isShaderLoaded(shaderName))
  {
    shaders.push_back(new Shader(shaderName));
  }
}

void Resources::addShader(Shader& shader)
{
  shaders.push_back(&shader);
}

void Resources::addTexture(std::string src)
{
  if (!isTextureLoaded(src))
  {
    textures.push_back(new Texture(src));
  }
}

void Resources::addTexture(Texture& texture)
{
  textures.push_back(&texture);
}

bool Resources::isTextureLoaded(std::string src)
{
  for (uint i = 0; i < textures.size(); i++)
  {
    if (textures[i]->getSrc() == src)
    {
      return true;
    }
  }
  return false;
}

bool Resources::isShaderLoaded(std::string name)
{
  for (uint i = 0; i < shaders.size(); i++)
  {
    if (shaders[i]->getName() == name)
    {
      return true;
    }
  }
  return false;
}

Shader& Resources::getShaderByID(uint id)
{
  for (uint i = 0; i < shaders.size(); i++)
  {
    if (shaders[i]->getID() == id)
    {
      return *shaders[i];
    }
  }
  return Fallback::shader;
}


Shader& Resources::getShaderByName(std::string name)
{
  for (uint i = 0; i < shaders.size(); i++)
  {
    if (shaders[i]->getName().compare(name) == 0)
    {
      return *shaders[i];
    }
  }
  return Fallback::shader;
}

Texture& Resources::getTextureByID(uint textureID)
{
  for (uint i = 0; i < textures.size(); i++)
  {
    if (textures[i]->getID() == textureID)
    {
      return *textures[i];
    }
  }
  return Fallback::texture;
}

Texture& Resources::getTextureBySrc(std::string src)
{
  for (uint i = 0; i < textures.size(); i++)
  {
    if (textures[i]->getSrc().compare(src) == 0)
    {
      return *textures[i];
    }
  }

  return Fallback::texture;
}

Shader& Resources::getDefaultShader()
{
  return Fallback::shader;
}

Texture& Resources::getDefaultTexture()
{
  return Fallback::texture;
}

uint Resources::getTextureIDBySrc(std::string src)
{
  return Resources::getTextureBySrc(src).getID();
}
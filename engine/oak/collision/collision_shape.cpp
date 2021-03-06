#include "collision_shape.h"

#include <oak/window/window.h>
#include <oak/scene/camera.h>
#include <oak/assets/shader.h>
#include <oak/core/resources.h>

#include <oak/ecs/entity.h>
#include "collision_rect.h"
#include "collision_circle.h"
#include <oak/oak_def.h>

using namespace oak;

CollisionShape::CollisionShape()
{

}
CollisionShape::~CollisionShape()
{

}

glm::vec3 CollisionShape::origin() const
{
  return this->entity->transform->position() + offset();
}

float CollisionShape::originX() const
{
  return this->entity->transform->position().x + m_offsetX;
}

float CollisionShape::originY() const
{
  return this->entity->transform->position().y + m_offsetY;
}

float CollisionShape::offsetX() const
{
  return m_offsetX;
}
float CollisionShape::offsetY() const
{
  return m_offsetY;
}

glm::vec3 CollisionShape::offset() const 
{
  return glm::vec3(m_offsetX, m_offsetY, 0.0f);
}

void CollisionShape::onDebugDraw() const
{
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, this->textureID);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glBindVertexArray(this->VAO);

  glm::mat4 model = glm::mat4(1.0);

  glm::vec3 camNPos = Camera::getNormalizedPos();

  glm::vec3 pos(
    Window::worldToViewportCoords(entity->transform->position().x + m_offsetX) - camNPos.x,
    Window::worldToViewportCoords(entity->transform->position().y + m_offsetY) - camNPos.y,
    0.0f
  );
  model = glm::translate(model, pos);


  Shader* shader = Resources::getDefaultShader();
  shader->use();
  shader->setMat4("model", model);

  glDrawArrays(GL_TRIANGLES, 0, 6);
}

void CollisionShape::initVAO(float quadW, float quadH)
{
  float xx = Window::worldToViewportCoords(quadW) * 0.5f;
  float yy = Window::worldToViewportCoords(quadH) * 0.5f;

  float texMin = 0.0f;
  float texMax = 1.0f;

  float vertices[] = {
    // positions    // texture coords
    -xx, -yy,       texMin, texMax, //bottom left
     xx, -yy,       texMax, texMax, //bottom right
     xx,  yy,       texMax, texMin, //top right

     xx,  yy,       texMax, texMin, //top right
    -xx,  yy,       texMin, texMin, //top left
    -xx, -yy,       texMin, texMax //botom left
  };

  glGenVertexArrays(1, &this->VAO);
  glGenBuffers(1, &this->VBO);

  glBindVertexArray(this->VAO);

  glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  // position attribute
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);

  // texture coord attribute
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
  glEnableVertexAttribArray(1);
}

CollisionShape::Type CollisionShape::getType() const
{
  return type;
}


bool CollisionShape::intersects(CollisionShape& shape)
{
  if (shape.getType() == Type::RECT)
  {
    CollisionRect* rect =  static_cast<CollisionRect*>(&shape);
    return intersectsRect(*rect);
  }
  else if (shape.getType() == Type::CIRCLE)
  {
    CollisionCircle* circle = static_cast<CollisionCircle*>(&shape);
    return intersectsCircle(*circle);
  }


  return false;
}

void CollisionShape::setOffset(float x, float y)
{
  m_offsetX = x;
  m_offsetY = y;
}
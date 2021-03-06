#include "base_rigid_body.h"
#include <oak/ecs/entity.h>


using namespace oak;

BaseRigidBody::BaseRigidBody(Component::Reflect REFLECT_ID, bool isStatic) : Component(REFLECT_ID)
{
  this->isStatic = isStatic;
  lastPos = glm::vec3(0.0f, 0.0f, 0.0f);
  velocity = glm::vec3(0.0f, 0.0f, 0.0f);
  nextPos = glm::vec3(0.0f, 0.0f, 0.0f);
  desiredNextPos = glm::vec3(0.0f, 0.0f, 0.0f);
}

void BaseRigidBody::onCreate()
{
  lastPos = entity->transform->position();
}

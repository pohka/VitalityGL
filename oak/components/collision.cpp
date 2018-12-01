#include "collision.h"
#include <cmath>
#include <cstdlib>
#include <debug.h>
#include <core/entity.h>
#include "collision_layer.h"


using namespace oak;

bool Collision::checkRectRect(const CollisionRect& a, const CollisionRect& b)
{
  return (
    std::abs(a.originX() - b.originX()) < (a.width() * 0.5f + b.width() * 0.5f) &&
    std::abs(a.originY() - b.originY()) < (a.height() * 0.5f + b.height() * 0.5f)
  );
}
bool Collision::checkRectCircle(const CollisionRect& rect, const CollisionCircle& circle)
{
  //STEP 1: treat rectangle like a circle (quick check)
    //take the largest dimension on the rectangle and use that as the radius
  float maxDimension = rect.width();
  if (rect.width() < rect.height()) 
  {
    maxDimension = rect.height();
  }

  float originSqrdDist =
    std::pow((rect.originX() - circle.originX()), 2.0f) +
    std::pow((rect.originY() - circle.originY()), 2.0f);

  //if no within range of the quick check, return false
  if(!(originSqrdDist < std::pow(maxDimension + circle.getRadius(), 2.0f)) )
  {
    return false;
  }

  //----------


  //STEP 2: check if circle within rect
  float diffX = std::abs(rect.originX() - circle.originX());
  float diffY = std::abs(rect.originY() - circle.originY());

  if (diffX < rect.width() / 2.0f && diffY < rect.height() / 2.0f) {
    return true;
  }

  //----------

  //STEP 3: check closest corner
  glm::vec2 closestPoint = findRectClosestCornerToCircle(rect, circle);

  //compare squared distance with squared radius
  float squaredDistToClosestPoint =
    std::pow(closestPoint.x - circle.originX(), 2.0f) +
    std::pow(closestPoint.y - circle.originY(), 2.0f);

  if (squaredDistToClosestPoint < circle.getRadius() * circle.getRadius())
  {
    return true;
  }

  //STEP 4: collision with side of rectangle and circle, but not colliding with corners
  return sideCheckRectCircle(rect,  circle);
}

bool Collision::checkCircleCircle(const CollisionCircle& a, const CollisionCircle& b)
{
  float sqrdDist = std::pow(a.originX() - b.originX(), 2.0f) + std::pow(b.originY() - a.originY(), 2.0f);
  float sqrdMaxDist = std::pow(a.getRadius() + b.getRadius(), 2);

  return sqrdDist < sqrdMaxDist;
}

 glm::vec2 Collision::findRectClosestCornerToCircle(const CollisionRect& rect, const CollisionCircle& circle) {
  //check closest corner
  //find corner of rect closest to circle
   float pointX = rect.minX();
   float pointY = rect.minY();

  //circle is left side
  if (circle.originX() < rect.originX())
  {
    //bottom left
    if (circle.originY() < rect.originY())
    {
      pointX = rect.minX();
      pointY = rect.minY();
    }
    //top left
    else 
    {
      pointX = rect.minX();
      pointY = rect.maxY();
    }
  }
  //circle is to right side
  else 
  {
    //bottom right
    if (circle.originY() < rect.originY())
    {
      pointX = rect.maxX();
      pointY = rect.minY();
    }
    //top right
    else 
    {
      pointX = rect.maxX();
      pointY = rect.maxY();
    }
  }

  return glm::vec2(pointX, pointY);
}

 //checks collision if side of rect intersects circle but with no corners
bool Collision::sideCheckRectCircle(
   const CollisionRect& rect,
   const CollisionCircle& circle)
 {
   //closest point on circumfrence of circle to the rectangle on a single axis

   //if X-axis has possible side collision
   if (circle.originY() < rect.maxY() && circle.originY() > rect.minY())
   {
     float circleEdgeX;

     //circle left of rect
     if (circle.originX() < rect.originX())
     {
       circleEdgeX = circle.originX() + circle.getRadius();
     }
     //circle right of rect
     else {
       circleEdgeX = circle.originX() - circle.getRadius();
     }

     float distOnX = std::abs(circleEdgeX - rect.originX());
     if (distOnX < rect.width() / 2.0f) {
       return true;
     }
   }

   //if Y-axis has possible side collision
   if (circle.originX() < rect.maxX() && circle.originX() > rect.minX()) {
     float circleEdgeY;

     //circle below rect
     if (circle.originY() < rect.originY()) {
       circleEdgeY = circle.originY() + circle.getRadius();
     }
     //circle above rect
     else {
       circleEdgeY = circle.originY() - circle.getRadius();
     }

     float distOnY = std::abs(circleEdgeY - rect.originY());
     if (distOnY < rect.height() / 2.0f) {
       return true;
     }
   }

   return false;
 }

bool Collision::checkEntEntCollision(Entity* entA, Entity* entB)
{
  for (uint a = 0; a < entA->collisionShapes.size(); a++)
  {
    BaseCollisionShape* colA = entA->collisionShapes[a];

    for (uint b = 0; b < entB->collisionShapes.size(); b++)
    {
      BaseCollisionShape* colB = entB->collisionShapes[b];
      if (colA->intersects(*colB))
      {
       // LOG << "HAS COLLISION";
        entA->notifyCollision(*entB);
        entB->notifyCollision(*entA);
        return true;
      }
    }
  }
  return false;
}

void Collision::resolveCollisions()
{
  for (uint a = 0; a < Entity::entitys.size() - 1; a++)
  {
    for (uint b = a + 1; b < Entity::entitys.size(); b++)
    {
      checkEntEntCollision(Entity::entitys[a], Entity::entitys[b]);
    }
  }
}
#include "physics.h"
#include <oak/ecs/entity_manager.h>
#include "collision_shape.h"
#include "collision_rect.h"
#include "collision_circle.h"
#include <iostream>
#include <bitset>

using namespace oak;



struct LineSegment
{
  const float x1, y1, x2, y2;
  float maxDist = -1.0f;

  LineSegment(const float x1, const float y1, const float x2, const float y2) :
    x1(x1),
    y1(y1),
    x2(x2),
    y2(y2)
  {
  }

  glm::vec2 getNormal() const
  {
    float x = x2 - x1;
    float y = y2 - y1;
    return glm::vec2(y, -x);
  }

  //returns true if point is on the line segment, distFromOrigin must be calculated first using distance(pt , (x1, y1))
  bool isPointOnLine(glm::vec2 pt, float distFromOrigin)
  {
    //calc maxDist only once
    if (maxDist == -1.0f)
    {
      maxDist = glm::distance(glm::vec2(x1, y1), glm::vec2(x2, y2));
    }

    if (distFromOrigin < maxDist)
    {
      glm::vec2 dir = glm::vec2(x2 - x1, y2 - y1);
      glm::vec2 ptDir = glm::vec2(pt.x - x1, pt.x - y1);

      float dot = glm::dot(dir, ptDir);
      //facing same direction
      if (dot > 0)
      {
        return true;
      }
    }
    return false;
  }
};

enum IntersectResult { PARALLEL, COINCIDENT, NOT_INTERESECTING, INTERESECTING };

//checks if 2 lines intersect
int checkLine(const LineSegment& line, const LineSegment& other, glm::vec2& intersection)
{
  float denom = (
    (other.y2 - other.y1)*(line.x2 - line.x1)) -
    ((other.x2 - other.x1)*(line.y2 - line.y1)
      );

  float nume_a = (
    (other.x2 - other.x1)*(line.y1 - other.y1)) -
    ((other.y2 - other.y1)*(line.x1 - other.x1)
      );

  float nume_b = (
    (line.x2 - line.x1)*(line.y1 - other.y1)) -
    ((line.y2 - line.y1)*(line.x1 - other.x1)
  );

  if (denom == 0.0f)
  {
    if (nume_a == 0.0f && nume_b == 0.0f)
    {
      return COINCIDENT;
    }
    return PARALLEL;
  }

  float ua = nume_a / denom;
  float ub = nume_b / denom;

  if (ua >= 0.0f && ua <= 1.0f && ub >= 0.0f && ub <= 1.0f)
  {
    // Get the intersection point.
    intersection.x = line.x1 + ua * (line.x2 - line.x1);
    intersection.y = line.y1 + ua * (line.y2 - line.y1);

    return INTERESECTING;
  }

  return NOT_INTERESECTING;
}

//checks rect line intersection and returns true if hit, output is set in hit
bool checkRect(CollisionRect* rect, LineSegment& other, RaycastHit2D& hit, const float maxDistance)
{
  RectBounds bounds = rect->getRectBounds();
  //nodes connected clockwise
  LineSegment top = LineSegment(bounds.minX, bounds.maxY, bounds.maxX, bounds.maxY); //top left -> top right
  LineSegment right = LineSegment(bounds.maxX, bounds.maxY, bounds.maxX, bounds.minY); //top right -> bottom right
  LineSegment bottom = LineSegment(bounds.maxX, bounds.minY, bounds.minX, bounds.minY); //bottom right -> bottom left
  LineSegment left = LineSegment(bounds.minX, bounds.minY, bounds.minX, bounds.maxY); //bottom left -> top left

  auto edges = { top, left, bottom, right };

  glm::vec2 point;
  glm::vec2 resultPt;
  LineSegment* edgeHit = nullptr;
  float closestDist = 0.0f;
  bool found = false;
  for (auto edge : edges)
  {
    //if intersecting the edge of rect
    if (checkLine(edge, other, point) == INTERESECTING)
    {
      //first intersection found
      if (!found)
      {
        resultPt = point;
        closestDist = glm::distance(glm::vec2(other.x1, other.y1), glm::vec2(point.x, point.y));
        found = true;
        edgeHit = &edge;
      }
      else
      {
        //uses closest intersection point
        float dist = glm::distance(glm::vec2(other.x1, other.y1), glm::vec2(point.x, point.y));
        if (dist < closestDist)
        {
          closestDist = dist;
          resultPt = point;
          edgeHit = &edge;
        }
      }
    }
  }


  if (found)
  {
    hit.distance = closestDist;
    hit.point = resultPt;
    hit.normal = glm::normalize(edgeHit->getNormal());
  }

  return found;
}

//checks line to circle collision and  returns true if hit, outputs result to hit
bool checkCircle(CollisionCircle* circle, LineSegment& other, RaycastHit2D& hit, const float maxDistance)
{
  //direction of line
  glm::vec2 dir = glm::vec2(other.x2 - other.x1, other.y2 - other.y1);

  //f= Vector from center sphere to ray origin 
  glm::vec3 circleOrigin = circle->origin();
  glm::vec2 f = { other.x1 - circleOrigin.x, other.y1 - circleOrigin.y };

  float a = glm::dot(dir, dir);
  float b = 2.0f * glm::dot(f, dir); 
  float t;

  //sqrd dist from ray origin to center minus radius sqrd
  float c = glm::dot(f, f) - circle->getRadius() * circle->getRadius();

  //b^2 - 4ac : Quadratic Formula
  float discriminant = b * b - 4.0f * a * c; 

  //0 intersections
  if (discriminant < 0.0f)
  {
    return false;
  }

  //1 intersection
  else if (discriminant < 0.0000001f)
  {
    t = -b / (2.0f * a);
    hit.distance = glm::length(glm::vec2(t*dir.x, t* dir.y));
    hit.point = glm::vec2(other.x1 + t * dir.x,  other.y1 + t * dir.y);

    if (!other.isPointOnLine(hit.point, hit.distance))
    {
      return false;
    }
    

    return true;
  }
  //2 intersections
  else
  {
    float sqrtDiscrim = std::sqrt(discriminant);

    //point A
    t = (-b + sqrtDiscrim) / (2.0f * a);
    glm::vec2 ptA = glm::vec2(other.x1 + t * dir.x, other.y1 + t * dir.y);
    float ptADist = glm::length(glm::vec2(t * dir.x, t*dir.y));

    //point B
    t = static_cast<float>((-b - sqrtDiscrim) / (2.0f * a));
    glm::vec2 ptB = glm::vec2(other.x1 + t * dir.x, other.y1 + t * dir.y);
    float ptBDist = glm::length(glm::vec2(t * dir.x, t * dir.y));

 
    //set raycast hit
    if (ptADist < ptBDist)
    {
      if (!other.isPointOnLine(ptA, ptADist))
      {
        return false;
      }

      hit.distance = ptADist;
      hit.point = ptA;
      glm::vec2 normal = glm::vec2(ptA.x - circleOrigin.x, ptA.y - circleOrigin.y);
      hit.normal = glm::normalize(normal);
    }
    else
    {
      if (!other.isPointOnLine(ptB, ptBDist))
      {
        return false;
      }

      hit.distance = ptBDist;
      hit.point = ptB;
      glm::vec2 normal = glm::vec2(ptB.x - circleOrigin.x, ptB.y - circleOrigin.y);
      hit.normal = glm::normalize(normal);
    }


    return true;
  }
}

//does a raycast
bool Physics::Raycast2D(const glm::vec2& origin, glm::vec2 direction, RaycastHit2D& hit, float distance, uint layers)
{
  direction = glm::normalize(direction);
  glm::vec2 end = origin + (direction * distance);

  LineSegment line = LineSegment(origin.x, origin.y, end.x, end.y);
  bool found = false;
  RaycastHit2D outHit; //output of a intersection hit

  std::vector<Entity*> entitys = EntityManager::getAllEntitys();
  for (Entity* ent : entitys)
  {
    //bitwise check, if entity collision layer is in layers
    if ((ent->getCollisionLayer() & layers) > 0)
    {
      std::vector<CollisionShape*> shapes = ent->getCollisionShapes();

      for (CollisionShape* shape : shapes)
      {
        switch (shape->getType())
        {
          //rect
        case CollisionShape::Type::RECT:
          if (checkRect(static_cast<CollisionRect*>(shape), line, outHit, distance))
          {
            //first intersection
            if (!found)
            {
              hit = outHit;
              hit.entityHit = ent;
              found = true;
            }
            //use closest intersection
            else if (hit.distance > outHit.distance)
            {
              hit = outHit;
              hit.entityHit = ent;
            }
          }
          break;
          //circle
        case CollisionShape::Type::CIRCLE:
          if (checkCircle(static_cast<CollisionCircle*>(shape), line, outHit, distance))
          {
            if (!found)
            {
              hit = outHit;
              hit.entityHit = ent;
              found = true;
            }
            else if (hit.distance > outHit.distance)
            {
              hit = outHit;
              hit.entityHit = ent;
            }
          }
          break;
        }
      }
    }
  }

  return found;
}

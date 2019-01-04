#ifndef ENTITY_H
#define ENTITY_H

#include <vector>
#include <oak/core/types.h>
#include "id_generator.h"

#include <map>
#include <glm/glm.hpp>
#include <string>
#include <queue>
#include <oak/collision/collision_layer.h>
#include <oak/components/base_rigid_body.h>

#include <unordered_map>

namespace oak
{
  class Script;
  class Component;
  struct IDGenerator;
  class BaseCollisionShape;


  ///<summary>An Object in the world</summary>
  class Entity
  {
    friend class Oakitus;
    friend struct EntityManager;
    friend class Collision;

	  uint entityID; ///<summary>Unique ID of this Entity</summary>

    std::vector<Component*> componentGroups[TICK_GROUP_MAX];

    std::vector<BaseCollisionShape*> collisionShapes; ///<summary>All of the CollisionShapes added to this Entity</summary>
	  IDGenerator componentIDGen; ///<summary>ID generator for components that are added to this Entity</summary>
    
    

    public:
      friend class Scene;

      glm::vec3 position; ///<summary>World position of this Entity</summary>
      glm::vec3 rotation;
      int layerID; ///<summary>Drawing layerID</summary>
      bool isGlobal; ///<summary>If true this Entity won't be destroyed at the end of a Scene</summary>
      std::string name; ///<summary>Name of this Entity</summary>
      

	    Entity(bool isEverRendered = true);
	    virtual ~Entity();
      
      ///<summary>Adds a Component to this Entity</summary>
	    void addComponent(Component* component);

      ///<summary>Adds a CollisionShape to this Entity</summary>
      void addCollision(BaseCollisionShape* shape); 

      void addRigidBody(BaseRigidBody* rigidBody);

      ///<summary>Adds this Entity to the world at zero zero</summary>
      void create();

      ///<summary>Adds this Entity to the world at the given position</summary>
      void create(float x, float y);

      ///<summary>Destroys this Entity</summary>
	    void destroy();

      ///<summary>Returns the Entity ID</summary>
	    uint getID() const;

      ///<summary>Returns the CollisionLayer of this Entity</summary>
      CollisionLayer getCollisionLayer() const;

      ///<summary>Returns the name of this Entity</summary>
      std::string getName() const;

      bool getIsTickingEnabled() const;
      void setIsTickingEnabled(bool isEnabled);

      void setIsVisible(bool isVisible);
      bool getIsRenderable() const;

      bool getCanTickWhenPaused() const;

      bool canTickThisFrame() const;

      //find component by type
      template <typename T>
      void getComponents(std::vector<T*>& out)
      {
        T* casted;
         for (uchar i = 0; i < TICK_GROUP_MAX; i++)
         {
           for (Component* comp : componentGroups[i])
           {
             casted = dynamic_cast<T*>(comp);
             if(casted != nullptr)
             {
               out.push_back(casted);
             }
           }
         }
      }

      template <typename T>
      T* getComponent()
      {
        T* casted;
        for (uchar i = 0; i < TICK_GROUP_MAX; i++)
        {
          for (Component* comp : componentGroups[i])
          {
            casted = dynamic_cast<T*>(comp);
            if (casted != nullptr)
            {
              return casted;
            }
          }
        }
        return nullptr;
      }

    protected:
      ///<summary>Catagory of this Entity in the collision system</summary> 
      CollisionLayer collisionLayer;
      BaseRigidBody* rigidBody = nullptr;

      //EVENTS
      //-------------------------------------------------------------
      ///<summary>Called once when this Entity is added to the world</summary>
      void onCreate();

      ///<summary>Called once each frame</summary>
      void onTick(const uchar TICK_GROUP);

      ///<summary>Draws all renderable components each frame</summary>
      void onRender() const;

      void onDebugDraw() const;

      ///<summary>Called once just before this Entity is removed from the world and deallocated</summary>
	    void onDestroy();

      ///<summary>Called when a collision occured</summary>
      void onCollisionHit(Entity& hit);
      //-------------------------------------------------------------

      bool canTickWhenPaused = false;

    private:
       bool isEverRendered;
       bool isRenderable;
       bool isTickingEnable = true;
  };
}

#endif
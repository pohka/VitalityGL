#ifndef LUA_COLLISION_CIRCLE_H
#define LUA_COLLISION_CIRCLE_H

#include <lua/lua.hpp>
#include <oak/collision/collision_circle.h>


namespace oak
{
  class LuaCollisionCircle
  {
    CollisionCircle* ptr;

  public:
    LuaCollisionCircle(CollisionCircle* cricle);
    ~LuaCollisionCircle();

    static void reg(lua_State* L);

    static int lua_delete(lua_State* L);
    static int getRadius(lua_State* L);
    static int setRadius(lua_State* L);
    static int getOffset(lua_State* L);
    static int setOffset(lua_State* L);
    static int getType(lua_State* L);
  };
}

#endif
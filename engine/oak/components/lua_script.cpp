#include <oak/components/lua_script.h>
#include <lua/lua.hpp>
#include <oak/lua/lua_scene.h>
#include <iostream>
#include <oak/lua/lua_input.h>
#include <oak/scene/scene_manager.h>
#include <oak/lua/luah_entity.h>
#include <oak/debug.h>
#include <oak/lua/lua_constants.h>
#include <oak/lua/lua_s.h>

using namespace oak;

const std::string LuaScript::PATH = "../Release/scripts/";

LuaScript::LuaScript(const std::string& name) : Component(REFLECT_LUA_SCRIPT), 
  name(name), 
  scriptFilePath(LuaScript::PATH + name + ".lua")
{
  LuaS::loadFile(scriptFilePath);
  LuaS::doFile(scriptFilePath);
  
  //automatically add the onTick function as a thinker
  lua_getglobal(LuaS::state, name.c_str());
  if (!lua_isnil(LuaS::state, -1) && lua_istable(LuaS::state, -1))
  {
    lua_getfield(LuaS::state, -1, LUA_ON_TICK);
    if (!lua_isnil(LuaS::state, -1) && lua_isfunction(LuaS::state, -1))
    {
      thinkers.push_back({ LUA_ON_TICK, LUA_ON_TICK, -0.0001f });
    }
  }
}

void LuaScript::onCreate()
{
  if (getFunc(LUA_ON_CREATE))
  {
    LuaS::setThisEntity(this->entity);
    LuaS::setThisScript(this);
    lua_getglobal(LuaS::state, name.c_str());
    lua_getfield(LuaS::state, -1, LUA_ON_CREATE);

    if (lua_pcall(LuaS::state, 0, 0, 0) != 0)
    {
      std::cout << "create function error" << std::endl;
    }
  }
}

void LuaScript::onDestroy()
{
  if (getFunc(LUA_ON_DESTROY))
  {
    LuaS::setThisEntity(this->entity);
    LuaS::setThisScript(this);
    lua_getglobal(LuaS::state, name.c_str());
    lua_getfield(LuaS::state, -1, LUA_ON_DESTROY);

    if (lua_pcall(LuaS::state, 0, 0, 0) != 0)
    {
      std::cout << "no destroy function" << std::endl;
    }
  }
}

LuaScript::~LuaScript()
{

}


bool LuaScript::getFunc(const char* funcName)
{
  LuaS::doFile(scriptFilePath);
  lua_getglobal(LuaS::state, name.c_str());

  //returns true if lua file has a matching function
  if (!lua_isnil(LuaS::state, -1) && lua_istable(LuaS::state, -1))
  {
    lua_getfield(LuaS::state, -1, funcName);
    if (!lua_isnil(LuaS::state, -1) && lua_isfunction(LuaS::state, -1))
    {
      return true;
    }
  }

  return false;
}

void LuaScript::onTick()
{
  if (thinkers.empty())
  {
    return;
  }

  LuaS::doFile(scriptFilePath);
  LuaS::setThisEntity(this->entity);
  LuaS::setThisScript(this);

  //manage thinkers
  for (unsigned int i=0; i<thinkers.size(); i++)
  {
    if (thinkers[i].nextTickTime <= Time::getGameTime())
    {
      lua_getglobal(LuaS::state, name.c_str());
      const int top = lua_gettop(LuaS::state);
      lua_getfield(LuaS::state, -1, thinkers[i].funcName);
      LuaS::call(1);

      if (lua_isnumber(LuaS::state, -1))
      {
        float interval = (float)lua_tonumber(LuaS::state, -1);
        //ending interval
        if (interval < 0.0)
        {
          thinkers.erase(thinkers.begin() + i);
          i--;
        }
        //next interval time
        else
        {
          thinkers[i].nextTickTime += interval;
        }
      }
      lua_settop(LuaS::state, top - 1);
    }
  }
}

//set a lua function to think, note: the thinkerName should be unique for this LuaScript instance
void LuaScript::setThink(const char* thinkerName, const char* funcName, float initialDelay)
{
  bool found = false;
  for (auto thinker : thinkers)
  {
    if (strcmp(thinker.funcName, thinkerName) == 0)
    {
      found = true;
      break;
    }
  }

  if (!found)
  {
    thinkers.push_back({ thinkerName, funcName, initialDelay });
  }
  else
  {
    LOG_WARNING << "a thinker of this name has already been set: '" << thinkerName << "'";
  }
}


const std::string& LuaScript::getName() const
{
  return name;
}
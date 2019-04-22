#ifndef ABILITY_H
#define ABILITY_H

#include <string>
#include <vector>

namespace oak
{
  class Unit;

  class Ability
  {
    

    //for later
    //abilityBehaviour i.e. no_target, point, aura, etc
    //unsigned char targetTeam;
    //unsigned char targetType;
    //unsigned char damageType;
    //float cooldown
    //int manaCost
    //castRange
    //channelTime
    //AoERadius

  public:
    Ability(const std::string& name);
    ~Ability();

    float getPreCastTime() const;

    virtual void onSpellCast() = 0;
    virtual void onSpellStart() = 0;

    const std::string& getName() const;
    unsigned int getAbilityID() const;
    float getPreCastTime();
    //for later
    //virtual void onDeath() = 0;
    //virtual void onCreate() = 0;
    //virtual void onDestroy() = 0;
    //virtual void onKill() = 0;
    //virtual void onTakeDamage() = 0;
    //virtual void onDealDamage() = 0;

    void setOwner(Unit* unit);

    int getManaCost() const;
    float getCooldown() const;

    void levelUp();
    unsigned int getMaxLevel();
    unsigned int getLevel();

  protected:
    unsigned int maxLevel = 1;
    const std::string name;
    unsigned int abilityID = 0;
    float preCastTime = 0.0f;
    std::vector<float> cooldown;
    std::vector<int> manaCost;
    Unit* owner = nullptr;
    unsigned int currentLevel = 1;
  };
}

#endif
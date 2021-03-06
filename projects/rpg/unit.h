#ifndef UNIT_H
#define UNIT_H

#include <oak/core/entity.h>
#include <oak/components/animator.h>
#include "game_def.h"
#include <oak/core/base_player.h>
#include <oak/event/event_manager.h>
#include "events/e_damage_taken.h"
#include "events/e_death.h"
#include "inventory.h"
#include "systems/modifier.h"

class oak::Entity;

namespace game
{
  class Player;
  class Ability;

  class Unit : public oak::Entity, public DamageTakenListener, public DeathListener
  {
    uint ownerID;
    bool m_hasOwner = false;
    
    //stats
    float health = 100.0f;
    float mana = 100.0f;
    int moveSpeed = 100;
    int damage = 10;
    int resist[ELEMENT_COUNT] = {};
    int amplify[ELEMENT_COUNT] = {};
    float healthRegen = 0;
    float manaRegen = 0;
    float maxHealth = 100.0f;
    float maxMana = 100.0f;
    
    oak::Animator* animator;
    bool m_isOwnerBotPlayer = false;
    
    //modifiers currently applied to this unit
    std::vector<Modifier*> modifiers;

    public:
      Unit();
      virtual ~Unit();

      oak::BasePlayer* getOwner() const;
      bool hasOwner() const;
      void setOwner(uint playerID);
      void removeOwner();
      int getMoveSpeed() const;
      void setMoveSpeed(int moveSpeed);
      void addAbility(Ability* ability);
      Ability* getAbilityByIndex(uint index) const;
      uchar getFaction() const;
      float  getHealth() const;
      void setHealth(float hp);
      bool isAlive() const;
      bool isOwnerBotPlayer() const;
      
      void addAnimator(oak::Animator* animator);
      void setAnimation(uchar animType);
      uchar getAnimDirection() const;
      void setAnimDirection(uchar direction);
      int getResist(uchar element);
      int getAmplify(uchar element);
      float getMana();
      void useMana(int amount);
      float getMaxHealth();
      float getMaxMana();
      void setManaRegen(float manaPerSecond);
      void setHealthRegen(float healthPerSecond);
      float getManaRegen();
      float getHealthRegen();
      void heal(int amount);
      Inventory& getInventory();
      

      void addModifier(uint casterID, Modifier* modifier);

      //if this unit has a matching modifier, removes it note:does not call onDestroy()
      void removeModifier(ushort modifierID, uint casterID); 

      //finds all modifiers with matching modifierID, and push them into mods vector
      void findModifiersByID(snum modifierID, std::vector<Modifier*>& mods);

      //finds all modifiers with matching modifierID and casterID, and push them into mods vector
      void findModifiersByID(snum modifierID, uint casterID, std::vector<Modifier*>& mods);

      //finds all modifiers with matching element type, and push them into mods vector
      void findModifiersByElement(cnum elementType, std::vector<Modifier*>& mods);

      std::vector<Modifier*>& getAllModifiers();

      void onCreate() override;
      //void onDestroy() override;
      //void onDraw() override;
      void onUpdate();

      static Unit* findUnit(uint unitID);

    protected:
      uchar faction;
      
      void onDamageTaken(DamageData& data) override;
      void onDeath(DeathData& data) override;

      Inventory inventory = Inventory(getID());
      std::vector<Ability*> abilitys;

    private:
      
      
  };

  
}

#endif
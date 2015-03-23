#include "main.hpp"

Destructible::Destructible(float maxHp, float defense, const char *corpseName) : maxHp(maxHp), hp(maxHp), defense(defense), corpseName(corpseName) {}

float Destructible::takeDamage(Actor *owner, float damage) {
  damage -= defense;
  if(damage > 0) {
    hp -= damage;
    if(hp <= 0)
      die(owner);
  } else {
    damage = 0;
  }
  return damage;
}

float Destructible::heal(float amount) {
  hp += amount;
  if(hp > maxHp) {
    amount -= hp - maxHp;
    hp = maxHp;
  }
  
  return amount;
}

void Destructible::die(Actor *owner) {
  owner->ch = '%';
  owner->col = owner->col * TCODColor::darkGrey;
  owner->name = corpseName;
  owner->blocks=false;
  engine.sendToBack(owner);
}

MonsterDestructible::MonsterDestructible(float maxHp, float defense, const char *corpseName) : Destructible(maxHp, defense, corpseName) {}
void MonsterDestructible::die(Actor *owner) {
  engine.gui->message(TCODColor::red, "Saracul %s e mort.", owner->name);
  Destructible::die(owner);
}

PlayerDestructible::PlayerDestructible(float maxHp, float defense, const char *corpseName) : Destructible(maxHp, defense, corpseName) {}
void PlayerDestructible::die(Actor *owner) {
  engine.gui->message(TCODColor::lightRed, "Ai murit.");
  engine.gameStatus = Engine::DEFEAT;
}

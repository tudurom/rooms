#include <stdio.h>
#include "main.hpp"

Attacker::Attacker(float power) : power(power) {}

void Attacker::attack(Actor *owner, Actor *target) {
  if(target->destructible && !target->destructible->isDead()) {
    if(power - target->destructible->defense > 0) {
      engine.gui->message(owner == engine.player ? TCODColor::red : owner->col * 1.5, "%s ataca pe %s pentru %g puncte.", owner->name, target->name, power - target->destructible->defense);
    } else {
      engine.gui->message(TCODColor::lightGrey, "%s ataca pe %s dar nu are nici un efect...", owner->name, target->name);
    }
    target->destructible->takeDamage(target, power);
  } else {
    engine.gui->message(TCODColor::lightGreen, "%s ataca pe %s degeaba...", owner->name, target->name);
  }
}

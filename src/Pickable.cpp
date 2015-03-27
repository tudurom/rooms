#include "main.hpp"

bool Pickable::pick(Actor *owner, Actor *wearer) {
  if(wearer->container && wearer->container->add(owner)) {
    engine.actors.remove(owner);
    return true;
  }
  return false;
}

bool Pickable::use(Actor *owner, Actor *wearer) {
  if(wearer->container) {
    wearer->container->remove(owner);
    delete owner;
    return true;
  }
  return false;
}

Healer::Healer(float amount) : amount(amount) {}

bool Healer::use(Actor *owner, Actor *wearer) {
  if(wearer->destructible) {
    float amountHealed = wearer->destructible->heal(amount);
    if(amountHealed > 0) {
      return Pickable::use(owner, wearer);
    }
  }
  return false;
}

LightingBolt::LightingBolt(float range, float damage) : range(range), damage(damage) {}

bool LightingBolt::use(Actor *owner, Actor *wearer) {
  Actor *closestMonster = engine.getClosestMonster(wearer->x, wearer->y, range);
  if(!closestMonster) {
    engine.gui->message(TCODColor::lightGrey, "Nici o persoana primejdioasa nu este suficient de aproape");
    return false;
  }
  engine.gui->message(TCODColor::cyan, "Un fulger provenit dintr-un dispozitiv carpit\n nimereste pe saracul %s!\n"
                                       "I-a luat %g hp.", closestMonster->name, damage);
  closestMonster->destructible->takeDamage(closestMonster, damage);
  
  return Pickable::use(owner, wearer);
};

Fireball::Fireball(float range, float damage) : LightingBolt(range, damage) {}

bool Fireball::use(Actor *owner, Actor *wearer) {
  engine.gui->message(TCODColor::lightCyan, "Da click pe un inamic pt. a trage in el.");
  int x, y;
  if(!engine.pickAtTile(&x, &y)) 
    return false;
  engine.gui->message(TCODColor::orange, "Biluta in flacari explodeaza, arzand mai tot ce ii sta in cale.");
  for(Actor **iterator = engine.actors.begin(); iterator != engine.actors.end(); iterator++) {
    Actor *actor = *iterator;
    if(actor->destructible && !actor->destructible->isDead() && actor->getDistance(x, y) <= range && actor != wearer) {
      engine.gui->message(TCODColor::orange, "\"%s\" arde pentru %g hp", actor->name, damage);
      actor->destructible->takeDamage(actor, damage);
    }
  }
  return Pickable::use(owner, wearer);
}

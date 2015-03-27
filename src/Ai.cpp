#include <cstdio>
#include <math.h>
#include "main.hpp"

void PlayerAi::update(Actor *owner) {
  if(owner->destructible && owner->destructible->isDead())
    return;
    
  int nextX = 0, nextY = 0;
  switch(engine.lastKey.vk) {
    case TCODK_UP: nextY = -1;break;
    case TCODK_DOWN: nextY = 1; break;
    case TCODK_LEFT: nextX = -1; break;
    case TCODK_RIGHT: nextX = 1; break;
    case TCODK_CHAR: handleActionKey(owner, engine.lastKey.c); break;
    default: 
      break;
  }
  if(nextX != 0 || nextY != 0) {
    engine.gameStatus = engine.NEW_TURN;
    if(moveOrAttack(owner, owner-> x+ nextX, owner->y + nextY))
      engine.map ->computeFov();
  }
}

bool PlayerAi::moveOrAttack(Actor *owner, int targetx, int targety) {
  if(engine.map->isWall(targetx, targety))
    return false;
    
  // Ataca
  for(Actor **actor = engine.actors.begin(); actor != engine.actors.end(); actor++) {
    Actor *act = *actor;
    
    if(act->destructible && !act->destructible->isDead() && act->x == targetx && act->y == targety) {
      owner->attacker->attack(owner, act);
      return false;
    }
  }
  // Cauta cadavre sau obiecte
  for(Actor **actor = engine.actors.begin(); actor != engine.actors.end(); actor++) {
    Actor *act = *actor;
    
    bool corpseOrItem = (act->destructible && act->destructible->isDead()) || act->pickable;
    if(corpseOrItem && act->x == targetx && act->y == targety)
      engine.gui->message(act->col, "E un %s aici.", act->name);
  }
  owner->x = targetx;
  owner->y = targety;
  return true;
}

void PlayerAi::handleActionKey(Actor *owner, int ascii) {
  switch(ascii) {
    case 'g': {
      bool found = false;
      for(Actor **iterator = engine.actors.begin(); iterator != engine.actors.end(); iterator++) {
        Actor *actor = *iterator;
        if(actor->pickable && actor->x == owner->x && actor->y == owner->y) {
          if(actor->pickable->pick(actor, owner)) {
            found = true;
            engine.gui->message(actor->col * 1.5f, "Ai luat: %s.", actor->name);
            break;
          } else if(!found) {
            found = true;
            engine.gui->message(TCODColor::orange, "Ai buzunarele pline.");
          }
        }
      }
      if(!found)
        engine.gui->message(TCODColor::lightGrey, "Nu ai ce sa iei de-aici.\nDoar gunoaie peste gunoaie.");
      engine.gameStatus = Engine::NEW_TURN;
    }
    break;
    case 'i': {
      Actor *actor = chooseFromInventory(owner);
      if(actor) {
        actor->pickable->use(actor, owner);
        engine.gameStatus = Engine::NEW_TURN;
      }
    }
    break;
  }
}

Actor* PlayerAi::chooseFromInventory(Actor *owner) {
  static TCODConsole con(INVENTORY_WIDTH, INVENTORY_HEIGHT);
  
  con.setDefaultForeground(TCODColor(200, 180, 50));
  con.printFrame(0, 0, INVENTORY_WIDTH, INVENTORY_HEIGHT, true, TCOD_BKGND_DEFAULT, "inventory");
  
  con.setDefaultForeground(TCODColor::white);
  int shortcut = 'a';
  int y = 1;
  for(Actor **iterator = owner->container->inventory.begin(); iterator != owner->container->inventory.end(); iterator++) {
    Actor *item = *iterator;
    con.print(2, y, "(%c) %s", shortcut, item->name);
    y++;
    shortcut++;
  }
  TCODConsole::blit(&con, 0, 0, INVENTORY_WIDTH, INVENTORY_HEIGHT, 
                    TCODConsole::root, (ROOT_WINDOW_WIDTH - INVENTORY_WIDTH) / 2,
                    (ROOT_WINDOW_HEIGHT - INVENTORY_HEIGHT) / 2);
  TCODConsole::flush();
  
  TCOD_key_t key;
  TCODSystem::waitForEvent(TCOD_EVENT_KEY_PRESS, &key, NULL, true);
  if(key.vk == TCODK_CHAR) {
    int index = key.c - 'a';
    if(index >= 0 && index < owner->container->inventory.size())
      return owner->container->inventory.get(index);
  }
  return NULL;
}

void MonsterAi::update(Actor *owner) {
  if(owner->destructible && owner->destructible->isDead())
    return;
  if(engine.map->isInFov(owner->x, owner->y)) {
    moveCount = MONSTER_TRACKING_TURNS;
  } else {
    moveCount--;
  }
  if(moveCount > 0)
    moveOrAttack(owner, engine.player->x, engine.player->y); // Se duce sa atace jucatorul doar daca jucatorul il vede
}

void MonsterAi::moveOrAttack(Actor *owner, int targetx, int targety) {
  int nextx = targetx - owner->x;
  int nexty = targety - owner->y;
  int stepdx = (nextx > 0 ? 1 : -1);
  int stepdy = (nexty > 0 ? 1 : -1);
  float distance = sqrtf(nextx * nextx + nexty * nexty);
  
  if(distance >= 2) {
    nextx = (int)(round(nextx / distance));
    nexty = (int)(round(nexty / distance));
    
    if(engine.map->canWalk(owner->x + nextx, owner->y + nexty)) {
      owner->x += nextx;
      owner->y += nexty;
    } else if (engine.map->canWalk(owner->x+stepdx, owner->y)) {
      owner->x += stepdx;
    } else if (engine.map->canWalk(owner->x, owner->y + stepdy)) {
      owner->y += stepdy;
    }
  } else if(owner->attacker) {
    owner->attacker->attack(owner, engine.player);
  }
}

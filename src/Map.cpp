#include "main.hpp"
#include <cstdio>

class BspListener : public ITCODBspCallback {
private:
  Map &map;
  int roomNum;
  int lastx, lasty;
public:
  BspListener(Map &map) : map(map), roomNum(0) {}
  
  bool visitNode(TCODBsp *node, void *userData) {
    if(node->isLeaf()) {
      int x, y, w, h;
      
      TCODRandom *rng = TCODRandom::getInstance();
      w = rng->getInt(ROOM_MIN_SIZE, node->w - 2);
      h = rng->getInt(ROOM_MIN_SIZE, node->h - 2);
      x = rng->getInt(node->x + 1, node->x + node->w - w - 1);
      y = rng->getInt(node->y + 1, node->y + node->h - h - 1);
      map.createRoom(roomNum == 0, x, y, x + w - 1, y + h - 1);
      if(roomNum != 0) {
          map.dig(lastx, lasty, x + w / 2, lasty);
          map.dig(x + w / 2, lasty, x + w / 2, y + h / 2);
      }
      lastx = x + w / 2;
      lasty = y + h / 2;
      roomNum++;
    }
    return true;
  }
};

Map::Map(int width, int height) : width(width), height(height) {
  tiles = new Tile[width * height];
  map = new TCODMap(width, height);
  TCODBsp bsp(0, 0, width, height);
  bsp.splitRecursive(NULL, 8, ROOM_MAX_SIZE, ROOM_MAX_SIZE, 1.5f, 1.5f);
  BspListener listener(*this);
  bsp.traverseInvertedLevelOrder(&listener, NULL);
}

Map::~Map() {
  delete [] tiles;
  delete map;
}

bool Map::isWall(int x, int y) const {
  return !map->isWalkable(x, y);
}

bool Map::canWalk(int x, int y) const {
  if(isWall(x, y))
    return false;
  for(Actor **actor = engine.actors.begin(); actor != engine.actors.end(); actor++) {
    Actor *act = *actor;
    if(act->blocks && act->x == x && act->y == y)
      return false;
  }
  return true;
}

bool Map::isExplored(int x, int y) const {
  return (*tileAt(x, y)).explored;
}

bool Map::isInFov(int x, int y) const {
  if(map->isInFov(x, y)) {
    (*tileAt(x, y)).explored = true;
    return true;
  }
  return false;
}

void Map::computeFov() {
  map->computeFov(engine.player->x, engine.player->y, engine.fovRadius);
}

void Map::dig(int x1, int y1, int x2, int y2) {
  if(x2 < x1) {
    int aux = x2;
    x2 = x1;
    x1 = aux;
  }
  if(y2 < y1) {
    int aux = y2;
    y2 = y1;
    y1 = aux;
  }
  for(int x = x1; x <= x2; x++)
    for(int y = y1; y <= y2; y++)
      map->setProperties(x, y, true, true);
}

void Map::createRoom(bool first, int x1, int y1, int x2, int y2) {
    dig(x1, y1, x2, y2);
    if(first) {
      engine.player->x = (x1 + x2) / 2;
      engine.player->y = (y1 + y2) / 2;
    } else {
      // pune monstrii
      TCODRandom *rng = TCODRandom::getInstance();
      int nrMonsters = rng->getInt(0, MAX_ROOM_MONSTERS);
      while(nrMonsters > 0) {
        int x = rng->getInt(x1, x2);
        int y = rng->getInt(y1, y2);
        if(canWalk(x, y)) {
          addMonster(x, y);
        }
        nrMonsters--;
      }
      // pune obiecte
      int nrItems = rng->getInt(0, MAX_ROOM_ITEMS);
      while(nrItems) {
        int x = rng->getInt(x1, x2);
        int y = rng->getInt(y1, y2);
        if(canWalk(x ,y))
          addItem(x, y);
        nrItems--;
      }
    }
}

void Map::addMonster(int x, int y) {
  TCODRandom *rng = TCODRandom::getInstance();
  if(rng->getInt(0, 100) <= 60) {
    // Pune un paznic
    Actor *paznic = new Actor(x, y, 'p', "paznic", TCODColor::azure);
    paznic->destructible = new MonsterDestructible(10, 0, "paznic lesinat");
    paznic->attacker = new Attacker(3);
    paznic->ai = new MonsterAi();
    engine.actors.push(paznic);
  } else if(rng->getInt(0, 100) > 60 && rng->getInt(0, 100) <= 95) {
    // Pune pe doamna directoare
    Actor *director = new Actor(x, y, 'D', "directoare", TCODColor::crimson);
    director->destructible = new MonsterDestructible(16, 1, "directoare lesinata");
    director->attacker = new Attacker(4);
    director->ai = new MonsterAi();
    engine.actors.push(director);
  } else if(rng->getInt(0, 100) > 95) {
    // Pune un floricel
    Actor *floricel = new Actor(x, y, 'F', "floricel", TCODColor::chartreuse);
    floricel->destructible = new MonsterDestructible(50, 2, "floricel ofilit");
    floricel->attacker = new Attacker(5);
    floricel->ai = new MonsterAi();
    engine.actors.push(floricel);
  }
}

void Map::render() const {
  static const TCODColor darkWall(0, 0, 100);
  static const TCODColor darkGround(50, 50, 150);
  static const TCODColor lightWall(130, 110, 50);
  static const TCODColor lightGround(200, 180, 50);
  static const TCODColor voidBackground(0, 0, 64);
  for(int x = 0; x < width; x++) {
    for(int y = 0; y < height; y++){
      if(isInFov(x, y)) {
        TCODConsole::root->setCharBackground(x, y, isWall(x, y) ? lightWall : lightGround);
      } else if(isExplored(x, y)) {
        TCODConsole::root->setCharBackground(x, y, isWall(x, y) ? darkWall : darkGround);  
      } else {
        TCODConsole::root->setCharBackground(x, y, voidBackground);
      }
    }
  }
}

void Map::addItem(int x, int y) {
  TCODRandom *rng = TCODRandom::getInstance();
  int rnd = rng->getInt(0, 100);
  if(rnd < 70) {
    Actor *healthPotion = new Actor(x, y, '!', "bautura energizanta", TCODColor::violet);
    healthPotion->blocks = false;
    healthPotion->pickable = new Healer(4);
    engine.sendToBack(healthPotion);
  } else if(rnd < 70 + 20) {
    Actor *scrollOfFireball = new Actor(x, y, '#', "foaie intitulata \"cum sa faci focul\"", TCODColor::lightOrange);
    scrollOfFireball->blocks = false;
    scrollOfFireball->pickable = new Fireball(3, 12);
    engine.sendToBack(scrollOfFireball);
  } else {
    Actor *scrollOfLightingBolt = new Actor(x, y, '#', "foaie intitulata \"cum sa faci o bobina tesla\"", TCODColor::lightYellow);
    scrollOfLightingBolt->blocks = false;
    scrollOfLightingBolt->pickable = new LightingBolt(5, 20);
    engine.sendToBack(scrollOfLightingBolt);
  }
}

Tile* Map::tileAt(int x, int y) const{
  return &tiles[x + y * width];
}


#include "main.hpp"

Engine::Engine(int width, int height) : gameStatus(STARTUP), fovRadius(FOV_RADIUS), width(width), height(height) {
  TCODConsole::initRoot(width, height, ROOT_WINDOW_TITLE, false);
  if(RELEASE) TCODConsole::root->credits();
  createPlayer();
  map = new Map(MAP_WIDTH, MAP_HEIGHT);
  gui = new Gui();
  gui->message(TCODColor::lightGreen, "Salut, elev curios!\nFii gata sa fii exmatriculat cand vei fii descoperit!");
}

Engine::~Engine() {
  actors.clearAndDelete();
  delete map;
  delete gui;
}

void Engine::update() {
  if(gameStatus == STARTUP)
    map->computeFov();
  gameStatus = IDLE;
  TCODSystem::checkForEvent(TCOD_EVENT_KEY_PRESS, &lastKey, NULL);
  player->update();
  if(gameStatus == NEW_TURN) {
    for(Actor **actor = actors.begin(); actor != actors.end(); actor++) {
      Actor *act = *actor;
      if(act != player) {
        act->update();
      }
    }
  }
  player->render();
}
void Engine::render() {
  TCODConsole::root->flush();
  TCODConsole::root -> clear();
  map -> render();
  for(Actor **actor = actors.begin(); actor != actors.end(); actor++) {
    Actor *act = *actor;
    if(map->isInFov(act->x, act->y))
      (*actor) -> render();
  }
  gui->render();
}

void Engine::sendToBack(Actor *actor) {
  actors.remove(actor);
  actors.insertBefore(actor, 0);
}

Actor* Engine::createPlayer() {
  player = new Actor(MAP_WIDTH / 2, MAP_HEIGHT, '@', "player", TCODColor::white);
  player->destructible = new PlayerDestructible(30, 2, "cadavrul tau");
  player->attacker = new Attacker(3);
  player->ai = new PlayerAi();
  player->container = new Container(26);
  actors.push(player);
  
  return player;
}

#include <stdio.h>
#include <string.h>
#include <string>
#include <iostream>

#include "main.hpp"
#include "SDL/SDL_mixer.h"

Mix_Music *music = NULL;

Engine::Engine(int width, int height) : gameStatus(STARTUP), fovRadius(FOV_RADIUS), width(width), height(height) {
  char *c_playerName = new char[100];
  std::string playerName;
  std::cout << "Your name? :";
  std::cin >> playerName;
  std::cout << playerName;
  strcpy(c_playerName, playerName.c_str());
  TCODConsole::initRoot(width, height, ROOT_WINDOW_TITLE, false);
  if(RELEASE) TCODConsole::root->credits();
  initMusic();
  createPlayer(c_playerName);
  map = new Map(MAP_WIDTH, MAP_HEIGHT);
  gui = new Gui();
  gui->message(TCODColor::lightGreen, "Salut, elev curios!\nFii gata sa fii exmatriculat cand vei fii descoperit!");
}

Engine::~Engine() {
  actors.clearAndDelete();
  delete map;
  delete gui;
  freeMusic();
}

void Engine::update() {
  if(gameStatus == STARTUP)
    map->computeFov();
  if(gameStatus == DEFEAT) {
    // fadeing
    endGame();
  } else {
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

Actor* Engine::createPlayer(char playerName[]) {
  player = new Actor(MAP_WIDTH / 2, MAP_HEIGHT, 1, playerName, TCODColor::white);
  player->destructible = new PlayerDestructible(30, 2, "cadavrul tau");
  player->attacker = new Attacker(3);
  player->ai = new PlayerAi();
  player->container = new Container(26);
  actors.push(player);
  
  return player;
}

void Engine::endGame() {
  fadePrint("Ai fost prins...");
  fadePrint("Pacat...");
  exit(0);
}

bool Engine::initMusic() {
  if(Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, 2, 4096) == -1) {
    printf("Nu am putut sa initializez dispozitivul audio.\n");
    return false;
  }
  music = Mix_LoadMUS("data/music.ogg");
  if(music == NULL) {
    printf("Nu am putut sa incarc muzica\n");
    return false;
  }
  if(Mix_PlayingMusic() == 0) {
    if(Mix_PlayMusic(music, -1) == -1)
      return false;
  }
  return true;
}

bool Engine::freeMusic() {
  Mix_FreeMusic(music);
  
  return true;
}
void Engine::fadePrint(const char *msg) {
  TCODConsole::root->setDefaultBackground(TCODColor::black);
  for(int f = 255; f >= 0; f--) {
      TCODConsole::root->setFade(f, TCODColor::black);
      TCODSystem::sleepMilli(20);
      TCODConsole::root->flush();
  }
  TCODConsole::root->clear();
  TCODConsole::root->print((ROOT_WINDOW_WIDTH - strlen(msg)) / 2, ROOT_WINDOW_HEIGHT / 2, msg);
  for(int f = 0; f <= 255; f++) {
      TCODConsole::root->setFade(f, TCODColor::black);
      TCODSystem::sleepMilli(20);
      TCODConsole::root->flush();
  }
  TCODSystem::sleepMilli(1500);
}

bool Engine::pickAtTile(int *x, int *y, float maxRange) {
  while(!TCODConsole::isWindowClosed()) {
    render();
    
    for(int cx = 0; cx < map->width; cx++) {
      for(int cy = 0; cy < map->height; cy++) {
        if(map->isInFov(cx, cy) && 
           (maxRange == 0 || player->getDistance(cx, cy) <= maxRange)) {
          TCODColor col = TCODConsole::root->getCharBackground(cx, cy);
          col = col * 1.2f;
          TCODConsole::root->setCharBackground(cx, cy, col);
        }
      }
    }
    TCODSystem::checkForEvent(TCOD_EVENT_KEY_PRESS | TCOD_EVENT_MOUSE, &lastKey, &mouse);
    if(map->isInFov(mouse.cx, mouse.cy) 
       && (maxRange == 0 || player->getDistance(mouse.cx, mouse.cy) <= maxRange)) {
      TCODConsole::root->setCharBackground(mouse.cx, mouse.cy, TCODColor::white);
      if(mouse.lbutton_pressed) {
        *x = mouse.cx;
        *y = mouse.cy;
        return true;
      }
    }
    if(mouse.rbutton_pressed || lastKey.vk != TCODK_NONE) {
      return false;
    }
    TCODConsole::flush();
  }
  return false;
}
  
Actor* Engine::getClosestMonster(int x, int y, float range) const {
  Actor *closest = NULL;
  float distMin = 1E6f;
  
  for(Actor **iterator = actors.begin(); iterator != actors.end(); iterator++) {
    Actor *actor = *iterator;
    if(actor != player && actor->destructible && !actor->destructible->isDead()) {
      float dist = actor->getDistance(x, y);
      if(dist < distMin && (dist <= range || range == 0.0f)) {
        distMin = dist;
        closest = actor;
      }
    }
  }
  return closest;
}

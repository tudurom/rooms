#include "Gui.hpp"

class Engine {
public:
  enum GameStatus {
    STARTUP,
    IDLE,
    NEW_TURN,
    VICTORY,
    DEFEAT
  } gameStatus;

  TCODList<Actor *> actors;
  Actor *player;
  Map *map;
  int fovRadius;
  TCOD_key_t lastKey;
  int width, height;
  Gui *gui;
  
  Engine(int width, int height);
  ~Engine();
  void update();
  void render();
  void sendToBack(Actor *actor);

private:
  Actor* createPlayer();
};

extern Engine engine;


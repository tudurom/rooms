class Actor {
public:
  int x, y;
  int ch;
  TCODColor col;
  const char *name;
  bool blocks;
  Attacker *attacker;
  Destructible *destructible;
  Ai *ai;
  Pickable *pickable;
  Container *container;
  
  Actor(int x, int y, int ch, const char *name, const TCODColor &col);
  ~Actor();
  
  void update();
  void render() const;
};


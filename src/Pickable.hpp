class Pickable {
public:
  bool pick(Actor *owner, Actor *wearer);
  virtual bool use(Actor *owner, Actor *wearer);
};

class Healer : public Pickable {
public:
  float amount; // cata viata ii da actorului
  
  Healer(float amount);
  bool use(Actor *owner, Actor *wearer);
};

class LightingBolt : public Pickable {
public:
  float range, damage;
  LightingBolt(float range, float damage);
  
  bool use(Actor *owner, Actor *wearer);
};

class Fireball : public LightingBolt {
public:
  Fireball(float range, float damage);
  bool use(Actor *owner, Actor *wearer);
};

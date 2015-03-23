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

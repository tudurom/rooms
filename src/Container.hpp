class Container {
public:
  int size; // nr. maxim de locuri. 
  TCODList<Actor *> inventory;
  
  Container(int size);
  ~Container();
  
  bool add(Actor *actor);
  void remove(Actor *actor);
};

#include <iostream>
using namespace std;

//
// This is a gist that show cases compile time Type Erasure in C++.
// It show how riduclous it can be just to do something so simple.
//
// The idea is for the user to create multiple different objects
// and unite them under one base class WITHOUT requiring them to inherit
// that class.
// 
// The objects still need to implement the functions required within them, 
// of course. The main point is to avoid inheritance and virtual polymorphism.
//
// The implementation is quite simple: 
// - Define a Concept interface that all user objects need to adhere to.
//
// - Create a Model class that wraps around a user object and 
//   forwards the function calls that are required by the Concept.
//   This Model class need to inherit from Concept. 
//

//
// This is the 'interface' that the user objects need to adhere to
// The important thing to note is that the user objects DOES NOT
// need to inherit this.
//
class Concept {
public:
  virtual ~Concept() {}
  virtual void call() const = 0;
};

//
// The Model class wraps around given user object (T) and
// forwards all the of the user object's calls that correspond
// to the Concept. 
//
// NOTE: At this point, you can tell that we are not going to 
// store a list of Model<T>, since they are all of a different type.
// We will be storing a list of Concepts (or something like that),
// and through the magic of dynamic dispatch and polmoprhism, call 
// the appropriate function.
//
template<typename T> 
class Model : public  Concept {
  T user_object;
public:
  Model(T&& user_object) :
    user_object(user_object)
  {
  }

  // This forwards the function call of the user object.
  // Note that this will return a compile error if the user object does
  // not implement the function in Concept (in this case, call()).
  void call() const override 
  {
    user_object.call();
  }
};


//
// These are user objects.
// Take note that they do not know anything about
// Model, Concept or Object
//
class Rice {
public:
  void call() const {
    cout << "Rice" << endl;
  }
};

class Fish {
public:
  void call() const {
    cout << "Fish" << endl;
  }
};

//
// Helper object to funnel Rice and Fish into a singular object
//
// NOTE the memory allocation needed. 
//
class Food {
  const Concept* c;
public:
  template<typename T>
  Food(T&& user_object) {
    cout << "creating" << endl;
    this->c = new Model<T>(forward<T>(user_object));
  }
  ~Food() {
    cout << "deleting" << endl;
    delete this->c;
  }

  // ...Yeah if we kind of have to forward the concept's function again
  void call() const {
    this->c->call();
  }
};


int main() {
  Food food(Rice);
  food.call();
#if 0
  Food foods[1] = {
    Food(Rice())
  };
  foods.push_back(Food(Fish()));
  foods.push_back(Food(Rice()));
  foods.push_back(Food(Fish()));
  foods.push_back(Food(Rice()));

  for (auto&& food : foods) 
  {
    food.call();
  }
#endif

}

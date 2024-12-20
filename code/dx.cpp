#include <iostream>
#include <utility>
using namespace std;

class Ingrediant
{
public:
  Ingrediant() 
  {
    cout << "Ingrediant constructor" << endl;
  }

  Ingrediant(const Ingrediant& ) 
  {
    cout << "Ingrediant copy constructor" << endl;
  }
  Ingrediant(Ingrediant&&) 
  {
    cout << "Ingrediant move constructor" << endl;
  }
};

class Bagel
{
  Ingrediant ingrediant;

public:
  Bagel(const Ingrediant& i)
    : ingrediant(i)
  {
    cout << "Bagel l-value constructor" << endl;
  }

  Bagel(Ingrediant&& i)
    : ingrediant(std::move(i))
  {
    cout << "Bagel r-value constructor" << endl;
  }
};

template<typename T>
Bagel PerfectMaker(T&& arg)
{
  cout << "Making Bagel Perfectly" << endl;
  return Bagel(std::forward<T>(arg));
}

template<typename T>
Bagel NotPerfectMaker(T arg)
{
  cout << "Making Bagel Not-Perfectly" << endl;
  return Bagel(arg);
}

template<typename T>
Bagel NotPerfectMaker2(T&& arg)
{
  cout << "Making Bagel Not-Perfectly 2" << endl;
  return Bagel(arg);
}

int main()
{
  PerfectMaker((Ingrediant())); 
  cout << "===" << endl;
  NotPerfectMaker((Ingrediant())); 
  cout << "===" << endl;
  NotPerfectMaker2((Ingrediant())); 
}

class B {
  // constructor that takes in some param
  B(/*..*/);
};

class A {
  public:
    A()
      // compile error, cannot default initialise data members data1, data2 and data3.
    {
      // i want to read from one file and then construct my respective data member from here, but data members are implicitly default constructed due to initialization list. 
    }
  private:
    B data1;
    B data2;
    B data3;
};

int main() {

}

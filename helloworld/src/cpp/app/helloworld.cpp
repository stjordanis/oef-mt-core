#include <iostream>

using std::cout;
using std::endl;

void wibble();

int bar()
{
  int *foo = 0;

  std::cout << "SEGV" << *foo <<  std::endl;
  foo += 1000;
  std::cout << "SEGV" << *foo <<  std::endl;

  //wibble();

  return *foo;
}

int foo()
{
  return bar();
}

int main(int argc, char *argv[])
{
  std::cout << "Hello world" << std::endl;

  return foo();
}

#include <iostream>
#include <iterator>
#include <string>

#include "cpp-utils/src/cpp/lib/Uri.hpp"

int main(int argc, char *argv[])
{
  std::cout << "-------------------------------" << std::endl;
  Uri u1("tcp://some.host.somewhere:3999");
  u1.diagnostic();
  std::cout << "-------------------------------" << std::endl;
  Uri u2("http://some.host.somewhere:3999/foobar");
  u2.diagnostic();
}

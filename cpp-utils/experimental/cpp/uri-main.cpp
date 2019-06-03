#include <iostream>
#include <iterator>
#include <string>

#include "cpp-utils/src/cpp/lib/Uri.hpp"

int main(int argc, char *argv[])
{

  const char *strings[] =
    {
      "tcp://some.host.somewhere:3999",
      "http://some.host.somewhere:3999/foobar",
      "outbound://search:99/registration",
      "outbound://search/registration",
      0
    };

  auto current_string = strings;

  while(*current_string)
  {
    std::string s(*current_string);
    std::cout << "-------------------------------" << std::endl;
    std::cout << s << std::endl;
    Uri u1(s);
    u1.diagnostic();

    current_string++;
  }
}

#include "main.hpp"

#include "mt-core/main/src/cpp/MtCore.hpp"

int main(int argc, char *argv[])
{
  // parse args

  MtCore::args args;

  MtCore myCore;
  myCore.run(args);
}

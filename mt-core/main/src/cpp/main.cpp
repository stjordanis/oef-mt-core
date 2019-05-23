#include "main.hpp"

#include "mt-core/main/src/cpp/MtCore.hpp"

#include <boost/program_options.hpp>


int main(int argc, char *argv[])
{
  // parse args

  MtCore::args args;

  boost::program_options::options_description desc{"Options"};
  desc.add_options()
    ("help,h", "Help screen")
    ;
  boost::program_options::variables_map vm;
  store(parse_command_line(argc, argv, desc), vm);
  notify(vm);

  // copy from VM to args.

  MtCore myCore;
  myCore.run(args);
}

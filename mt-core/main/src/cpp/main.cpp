#include "main.hpp"

#include "mt-core/main/src/cpp/MtCore.hpp"

#include <boost/program_options.hpp>


int main(int argc, char *argv[])
{
  // parse args

  MtCore::args args;
  std::string host, search_host;
  int port, search_port;

  boost::program_options::options_description desc{"Options"};
  desc.add_options()
    ("help,h", "Help screen")
    ;
  desc.add_options()
    ("core_key", boost::program_options::value<std::string>(&(args.core_key))->default_value("CoreKey"),
        "Public key for this node");
  desc.add_options()
    ("core_uri", boost::program_options::value<std::string>(&(args.core_uri_str))->default_value("tcp://127.0.0.1:10001"),
        "URI for this node");
  desc.add_options()
    ("search_uri", boost::program_options::value<std::string>(&(args.search_uri_str))->default_value("tcp://127.0.0.1:20000/"),
        "URI of the search node");
  boost::program_options::variables_map vm;
  store(parse_command_line(argc, argv, desc), vm);
  notify(vm);

  args.init();

  // copy from VM to args.

  MtCore myCore;
  myCore.run(args);
}

#include "Outbound.hpp"

bool Outbound::run(void)
{
  boost::asio::ip::tcp::resolver resolver( core );
  boost::asio::ip::tcp::resolver::query query( uri.host, std::to_string( uri.port ));
  auto results = resolver.resolve( query );
  boost::system::error_code ec;

  for(auto &endpoint : results)
  {
    sock.connect( endpoint );
    if (ec)
    {
      // An error occurred.
      std::cout << ec.value() << std::endl;
    }
    else
    {
      return true;
    }
  }
  return false;
}

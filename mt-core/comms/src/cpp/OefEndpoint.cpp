#include "OefEndpoint.hpp"

#include "cpp-utils/src/cpp/lib/Uri.hpp"

bool OefEndpoint::connect(const Uri &uri, Core &core)
{
  boost::asio::ip::tcp::resolver resolver( core );
  boost::asio::ip::tcp::resolver::query query( uri.host, std::to_string( uri.port ));
  auto results = resolver.resolve( query );
  boost::system::error_code ec;

  for(auto &endpoint : results)
  {
    socket().connect( endpoint );
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

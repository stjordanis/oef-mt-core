#include "Endpoint.hpp"
#include "fetch_teams/ledger/logger.hpp"

#include "cpp-utils/src/cpp/lib/Uri.hpp"
#include <cstdlib>
#include "monitoring/src/cpp/lib/Gauge.hpp"

static Gauge count("mt-core.network.Endpoint");

bool Endpoint::connect(const Uri &uri, Core &core)
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
      *state = RUNNING_ENDPOINT;
      return true;
    }
  }
  return false;
}

Endpoint::Endpoint(
      Core &core
      ,std::size_t sendBufferSize
      ,std::size_t readBufferSize
  )
    :sock(core)
    ,sendBuffer(sendBufferSize)
    ,readBuffer(readBufferSize)
    ,asio_sending(false)
    ,asio_reading(false)
{
  state = std::make_shared<StateType>(0);
  count++;
}

Endpoint::~Endpoint()
{
  count--;
}


void Endpoint::run_sending()
{
  {
    Lock lock(mutex);
    if (asio_sending || *state != RUNNING_ENDPOINT)
    {
      FETCH_LOG_INFO(LOGGING_NAME, "early exit 1 sending=", asio_sending, " state=", *state);
      return;
    }
    if (sendBuffer.getDataAvailable() == 0)
    {
      FETCH_LOG_INFO(LOGGING_NAME, "create messages");
      create_messages();
    }
    if (sendBuffer.getDataAvailable() == 0)
    {
      return;
    }
    asio_sending = true;
  }
  FETCH_LOG_INFO(LOGGING_NAME, "ok data available");
  auto data = sendBuffer.getDataBuffers();

  int i = 0;
  for(auto &d : data)
  {
    FETCH_LOG_INFO(LOGGING_NAME, "Send buffer ", i, "=", d.size(), " bytes on thr=", std::this_thread::get_id());
  }

  FETCH_LOG_INFO(LOGGING_NAME, "run_sending: START");

  auto my_state = state;
  boost::asio::async_write(
                           sock,
                           data,
                           [this, my_state](const boost::system::error_code& ec, const size_t &bytes){
                             this -> complete_sending(state, ec, bytes);
                           }
                           );
}
void Endpoint::run_reading()
{
  std::size_t read_needed_local = 0;

  //std::cout << reader.get() << ":Endpoint::run_reading" << std::endl;
  {
    Lock lock(mutex);
    if (asio_reading || *state != RUNNING_ENDPOINT)
    {
      FETCH_LOG_INFO(LOGGING_NAME, reader.get(), ":early exit 1 reading=", asio_sending, " state=", *state);
      return;
    }
    if (read_needed == 0)
    {
      FETCH_LOG_INFO(LOGGING_NAME, reader.get(), ":early exit 1 read_needed=", read_needed, " state=", *state);
      return;
    }
    read_needed_local = read_needed;
    if (read_needed_local > readBuffer.getFreeSpace())
    {
      FETCH_LOG_ERROR(LOGGING_NAME, "********** READ BUFFER FULL!");
      read_needed_local = readBuffer.getFreeSpace();
    }
    read_needed = 0;
    asio_reading = true;
  }

  if (read_needed_local == 0) {
    return;
  }
  auto space = readBuffer.getSpaceBuffers();
  auto my_state = state;

  boost::asio::async_read(
                          sock,
                          space,
                          boost::asio::transfer_at_least(read_needed_local),
                          [this, my_state](const boost::system::error_code& ec, const size_t &bytes){
                            this -> complete_reading(state, ec, bytes);
                          }
                          );
}

void Endpoint::close()
{
  Lock lock(mutex);
  *state |= CLOSED_ENDPOINT;
  sock.close();
}

void Endpoint::eof()
{
  if (*state & EOF_ENDPOINT)
  {
    return;
  }

  EofNotification local_handler_copy;
  {
    Lock lock(mutex);
    *state |= EOF_ENDPOINT;
    *state |= CLOSED_ENDPOINT;
    sock.close();

    local_handler_copy = onEof;
    onEof = 0;
    onError = 0;
    onProtoError = 0;
  }

  if (local_handler_copy)
  {
    try { local_handler_copy(); } catch(...) {} // Ignore exceptions.
  }
}

void Endpoint::error(const boost::system::error_code& ec)
{
  if (*state & ERRORED_ENDPOINT)
  {
    return;
  }

  ErrorNotification local_handler_copy;

  {
    Lock lock(mutex);
    *state |= ERRORED_ENDPOINT;
    *state |= CLOSED_ENDPOINT;
    sock.close();

    local_handler_copy = onError;
    onEof = 0;
    onError = 0;
    onProtoError = 0;  }

  if (local_handler_copy)
  {
    try { local_handler_copy(ec); } catch(...) {} // Ignore exceptions.
  }
}

void Endpoint::proto_error(const std::string &msg)
{
  //std::cout << "proto error: " << msg << std::endl;

  if (*state & ERRORED_ENDPOINT)
  {
    return;
  }

  ProtoErrorNotification local_handler_copy;

  {
    Lock lock(mutex);
    *state |= ERRORED_ENDPOINT;
    *state |= CLOSED_ENDPOINT;
    sock.close();

    local_handler_copy = onProtoError;
    onEof = 0;
    onError = 0;
    onProtoError = 0;
  }

  if (local_handler_copy)
  {
    try { local_handler_copy(msg); } catch(...) {} // Ignore exceptions.
  }
}

void Endpoint::go()
{
  remote_id = socket().remote_endpoint().address().to_string();
  
   //std::cout << "Endpoint::go" << std::endl;
  if (onStart)
  {
    auto myStart = onStart;
    onStart = 0;
    myStart();
  }

  *state |= RUNNING_ENDPOINT;

  {
    Lock lock(mutex);
    read_needed++;
  }

  run_reading();
}

void Endpoint::complete_sending(StateTypeP state, const boost::system::error_code& ec, const size_t &bytes)
{
  try
  {
    if (*state > RUNNING_ENDPOINT)
    {
      FETCH_LOG_INFO(LOGGING_NAME, "complete_sending on already dead socket", ec);
      return;
    }

    if (ec == boost::asio::error::eof || ec == boost::asio::error::operation_aborted)
    {
      FETCH_LOG_INFO(LOGGING_NAME, "complete_sending EOF:  ", ec);
      eof();
      return; // We are done with this thing!
    }

    if (ec)
    {
      FETCH_LOG_INFO(LOGGING_NAME, "complete_sending ERR:  ", ec);
      error(ec);
      return; // We are done with this thing!
    }

    //std::cout << "complete_sending OK:  " << bytes << std::endl;
    sendBuffer.markDataUsed(bytes);
    create_messages();
    {
      Lock lock(mutex);
      asio_sending = false;
    }
    //std::cout << "complete_sending: kick" << std::endl;
    run_sending();
  }

  catch(std::exception &ex)
  {
    close();
    return;
  }
}

void Endpoint::create_messages()
{
  auto consumed_needed = writer -> checkForSpace(sendBuffer.getSpaceBuffers());
  auto consumed = consumed_needed.first;
  sendBuffer.markSpaceUsed(consumed);
}

void Endpoint::complete_reading(StateTypeP state, const boost::system::error_code& ec, const size_t &bytes)
{
  try
  {
    if (*state > RUNNING_ENDPOINT)
    {
      FETCH_LOG_INFO(LOGGING_NAME, "complete_sending on already dead socket", ec);
      return;
    }

    //std::cout << reader.get() << ":  complete_reading:  " << ec << ", "<< bytes << std::endl;
    
    if (ec == boost::asio::error::eof || ec == boost::asio::error::operation_aborted)
    {
      //std::cout << "complete_reading: eof" << std::endl;
      eof();
      return; // We are done with this thing!
    }

    if (ec)
    {
      //std::cout << "complete_reading: error" << std::endl;
      error(ec);
      close();
      return; // We are done with this thing!
    }


    //std::cout << reader.get() << ":complete_reading: 1 " << std::endl;
    readBuffer.markSpaceUsed(bytes);

    //std::cout << reader.get() << ":complete_reading: 2" << std::endl;
    IMessageReader::consumed_needed_pair consumed_needed;

    //std::cout << reader.get() << ":complete_reading: 3" << std::endl;
    try
    {
      //std::cout << reader.get() << ":complete_reading: 4" << std::endl;
      //std::cout << reader.get() << ": @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ " << reader.get() << std::endl;
       consumed_needed = reader -> checkForMessage(readBuffer.getDataBuffers());
       //std::cout << "     DONE." << std::endl;
    }
    catch(std::exception &ex)
    {
    //std::cout << "complete_reading: 5" << std::endl;
      proto_error(ex.what());
      return;
    }
    catch(...)
    {
    //std::cout << "complete_reading: 6" << std::endl;
      proto_error("unknown protocol error");
      return;
    }

    //std::cout << "complete_reading: 7" << std::endl;
    auto consumed = consumed_needed.first;
    auto needed = consumed_needed.second;

    readBuffer.markDataUsed(consumed);

    {
      Lock lock(mutex);
      read_needed = needed;
    }

    {
      Lock lock(mutex);
      asio_reading = false;
    }
    run_reading();
  }
  catch(std::exception &ex)
  {
    close();
    return;
  }
}

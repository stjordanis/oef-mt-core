#include "Endpoint.hpp"

//std::ostream& operator<<(std::ostream& os, const Endpoint &output) {}
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
    ,state(0)
{
  }
Endpoint::~Endpoint()
{
}


void Endpoint::run_sending()
{
  {
    Lock lock(mutex);
    if (asio_sending || state != RUNNING_ENDPOINT)
    {
      return;
    }
    if (sendBuffer.getDataAvailable() == 0)
    {
      create_messages();
    }
    if (sendBuffer.getDataAvailable() == 0)
    {
      return;
    }
    asio_sending = true;
  }

  auto data = sendBuffer.getDataBuffers();
  boost::asio::async_write(
                           sock,
                           data,
                           [this](const boost::system::error_code& ec, const size_t &bytes){
                             this -> complete_sending(ec, bytes);
                           }
                           );
}
void Endpoint::run_reading()
{
  std::size_t read_needed_local = 0;

  {
    Lock lock(mutex);
    if (asio_reading || state != RUNNING_ENDPOINT)
    {
      return;
    }
    if (read_needed == 0)
    {
      return;
    }
    read_needed_local = read_needed;
    if (read_needed_local > readBuffer.getFreeSpace())
    {
      read_needed_local = readBuffer.getFreeSpace();
    }
    read_needed = 0;
    asio_reading = true;
  }

  //std::cout << "start_reading:" << read_needed_local << " bytes." << std::endl;
  auto space = readBuffer.getSpaceBuffers();
  boost::asio::async_read(
                          sock,
                          space,
                          boost::asio::transfer_at_least(read_needed_local),
                          [this](const boost::system::error_code& ec, const size_t &bytes){
                            this -> complete_reading(ec, bytes);
                          }
                          );
}

void Endpoint::close()
{
  Lock lock(mutex);
  state |= CLOSED_ENDPOINT;
  sock.close();
}

void Endpoint::eof()
{
  if (state & EOF_ENDPOINT)
  {
    return;
  }

  EofNotification local_handler_copy;
  {
    Lock lock(mutex);
    state |= EOF_ENDPOINT;
    state |= CLOSED_ENDPOINT;
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
  if (state & ERRORED_ENDPOINT)
  {
    return;
  }

  ErrorNotification local_handler_copy;

  {
    Lock lock(mutex);
    state |= ERRORED_ENDPOINT;
    state |= CLOSED_ENDPOINT;
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
  if (state & ERRORED_ENDPOINT)
  {
    return;
  }

  ProtoErrorNotification local_handler_copy;

  {
    Lock lock(mutex);
    state |= ERRORED_ENDPOINT;
    state |= CLOSED_ENDPOINT;
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
  if (onStart)
  {
    auto myStart = onStart;
    onStart = 0;
    myStart();
  }

  state |= RUNNING_ENDPOINT;

  {
    Lock lock(mutex);
    read_needed++;
  }

  run_reading();
}


void Endpoint::complete_sending(const boost::system::error_code& ec, const size_t &bytes)
{
  try
  {
    {
      Lock lock(mutex);
      asio_sending = false;
    }

    if (ec == boost::asio::error::eof || ec == boost::asio::error::operation_aborted)
    {
      eof();
      return; // We are done with this thing!
    }

    if (ec)
    {
      error(ec);
      return; // We are done with this thing!
    }
    sendBuffer.markDataUsed(bytes);
    create_messages();
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

void Endpoint::complete_reading(const boost::system::error_code& ec, const size_t &bytes)
{
  try
  {
    std::cout << "complete_reading:  " << ec << ", "<< bytes << std::endl;
    {
      Lock lock(mutex);
      asio_reading = false;
    }

    if (ec == boost::asio::error::eof || ec == boost::asio::error::operation_aborted)
    {
      eof();
      return; // We are done with this thing!
    }

    if (ec)
    {
      error(ec);
      close();
      return; // We are done with this thing!
    }


    readBuffer.markSpaceUsed(bytes);

    IMessageReader::consumed_needed_pair consumed_needed;

    try
    {
      consumed_needed = reader -> checkForMessage(readBuffer.getDataBuffers());
    }
    catch(std::exception &ex)
    {
      proto_error(ex.what());
      return;
    }
    catch(...)
    {
      proto_error("unknown protocol error");
      return;
    }

    auto consumed = consumed_needed.first;
    auto needed = consumed_needed.second;

    readBuffer.markDataUsed(consumed);

    {
      Lock lock(mutex);
      read_needed = needed;
    }

    run_reading();
  }
  catch(std::exception &ex)
  {
    close();
    return;
  }
}

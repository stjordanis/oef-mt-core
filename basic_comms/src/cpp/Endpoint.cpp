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
{
  }
Endpoint::~Endpoint()
{
}


void Endpoint::run_sending()
{
  {
    Lock lock(mutex);
    if (asio_sending || closing)
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
    if (asio_reading || closing)
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
  closing = true;
  sock.close();
  onStart = 0;
  onError = 0;
  if (onEof)
  {
    auto  myEof = onEof;
    onEof = 0;
    myEof();
  }
}




void Endpoint::complete_sending(const boost::system::error_code& ec, const size_t &bytes)
{
  {
    Lock lock(mutex);
    asio_sending = false;
  }
  if (ec == boost::asio::error::eof || ec == boost::asio::error::operation_aborted)
  {
    std::cout << "EOF" << std::endl;
    if (onEof)
    {
      auto  myEof = onEof;
      onEof = 0;
      myEof();
    }
    return;
  }

  if (ec)
  {
    if (onError)
    {
      auto myError = onError;
      onError = 0;
      myError(ec);
    }
    return;
  }

  sendBuffer.markDataUsed(bytes);
  create_messages();
  run_sending();
}

void Endpoint::create_messages()
{
  auto consumed_needed = writer -> checkForSpace(sendBuffer.getSpaceBuffers());
  auto consumed = consumed_needed.first;
  sendBuffer.markSpaceUsed(consumed);
}

void Endpoint::complete_reading(const boost::system::error_code& ec, const size_t &bytes)
{
  //std::cout << "complete_reading:" << ec << ", "<< bytes << std::endl;
  {
    Lock lock(mutex);
    asio_reading = false;
  }
  if (ec == boost::asio::error::eof || ec == boost::asio::error::operation_aborted)
  {
    std::cout << "EOF" << std::endl;
    if (onEof)
    {
      auto  myEof = onEof;
      onEof = 0;
      myEof();
    }
    return;
  }

  if (ec)
  {
    if (onError)
    {
      auto myError = onError;
      onError = 0;
      myError(ec);
    }
    return;
  }


  readBuffer.markSpaceUsed(bytes);

  //std::cout << "Endpoint::complete_reading READER=" << reader.get() << std::endl;

  IMessageReader::consumed_needed_pair consumed_needed;
  try
  {
    consumed_needed = reader -> checkForMessage(readBuffer.getDataBuffers());
  }
  catch(std::exception &ex)
  {
    if (onProtoError)
    {
      std::cout << "ERROR:" << ex.what() << std::endl;
      onProtoError(ex.what());
    }
    if (onProtoError)
    {
      std::cout << "ERROR:" << "???" << std::endl;
      onProtoError("");
    }
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

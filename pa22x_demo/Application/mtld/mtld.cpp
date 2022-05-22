#include <assert.h>
#include <list>
#include <functional>
#include <numeric>
#include <string.h>
#include "mtld.h"

#define HTONL(n) (((((unsigned long)(n) & 0xFF)) << 24) |       \
                  ((((unsigned long)(n) & 0xFF00)) << 8) |      \
                  ((((unsigned long)(n) & 0xFF0000)) >> 8) |    \
                  ((((unsigned long)(n) & 0xFF000000)) >> 24))

#define NTOHL(n) (((((unsigned long)(n) & 0xFF)) << 24) |       \
                  ((((unsigned long)(n) & 0xFF00)) << 8) |      \
                  ((((unsigned long)(n) & 0xFF0000)) >> 8) |    \
                  ((((unsigned long)(n) & 0xFF000000)) >> 24))

/*
数据块格式：
Mark: 4字节，固定0x55AA55AA
Type: 8字节，第1字节固定‘T’，后7个字节自定义字符
Length: 4字节，高位字节固定0x22，后3字节为数据块总长度，必须为4的倍数，大于等于16，小于等于8*1024*1024 (8M)
Data: 最小长度0，最大8M-16字节。*
*/

typedef std::function<bool (unsigned char&)> BYTE_READ_FUNC;

unsigned int length_padding(int length){
  return length % 4 != 0 ? 4 - length % 4 : 0;
}


bool window_match(const BYTE_READ_FUNC& reader, const std::list<char> match_list) {
  std::list<char>  buffer_list;
  unsigned char data = 0;
  size_t count = match_list.size();
  while (true) {
    if (!reader(data))
      return false;
      if (buffer_list.size() == count) {
        buffer_list.erase(buffer_list.begin());
      }
      buffer_list.push_back(data);
      if (buffer_list == match_list)
        return true;
  }
  return false;
}


size_t MTLDPacket::get_packet_org_size() {
  return packet_org_size_;
}

MTLDPacket::MTLDPacket()
{
  //test_type();
}


MTLDPacket::MTLDPacket(unsigned char* buffer, size_t sz)
{
  clear();
  this->append(buffer, sz);
}


const char* MTLDPacket::get_type() const
{
  return type_.c_str();
}


void MTLDPacket::set_type(const std::string& type)
{
  if (type.length() != 7) {
    assert(0); // "type length != 7";
  }
  type_ = type;
  // type_.resize(7);
}

size_t MTLDPacket::get_size() const
{
  return buffer_.size();
}

void MTLDPacket::resize(size_t sz)
{
  return buffer_.resize(sz);
}


void MTLDPacket::clear()
{
  return buffer_.clear();
}


void MTLDPacket::append(unsigned char* buffer, size_t sz)
{
  if(sz + buffer_.size() > 8 * 1024 * 1024){
    qDebug("error packet size > 8M");
    return ;
  }
  buffer_.insert(buffer_.end(), buffer, buffer + sz);
}


const unsigned char* MTLDPacket::get_data()
{
  if(buffer_.size() > 1)
    return &buffer_[0];
  return nullptr;
}


MTLDFileReadWrite::MTLDFileReadWrite(const std::string& filename, MTLD_MODE mode):IMTLDReadWrite(mode)
{
  filename_ = filename;
}


MTLD_ERROR_CODE MTLDFileReadWrite::begin()
{
  if (fp_) {
    assert(0); //"already opened");
    set_last_error("already begined");
    return ME_ERROR;
  }

  if(GetMode() == MM_READ)
    fp_ = fopen(filename_.c_str(), "rb");
  else if (GetMode() == MM_WRITE){
    fp_ = fopen(filename_.c_str(), "wb");
  }
  else {
    set_last_error("unsupported mode");
    assert(0); //"unsupported mode");
  }
  if (!fp_)
    return ME_ERROR;
  return ME_SUCCESS;
}


MTLD_ERROR_CODE MTLDFileReadWrite::end()
{
  if (!fp_) {
    set_last_error("file not begin");
    return ME_ERROR;
  }
  fclose(fp_);
  fp_ = nullptr;
  return ME_SUCCESS;
}


MTLD_ERROR_CODE MTLDFileReadWrite::raw_read(unsigned char* buffer, size_t sz, size_t& readed)
{
  readed = fread(buffer, 1, sz, fp_);
  if(feof(fp_)){
      set_last_error("eof");
      return ME_ERROR;
  }
  set_last_error("");
  return ME_SUCCESS;
}


MTLD_ERROR_CODE MTLDFileReadWrite::raw_write(unsigned char* buffer, size_t sz, size_t& written)
{
  set_last_error("");
  written = fwrite(buffer, 1, sz, fp_);
  return ME_SUCCESS;
}


IMTLDReadWrite::IMTLDReadWrite(MTLD_MODE mode):mode_(mode)
{
}

MTLD_ERROR_CODE IMTLDReadWrite::read(MTLDPacket& packet)
{
  MTLDPacketFinder finder;
  size_t readed = 0;
  unsigned char ch = 0;
  while (1) {
    if (this->raw_read(&ch, 1, readed) == ME_ERROR)
      return ME_ERROR;
    if (readed != 1) {
      set_last_error("read error maybe file end");
      return ME_ERROR;
    }

    if (finder.put(ch))
      break;

    if(finder.get_buffer_size() == MTLD_HEADER_LEN)
      error_bytes_ ++;
  }

  if (!finder.match()) {
    set_last_error("packet not found");
    return ME_ERROR;
  }

  packet.set_type(finder.type_.c_str());
  packet.set_packet_org_size(finder.length);
  packet.resize(finder.length);
  MTLD_ERROR_CODE ret;
  ret = this->raw_read((unsigned char*) packet.get_data(), finder.length, readed);
  packet.resize(readed);
  if (ret != ME_SUCCESS)
    return ME_ERROR;
  if (readed != packet.get_packet_org_size()) {
    set_last_error("truncate");
    return ME_ERROR;
  }
  return ME_SUCCESS;
}


MTLD_ERROR_CODE IMTLDReadWrite::write(MTLDPacket& packet)
{
  if (packet.get_size() > 8 * 1024 * 1024)
    {
      set_last_error("packet is too large > 8 * 1024 * 1024");
      return ME_ERROR;
    }

  std::string write_buffer;
  make_packet_header(packet, write_buffer);
  unsigned char* p = (unsigned char*) write_buffer.c_str();
  size_t written;
  MTLD_ERROR_CODE ret = this->raw_write(p, write_buffer.size(), written);
  if (ret != ME_SUCCESS) {
    return ME_ERROR;
  }

  if (written != write_buffer.size()) {
    set_last_error("packet header write fail, maybe disk full!");
    return ME_ERROR;
  }

  ret = this->raw_write((unsigned char*) packet.get_data(), packet.get_size(), written);
  if (ret != ME_SUCCESS || written != packet.get_size()) {
    set_last_error("packet header write fail, maybe disk full!");
    return ME_ERROR;
  }

  return ME_SUCCESS;
}


MTLD_MODE IMTLDReadWrite::GetMode()
{
  return mode_;
}


std::string IMTLDReadWrite::get_last_error()
{
  return last_error_;
}


void IMTLDReadWrite::set_last_error(const std::string& error)
{
  last_error_ = error;
}


bool MTLDPacketFinder::put(unsigned ch)
{
  while (buffer_.size() >= MTLD_HEADER_LEN){
    buffer_.erase(buffer_.begin());
  }
  buffer_.push_back(ch);
  return match();
}


void MTLDPacketFinder::clear()
{
  length = 0;
  type_.clear();
  buffer_.clear();
}


size_t MTLDPacketFinder::get_buffer_size()
{
  return buffer_.size();
}


bool MTLDPacketFinder::match()
{
  if(buffer_.size() != MTLD_HEADER_LEN)
    return false;

  if (buffer_[0] != 0x55 || buffer_[1] != 0xaa
      || buffer_[2] != 0x55 || buffer_[3] != 0xaa
      || buffer_[4] != 'T' || buffer_[12] != 0x22){
      return false;
  }
  type_.resize(7);
  memcpy((void*)type_.c_str(), (void*) &buffer_[5], 7);
  unsigned int length = NTOHL(*(unsigned int*) &buffer_[12]);
  this->length = length & 0x00FFFFFF;
  return true;
}


size_t IMTLDReadWrite::get_error_bytes()
{
  return error_bytes_;
}

bool make_packet_header(MTLDPacket& packet, std::string& buffer)
{
  if (packet.get_size() > 8 * 1024 * 1024)
    return false;

  buffer.clear();
  buffer.resize(MTLD_HEADER_LEN);
  unsigned char* pbuffer = (unsigned char*) buffer.c_str();
  pbuffer[0] = 0x55;
  pbuffer[1] = 0xaa;
  pbuffer[2] = 0x55;
  pbuffer[3] = 0xaa;
  pbuffer[4] = 'T';
  memcpy(&pbuffer[5], (void*) packet.get_type(), 7);
  unsigned int length = (unsigned int) packet.get_size();
  unsigned int padding_length = length_padding(length);
  if (padding_length != 0)
    packet.resize(packet.get_size() + padding_length);
  *(unsigned int*) (&pbuffer[12])  = HTONL((unsigned int) packet.get_size());
  pbuffer[12] = 0x22;
  return true;
}

MTLDQTcpSocketReadWrite::MTLDQTcpSocketReadWrite(QTcpSocket* socket, MTLD_MODE mode/*=MM_READWRITE*/):IMTLDReadWrite(mode), socket_(socket) {
  assert(socket_);
  socket_ = socket;
  socket_->waitForConnected();
}

MTLD_ERROR_CODE MTLDQTcpSocketReadWrite::begin()
{
  if(!socket_)
    return ME_ERROR;
  return ME_SUCCESS;
}

MTLD_ERROR_CODE MTLDQTcpSocketReadWrite::end()
{
  if(!socket_)
    return ME_ERROR;
  return ME_SUCCESS;
}

MTLD_ERROR_CODE MTLDQTcpSocketReadWrite::raw_read(unsigned char* buffer, size_t sz, size_t& readed)
{
  if(!socket_){
    set_last_error("socket null");
    return ME_ERROR;
  }

  bool connected = socket_->state() == QTcpSocket::ConnectedState;
  if (!connected) {
    set_last_error("not connected");
    return ME_ERROR;
  }

  if (!socket_->isReadable()) {
    set_last_error("socket unreadable");
    return ME_ERROR;
  }

  readed = (size_t)socket_->read((char*) buffer, sz);
  set_last_error("");
  return ME_SUCCESS;
}

MTLD_ERROR_CODE MTLDQTcpSocketReadWrite::raw_write(unsigned char* buffer, size_t sz, size_t& written)
{
  if(!socket_){
    set_last_error("socket null");
    return ME_ERROR;
  }

  bool connected = socket_->state() == QTcpSocket::ConnectedState;
  if (!connected) {
    set_last_error("not connected");
    return ME_ERROR;
  }

  if (!socket_->isWritable()) {
    set_last_error("socket unwritable");
    return ME_ERROR;
  }

  written = (size_t)socket_->write((char*) buffer, sz);
  socket_->waitForBytesWritten();
  set_last_error("");
  return ME_SUCCESS;
}

void MTLDPacket::set_packet_org_size(size_t sz) {
  this->packet_org_size_ = sz;
}


MTLD_ERROR_CODE MTLDQTcpSocketReadWrite::read(MTLDPacket& packet)
{
  if(!is_truncate){
    last_error_ = "";
    MTLD_ERROR_CODE ret = IMTLDReadWrite::read(read_buffer_);
    if (ret == ME_SUCCESS) {
      packet = std::move(read_buffer_);
      return ret;
    }
    // if truncate
    if (last_error_ == "truncate"){
      is_truncate = true;
      return ret;
    }
    // other error
    return ret;
  }

  // deal with truncate
  size_t sz  = read_buffer_.get_packet_org_size();
  if(sz <= read_buffer_.get_size()){
    is_truncate = false;
    last_error_ = "bad packet";
    return ME_ERROR;
  }

  size_t org_size = read_buffer_.get_size();
  size_t remain = sz - org_size;
  size_t readed = 0;
  read_buffer_.resize(read_buffer_.get_packet_org_size());
  //socket_->waitForReadyRead();
  MTLD_ERROR_CODE ret = this->raw_read((unsigned char*) read_buffer_.get_data() + org_size, remain, readed);
  read_buffer_.resize(readed + org_size);
  if(read_buffer_.get_size() == read_buffer_.get_packet_org_size()){
    is_truncate = false;
    packet = std::move(read_buffer_);
    last_error_ = "";
    return ME_SUCCESS;
  }

  is_truncate = true;
  last_error_ = "truncate";
  return ME_ERROR;
}

MTLD_ERROR_CODE MTLDQTcpSocketReadWrite::write(MTLDPacket& packet)
{

  bool connected = socket_->state() == QTcpSocket::ConnectedState;
  if (!connected) {
    set_last_error("not connected");
    return ME_ERROR;
  }

  if (socket_->bytesToWrite() > 10 * 1024 * 1024){
    while (socket_->bytesToWrite() > 0)
        socket_->waitForBytesWritten();
    qDebug("error, bytes to write > 10M");
    last_error_ = "net buffer full";
    return ME_ERROR;
  }
  MTLD_ERROR_CODE ec = IMTLDReadWrite::write(packet);
  while (socket_->bytesToWrite() > 0)
    socket_->waitForBytesWritten();

  return ec;
}



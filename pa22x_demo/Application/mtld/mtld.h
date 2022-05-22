#ifndef _MTLD_HPP_
#define _MTLD_HPP_

#include <string>
#include <vector>
#include <QtNetwork>

enum MTLD_ERROR_CODE { ME_SUCCESS,  ME_ERROR, ME_EOF, ME_UNOPEN};
enum MTLD_MODE { MM_READ, MM_WRITE, MM_READWRITE };

const int MTLD_HEADER_LEN = 16;

class MTLDPacket {

public:

  MTLDPacket();
  MTLDPacket(unsigned char* buffer, size_t sz);
  const char* get_type() const;
  void set_type(const std::string& type);
  size_t get_size() const;
  void resize(size_t sz);
  void clear();
  void append(unsigned char* buffer, size_t sz);
  const unsigned char* get_data();
  size_t get_packet_org_size();
  void set_packet_org_size(size_t sz);

private:
  std::string type_;
  std::vector<unsigned char> buffer_;
  size_t packet_org_size_ = 0;
};


bool make_packet_header(MTLDPacket& packet, std::string& buffer);

class MTLDPacketFinder{

public:
  bool put(unsigned ch);
  size_t get_buffer_size();
  void clear();
  std::string type_;
  size_t length;
  bool match();
private:
  std::vector<unsigned char> buffer_;
};


class IMTLDReadWrite {

public:
  IMTLDReadWrite(MTLD_MODE mode);
  virtual MTLD_ERROR_CODE begin()=0;
  virtual MTLD_ERROR_CODE end()=0;
  virtual MTLD_ERROR_CODE read(MTLDPacket& packet);
  virtual MTLD_ERROR_CODE write(MTLDPacket& packet);
  virtual  ~IMTLDReadWrite() {}
  MTLD_MODE GetMode();
  std::string get_last_error();
  size_t get_error_bytes();

protected:

  virtual MTLD_ERROR_CODE raw_read(unsigned char* buffer, size_t sz, size_t& readed) = 0;
  virtual MTLD_ERROR_CODE raw_write(unsigned char* buffer, size_t sz, size_t& written) = 0;
  void set_last_error(const std::string& error);

private:
  MTLD_MODE mode_;
  size_t error_bytes_ = 0;
protected:
  std::string last_error_;
};


class MTLDFileReadWrite : public IMTLDReadWrite {

public:

  MTLDFileReadWrite(const std::string& filename, MTLD_MODE mode);
  virtual MTLD_ERROR_CODE  begin();
  virtual MTLD_ERROR_CODE  end();

protected:

  virtual MTLD_ERROR_CODE raw_read(unsigned char* buffer, size_t, size_t& sz);
  virtual MTLD_ERROR_CODE raw_write(unsigned char* buffer, size_t sz, size_t& written);

private:

  std::string filename_;
  FILE* fp_ = nullptr;
};

class MTLDQTcpSocketReadWrite : public IMTLDReadWrite {

public:

  MTLDQTcpSocketReadWrite(QTcpSocket* socket, MTLD_MODE mode=MM_READWRITE);
  virtual MTLD_ERROR_CODE  read(MTLDPacket& packet);
  virtual MTLD_ERROR_CODE  write(MTLDPacket& packet);
  virtual MTLD_ERROR_CODE  begin();
  virtual MTLD_ERROR_CODE  end();

protected:

  virtual MTLD_ERROR_CODE raw_read(unsigned char* buffer, size_t, size_t& sz);
  virtual MTLD_ERROR_CODE raw_write(unsigned char* buffer, size_t sz, size_t& written);

private:

  QTcpSocket* socket_ = nullptr;
  bool is_truncate = false;
  MTLDPacket read_buffer_;
};

#endif// _MTLD_HPP_


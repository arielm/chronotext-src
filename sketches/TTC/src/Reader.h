#pragma once

#include "chr/FileSystem.h"

#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/io/gzip_stream.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>

class Reader
{
public:
  Reader() = default;
  Reader(const chr::InputSource &inputSource);

  ~Reader();
  void close();

  void skip(int count);
  bool good() const;

  std::u16string readJavaString(size_t size);
  int8_t readJavaByte();
  int16_t readJavaShort();
  char16_t readJavaChar();
  int32_t readJavaInt();
  int64_t readJavaLong();

protected:
  std::shared_ptr<chr::ResourceBuffer> resourceBuffer;
  google::protobuf::io::ArrayInputStream *arrayInput = nullptr;
  google::protobuf::io::CodedInputStream *codedInput = nullptr;
  google::protobuf::io::GzipInputStream *zipInput = nullptr;
  google::protobuf::io::ZeroCopyInputStream *fileInput = nullptr;
  int fd = 0;

  bool failed = false;

  void fail();
};

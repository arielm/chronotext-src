#pragma once

#include "chr/FileSystem.h"

#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/io/gzip_stream.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>

class Writer
{
public:
  Writer(chr::OutputTarget &outputTarget);

  ~Writer();
  void close();

  void writeJavaString(const std::u16string &input);
  void WriteJavaByte(int8_t input);
  void writeJavaShort(int16_t input);
  void writeJavaChar(char16_t input);
  void writeJavaInt(int32_t input);
  void writeJavaLong(int64_t input);

protected:
  chr::OutputTarget &outputTarget;
  std::shared_ptr<std::ostream> stream;
  google::protobuf::io::OstreamOutputStream *rawOutput = nullptr;
  google::protobuf::io::CodedOutputStream *codedOutput = nullptr;
  google::protobuf::io::GzipOutputStream *zipOutput = nullptr;
};

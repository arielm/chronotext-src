#include "Writer.h"

#include <sstream>

using namespace std;
using namespace chr;
using namespace google;

Writer::Writer(OutputTarget &outputTarget)
:
outputTarget(outputTarget)
{
  stream = outputTarget.getStream();

  if (stream->good())
  {
    rawOutput = new protobuf::io::OstreamOutputStream(stream.get());
    zipOutput = new protobuf::io::GzipOutputStream(rawOutput);
    codedOutput = new protobuf::io::CodedOutputStream(zipOutput);
  }
}

Writer::~Writer()
{
  close();
}

void Writer::close()
{
  if (rawOutput)
  {
    delete codedOutput;
    codedOutput = nullptr;

    delete zipOutput;
    zipOutput = nullptr;

    delete rawOutput;
    rawOutput = nullptr;

    if (outputTarget.isBuffer())
    {
      outputTarget.setBuffer(*static_cast<ostringstream*>(stream.get())); // XXX
    }

    stream.reset();
  }
}

void Writer::writeJavaString(const u16string &input)
{
  u16string buffer = input;

  for (int i = 0; i < input.size(); i++)
  {
    buffer[i] = (buffer[i] & 0xff00) >> 8 | (buffer[i] & 0xff) << 8;
  }

  codedOutput->WriteRaw(buffer.data(), buffer.size() * 2);
}

void Writer::WriteJavaByte(int8_t input)
{
  codedOutput->WriteRaw(&input, 1);
}

void Writer::writeJavaShort(int16_t input)
{
  uint8_t buffer[2];

  buffer[0] = (input >> 8) & 0xff;
  buffer[1] = input & 0xff;

  codedOutput->WriteRaw(&buffer[0], 2);
}

void Writer::writeJavaChar(char16_t input)
{
  uint8_t buffer[2];

  buffer[0] = (input >> 8) & 0xff;
  buffer[1] = input & 0xff;

  codedOutput->WriteRaw(&buffer[0], 2);
}

void Writer::writeJavaInt(int32_t input)
{
  uint8_t buffer[4];

  buffer[0] = (input >> 24) & 0xff;
  buffer[1] = (input >> 16) & 0xff;
  buffer[2] = (input >> 8) & 0xff;
  buffer[3] = input & 0xff;

  codedOutput->WriteRaw(&buffer[0], 4);
}

void Writer::writeJavaLong(int64_t input)
{
  uint8_t buffer[8];

  buffer[0] = (input >> 56) & 0xff;
  buffer[1] = (input >> 48) & 0xff;
  buffer[2] = (input >> 40) & 0xff;
  buffer[3] = (input >> 32) & 0xff;
  buffer[4] = (input >> 24) & 0xff;
  buffer[5] = (input >> 16) & 0xff;
  buffer[6] = (input >> 8) & 0xff;
  buffer[7] = input & 0xff;

  codedOutput->WriteRaw(&buffer[0], 8);
}

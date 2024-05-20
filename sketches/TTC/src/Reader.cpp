#include "Reader.h"

#include "chr/ResourceBuffer.h"

#include <fcntl.h> // FOR MINGW

using namespace std;
using namespace chr;
using namespace google;

Reader::~Reader()
{
  close();
}

void Reader::close()
{
  if (codedInput)
  {
    delete codedInput;
    codedInput = nullptr;

    delete zipInput;
    zipInput = nullptr;
  }

  if (arrayInput)
  {
    delete arrayInput;
    arrayInput = nullptr;
  }

  if (fileInput)
  {
    delete fileInput;
    fileInput = nullptr;

    ::close(fd);
    fd = 0;
  }
}

Reader::Reader(const InputSource &inputSource)
{
  if (inputSource.isFile() || (inputSource.isResource() && hasFileResources()))
  {
    fd = open(inputSource.getFilePath().string().data(), O_RDONLY);

    if (fd > 0)
    {
      fileInput = new protobuf::io::FileInputStream(fd);
      zipInput = new protobuf::io::GzipInputStream(fileInput);
      codedInput = new protobuf::io::CodedInputStream(zipInput);
    }
  }
  else if (inputSource.isResource() && hasMemoryResources())
  {
    resourceBuffer = chr::getResourceBuffer(inputSource.getRelativePath());

    if (resourceBuffer)
    {
      arrayInput = new protobuf::io::ArrayInputStream(resourceBuffer->data(), resourceBuffer->size());
      zipInput = new protobuf::io::GzipInputStream(arrayInput);
      codedInput = new protobuf::io::CodedInputStream(zipInput);
    }
  }
  else if (inputSource.isBuffer())
  {
    arrayInput = new protobuf::io::ArrayInputStream(inputSource.getData(), inputSource.getDataSize());
    zipInput = new protobuf::io::GzipInputStream(arrayInput);
    codedInput = new protobuf::io::CodedInputStream(zipInput);
  }
}

u16string Reader::readJavaString(size_t size)
{
  u16string buffer;
  buffer.resize(size);

  if (good())
  {
    if (codedInput->ReadRaw(&buffer[0], size * 2))
    {
      for (int i = 0; i < size; i++)
      {
        buffer[i] = (buffer[i] & 0xff00) >> 8 | (buffer[i] & 0xff) << 8;
      }
    }
  }
  else
  {
    fail();
  }

  return buffer;
}

int8_t Reader::readJavaByte()
{
  if (good())
  {
    uint8_t buffer;

    if (codedInput->ReadRaw(&buffer, 1))
    {
      return buffer;
    }
    else
    {
      fail();
    }
  }

  return 0;
}

int16_t Reader::readJavaShort()
{
  if (good())
  {
    uint8_t buffer[2];

    if (codedInput->ReadRaw(&buffer[0], 2))
    {
      return buffer[1] | buffer[0] << 8;
    }
    else
    {
      fail();
    }
  }

  return 0;
}

char16_t Reader::readJavaChar()
{
  if (good())
  {
    uint8_t buffer[2];

    if (codedInput->ReadRaw(&buffer[0], 2))
    {
      return buffer[1] | buffer[0] << 8;
    }
    else
    {
      fail();
    }
  }

  return 0;
}

int32_t Reader::readJavaInt()
{
  if (good())
  {
    uint8_t buffer[4];

    if (codedInput->ReadRaw(&buffer[0], 4))
    {
      return buffer[3] | buffer[2] << 8 | buffer[1] << 16 | buffer[0] << 24;
    }
    else
    {
      fail();
    }
  }

  return 0;
}

int64_t Reader::readJavaLong()
{
  if (good())
  {
    uint8_t buffer[8];

    if (codedInput->ReadRaw(&buffer[0], 8))
    {
      uint32_t part1 = buffer[3] | buffer[2] << 8 | buffer[1] << 16 | buffer[0] << 24;
      uint32_t part2 = buffer[7] | buffer[6] << 8 | buffer[5] << 16 | buffer[4] << 24;

      return uint64_t(part2) | uint64_t(part1) << 32;
    }
    else
    {
      fail();
    }
  }

  return 0;
}

void Reader::skip(int count)
{
  if (good())
  {
    if (!codedInput->Skip(count))
    {
      fail();
    }
  }
}

bool Reader::good() const
{
  return codedInput && !failed;
}

void Reader::fail()
{
  failed = true;
}

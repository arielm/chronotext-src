#include "LineProvider.h"

#include "chr/utils/Utils.h"

using namespace std;
using namespace chr;

LineProvider::LineProvider(XFont &font, float fontSize, const InputSource &inputSource)
{
  lines = utils::readLines<u16string>(inputSource);

  size = lines.size();
  linesLen.assign(size + 1, 0);
  charsLen.assign(size, vector<float>());

  float total = 0;
  font.setSize(fontSize);

  for (int i = 0; i < size; i++)
  {
    const u16string &line = lines[i];
    int lineSize = line.size();
    
    charsLen[i].assign(lineSize + 2, 0);
    charsLen[i][0] = total;
    
    for (int j = 0; j < lineSize; j++)
    {
      total += font.getCharAdvance(line[j]);
      charsLen[i][j + 1] = total;
    }
    
    total += font.getCharAdvance(u' ');

    charsLen[i][lineSize + 1] = total;
    linesLen[i + 1] = total;
  }
}

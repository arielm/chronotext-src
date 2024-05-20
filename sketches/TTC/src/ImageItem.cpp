#include "ImageItem.h"
#include "UI.h"

#include "chr/gl/draw/Rect.h"

using namespace std;
using namespace chr;
using namespace gl;
using namespace math;

ImageItem::ImageItem(shared_ptr<UI> ui, const string &name)
:
ui(ui),
name(name)
{}

void ImageItem::setBounds(const Rectf &bounds)
{
  this->bounds = bounds;
}

void ImageItem::draw()
{
  const auto &texture = ui->textures[name];
  auto &batch = ui->batchMap.getBatch(texture);

  draw::Rect()
    .setColor(1, 1, 1, 1)
    .setBounds(bounds)
    .setTextureScale(bounds.height() / texture.height)
    .append(batch);
}

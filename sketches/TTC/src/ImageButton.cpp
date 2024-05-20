#include "ImageButton.h"
#include "UI.h"

#include "chr/gl/draw/Rect.h"

using namespace std;
using namespace chr;
using namespace math;

ImageButton::ImageButton(shared_ptr<UI> ui, const string &name)
:
Button(ui),
name(name)
{}

void ImageButton::draw()
{
  Button::draw();

  const auto &texture = ui->textures[name];
  auto &batch = ui->batchMap.getBatch(texture);

  draw::Rect()
    .setColor(color_body)
    .setBounds(bounds)
    .setTextureScale(bounds.height() / texture.height)
    .append(batch);
}

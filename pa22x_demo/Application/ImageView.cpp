#include "ImageView.h"

ImageView::ImageView(QWidget *parent) :
    QWidget(parent),
    _identifier("ImageView_?"),
    _rotation(kImageViewDrawHorizontal),
    _colorMode(kImageViewColorMode0)
{
}

ImageView::~ImageView()
{
}

void ImageView::setIdentifier(QString identifier)
{
    _identifier = identifier;
}

QString& ImageView::getIdentifier()
{
    return _identifier;
}

void ImageView::mouseReleaseEvent(QMouseEvent *)
{
    emit toggled();
}

void ImageView::paintEvent(QPaintEvent *)
{

}

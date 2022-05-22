
#include "Ruler.h"
#include "SmartRuler.h"

//////////////////////////////////////////////////////////

Ruler::Ruler(QWidget *parent)
    : QWidget(parent)
{
}

Qt::AnchorPoint Ruler::getAnchorPoint() const
{
    return anchor;
}

void Ruler::setAnchorPoint(Qt::AnchorPoint anchor)
{
    this->anchor = anchor;
    update();
}

void Ruler::setRange(double begin, double end)
{
    this->begin = begin;
    this->end = end;
    update();
}

void Ruler::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    QFont f;
    f.setPixelSize(9);
    p.setFont(f);
    DrawSmartRuler(p, rect(), begin, end, anchor);
}

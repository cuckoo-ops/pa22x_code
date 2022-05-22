#include "PAImageView.h"

QColor Value2Color2(unsigned int v)
{
    if (/*0 <=v && */v<=84)
        return QColor(254-3*v, 254-3*v, 255);
    else if (85 <=v && v<=170)
        return QColor(0, 3*v-255, 510-3*v);
    else
        return QColor(3*v-510,765-3*v,0);
}


QColor Value2Color1(unsigned int v)
{
    if (/*0 <= v &&*/ v <= 63)
        return QColor(0, 254-4*v, 255);
    else if (64 <= v && v <= 127)
        return QColor(0, 4*v-254, 510-4*v);
    else if (128 <= v && v <= 191)
        return QColor(4*v-510, 255, 0);
    else
        return QColor(255, 1022-4*v, 0);
}

QColor Value2Color3(unsigned int v)
{
    if (85 < v && v < 172)
        return QColor(0, (v-86)*3, 255-(v-86)*3);
    else if(v < 86)
        return QColor(0, 0, v * 3);
    else if (171 < v && v < 256)
        return QColor((v-171)*3, 255-(v-171)*3, 0);
    else
        return QColor(0,0,0);
}

QColor hsv2rgb(double h, double s, double v)
{
    s = s / 100.0;
    v = v / 100.0;
    double c = v * s;
    double x = c * (1 - std::abs(fmod(h / 60,2) - 1));

    double m = v - c;
    double r = 0, g = 0, b = 0;

    if(/*( h >= 0 ) &&*/ ( h < 60 ))
    {
        r = c;
        g = x;
        b = 0;
    }
    else if(( h >= 60 ) && ( h < 120 ))
    {
        r = x;
        g = c;
        b = 0;
    }
    else if(( h >= 120 ) && ( h < 180 ))
    {
        r = 0;
        g = c;
        b = x;
    }
    else if(( h >= 180 ) && ( h < 240 ))
    {
        r = 0;
        g = x;
        b = c;
    }
    else if(( h >= 240 ) && ( h < 300 ))
    {
        r = x;
        g = 0;
        b = c;
    }
    else if(( h >= 300 ) && ( h < 360 ))
    {
        r = c;
        g = 0;
        b = x;
    }

    r = (r + m) * 255 + 0.5;//round((r + m) * 255);
    g = (g + m) * 255 + 0.5;//round((g + m) * 255);
    b = (b + m) * 255 + 0.5;//round((b + m) * 255);

    return QColor((int)r, (int)g, (int)b);
}


QColor Value2Color5(unsigned int value)
{
    double h = 0;
    double s = 0;
    double v = 0;

const int SEG0_BASE = 0;
const int SEG0_TOP = 125;
const int SEG0_SPAN = (SEG0_TOP - SEG0_BASE + 1);
const int SEG1_BASE = 126;
const int SEG1_TOP = 187;
const int SEG1_SPAN = (SEG1_TOP - SEG1_BASE + 1);
const int SEG2_BASE = 188;
const int SEG2_TOP  = 255;
const int SEG2_SPAN = (SEG2_TOP - SEG2_BASE + 1);
    if ((value >= SEG0_BASE) && (value <= SEG0_TOP))
    {
        h = 240 - (24.0 / SEG0_SPAN) * (value - SEG0_BASE);
        s = (73.0 / SEG0_SPAN) * (value - SEG0_BASE);
        v = 100 - (21.0 / SEG0_SPAN) * (value - SEG0_BASE); // 51.0 => 21.0
    }
    else if ((value >= SEG1_BASE) && (value <= SEG1_TOP))
    {
        h = 141 - (84.0 / SEG1_SPAN) * (value - SEG1_BASE);
        s = 86 - (15.0 / SEG1_SPAN) * (value - SEG1_BASE);
        v = 55 + (36.0 / SEG1_SPAN) * (value - SEG1_BASE);
    }
    else if ((value >= SEG2_BASE) && (value <= SEG2_TOP))
    {
        h = 1;
        s = 86 + (13.0 / SEG2_SPAN) * (value - SEG2_BASE);
        v = 82 + (17.0 / SEG2_SPAN) * (value - SEG2_BASE);
    }
     return hsv2rgb(h, s, v);
}

QColor Value2Color4(unsigned int value)
{
    double h = 0;
    double s = 0;
    double v = 0;

    if (/*(value >= 0) &&*/ (value <= 60))
    {
        h = 240 - (24.0 / 61) * (value - 0);
        s = (73.0 / 61) * (value - 0);
        v = 100 - (51.0 / 61) * (value - 0);
    }
    else if ((value >= 61) && (value <= 102))
    {
        h = 216 - (75.0 / 42) * (value - 61);
        s = 73 + (7.0 / 42) * (value - 61);
        v = 49 + (6.0 / 42) * (value - 61);
    }
    else if ((value >= 103) && (value <= 179))
    {
        h = 141 - (84.0 / 76) * (value - 103);
        s = 86 - (15.0 / 76) * (value - 103);
        v = 55 + (36.0 / 76) * (value - 103);
    }
    else if ((value >= 180) && (value <= 230))
    {
        h = 57 - (56.0 / 50) * (value - 180);
        s = 71 + (5.0 / 50) * (value - 180);
        v = 91 - (9.0 / 50) * (value - 180);
    }
    else if ((value >= 231) && (value <= 255))
    {
        h = 1;
        s = 86 + (13.0 / 25) * (value - 231);
        v = 82 + (17.0 / 25) * (value - 231);
    }
     return hsv2rgb(h, s, v);
}

PAImageView::PAImageView(QWidget *parent) :
    ImageView(parent),
    _display_image(512, 400, QImage::Format_Indexed8),
    _x0(-1), _x1(-1), _y0(-1), _y1(-1)
{
    setColorMode(_colorMode);
}

PAImageView::~PAImageView()
{
}

void PAImageView::setData(void* data)
{
    _pa_image = (unsigned char*)data;
    for (int i = 0; i < 400; i++) {
        memcpy(_display_image.scanLine(399 - i), _pa_image + i * 512, 512);
    }
}

void PAImageView::setBeamMark(double x0, double y0, double x1, double y1)
{
    _x0 = x0;
    _y0 = y0;
    _x1 = x1;
    _y1 = y1;
}

void PAImageView::setColorMode(ImageViewColorMode mode)
{
    _colorMode = mode;
    if (_colorMode == kImageViewColorMode0) {
        _display_image.setColorCount(256);
        for (int i = 0; i < 255; i++) {
            _display_image.setColor(i, Value2Color5(i).rgb());
        }
    }
    else if (_colorMode == kImageViewColorMode1) {
        _display_image.setColorCount(256);
        for (int i = 0; i < 255; i++) {
            _display_image.setColor(i, QColor(i, i, i).rgb());
        }
    }

}

void PAImageView::draw(ImageViewDrawRotation rotation,
                       QString& rectification)
{
    _rotation = rotation;
    _rectification = rectification;
    update();
}

void PAImageView::paintEvent(QPaintEvent *e)
{
    if (_pa_image) {
        QPainter p(this);
        QRectF target(0, 0, this->geometry().width(), this->geometry().height());
        QRectF source(0, 0, 512.0, 400.0);
        p.drawImage(target, _display_image, source);
        p.setPen(QColor(255, 0, 0));
        p.drawLine(_x0, _y0, _x1, _y1);
    }
}


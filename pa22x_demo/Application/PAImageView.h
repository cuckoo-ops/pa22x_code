#ifndef PAIMAGEVIEW_H
#define PAIMAGEVIEW_H
#include <QPainter>
#include "ImageView.h"
#include "PA22XClient.h"

struct PABeamVertex {
    double x0;
    double y0;
    double x1;
    double y1;
};

struct PASViewParameter {
    PABeamVertex beam_vertexs[UTShadowDevice::_nb_pa_beams];
};

class PAImageView : public ImageView
{
    Q_OBJECT

public:
    explicit PAImageView(QWidget *parent = 0);
    ~PAImageView();
    virtual void setData(void* data) override;
    virtual void setColorMode(ImageViewColorMode mode) override;
    virtual void draw(ImageViewDrawRotation rotation,
                      QString& rectification) override;
    void setBeamMark(double x0, double y0, double x1, double y1);

protected:
    void paintEvent(QPaintEvent *) override;
private:

    unsigned char* _pa_image;
    QImage _display_image;
    double _x0, _y0, _x1, _y1, _angle;
};

#endif // PAIMAGEVIEW_H

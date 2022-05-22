#ifndef PEIMAGEVIEW_H
#define PEIMAGEVIEW_H
#include <QPainter>
#include "ImageView.h"
#include "PA22XClient.h"

class PEImageView : public ImageView
{
    Q_OBJECT

public:
    explicit PEImageView(QWidget *parent = 0);
    ~PEImageView();
    virtual void setData(void* data) override;
    virtual void setColorMode(ImageViewColorMode mode) override;
    virtual void draw(ImageViewDrawRotation rotation,
                      QString& rectification) override;
    void setGate(int start, int width, int height, QString name);

protected:
    void paintEvent(QPaintEvent *) override;

private:
    struct {
        int start;
        int width;
        int height;
    } gate[4];
    X22_Waveform* _wave_form;
};

#endif // PEIMAGEVIEW_H

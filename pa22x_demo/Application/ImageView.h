#ifndef IMAGEVIEW_H
#define IMAGEVIEW_H

#include <QWidget>
#include <QString>

typedef enum _ImageViewDrawRotation {
    kImageViewDrawHorizontal,
    kImageViewDrawVertical,
} ImageViewDrawRotation;

typedef enum _ImageViewColorMode {
    kImageViewColorMode0 = 0,
    kImageViewColorMode1 = 1,
} ImageViewColorMode;


class ImageView : public QWidget
{
    Q_OBJECT

public:
    explicit ImageView(QWidget *parent = 0);
    ~ImageView();
    void setIdentifier(QString identifier);
    QString& getIdentifier();
    virtual void setData(void* data) = 0;
    virtual void setColorMode(ImageViewColorMode mode) = 0;
    virtual void draw(ImageViewDrawRotation rotation,
                      QString& rectification) = 0;

protected:
    ImageViewDrawRotation _rotation;
    ImageViewColorMode _colorMode;
    QString _rectification;
    void mouseReleaseEvent(QMouseEvent *) override;
    void paintEvent(QPaintEvent *) override;
    QString _identifier;
signals:
    void toggled();
};

#endif // PAIMAGEVIEW_H

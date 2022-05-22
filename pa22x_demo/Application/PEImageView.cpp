#include "PEImageView.h"


PEImageView::PEImageView(QWidget *parent) :
    ImageView(parent),
    _wave_form(nullptr)
{
}

PEImageView::~PEImageView()
{
}

void PEImageView::setData(void* data)
{
    _wave_form = (X22_Waveform*)data;
}

void PEImageView::setGate(int start, int width, int height, QString name)
{
    if (name == "a") {
        gate[0].start = start;
        gate[0].width = width;
        gate[0].height = height;
    }
    else if (name == "b") {
        gate[1].start = start;
        gate[1].width = width;
        gate[1].height = height;
    }
    else if (name == "c") {
        gate[2].start = start;
        gate[2].width = width;
        gate[2].height = height;
    }
    else if (name == "d") {
        gate[3].start = start;
        gate[3].width = width;
        gate[3].height = height;
    }
}

void PEImageView::draw(ImageViewDrawRotation rotation,
                       QString& rectification)
{
    _rotation = rotation;
    _rectification = rectification;
    update();
}

void PEImageView::setColorMode(ImageViewColorMode mode)
{
    _colorMode = mode;
}

void PEImageView::paintEvent(QPaintEvent *e)
{
    if (_wave_form) {
        int width = this->size().width();
        int height = this->size().height();

        QPainter p(this);
        p.setWindow(QRect(0, 0, width, height));
        QPen pen(QColor(255, 255, 255));
        pen.setWidth(0);
        p.setPen(pen);

        if (_rotation == kImageViewDrawHorizontal) {
            uint8_t P[448];
            uint8_t N[448];

            for (int i = 0; i < 448; i++) {
                P[i] = _wave_form->waveP[i];
                N[i] = _wave_form->waveN[i];
            }

            if (_rectification == "rf") {

                for (int i = 0; i < 448; i++) {
                    P[i] /=2;
                    N[i] /=2;
                }

                for (int i = 1; i < 448; i++) {
                    unsigned char pre = 0;
                    unsigned char current = 0;

                    if (P[i-1] == 0 && N[i-1] == 0) {
                        pre = 128;
                    }
                    else if (P[i-1] < N[i-1]) {
                        pre = N[i-1] + 128;
                    }
                    else if (P[i-1] >= N[i-1]) {
                        pre = 256 - (P[i-1]+128);
                    }

                    if (P[i] == 0 && N[i] == 0) {
                        current = 128;
                    }
                    else if (P[i] < N[i]) {
                        current = N[i]+128;
                    }
                    else if (P[i] >= N[i]) {
                        current = 256 - (P[i]+128);
                    }

                    unsigned char append = 128;
                    if (P[i] != 0 && N[i] != 0) {
                        if (P[i] > N[i]) {
                            append = N[i]+128;
                        }
                        else if (P[i] < N[i]) {
                            append = 256 - (P[i]+128);
                        }
                    }

                    p.drawLine(i - 1, pre, i, current);
                    if (append != 128)
                        p.drawLine(i, 128, i, append);

                }
            }
            else {
                for (int i = 1; i < 448; i++)
                    p.drawLine(i - 1, (height - P[i-1]), i, (height - P[i]));
            }
            p.setPen(QColor(255, 0, 0));
            p.drawLine(gate[0].start, gate[0].height, gate[0].start + gate[0].width, gate[0].height);
            p.setPen(QColor(255, 255, 0));
            p.drawLine(gate[1].start, gate[1].height, gate[1].start + gate[1].width, gate[1].height);
            p.setPen(QColor(255, 0, 255));
            p.drawLine(gate[2].start, gate[2].height, gate[2].start + gate[2].width, gate[2].height);
            p.setPen(QColor(0, 255, 0));
            p.drawLine(gate[3].start, gate[3].height, gate[3].start + gate[3].width, gate[3].height);
        }
        else if (_rotation == kImageViewDrawVertical) {
            for (int i = 1; i < 448; i++)
                p.drawLine((unsigned char)_wave_form->waveP[i-1], i - 1, (unsigned char)_wave_form->waveP[i], i);

            p.setPen(QColor(255, 0, 0));
            p.drawLine(gate[0].height, gate[0].start, gate[0].height, gate[0].start + gate[0].width);
            p.setPen(QColor(255, 255, 0));
            p.drawLine(gate[1].height, gate[1].start, gate[1].height, gate[1].start + gate[1].width);
            p.setPen(QColor(255, 0, 255));
            p.drawLine(gate[2].height, gate[2].start, gate[2].height, gate[2].start + gate[2].width);
            p.setPen(QColor(0, 255, 0));
            p.drawLine(gate[3].height, gate[3].start, gate[3].height, gate[3].start + gate[3].width);
        }

    }
}



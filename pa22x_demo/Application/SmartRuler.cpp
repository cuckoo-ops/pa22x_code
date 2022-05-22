
#include "SmartRuler.h"
#include <cmath>
#include <algorithm>
#include <QRect>
#include <QPainter>
#include <QDebug>
#include "Platform.h"

namespace
{
    const double kMark[] =
    {
        1.0,
        2.0,
        5.0
    };

    double FirstMark(double beginValue, double interval)
    {
        double lf = beginValue / interval;
        if (lf > 0)
        {
            lf += 1.0;
        }
        return static_cast<int>(lf) * interval;
    }

    double LastMark(double endValue, double interval)
    {
        double lf = endValue / interval;
        if (lf < 0)
        {
            lf -= 1.0;
        }
        return static_cast<int>(lf) * interval;
    }

    struct SmartRulerOptions
    {
        double firstMark;
        double lastMark;
        double interval;
        double pixelValue;

        SmartRulerOptions(QPainter& p, int length, double beginValue, double endValue)
        {
            double range = endValue - beginValue;
            pixelValue = range / length;
            if (pixelValue == 0.0)
                pixelValue = 0.0001;

            double ratio = 1.0;
            double val = std::abs(pixelValue);

            while (val < 0.1)
            {
                val *= 10.0;
                ratio /= 10.0;
            }
            while (val > 1.0)
            {
                val /= 10.0;
                ratio *= 10.0;
            }

            int x = 1;
            bool found  = false;

            interval = 1.0;
            while (!found)
            {
                for (int i = 0; i < (int)_countof(kMark); i++)
                {
                    interval = kMark[i] * x;
                    firstMark = FirstMark(beginValue, interval * ratio);
                    lastMark = LastMark(endValue, interval * ratio);
                    int firstW = p.fontMetrics().width(QString("%0").arg(firstMark + interval * ratio));
                    int lastW = p.fontMetrics().width(QString("%0").arg(lastMark + interval * ratio));
                    int width = std::max(firstW, lastW);
                    if (interval / val > width * 1.5)
                    {
                        found = true;
                        break;
                    }
                }
                x *= 10;
            }
            interval *= ratio;
            if (beginValue > endValue)
                interval = -interval;
        }
    };

    void DrawTopRuler(QPainter& p, int length, double beginValue, double endValue)
    {
        int h = p.fontMetrics().height() / 2;
        const SmartRulerOptions opt(p, length, beginValue, endValue);
        for (double pos = opt.firstMark; ; pos += opt.interval)
        {
            int px = int((pos - beginValue) / opt.pixelValue + 0.5);
            if (px > length)
            {
                break;
            }
            p.drawLine(px, 0, px, -h);
            QString txt;
            txt.setNum(pos);
            QRect txtrt = p.fontMetrics().boundingRect(txt);
            txtrt.translate(QPoint(px - txtrt.width() / 2, -h - txtrt.bottom()));
            if (txtrt.left() < 0)
                txtrt.translate(-txtrt.left(), 0);
            if (txtrt.right() > length)
                txtrt.translate(length - txtrt.right(), 0);
            p.drawText(txtrt, txt);
        }
    }

    void DrawBottomRuler(QPainter& p, int length, double beginValue, double endValue)
    {
        int h = p.fontMetrics().height() / 2;
        const SmartRulerOptions opt(p, length, beginValue, endValue);
        for (double pos = opt.firstMark; ; pos += opt.interval)
        {
            int px = int((pos - beginValue) / opt.pixelValue + 0.5);
            if (px > length)
            {
                break;
            }
            p.drawLine(px, 0, px, h);
            QString txt;
            txt.setNum(pos);
            QRect txtrt = p.fontMetrics().boundingRect(txt);
            txtrt.translate(QPoint(px - txtrt.width() / 2, h - txtrt.top()));
            if (txtrt.left() < 0)
                txtrt.translate(-txtrt.left(), 0);
            if (txtrt.right() > length)
                txtrt.translate(length - txtrt.right(), 0);
            p.drawText(txtrt, txt);
        }
    }
}

void DrawSmartRuler(QPainter& p, const QRect& r, double beginValue, double endValue, Qt::AnchorPoint anchor)
{
    switch (anchor)
    {
    case Qt::AnchorTop:
        p.save();
        p.translate(r.bottomLeft());
        DrawTopRuler(p, r.width() - 1, beginValue, endValue);
        p.restore();
        break;
    case Qt::AnchorRight:
        p.save();
        p.translate(r.topLeft());
        p.rotate(90);
        DrawTopRuler(p, r.height() - 1, beginValue, endValue);
        p.restore();
        break;
    case Qt::AnchorBottom:
        p.save();
        p.translate(r.topLeft());
        DrawBottomRuler(p, r.width() - 1, beginValue, endValue);
        p.restore();
        break;
    case Qt::AnchorLeft:
        p.save();
        p.translate(r.topRight());
        p.rotate(90);
        DrawBottomRuler(p, r.height() - 1, beginValue, endValue);
        p.restore();
        break;
    default:
        break;
    }
}



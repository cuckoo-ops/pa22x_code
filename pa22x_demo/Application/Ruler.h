
#include <QWidget>

#pragma once

class Ruler
    : public QWidget
{
    Q_OBJECT

private:
    Qt::AnchorPoint anchor;
    double begin {0.0};
    double end {100.0};

public:
    Ruler(QWidget* parent);
    Qt::AnchorPoint getAnchorPoint() const;
    void setAnchorPoint(Qt::AnchorPoint);
    void setRange(double begin, double end);

protected:
    void paintEvent(QPaintEvent *) override;
};

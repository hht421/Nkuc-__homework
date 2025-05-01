#ifndef PLATFORM_H
#define PLATFORM_H

#include <QGraphicsRectItem>

class Platform : public QGraphicsRectItem {
public:
    Platform(double x, double y, double width, double height);
    bool isColliding(const QGraphicsItem *item) const;
};

#endif // PLATFORM_H

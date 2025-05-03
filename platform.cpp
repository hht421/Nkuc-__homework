#include "platform.h"
#include <QBrush>
#include <QPen>

Platform::Platform(double x, double y, double width, double height)
    : QGraphicsRectItem(0, 0, width, height)  // 初始化基类
{
    setPos(x, y);

    // 设置填充色为透明
    QBrush brush(Qt::transparent);
    setBrush(brush);

    // 设置边框为透明
    QPen pen(Qt::transparent);
    pen.setWidth(2);
    setPen(pen);
}

bool Platform::isColliding(const QGraphicsItem *item) const
{
    // 获取平台和物体的边界矩形
    QRectF platformRect = rect();
    platformRect.translate(pos());
    QRectF itemRect = item->boundingRect();
    itemRect.translate(item->pos());

    // 检查是否有重叠
    return platformRect.intersects(itemRect);
}

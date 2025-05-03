#include "swordqi.h"
#include <QTimer>
#include <QGraphicsScene>
#include <QTransform>

SwordQi::SwordQi(bool facingRight, bool isPlayer1, QGraphicsItem *parent)
    : QObject(), QGraphicsPixmapItem(parent), facingRight(facingRight), isPlayer1(isPlayer1)
{
    QPixmap pixmap;
    // 根据玩家选择不同的剑气图片
    if (isPlayer1) {
        pixmap = QPixmap(":/image/weapon/jianqi-1(2).png"); // 红色剑气
    } else {
        pixmap = QPixmap(":/image/weapon/jianqi-1(1).png"); // 蓝色剑气
    }

    // 根据朝向反转图片
    if (!facingRight) {
        pixmap = pixmap.transformed(QTransform().scale(-1, 1));
    }
    
    setPixmap(pixmap);
    
    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &SwordQi::move);
    timer->start(50); // 每50毫秒移动一次
}

SwordQi::~SwordQi()
{
    // 析构函数
}

void SwordQi::move()
{
    if (facingRight) {
        setPos(x() + speed, y());
    } else {
        setPos(x() - speed, y());
    }

    // 检查是否离开主区域
    if (x() < 0 || x() > 800) {
        scene()->removeItem(this);
        delete this;
    }
}
#ifndef SWORDQI_H
#define SWORDQI_H

#include <QGraphicsPixmapItem>
#include <QObject>

class SwordQi : public QObject, public QGraphicsPixmapItem {
    Q_OBJECT
public:
    SwordQi(bool facingRight, bool isPlayer1, QGraphicsItem *parent = nullptr);
    ~SwordQi() override;

    // 获取剑气所属玩家
    bool belongsToPlayer1() const { return isPlayer1; }
    
    // 获取和设置伤害值
    int getDamage() const { return damage; }
    void setDamage(int newDamage) { damage = newDamage; }

public slots:
    void move();

private:
    bool facingRight;
    bool isPlayer1;  // 用于区分玩家1和玩家2
    const int speed = 20; // 剑气移速
    int damage = 3; // 默认伤害值为3
};

#endif // SWORDQI_H
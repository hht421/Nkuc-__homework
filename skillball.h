#ifndef SKILLBALL_H
#define SKILLBALL_H

#include <QGraphicsPixmapItem>
#include <QObject>
#include <QTimer>

class SkillBall : public QObject, public QGraphicsPixmapItem {
    Q_OBJECT
public:
    enum SkillType {
        HEAL,           // 回复生命
        SPEED_BOOST,    // 移动速度提升
        JUMP_BOOST,     // 跳跃高度提升
        FREE_SWORD_QI,  // 剑气不消耗充能
        SHORT_ATTACK_BOOST, // 短时间加近战伤害
        PERMANENT_ATTACK_BOOST, // 永久增加伤害
        INCREASE_CURRENT_HP, // 增加现有生命值
        INCREASE_MAX_HP, // 增加最大生命值
        SHORT_JUMP_BOOST, // 短时间跳跃增强
        PERMANENT_JUMP_BOOST, // 永久跳跃增强
        PERMANENT_SPEED_BOOST // 永久速度增加
    };

    SkillBall(SkillType type, QGraphicsItem *parent = nullptr);
    SkillType getType() const { return type; }

private:
    SkillType type;
    QTimer *blinkTimer;
    bool visible;

private slots:
    void blink();
};
#endif // SKILLBALL_H 
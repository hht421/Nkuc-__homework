#include "skillball.h"
#include <QPixmap>

SkillBall::SkillBall(SkillType type, QGraphicsItem *parent)
    : QObject(), QGraphicsPixmapItem(parent), type(type), visible(true)
{
    // 根据技能类型设置不同的图片
    QString imagePath;
    switch(type) {
        case HEAL:
            imagePath = ":/image/skillball/heal_blue.png";
            break;
        case SPEED_BOOST:
            imagePath = ":/image/skillball/speed_blue.png";
            break;
        case JUMP_BOOST:
            imagePath = ":/image/skillball/jump_blue.png";
            break;
        case FREE_SWORD_QI:
            imagePath = ":/image/skillball/swordair_limitless.png";
            break;
        case SHORT_ATTACK_BOOST:
            imagePath = ":/image/skillball/attack_more_blue.png";
            break;
        case PERMANENT_ATTACK_BOOST:
            imagePath = ":/image/skillball/attack_more_red.png";
            break;
        case INCREASE_CURRENT_HP:
            imagePath = ":/image/skillball/heal_blue.png";
            break;
        case INCREASE_MAX_HP:
            imagePath = ":/image/skillball/heal_red.png";
            break;
        case SHORT_JUMP_BOOST:
            imagePath = ":/image/skillball/jump_blue.png";
            break;
        case PERMANENT_JUMP_BOOST:
            imagePath = ":/image/skillball/jump_red.png"; 
            break;
        case PERMANENT_SPEED_BOOST: 
            imagePath = ":/image/skillball/speed_red.png";
            break;
    }

    setPixmap(QPixmap(imagePath).scaled(50, 50, Qt::KeepAspectRatio, Qt::SmoothTransformation));

    // 设置闪烁效果
    blinkTimer = new QTimer(this);
    connect(blinkTimer, &QTimer::timeout, this, &SkillBall::blink);
    blinkTimer->start(500); // 每500毫秒闪烁一次
}

void SkillBall::blink()
{
    visible = !visible;
    setOpacity(visible ? 1.0 : 0.3);
}
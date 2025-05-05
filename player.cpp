#include "player.h"
#include "swordqi.h"
#include "skillball.h"
#include <QGraphicsScene>
#include <QPixmap>
#include <QDir>

// 基类：Player
Player::Player(bool isPlayer1) : QObject(), QGraphicsPixmapItem()
{
    // 初始化属性
    moveSpeed = 3.0f;     // 移动速度
    jumpForce = 7.0f;     // 跳跃力
    gravity = 0.2f;       // 重力
    maxFallSpeed = 15.0f;
    health = 100;
    isGrounded = false;   // 初始状态设为未着地
    verticalSpeed = 0.0f;
    currentWeaponIndex = 0;
    facingRight = true;
    this->isPlayer1 = isPlayer1;  // 设置玩家标识

    // 初始化武器
    weapons.push_back(std::make_unique<sword>());
    weapons.push_back(std::make_unique<gun>());
    
    // 初始化动画相关
    currentFrame = 0;
    totalFrames = 10;  // 每个动画序列都有10帧
    isMoving = false;
    isJumping = false;
    isAttacking = false;
    isDead = false;
    canAttack = true;  // 初始可以攻击
    
    // 创建动画定时器
    animationTimer = new QTimer(this);
    QObject::connect(animationTimer, &QTimer::timeout, this, &Player::updateAnimation);
    animationTimer->start(50); // 每50毫秒更新一次动画（提高帧率）
    
    // 创建攻击冷却定时器
    attackCooldownTimer = new QTimer(this);
    QObject::connect(attackCooldownTimer, &QTimer::timeout, [this]() {
        canAttack = true;  // 冷却结束，可以再次攻击
    });
    
    // 初始化技能相关
    speedBoostActive = false;
    jumpBoostActive = false;
    freeSwordQiActive = false;
    
    originalMoveSpeed = moveSpeed;
    originalJumpForce = jumpForce;
    
    // 创建技能计时器
    speedBoostTimer = new QTimer(this);
    jumpBoostTimer = new QTimer(this);
    freeSwordQiTimer = new QTimer(this); 
    
    connect(speedBoostTimer, &QTimer::timeout, this, &Player::removeSpeedBoost);
    connect(jumpBoostTimer, &QTimer::timeout, this, &Player::removeJumpBoost);
    connect(freeSwordQiTimer, &QTimer::timeout, this, &Player::removeFreeSwordQi);
    
    // 加载动画帧
    loadAnimationFrames();
}

Player::~Player()
{
    // 智能指针会自动清理武器
    delete animationTimer;
    delete attackCooldownTimer;
    delete speedBoostTimer;
    delete jumpBoostTimer;
    delete freeSwordQiTimer;
}

void Player::loadAnimationFrames()
{
    // 加载所有动画序列
    QString prefix = ":/image/person1/person1_";
    
    // 加载行走动画帧
    for (int i = 1; i <= 10; i++) {
        QString path = QString("%1Walk (%2).png").arg(prefix).arg(i);
        QPixmap frame(path);
        if (!frame.isNull()) {
            frame = frame.scaled(100, 100, Qt::KeepAspectRatio, Qt::SmoothTransformation);
            walkFrames.push_back(frame);
        }
    }
    
    // 加载静止动画帧
    for (int i = 1; i <= 10; i++) {
        QString path = QString("%1Idle (%2).png").arg(prefix).arg(i);
        QPixmap frame(path);
        if (!frame.isNull()) {
            frame = frame.scaled(100, 100, Qt::KeepAspectRatio, Qt::SmoothTransformation);
            idleFrames.push_back(frame);
        }
    }
    
    // 加载跳跃动画帧
    for (int i = 1; i <= 10; i++) {
        QString path = QString("%1Jump (%2).png").arg(prefix).arg(i);
        QPixmap frame(path);
        if (!frame.isNull()) {
            frame = frame.scaled(100, 100, Qt::KeepAspectRatio, Qt::SmoothTransformation);
            jumpFrames.push_back(frame);
        }
    }
    
    // 加载攻击动画帧
    for (int i = 1; i <= 10; i++) {
        QString path = QString("%1Attack (%2).png").arg(prefix).arg(i);
        QPixmap frame(path);
        if (!frame.isNull()) {
            frame = frame.scaled(100, 100, Qt::KeepAspectRatio, Qt::SmoothTransformation);
            attackFrames.push_back(frame);
        }
    }
    
    // 加载死亡动画帧
    for (int i = 1; i <= 10; i++) {
        QString path = QString("%1Dead (%2).png").arg(prefix).arg(i);
        QPixmap frame(path);
        if (!frame.isNull()) {
            frame = frame.scaled(100, 100, Qt::KeepAspectRatio, Qt::SmoothTransformation);
            deadFrames.push_back(frame);
        }
    }
    
    // 设置初始图片
    if (!idleFrames.empty()) {
        setPixmap(idleFrames[0]);
    }
}

void Player::updateAnimation()
{
    if (isDead) {
        // 死亡动画只播放一次
        if (currentFrame < deadFrames.size() - 1) {
            currentFrame++;
            setAnimationFrame(currentFrame);
        }
    } else if (isAttacking) {
        // 攻击动画
        currentFrame = (currentFrame + 1) % attackFrames.size();
        if (currentFrame == 0) { // 当攻击动画回到第一帧时，结束攻击状态
            isAttacking = false;  // 攻击动画播放完毕
            hasHit = false;       // 重置命中标记
        }
        setAnimationFrame(currentFrame);
    } else if (isJumping) {
        // 跳跃动画
        currentFrame = (currentFrame + 1) % jumpFrames.size();
        setAnimationFrame(currentFrame);
    } else if (isMoving) {
        // 移动动画
        currentFrame = (currentFrame + 1) % walkFrames.size();
        setAnimationFrame(currentFrame);
    } else {
        // 静止动画
        currentFrame = (currentFrame + 1) % idleFrames.size();
        setAnimationFrame(currentFrame);
    }
}

void Player::setAnimationFrame(int frameIndex)
{
    QPixmap currentPixmap;
    
    if (isDead && !deadFrames.empty()) {
        currentPixmap = deadFrames[frameIndex];
    } else if (isAttacking && !attackFrames.empty()) {
        currentPixmap = attackFrames[frameIndex];
    } else if (isJumping && !jumpFrames.empty()) {
        currentPixmap = jumpFrames[frameIndex];
    } else if (isMoving && !walkFrames.empty()) {
        currentPixmap = walkFrames[frameIndex];
    } else if (!idleFrames.empty()) {
        currentPixmap = idleFrames[frameIndex];
    }
    
    if (!currentPixmap.isNull()) {
        // 保存当前位置
        QPointF currentPos = pos();
        
        // 处理Player2的特殊情况
        if (!isPlayer1) {
            // Player2的动画方向与Player1相反
            // 如果Player2向左移动，需要翻转图片
            if (!facingRight) {
                currentPixmap = currentPixmap.transformed(QTransform().scale(-1, 1));
            }
        } else {
            // Player1的正常处理
            // 如果Player1向左移动，需要翻转图片
            if (!facingRight) {
                currentPixmap = currentPixmap.transformed(QTransform().scale(-1, 1));
            }
        }
        
        // 设置翻转后的图片
        setPixmap(currentPixmap);
        
        // 确保位置不变
        setPos(currentPos);
    }
}

QRectF Player::getFeetRect() const
{
    // 返回脚部碰撞区域（比实际图片稍小一些）
    return QRectF(x() + 10, y() + boundingRect().height() - 10, 
                 boundingRect().width() - 20, 10);
}

void Player::jump()
{
    if (isGrounded) {
        verticalSpeed = -jumpForce;  // 使用更大的跳跃力
        isGrounded = false;
        isJumping = true;
    }
}

void Player::moveLeft()
{
    setPos(x() - moveSpeed * weapons[currentWeaponIndex]->speedModifier, y());
    facingRight = false;
    isMoving = true;
}

void Player::moveRight()
{
    setPos(x() + moveSpeed * weapons[currentWeaponIndex]->speedModifier, y());
    facingRight = true;
    isMoving = true;
}

void Player::updateDirection()
{
    // 保存当前位置
    QPointF currentPos = pos();
    
    // 重置变换
    setTransform(QTransform());
    
    // 设置变换原点为图片中心，并添加偏移量
    QPointF center = boundingRect().center();
    center.setY(center.y() + 20);  // 垂直偏移
    center.setX(center.x() - 5000);  // 水平偏移
    setTransformOriginPoint(center);
    
    // 根据朝向翻转图片
    if (!facingRight) {
        setTransform(QTransform().scale(-1, 1));
    }
    
    // 如果是玩家2，需要额外翻转
    if (!isPlayer1) {
        setTransform(transform().scale(-1, 1));
    }
    
    // 确保位置不变
    setPos(currentPos);
}

void Player::attack()
{
    if(!canAttack || isDead) return;  // 如果正在冷却或已死亡，不能攻击
    
    weapons[currentWeaponIndex]->attack();
    isAttacking = true;
    hasHit = false;  // 重置命中标记
    currentFrame = 0;  // 重置动画帧
    canAttack = false;  // 开始冷却
    attackCooldownTimer->start(500);  // 设置0.5秒的冷却时间
}

void Player::rangedAttack()
{
    if(!canAttack || isDead) return;  // 如果正在冷却或已死亡，不能攻击

    // 创建剑气
    SwordQi *swordQi = new SwordQi(facingRight, isPlayer1);
    
    //向下偏移量
    int verticalOffset = 20; 

    // 设置剑气位置
    // 设置剑气位置
    if (facingRight) {
        // 在原来的基础上，y 坐标增加偏移量
        swordQi->setPos(x() + boundingRect().width(), y() + verticalOffset); 
    } else {
        // 在原来的基础上，y 坐标增加偏移量
        swordQi->setPos(x() - swordQi->boundingRect().width(), y() + verticalOffset); 
    }
    scene()->addItem(swordQi);

    isAttacking = true;
    hasHit = false;  // 重置命中标记
    currentFrame = 0;  // 重置动画帧
    canAttack = false;  // 开始冷却
    attackCooldownTimer->start(500);  // 设置0.5秒的冷却时间
}


void Player::switchWeapon()
{
    currentWeaponIndex = (currentWeaponIndex + 1) % weapons.size();
}

void Player::update()
{
    // 应用重力
    if (!isGrounded) {
        verticalSpeed += gravity;
        // 限制最大下落速度
        if (verticalSpeed > maxFallSpeed) {
            verticalSpeed = maxFallSpeed;
        }
        setPos(x(), y() + verticalSpeed);
    } else {
        isJumping = false;
    }
    
    // 如果没有移动，设置为静止状态
    if (verticalSpeed == 0 && isGrounded) {
        isMoving = false;
    }

    // 限制玩家在场景范围内
    if (x() < 0) setPos(0, y());
    if (x() > 750) setPos(750, y()); // 800 - 50(玩家宽度)
    if (y() < 0) {
        setPos(x(), 0);
        verticalSpeed = 0;
    }
    if (y() > 600) { // 600 - 50(玩家高度)
        setPos(x(), 600);
        isGrounded = true;
        verticalSpeed = 0;
    }

    // 更新当前武器
    weapons[currentWeaponIndex]->update();
}

void Player::resetMovement()
{
    isMoving = false;
}

void Player::takeDamage(int damage)
{
    if (isDead) return;
    
    health -= damage;
    if (health <= 0) {
        health = 0;
        isDead = true;
        currentFrame = 0;
        animationTimer->start(100); // 减慢死亡动画速度
    }
}

QPixmap Player::getCurrentFrame() const
{
    QPixmap currentPixmap;
    
    // 根据当前状态选择动画帧
    if (isDead && !deadFrames.empty()) {
        currentPixmap = deadFrames[currentFrame];
    } else if (isAttacking && !attackFrames.empty()) {
        currentPixmap = attackFrames[currentFrame];
    } else if (isJumping && !jumpFrames.empty()) {
        currentPixmap = jumpFrames[currentFrame];
    } else if (isMoving && !walkFrames.empty()) {
        currentPixmap = walkFrames[currentFrame];
    } else if (!idleFrames.empty()) {
        currentPixmap = idleFrames[currentFrame];
    }
    
    // 处理Player2的特殊情况
    if (!isPlayer1) {
        // Player2的动画方向与Player1相反
        // 如果Player2向左移动，需要翻转图片
        // 如果Player2向右移动，不需要翻转图片
        if (!facingRight) {
            currentPixmap = currentPixmap.transformed(QTransform().scale(-1, 1));
        }
    } else {
        // Player1的正常处理
        // 如果Player1向左移动，需要翻转图片
        if (!facingRight) {
            currentPixmap = currentPixmap.transformed(QTransform().scale(-1, 1));
        }
    }
    
    return currentPixmap;
}

void Player::applySkill(SkillBall::SkillType type)
{
    switch(type) {
        case SkillBall::HEAL:
            health = qMin(health + 30, 100);
            break;

        case SkillBall::SPEED_BOOST:
            if (!speedBoostActive) {
                moveSpeed *= 1.5; // 增加50%移动速度
                speedBoostActive = true;
                speedBoostTimer->start(30000); // 30秒后恢复
            }
            break;

        case SkillBall::JUMP_BOOST:
            if (!jumpBoostActive) {
                jumpForce *= 1.5; // 增加50%跳跃力
                jumpBoostActive = true;
                jumpBoostTimer->start(30000); // 30秒后恢复
            }
            break;

        case SkillBall::FREE_SWORD_QI:
            if (!freeSwordQiActive) {
                freeSwordQiActive = true;
                freeSwordQiTimer->start(30000); // 30秒后恢复
            }
            break;

        case SkillBall::SHORT_ATTACK_BOOST:
            if (!shortAttackBoostActive) {
                for (auto& weapon : weapons) {
                    if (weapon->type == weapon::MELEE) {
                        weapon->damage = static_cast<int>(weapon->damage * shortAttackBoostMultiplier);
                    }
                }
                shortAttackBoostActive = true;
                shortAttackBoostTimer->start(shortAttackBoostDuration);
            }
            break;

        case SkillBall::PERMANENT_ATTACK_BOOST:
            for (auto& weapon : weapons) {
                weapon->damage = static_cast<int>(weapon->damage * permanentAttackBoostMultiplier);
            }
            break;

        case SkillBall::INCREASE_CURRENT_HP:
            health = qMin(health + increaseHpAmount, maxHealth);
            break;

        case SkillBall::INCREASE_MAX_HP:
            maxHealth += increaseMaxHpAmount;
            health = qMin(health, maxHealth);
            break;

        case SkillBall::SHORT_JUMP_BOOST:
            if (!jumpBoostActive) {
                jumpForce *= shortJumpBoostMultiplier;
                jumpBoostActive = true;
                jumpBoostTimer->start(30000);
            }
            break;

        case SkillBall::PERMANENT_JUMP_BOOST:
            jumpForce *= permanentJumpBoostMultiplier;
            break;

        case SkillBall::PERMANENT_SPEED_BOOST: //永久速度增加
            moveSpeed *= 1.1; // 增加10%移动速度
            originalMoveSpeed = moveSpeed; // 更新原始速度
            break;
    }

}
void Player::updateShortAttackBoostDisplay() {
    if (shortAttackBoostActive) {
        int increase = getShortAttackBoostIncrease();
        int remainingTime = getShortAttackBoostRemainingTime() / 1000; // 转换为秒
        QString text = QString("伤害+%1 剩余时间: %2s").arg(increase).arg(remainingTime);
        shortAttackBoostText->setPlainText(text);
    } else {
        shortAttackBoostText->setPlainText("");
    }
}

void Player::removeShortAttackBoost()
{
    if (shortAttackBoostActive) {
        for (auto& weapon : weapons) {
            if (weapon->type == weapon::MELEE) {
                weapon->damage = static_cast<int>(weapon->damage / shortAttackBoostMultiplier);
            }
        }
        shortAttackBoostActive = false;
    }
}

#include "player.h"
// ... 其他代码 ...

// 实现 getShortAttackBoostRemainingTime 函数
int Player::getShortAttackBoostRemainingTime() const
{
    if (shortAttackBoostActive && shortAttackBoostTimer) {
        return shortAttackBoostTimer->remainingTime();
    }
    return 0;
}

// 实现 getShortAttackBoostIncrease 函数
int Player::getShortAttackBoostIncrease() const
{
    if (shortAttackBoostActive) {
        int originalDamage = 0;
        int boostedDamage = 0;
        for (const auto& weapon : weapons) {
            if (weapon->type == weapon::MELEE) {
                originalDamage = static_cast<int>(weapon->damage / shortAttackBoostMultiplier);
                boostedDamage = weapon->damage;
                break;
            }
        }
        return boostedDamage - originalDamage;
    }
    return 0;
}


void Player::removeSpeedBoost()
{
    moveSpeed = originalMoveSpeed;
    speedBoostActive = false;
}

void Player::removeJumpBoost()
{
    jumpForce = originalJumpForce;
    jumpBoostActive = false;
}

void Player::removeFreeSwordQi()
{
    freeSwordQiActive = false;
}

// 派生类：Player1
Player1::Player1() : Player(true)
{
    // 使用基类的动画系统
    loadAnimationFrames();
}

Player1::~Player1()
{
    // 基类析构函数会自动清理
}

void Player1::loadAnimationFrames()
{
    // 使用基类的动画加载系统
    Player::loadAnimationFrames();
}

void Player1::jump()
{
    Player::jump();
}

void Player1::moveLeft()
{
    Player::moveLeft();
}

void Player1::moveRight()
{
    Player::moveRight();
}

void Player1::attack()
{
    Player::attack();
}

void Player1::update()
{
    Player::update();
}

void Player1::resetMovement()
{
    Player::resetMovement();
}

QPixmap Player1::getCurrentFrame() const
{
    return Player::getCurrentFrame();
}

// 派生类：Player2
Player2::Player2() : Player(false)
{
    // 使用基类的动画系统
    loadAnimationFrames();
    // 设置初始朝向为向左
    facingRight = false;
}

Player2::~Player2()
{
    // 基类析构函数会自动清理
}

void Player2::loadAnimationFrames()
{
    // 使用基类的动画加载系统
    Player::loadAnimationFrames();
}

void Player2::jump()
{
    Player::jump();
}

void Player2::moveLeft()
{
    Player::moveLeft();
}

void Player2::moveRight()
{
    Player::moveRight();
}

void Player2::attack()
{
    Player::attack();
}

void Player2::update()
{
    Player::update();
}

void Player2::resetMovement()
{
    Player::resetMovement();
}

QPixmap Player2::getCurrentFrame() const
{
    return Player::getCurrentFrame();
}    
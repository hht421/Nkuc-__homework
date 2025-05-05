#ifndef PLAYER_H
#define PLAYER_H

#include <QObject>
#include <QGraphicsPixmapItem>
#include <QTimer>
#include <vector>
#include <memory>
#include "skillball.h"
#include "weapon.h"

// 前向声明 weapon 类
class weapon;

class Player : public QObject, public QGraphicsPixmapItem {
    Q_OBJECT
public:
    Player(bool isPlayer1 = true);
    virtual ~Player();

    virtual void jump();
    virtual void moveLeft();
    virtual void moveRight();
    virtual void attack();   
    virtual void switchWeapon();
    virtual void update();
    virtual void resetMovement(); // 新增：重置移动状态
    virtual QPixmap getCurrentFrame() const; // 新增：获取当前帧
    virtual void takeDamage(int damage); // 新增：处理受伤
    bool hasHitTarget() const { return hasHit; }  // 新增：获取是否已经造成伤害
    void setHitTarget(bool hit) { hasHit = hit; }  // 新增：设置是否已经造成伤害
    void rangedAttack();
    int getShortAttackBoostRemainingTime() const;// 获取短时间近战伤害增益的剩余时间
    int getShortAttackBoostIncrease() const;// 获取短时间近战伤害增益的增加量

    // 属性
    float moveSpeed;      // 基础移动速度
    float jumpForce;      // 跳跃力
    float gravity;        // 重力加速度
    float maxFallSpeed;   // 最大下落速度
    int health;
    int maxHealth = 100;  // 最大生命值
    bool isPlayer1;       // 玩家标识

    // 碰撞相关
    void setGrounded(bool value) { isGrounded = value; }
    bool isGroundedState() const { return isGrounded; }
    QRectF getFeetRect() const;  // 获取脚部碰撞区域
    bool isAttacking;     // 是否正在攻击
    int getCurrentWeaponDamage() const { return weapons[currentWeaponIndex]->damage; } // 新增：获取当前武器伤害
    bool isDeadState() const { return isDead; } // 新增：获取死亡状态
    float getVerticalSpeed() const { return verticalSpeed; }  // 新增：获取垂直速度

    // 技能效果相关
    void applySkill(SkillBall::SkillType type);
    void removeSkill(SkillBall::SkillType type);
    bool hasSpeedBoost() const { return speedBoostActive; }
    bool hasJumpBoost() const { return jumpBoostActive; }
    bool hasFreeSwordQi() const { return freeSwordQiActive; }
    bool hasShortAttackBoost() const { return shortAttackBoostActive; }

protected:
    std::vector<std::unique_ptr<weapon>> weapons;
    int currentWeaponIndex;
    bool isGrounded;
    float verticalSpeed;  // 垂直速度
    bool facingRight;     // 角色朝向
    void updateDirection(); // 更新角色朝向
    
    // 动画相关
    QTimer* animationTimer;
    QTimer* attackCooldownTimer;  // 攻击冷却计时器
    bool canAttack;               // 是否可以攻击
    bool hasHit;                  // 新增：是否已经造成伤害
    int currentFrame;
    int totalFrames;
    std::vector<QPixmap> walkFrames;
    std::vector<QPixmap> idleFrames;
    std::vector<QPixmap> jumpFrames;
    std::vector<QPixmap> attackFrames;  // 攻击动画帧
    std::vector<QPixmap> deadFrames;    // 死亡动画帧
    bool isMoving;
    bool isJumping;
    bool isDead;          // 是否已死亡
    
    virtual void loadAnimationFrames();
    void updateAnimation();
    void setAnimationFrame(int frameIndex);

    // 技能效果计时器
    QTimer *speedBoostTimer;
    QTimer *jumpBoostTimer;
    QTimer *freeSwordQiTimer;
    QTimer *shortAttackBoostTimer;
    
    // 技能效果状态
    bool speedBoostActive;
    bool jumpBoostActive;
    bool freeSwordQiActive;
    bool shortAttackBoostActive;
    
    // 原始属性值
    float originalMoveSpeed;
    float originalJumpForce;
    float shortAttackBoostMultiplier = 1.5; // 短时间近战伤害倍数
    int shortAttackBoostDuration = 10000; // 短时间近战伤害持续时间（毫秒）
    float permanentAttackBoostMultiplier = 1.2; // 永久伤害增加倍数
    int increaseHpAmount = 20; // 增加现有生命值的量
    int increaseMaxHpAmount = 20; // 增加最大生命值的量
    float shortJumpBoostMultiplier = 1.5; // 短时间跳跃增强倍数
    float permanentJumpBoostMultiplier = 1.2; // 永久跳跃增强倍数

private slots:
    void removeSpeedBoost();
    void removeJumpBoost();
    void removeFreeSwordQi();
    void removeShortAttackBoost();
    void updateShortAttackBoostDisplay();
private:
    QGraphicsTextItem *shortAttackBoostText; // 显示伤害增加和剩余时间的文本项
    QTimer *shortAttackBoostDisplayTimer; // 用于更新显示的定时器
};

// 派生类：Player1
class Player1 : public Player {
public:
    Player1();
    ~Player1() override;
    
    void jump() override;
    void moveLeft() override;
    void moveRight() override;
    void attack() override;
    void update() override;
    void resetMovement() override;
    QPixmap getCurrentFrame() const override;
    
protected:
    void loadAnimationFrames() override;
};

// 派生类：Player2
class Player2 : public Player {
public:
    Player2();
    ~Player2() override;
    
    void jump() override;
    void moveLeft() override;
    void moveRight() override;
    void attack() override;
    void update() override;
    void resetMovement() override;
    QPixmap getCurrentFrame() const override;
    
protected:
    void loadAnimationFrames() override;
};

#endif // PLAYER_H
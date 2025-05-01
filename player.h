#ifndef PLAYER_H
#define PLAYER_H
#include "weapon.h"
#include <QGraphicsPixmapItem>
#include <QTimer>
#include <memory>
#include <vector>

// 基类：Player
class Player : public QObject, public QGraphicsPixmapItem {
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

    // 属性
    float moveSpeed;      // 基础移动速度
    float jumpForce;      // 跳跃力
    float gravity;        // 重力加速度
    float maxFallSpeed;   // 最大下落速度
    int health;
    bool isPlayer1;       // 玩家标识

    // 碰撞相关
    void setGrounded(bool value) { isGrounded = value; }
    bool isGroundedState() const { return isGrounded; }
    QRectF getFeetRect() const;  // 获取脚部碰撞区域
    bool isAttacking;     // 是否正在攻击
    int getCurrentWeaponDamage() const { return weapons[currentWeaponIndex]->damage; } // 新增：获取当前武器伤害
    bool isDeadState() const { return isDead; } // 新增：获取死亡状态
    float getVerticalSpeed() const { return verticalSpeed; }  // 新增：获取垂直速度

protected:
    std::vector<std::unique_ptr<weapon>> weapons;
    int currentWeaponIndex;
    bool isGrounded;
    float verticalSpeed;  // 垂直速度
    bool facingRight;     // 角色朝向
    void updateDirection(); // 更新角色朝向
    
    // 动画相关
    QTimer* animationTimer;
    int currentFrame;
    int totalFrames;
    std::vector<QPixmap> walkFrames;
    std::vector<QPixmap> idleFrames;
    std::vector<QPixmap> jumpFrames;
    std::vector<QPixmap> attackFrames;  // 新增：攻击动画帧
    std::vector<QPixmap> deadFrames;    // 新增：死亡动画帧
    bool isMoving;
    bool isJumping;
    bool isDead;          // 新增：是否已死亡
    
    virtual void loadAnimationFrames();
    void updateAnimation();
    void setAnimationFrame(int frameIndex);
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

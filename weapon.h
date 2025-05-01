#ifndef WEAPON_H
#define WEAPON_H
#include<string>
using namespace std;

class weapon//武器基类
{
public:
    enum WeaponType { MELEE, RANGED };//近战，远程

    weapon(string name, int damage, float speedModifier);//名字；伤害；攻击速度
    virtual ~weapon() = default;
    virtual void attack() = 0;
    virtual void update() = 0;

    string name;
    int damage;
    float speedModifier; // 移动速度系数（0.8 = 降低20%速度）
    WeaponType type;
};

class sword:public weapon{//近战武器
private:
    int durability;//耐久度
public:
    sword():weapon("sword",5,1.0){
        durability = 100;
        type = MELEE;
    }
    void attack() override { durability -= 5; }
    void update() override {}
};

class gun:public weapon{//远程武器
private:
    int ammo;//弹药量
public:
    gun():weapon("gun",3,1.0){
        ammo = 30;
        type = RANGED;
    }
    void attack() override 
    { 
        if (ammo > 0) 
            ammo--; 
    }
    void update() override {}
};

#endif // WEAPON_H

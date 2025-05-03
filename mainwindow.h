#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QTimer>
#include <QSet>
#include "player.h"
#include "platform.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;

private slots:
    void updateGame();
    void checkSwordQiCollisions();
    void updateSwordQiCharges();

private:
    Ui::MainWindow *ui;
    QGraphicsScene *gameScene;
    QGraphicsView *gameView;
    QTimer *gameTimer;
    Player *player1;
    Player *player2;
    QList<Platform*> platforms;
    QSet<int> pressedKeys;  // 追踪按下的键
    QGraphicsTextItem *player1HealthText;
    QGraphicsTextItem *player2HealthText;
    QGraphicsPixmapItem *player1Icon;  // 玩家1图标
    QGraphicsPixmapItem *player2Icon;  // 玩家2图标
    bool gameOver;
    bool player1Moving;
    bool player2Moving;
    
    // 剑气充能系统
    int player1SwordQiCharges;  // 玩家1剑气存储次数
    int player2SwordQiCharges;  // 玩家2剑气存储次数
    QTimer *swordQiChargeTimer; // 剑气充能计时器
    QGraphicsTextItem *player1SwordQiText; // 玩家1剑气次数显示
    QGraphicsTextItem *player2SwordQiText; // 玩家2剑气次数显示
    const int MAX_SWORD_QI_CHARGES = 5; // 最大剑气存储次数
    const int SWORD_QI_CHARGE_TIME = 10000; // 充能时间（毫秒）
    bool player1SwordQiCooldown;  // 玩家1剑气充能冷却
    bool player2SwordQiCooldown;  // 玩家2剑气充能冷却
    
    void setupGame();
    void initializePlayers();
    void createPlatforms();
    void checkCollisions();
    void processMovement();
    void updateHealthDisplay();
    void checkGameOver();
    void setupSwordQiCollisionCheck();
};
#endif // MAINWINDOW_H

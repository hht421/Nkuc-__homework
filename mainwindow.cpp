#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QKeyEvent>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setupGame();
}

MainWindow::~MainWindow()
{
    delete ui;
    // 清理平台
    for(auto platform : platforms) {
        delete platform;
    }
}

void MainWindow::setupGame()
{
    // 创建游戏场景
    gameScene = new QGraphicsScene(this);
    gameScene->setSceneRect(0, 0, 800, 600);
    gameScene->setBackgroundBrush(Qt::black);

    // 创建游戏视图
    gameView = new QGraphicsView(gameScene, this);
    setCentralWidget(gameView);
    
    // 禁用滚动条
    gameView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    gameView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    
    // 禁用视图的键盘焦点
    gameView->setFocusPolicy(Qt::NoFocus);
    
    // 设置固定大小
    gameView->setFixedSize(800, 600);
    
    // 禁用视图的拖动
    gameView->setDragMode(QGraphicsView::NoDrag);
    
    // 设置视图的渲染质量
    gameView->setRenderHint(QPainter::Antialiasing);
    gameView->setRenderHint(QPainter::SmoothPixmapTransform);

    // 创建平台
    createPlatforms();

    // 初始化玩家
    initializePlayers();

    // 设置游戏定时器
    gameTimer = new QTimer(this);
    connect(gameTimer, &QTimer::timeout, this, &MainWindow::updateGame);
    gameTimer->start(16); // 约60FPS

    // 初始化血量显示
    player1HealthText = new QGraphicsTextItem();
    player1HealthText->setDefaultTextColor(Qt::blue);
    player1HealthText->setFont(QFont("Arial", 16));
    player1HealthText->setPos(20, 20);
    gameScene->addItem(player1HealthText);
    
    player2HealthText = new QGraphicsTextItem();
    player2HealthText->setDefaultTextColor(Qt::red);
    player2HealthText->setFont(QFont("Arial", 16));
    player2HealthText->setPos(700, 20);
    gameScene->addItem(player2HealthText);
    
    gameOver = false;
}

void MainWindow::createPlatforms()
{
    // 创建主地面（最长的地面）
    Platform* ground = new Platform(0, 550, 800, 50);
    ground->setData(0, "ground"); // 设置地面标识
    gameScene->addItem(ground);
    platforms.append(ground);

    // 创建一些浮动平台
    Platform* platform1 = new Platform(100, 400, 200, 10);
    Platform* platform2 = new Platform(500, 400, 200, 10);
    Platform* platform3 = new Platform(100, 150, 200, 10);
    Platform* platform4 = new Platform(500, 150, 200, 10);
    Platform* platform5 = new Platform(300, 300, 200, 10);

    gameScene->addItem(platform1);
    gameScene->addItem(platform2);
    gameScene->addItem(platform3);
    gameScene->addItem(platform4);
    gameScene->addItem(platform5);

    platforms.append(platform1);
    platforms.append(platform2);
    platforms.append(platform3);
    platforms.append(platform4);
    platforms.append(platform5);
}

void MainWindow::initializePlayers()
{
    // 创建两个不同的玩家对象
    player1 = new Player1();
    player2 = new Player2();
    
    // 设置初始位置
    player1->setPos(100, 300);
    player2->setPos(600, 300);
    
    // 添加到场景
    gameScene->addItem(player1);
    gameScene->addItem(player2);
    
    // 强制更新一次动画帧，确保初始朝向正确
    // 这里我们手动调用一次updateAnimation，确保初始状态正确
    QTimer::singleShot(0, [this]() {
        // 确保Player1朝向右边
        player1->setPixmap(player1->getCurrentFrame());
        
        // 确保Player2朝向左边
        player2->setPixmap(player2->getCurrentFrame());
    });
}

void MainWindow::updateGame()
{
    if(gameOver) return;
    
    // 处理移动
    processMovement();
    
    // 更新游戏状态
    player1->update();
    player2->update();
    checkCollisions();
    updateHealthDisplay();
    checkGameOver();
}

void MainWindow::updateHealthDisplay()
{
    player1HealthText->setPlainText(QString("HP: %1").arg(player1->health));
    player2HealthText->setPlainText(QString("HP: %1").arg(player2->health));
}

void MainWindow::checkCollisions()
{
    bool player1OnPlatform = false;
    bool player2OnPlatform = false;

    // 检查玩家与平台的碰撞
    for(auto platform : platforms) {
        // 玩家1的碰撞检测
        if(platform->isColliding(player1)) {
            QRectF feetRect1 = player1->getFeetRect();
            QRectF platformRect = platform->boundingRect();
            platformRect.translate(platform->pos());

            // 检测脚部是否在平台上方
            if(feetRect1.bottom() <= platformRect.top() + 10) {
                // 如果玩家正在下落或静止，则允许落在平台上
                if(player1->getVerticalSpeed() >= 0 || player1->isGroundedState()) {
                    // 更精确的碰撞检测
                    if(feetRect1.intersects(platformRect)) {
                        player1->setPos(player1->x(), platformRect.top() - player1->boundingRect().height() + 5);
                        player1->setGrounded(true);
                        player1OnPlatform = true;
                    }
                }
            }
        }
        
        // 玩家2的碰撞检测
        if(platform->isColliding(player2)) {
            QRectF feetRect2 = player2->getFeetRect();
            QRectF platformRect = platform->boundingRect();
            platformRect.translate(platform->pos());

            // 检测脚部是否在平台上方
            if(feetRect2.bottom() <= platformRect.top() + 10) {
                // 如果玩家正在下落或静止，则允许落在平台上
                if(player2->getVerticalSpeed() >= 0 || player2->isGroundedState()) {
                    if(feetRect2.intersects(platformRect)) {
                        player2->setPos(player2->x(), platformRect.top() - player2->boundingRect().height() + 5);
                        player2->setGrounded(true);
                        player2OnPlatform = true;
                    }
                }
            }
        }
    }

    // 检查玩家是否在地面上
    if (player1->y() + player1->boundingRect().height() >= 550) {
        player1->setPos(player1->x(), 550 - player1->boundingRect().height());
        player1->setGrounded(true);
        player1OnPlatform = true;
    }
    
    if (player2->y() + player2->boundingRect().height() >= 550) {
        player2->setPos(player2->x(), 550 - player2->boundingRect().height());
        player2->setGrounded(true);
        player2OnPlatform = true;
    }

    // 如果没有站在任何平台上，设置为未着地
    if (!player1OnPlatform) {
        player1->setGrounded(false);
    }
    if (!player2OnPlatform) {
        player2->setGrounded(false);
    }

    // 检查玩家之间的攻击碰撞
    if (player1->isAttacking && !player2->isDeadState() && !player1->hasHitTarget()) {
        QRectF attackRect1 = player1->boundingRect();
        attackRect1.translate(player1->pos());
        QRectF player2Rect = player2->boundingRect();
        player2Rect.translate(player2->pos());
        
        if (attackRect1.intersects(player2Rect)) {
            player2->takeDamage(player1->getCurrentWeaponDamage());
            player1->setHitTarget(true);  // 标记已经造成伤害
        }
    }
    
    if (player2->isAttacking && !player1->isDeadState() && !player2->hasHitTarget()) {
        QRectF attackRect2 = player2->boundingRect();
        attackRect2.translate(player2->pos());
        QRectF player1Rect = player1->boundingRect();
        player1Rect.translate(player1->pos());
        
        if (attackRect2.intersects(player1Rect)) {
            player1->takeDamage(player2->getCurrentWeaponDamage());
            player2->setHitTarget(true);  // 标记已经造成伤害
        }
    }
}

void MainWindow::processMovement()
{
    if(gameOver) return; // 游戏结束后禁止移动
    
    // 处理玩家1的移动
    player1Moving = false;
    if (pressedKeys.contains(Qt::Key_A)) {
        player1->moveLeft();
        player1Moving = true;
    }
    if (pressedKeys.contains(Qt::Key_D)) {
        player1->moveRight();
        player1Moving = true;
    }
    if (!player1Moving) {
        player1->resetMovement();
    }

    // 处理玩家2的移动
    player2Moving = false;
    if (pressedKeys.contains(Qt::Key_Left)) {
        player2->moveLeft();
        player2Moving = true;
    }
    if (pressedKeys.contains(Qt::Key_Right)) {
        player2->moveRight();
        player2Moving = true;
    }
    if (!player2Moving) {
        player2->resetMovement();
    }
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if(gameOver) return; // 游戏结束后禁止按键

    // 处理攻击键（添加防重复判断）
    if (!event->isAutoRepeat()) {
        if (event->key() == Qt::Key_Space) {
            player1->attack();
        }
        if (event->key() == Qt::Key_Return) {
            player2->attack();
        }
    }
    
    
    // 将按下的键添加到集合中
    pressedKeys.insert(event->key());
    
    // 处理跳跃和攻击
    if (event->key() == Qt::Key_W) {
        player1->jump();
    }
    if (event->key() == Qt::Key_Up) {
        player2->jump();
    }
    if (event->key() == Qt::Key_J) {
        player1->attack();
    }
    if (event->key() == Qt::Key_1) {
        player2->attack();
    }
    if (event->key() == Qt::Key_Q) {
        player1->switchWeapon();
    }
    if (event->key() == Qt::Key_2) {
        player2->switchWeapon();
    }
}

void MainWindow::keyReleaseEvent(QKeyEvent *event)
{
    // 从集合中移除释放的键
    pressedKeys.remove(event->key());
}

void MainWindow::checkGameOver()
{
    if(player1->isDeadState() || player2->isDeadState()) {
        gameOver = true;
        gameTimer->stop();
        
        // 显示胜利者
        QGraphicsTextItem *winnerText = new QGraphicsTextItem();
        winnerText->setDefaultTextColor(Qt::yellow);
        winnerText->setFont(QFont("Arial", 32, QFont::Bold));
        
        if(player1->isDeadState()) {
            winnerText->setPlainText("Player 2 Wins!");
        } else {
            winnerText->setPlainText("Player 1 Wins!");
        }
        
        winnerText->setPos(250, 250);
        gameScene->addItem(winnerText);
    }
}

#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "swordqi.h"
#include "skillball.h"
#include <QKeyEvent>
#include <QRandomGenerator>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setupGame();
    player1SwordQiCooldown = false;  // 初始化玩家1剑气充能冷却
    player2SwordQiCooldown = false;  // 初始化玩家2剑气充能冷却
    
    // 初始化技能球相关
    skillBallSpawnTimer = new QTimer(this);
    connect(skillBallSpawnTimer, &QTimer::timeout, this, &MainWindow::spawnSkillBall);

    //结束游戏相关
    gameOverDialog = new GameOverDialog(this);
    connect(gameOverDialog, &GameOverDialog::returnToMainMenuClicked, this, &MainWindow::onReturnToMainMenu);
    connect(gameOverDialog, &GameOverDialog::exitGameClicked, this, &MainWindow::onExitGame);
}

MainWindow::~MainWindow()
{
    delete ui;
    delete gameTimer;
    delete swordQiChargeTimer;
    delete player1;
    delete player2;
    delete player1Icon;
    delete player2Icon;
    for(auto platform : platforms) {
        delete platform;
    }
    delete gameOverDialog;
}

void MainWindow::setupGame()
{
    // 创建游戏场景
    gameScene = new QGraphicsScene(this);
    gameScene->setSceneRect(0, 0, 800, 600);
    
    // 设置背景图片
    QPixmap backgroundPixmap(":/image/place/2.png");
    gameScene->setBackgroundBrush(backgroundPixmap.scaled(800, 600, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));

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

    // 初始化剑气碰撞检测
    setupSwordQiCollisionCheck();

    // 设置游戏定时器
    gameTimer = new QTimer(this);
    connect(gameTimer, &QTimer::timeout, this, &MainWindow::updateGame);
    gameTimer->start(16); // 约60FPS

    // 初始化血量显示
    player1HealthText = new QGraphicsTextItem();
    player1HealthText->setDefaultTextColor(Qt::red);
    player1HealthText->setFont(QFont("Arial", 16));
    player1HealthText->setPos(20, 20);
    gameScene->addItem(player1HealthText);
    
    player2HealthText = new QGraphicsTextItem();
    player2HealthText->setDefaultTextColor(Qt::blue);
    player2HealthText->setFont(QFont("Arial", 16));
    player2HealthText->setPos(700, 20);
    gameScene->addItem(player2HealthText);
    
    // 初始化剑气充能系统
    player1SwordQiCharges = MAX_SWORD_QI_CHARGES;
    player2SwordQiCharges = MAX_SWORD_QI_CHARGES;

    // 初始化剑气次数显示
    player1SwordQiText = new QGraphicsTextItem();
    player1SwordQiText->setDefaultTextColor(Qt::cyan);
    player1SwordQiText->setFont(QFont("Arial", 16));
    player1SwordQiText->setPos(20, 50);
    gameScene->addItem(player1SwordQiText);

    player2SwordQiText = new QGraphicsTextItem();
    player2SwordQiText->setDefaultTextColor(Qt::magenta);
    player2SwordQiText->setFont(QFont("Arial", 16));
    player2SwordQiText->setPos(700, 50);
    gameScene->addItem(player2SwordQiText);

    // 设置剑气充能计时器
    swordQiChargeTimer = new QTimer(this);
    connect(swordQiChargeTimer, &QTimer::timeout, this, &MainWindow::updateSwordQiCharges);
    swordQiChargeTimer->start(SWORD_QI_CHARGE_TIME);

    gameOver = false;
    
    // 更新显示
    updateHealthDisplay();
}

void MainWindow::createPlatforms()
{
    // 创建主地面（最长的地面）
    Platform* ground = new Platform(0, 600, 800, 50);
    ground->setData(0, "ground"); // 设置地面标识
    gameScene->addItem(ground);
    platforms.append(ground);

    // 创建一些浮动平台
    Platform* platform1 = new Platform(78, 382, 182, 13);
    Platform* platform2 = new Platform(542, 382, 182, 13);
    Platform* platform3 = new Platform(0, 288, 165, 13);
    Platform* platform4 = new Platform(635, 288, 165, 13);
    Platform* platform5 = new Platform(260, 288, 280, 13);
    Platform* platform6 = new Platform(0, 475, 165, 13);
    Platform* platform7 = new Platform(635, 475, 165, 13);
    Platform* platform8 = new Platform(260, 475, 280, 13);
    Platform* platform9 = new Platform(0, 195, 75, 13);
    Platform* platform10 = new Platform(725, 195, 75, 13);

    gameScene->addItem(platform1);
    gameScene->addItem(platform2);
    gameScene->addItem(platform3);
    gameScene->addItem(platform4);
    gameScene->addItem(platform5);
    gameScene->addItem(platform6);
    gameScene->addItem(platform7);
    gameScene->addItem(platform8);
    gameScene->addItem(platform9);
    gameScene->addItem(platform10);

    platforms.append(platform1);
    platforms.append(platform2);
    platforms.append(platform3);
    platforms.append(platform4);
    platforms.append(platform5);
    platforms.append(platform6);
    platforms.append(platform7);
    platforms.append(platform8);
    platforms.append(platform9);
    platforms.append(platform10);
}

void MainWindow::initializePlayers()
{
    // 创建玩家1
    player1 = new Player1();
    player1->setPos(100, 300);
    gameScene->addItem(player1);

    // 创建玩家2
    player2 = new Player2();
    player2->setPos(600, 300);
    gameScene->addItem(player2);

    // 创建并设置玩家1图标
    QPixmap p1Icon(":/image/person1/P1.png");  // 假设图标在资源文件中
    p1Icon = p1Icon.scaled(30, 30, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    player1Icon = new QGraphicsPixmapItem(p1Icon);
    player1Icon->setParentItem(player1);
    player1Icon->setPos(30, -20);  // 设置相对于玩家的位置，使其显示在头顶

    // 创建并设置玩家2图标
    QPixmap p2Icon(":/image/person1/P2.png");  // 假设图标在资源文件中
    p2Icon = p2Icon.scaled(30, 30, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    player2Icon = new QGraphicsPixmapItem(p2Icon);
    player2Icon->setParentItem(player2);
    player2Icon->setPos(30, -20);  // 设置相对于玩家的位置，使其显示在头顶
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
    
    // 检查技能球碰撞
    checkSkillBallCollisions();
    updateSkillBalls();  // 添加技能球更新
}

void MainWindow::updateHealthDisplay()
{
    player1HealthText->setPlainText(QString("HP: %1").arg(player1->health));
    player2HealthText->setPlainText(QString("HP: %1").arg(player2->health));
    
    // 更新剑气次数显示
    player1SwordQiText->setPlainText(QString("充能: %1").arg(player1SwordQiCharges));
    player2SwordQiText->setPlainText(QString("充能: %1").arg(player2SwordQiCharges));
}

void MainWindow::updateSwordQiCharges()
{
    // 为玩家1充能
    if (player1SwordQiCharges < MAX_SWORD_QI_CHARGES) {
        player1SwordQiCharges++;
        updateHealthDisplay(); // 更新显示
    }

    // 为玩家2充能
    if (player2SwordQiCharges < MAX_SWORD_QI_CHARGES) {
        player2SwordQiCharges++;
        updateHealthDisplay(); // 更新显示
    }
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

            // 检测脚部是否在平台上方，增加容错范围
            if(feetRect1.bottom() <= platformRect.top() + 15) {
                // 如果玩家正在下落或静止，则允许落在平台上
                if(player1->getVerticalSpeed() >= 0 || player1->isGroundedState()) {
                    // 更精确的碰撞检测，考虑水平方向的偏移
                    if(feetRect1.intersects(platformRect)) {
                        // 确保玩家不会卡在平台边缘，增加水平检测的容错范围
                        if (player1->x() + player1->boundingRect().width() - 5 > platformRect.left() &&
                            player1->x() + 5 < platformRect.right()) {
                            // 如果玩家正在下落，确保完全落在平台上
                            if (player1->getVerticalSpeed() > 0) {
                                player1->setPos(player1->x(), platformRect.top() - player1->boundingRect().height() + 5);
                            }
                            player1->setGrounded(true);
                            player1OnPlatform = true;
                        }
                    }
                }
            }
        }
        
        // 玩家2的碰撞检测
        if(platform->isColliding(player2)) {
            QRectF feetRect2 = player2->getFeetRect();
            QRectF platformRect = platform->boundingRect();
            platformRect.translate(platform->pos());

            // 检测脚部是否在平台上方，增加容错范围
            if(feetRect2.bottom() <= platformRect.top() + 15) {
                // 如果玩家正在下落或静止，则允许落在平台上
                if(player2->getVerticalSpeed() >= 0 || player2->isGroundedState()) {
                    if(feetRect2.intersects(platformRect)) {
                        // 确保玩家不会卡在平台边缘，增加水平检测的容错范围
                        if (player2->x() + player2->boundingRect().width() - 5 > platformRect.left() &&
                            player2->x() + 5 < platformRect.right()) {
                            // 如果玩家正在下落，确保完全落在平台上
                            if (player2->getVerticalSpeed() > 0) {
                                player2->setPos(player2->x(), platformRect.top() - player2->boundingRect().height() + 5);
                            }
                            player2->setGrounded(true);
                            player2OnPlatform = true;
                        }
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
        if (event->key() == Qt::Key_K) {
            // 检查玩家1是否有剑气可用，并且不在冷却时间内
            if (player1SwordQiCharges > 0 && !player1SwordQiCooldown) {
                player1->rangedAttack();
                player1SwordQiCharges--;
                player1SwordQiCooldown = true;
                // 设置冷却时间
                QTimer::singleShot(500, this, [this]() {
                    player1SwordQiCooldown = false;
                });
                updateHealthDisplay();
            }
        }
        if (event->key() == Qt::Key_2) {
            // 检查玩家2是否有剑气可用，并且不在冷却时间内
            if (player2SwordQiCharges > 0 && !player2SwordQiCooldown) {
                player2->rangedAttack();
                player2SwordQiCharges--;
                player2SwordQiCooldown = true;
                // 设置冷却时间
                QTimer::singleShot(500, this, [this]() {
                    player2SwordQiCooldown = false;
                });
                updateHealthDisplay();
            }
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

void MainWindow::setupSwordQiCollisionCheck()
{
    QTimer *swordQiTimer = new QTimer(this);
    connect(swordQiTimer, &QTimer::timeout, this, &MainWindow::checkSwordQiCollisions);
    swordQiTimer->start(50); // 每50毫秒检查一次碰撞
}

void MainWindow::checkSwordQiCollisions()
{
    // 使用QList来存储需要删除的剑气，避免在遍历过程中直接删除
    QList<SwordQi*> swordQisToDelete;
    
    // 获取所有剑气
    QList<SwordQi*> allSwordQis;
    QList<QGraphicsItem *> items = gameScene->items();
    for (QGraphicsItem *item : items) {
        SwordQi *swordQi = dynamic_cast<SwordQi *>(item);
        if (swordQi) {
            allSwordQis.append(swordQi);
        }
    }

    // 检查剑气之间的碰撞
    for (int i = 0; i < allSwordQis.size(); ++i) {
        for (int j = i + 1; j < allSwordQis.size(); ++j) {
            SwordQi *qi1 = allSwordQis[i];
            SwordQi *qi2 = allSwordQis[j];
            
            // 只有当两个剑气属于不同玩家且碰撞时才处理
            if (qi1->belongsToPlayer1() != qi2->belongsToPlayer1() && 
                qi1->collidesWithItem(qi2) &&
                !swordQisToDelete.contains(qi1) && 
                !swordQisToDelete.contains(qi2)) {
                
                int damage1 = qi1->getDamage();
                int damage2 = qi2->getDamage();
                
                if (damage1 == damage2) {
                    // 伤害相等，两个都消失
                    swordQisToDelete.append(qi1);
                    swordQisToDelete.append(qi2);
                } else if (damage1 > damage2) {
                    // 剑气1伤害更大
                    qi1->setDamage(damage1 - damage2);
                    swordQisToDelete.append(qi2);
                } else {
                    // 剑气2伤害更大
                    qi2->setDamage(damage2 - damage1);
                    swordQisToDelete.append(qi1);
                }
            }
        }
    }

    // 检查剑气与玩家的碰撞
    for (SwordQi *swordQi : allSwordQis) {
        if (swordQisToDelete.contains(swordQi)) continue; // 跳过已标记删除的剑气
        
        // 检查是否碰撞到玩家1（只有玩家2的剑气才能伤害玩家1）
        if (swordQi->collidesWithItem(player1) && !swordQi->belongsToPlayer1() && !player1->isDeadState()) {
            player1->takeDamage(swordQi->getDamage()); // 使用剑气的实际伤害值
            swordQisToDelete.append(swordQi);
        }
        // 检查是否碰撞到玩家2（只有玩家1的剑气才能伤害玩家2）
        if (swordQi->collidesWithItem(player2) && swordQi->belongsToPlayer1() && !player2->isDeadState()) {
            player2->takeDamage(swordQi->getDamage()); // 使用剑气的实际伤害值
            swordQisToDelete.append(swordQi);
        }
    }
    
    // 在遍历结束后统一删除剑气
    for (SwordQi *swordQi : swordQisToDelete) {
        if (swordQi && gameScene->items().contains(swordQi)) {
            gameScene->removeItem(swordQi);
            delete swordQi;
        }
    }
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

        // 调用 onGameOver 函数弹出游戏结束的弹窗
        onGameOver();
    }
}

void MainWindow::onClassicModeSelected()
{
    // 经典模式不需要技能球
    skillBallSpawnTimer->stop();
    qDeleteAll(skillBalls);
    skillBalls.clear();
    
    setupGame();
}

void MainWindow::onEvolutionModeSelected()
{
    // 清理现有的技能球
    for (auto ball : skillBalls) {
        if (ball) {
            gameScene->removeItem(ball);
            delete ball;
        }
    }
    skillBalls.clear();
    
    // 启动技能球生成
    skillBallSpawnTimer->start(10000); // 每10秒生成一个技能球
    
    // 重置游戏状态
    setupGame();
}

void MainWindow::spawnSkillBall()
{
    // 随机选择技能类型
    SkillBall::SkillType type = static_cast<SkillBall::SkillType>(QRandomGenerator::global()->bounded(11));

    // 随机位置（确保在平台范围内）
    int x = QRandomGenerator::global()->bounded(100, 700);
    int y = QRandomGenerator::global()->bounded(100, 500);

    SkillBall *ball = new SkillBall(type);
    ball->setPos(x, y);
    gameScene->addItem(ball);
    skillBalls.append(ball);
}

void MainWindow::checkSkillBallCollisions()
{
    for (auto it = skillBalls.begin(); it != skillBalls.end();) {
        SkillBall *ball = *it;
        
        // 检查与玩家1的碰撞
        if (ball->collidesWithItem(player1)) {
            player1->applySkill(ball->getType());
            gameScene->removeItem(ball);
            delete ball;
            it = skillBalls.erase(it);
            continue;
        }
        
        // 检查与玩家2的碰撞
        if (ball->collidesWithItem(player2)) {
            player2->applySkill(ball->getType());
            gameScene->removeItem(ball);
            delete ball;
            it = skillBalls.erase(it);
            continue;
        }
        
        ++it;
    }
}

void MainWindow::updateSkillBalls()
{
    // 更新所有技能球的状态
    for (auto ball : skillBalls) {
        if (ball) {
            // 检查技能球是否超出场景范围
            if (ball->x() < 0 || ball->x() > 800 || ball->y() < 0 || ball->y() > 600) {
                gameScene->removeItem(ball);
                skillBalls.removeOne(ball);
                delete ball;
            }
        }
    }
}

void MainWindow::onGameOver()
{
    gameOverDialog->show();
}

void MainWindow::onReturnToMainMenu()
{
    this->hide();
    emit showMainMenu();
}

void MainWindow::onExitGame()
{
    QApplication::quit();
}

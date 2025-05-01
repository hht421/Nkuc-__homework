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
    bool gameOver;
    bool player1Moving;
    bool player2Moving;
    
    void setupGame();
    void initializePlayers();
    void createPlatforms();
    void checkCollisions();
    void processMovement();
    void updateHealthDisplay();
    void checkGameOver();
};
#endif // MAINWINDOW_H

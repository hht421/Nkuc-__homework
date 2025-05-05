#include "gameoverdialog.h"

GameOverDialog::GameOverDialog(QWidget *parent) : QDialog(parent)
{
    setWindowTitle("游戏结束");
    setFixedSize(300, 200);

    returnToMainMenuButton = new QPushButton("返回主页", this);
    returnToMainMenuButton->setGeometry(50, 50, 200, 50);
    connect(returnToMainMenuButton, &QPushButton::clicked, this, &GameOverDialog::onReturnToMainMenuButtonClicked);

    exitGameButton = new QPushButton("退出游戏", this);
    exitGameButton->setGeometry(50, 120, 200, 50);
    connect(exitGameButton, &QPushButton::clicked, this, &GameOverDialog::onExitGameButtonClicked);
}

GameOverDialog::~GameOverDialog()
{
    delete returnToMainMenuButton;
    delete exitGameButton;
}

void GameOverDialog::onReturnToMainMenuButtonClicked()
{
    emit returnToMainMenuClicked();
    close();
}

void GameOverDialog::onExitGameButtonClicked()
{
    emit exitGameClicked();
    close();
}
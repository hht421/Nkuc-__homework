#include "mainmenu.h"
#include <QPixmap>

MainMenu::MainMenu(QWidget *parent) : QWidget(parent)
{
    // 设置背景图
    backgroundLabel = new QLabel(this);
    QPixmap backgroundPixmap(":/image/place/1.png"); // 替换为实际的背景图路径
    backgroundLabel->setPixmap(backgroundPixmap.scaled(800, 600, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    backgroundLabel->setGeometry(0, 0, 800, 600);
    backgroundLabel->lower();

    // 创建隐形按钮
    startGameButton = new QPushButton(this);
    gameIntroductionButton = new QPushButton(this);

    // 设置按钮样式为透明
    startGameButton->setStyleSheet("background-color: transparent; border: none;");
    gameIntroductionButton->setStyleSheet("background-color: transparent; border: none;");

    // 调整按钮位置和大小（初始值，根据实际情况修改）
    startGameButton->setGeometry(187, 463, 190, 51); // 开始游戏按钮位置和大小
    gameIntroductionButton->setGeometry(423, 463, 190, 51); // 游戏介绍按钮位置和大小

    // 连接按钮信号和槽
    connect(startGameButton, &QPushButton::clicked, this, &MainMenu::onStartGameClicked);
    connect(gameIntroductionButton, &QPushButton::clicked, this, &MainMenu::onGameIntroductionClicked);
}

MainMenu::~MainMenu()
{
    delete startGameButton;
    delete gameIntroductionButton;
    delete backgroundLabel;
}

void MainMenu::onStartGameClicked()
{
    // 弹出游戏模式选择对话框
    QMessageBox modeBox;
    modeBox.setWindowTitle("选择游戏模式");
    modeBox.setText("请选择游戏模式：");
    QPushButton *classicModeButton = modeBox.addButton("经典模式", QMessageBox::AcceptRole);
    QPushButton *evolutionModeButton = modeBox.addButton("进化模式", QMessageBox::AcceptRole);
    QPushButton *cancelButton = modeBox.addButton("取消", QMessageBox::RejectRole);
    modeBox.exec();

    if (modeBox.clickedButton() == classicModeButton) {
        emit startGame("经典模式");
    } else if (modeBox.clickedButton() == evolutionModeButton) {
        emit startGame("进化模式");
    } else if (modeBox.clickedButton() == cancelButton) {
        return;
    }
}

void MainMenu::onGameIntroductionClicked()
{
    // 弹出游戏介绍窗口
    QMessageBox::information(this, "游戏介绍", "待填写");
}
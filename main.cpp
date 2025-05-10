#include "mainwindow.h"
#include "mainmenu.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    MainMenu menu;
    menu.setWindowTitle("游戏主页");
    menu.resize(800, 600); // 设置开始界面的大小，和游戏界面一致
    menu.show();

    MainWindow gameWindow;
    gameWindow.setWindowTitle("2D Battle Game");
    gameWindow.resize(800, 600);

    QObject::connect(&menu, &MainMenu::startGame, [&](const QString& mode) {
        menu.hide();
        if (mode == "经典模式") {
            gameWindow.onClassicModeSelected();
        } else if (mode == "进化模式") {
            gameWindow.onEvolutionModeSelected();
        }
        gameWindow.show();
    });

    // 确保信号和槽的连接
    QObject::connect(&gameWindow, &MainWindow::showMainMenu, &menu, &MainMenu::onShowMainMenu);

    return a.exec();
}
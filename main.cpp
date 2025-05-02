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
        if (mode == "经典模式") {
            menu.hide();
            gameWindow.show();
        } else if (mode == "进化模式") {
            // 这里可以添加进化模式的启动逻辑
            QMessageBox::information(nullptr, "提示", "进化模式暂未开放，敬请期待！");
        }
    });

    return a.exec();
}
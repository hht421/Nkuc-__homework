// mainmenu.h
#ifndef MAINMENU_H
#define MAINMENU_H

#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QMessageBox>

class MainMenu : public QWidget
{
    Q_OBJECT
public:
    explicit MainMenu(QWidget *parent = nullptr);
    ~MainMenu() override;

signals:
    void startGame(const QString& mode);

private slots:
    void onStartGameClicked();
    void onGameIntroductionClicked();

private:
    QPushButton *startGameButton;
    QPushButton *gameIntroductionButton;
    QLabel *backgroundLabel;
};

#endif // MAINMENU_H
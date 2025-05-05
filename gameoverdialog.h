#ifndef GAMEOVERDIALOG_H
#define GAMEOVERDIALOG_H

#include <QDialog>
#include <QPushButton>

class GameOverDialog : public QDialog
{
    Q_OBJECT
public:
    explicit GameOverDialog(QWidget *parent = nullptr);
    ~GameOverDialog() override;

signals:
    void returnToMainMenuClicked();
    void exitGameClicked();

private slots:
    void onReturnToMainMenuButtonClicked();
    void onExitGameButtonClicked();

private:
    QPushButton *returnToMainMenuButton;
    QPushButton *exitGameButton;
};

#endif // GAMEOVERDIALOG_H
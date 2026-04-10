#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStackedWidget>
#include <QVector>
#include "menuwidget.h"
#include "tetriswidget.h"
#include "apiclient.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    void showMenu();
    void showGame(const QString &nickname);
    void onGameOver(int score);

    Ui::MainWindow *ui      = nullptr;
    QStackedWidget *stack   = nullptr;
    MenuWidget     *menu    = nullptr;
    TetrisWidget   *game    = nullptr;
    ApiClient      *api     = nullptr;

    QString currentPlayer;
};

#endif

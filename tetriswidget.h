#ifndef TETRISWIDGET_H
#define TETRISWIDGET_H

#include <QWidget>
#include <QTimer>
#include <QVector>
#include "tetromino.h"

class TetrisWidget : public QWidget {
    Q_OBJECT

signals:
    void gameFinished(int score);

public:
    explicit TetrisWidget(QWidget *parent = nullptr);
    ~TetrisWidget();
    void startGame();

    static const int BOARD_WIDTH  = 10;
    static const int BOARD_HEIGHT = 20;
    static const int BLOCK_SIZE   = 30;

protected:
    void paintEvent(QPaintEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

private slots:
    void timerTick();

private:
    bool isValidPosition(const Tetromino &tet) const;
    void lockTetromino();
    void clearLines();
    void spawnNewTetromino();
    void dropTetromino();
    int  speedForLevel(int lvl) const;
    int  ghostDropY() const;
    void drawBlock(QPainter &p, int x, int y, QColor color);

    QVector<QVector<int>> board;
    Tetromino *currentTetromino = nullptr;
    Tetromino *nextTetromino    = nullptr;
    Tetromino *heldTetromino    = nullptr;
    QTimer    *gameTimer        = nullptr;
    QTimer    *flashTimer       = nullptr;

    bool canHold      = true;
    int  score        = 0;
    int  level        = 1;
    int  linesCleared = 0;
    bool gameOver     = false;

    QVector<int> flashLines;
    int          flashStep = 0;
};

#endif

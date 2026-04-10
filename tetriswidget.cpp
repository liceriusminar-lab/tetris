#include "tetriswidget.h"
#include <QPainter>
#include <QRandomGenerator>
#include <QKeyEvent>
#include <QLinearGradient>

TetrisWidget::TetrisWidget(QWidget *parent) : QWidget(parent)
{
    setFixedSize(BOARD_WIDTH * BLOCK_SIZE + 160, BOARD_HEIGHT * BLOCK_SIZE);
    board = QVector<QVector<int>>(BOARD_HEIGHT, QVector<int>(BOARD_WIDTH, 0));
    setFocusPolicy(Qt::StrongFocus);

    gameTimer = new QTimer(this);
    connect(gameTimer, &QTimer::timeout, this, &TetrisWidget::timerTick);

    flashTimer = new QTimer(this);
    flashTimer->setInterval(60);
    connect(flashTimer, &QTimer::timeout, this, [this]() {
        flashStep++;
        if (flashStep >= 6) {
            flashTimer->stop();
            flashStep = 0;
            for (int y : flashLines) {
                for (int yy = y; yy > 0; --yy)
                    board[yy] = board[yy - 1];
                board[0].fill(0);
            }
            flashLines.clear();
            spawnNewTetromino();
            gameTimer->start(speedForLevel(level));
        }
        update();
    });
}

TetrisWidget::~TetrisWidget()
{
    delete currentTetromino;
    delete nextTetromino;
    delete heldTetromino;
}

int TetrisWidget::speedForLevel(int lvl) const
{
    static const int speeds[] = {
        800, 717, 633, 550, 467, 383, 300, 217, 133, 100,
        83,  83,  83,  67,  67,  67,  50,  50,  50,  33
    };
    if (lvl < 1)  lvl = 1;
    if (lvl > 20) lvl = 20;
    return speeds[lvl - 1];
}

void TetrisWidget::startGame()
{
    score        = 0;
    level        = 1;
    linesCleared = 0;
    gameOver     = false;
    canHold      = true;
    flashLines.clear();
    flashStep = 0;
    flashTimer->stop();
    gameTimer->stop();

    board.fill(QVector<int>(BOARD_WIDTH, 0));

    delete currentTetromino; currentTetromino = nullptr;
    delete nextTetromino;    nextTetromino    = nullptr;
    delete heldTetromino;    heldTetromino    = nullptr;

    int r1 = QRandomGenerator::global()->bounded(7);
    int r2 = QRandomGenerator::global()->bounded(7);
    currentTetromino = new Tetromino(static_cast<Tetromino::Type>(r1));
    nextTetromino    = new Tetromino(static_cast<Tetromino::Type>(r2));

    gameTimer->start(speedForLevel(level));
    setFocus();
    update();
}

void TetrisWidget::timerTick()
{
    if (!currentTetromino || gameOver) return;
    currentTetromino->move(0, 1);
    if (!isValidPosition(*currentTetromino)) {
        currentTetromino->move(0, -1);
        lockTetromino();
    }
    update();
}

void TetrisWidget::keyPressEvent(QKeyEvent *event)
{
    if (gameOver || flashTimer->isActive()) return;
    if (!currentTetromino) return;

    switch (event->key()) {
    case Qt::Key_Left:
        currentTetromino->move(-1, 0);
        if (!isValidPosition(*currentTetromino)) currentTetromino->move(1, 0);
        break;
    case Qt::Key_Right:
        currentTetromino->move(1, 0);
        if (!isValidPosition(*currentTetromino)) currentTetromino->move(-1, 0);
        break;
    case Qt::Key_Down:
        dropTetromino();
        return;
    case Qt::Key_Up:
        currentTetromino->rotate();
        if (!isValidPosition(*currentTetromino)) {
            currentTetromino->move(-1, 0);
            if (!isValidPosition(*currentTetromino)) {
                currentTetromino->move(2, 0);
                if (!isValidPosition(*currentTetromino)) {
                    currentTetromino->move(-1, 0);
                    currentTetromino->rotate();
                    currentTetromino->rotate();
                    currentTetromino->rotate();
                }
            }
        }
        break;
    case Qt::Key_Space:
        dropTetromino();
        return;
    case Qt::Key_C:
    case Qt::Key_Shift:
        if (canHold) {
            canHold = false;
            Tetromino::Type currentType = currentTetromino->getType();
            if (heldTetromino == nullptr) {
                delete currentTetromino;
                currentTetromino = nextTetromino;
                int r = QRandomGenerator::global()->bounded(7);
                nextTetromino = new Tetromino(static_cast<Tetromino::Type>(r));
            } else {
                Tetromino::Type heldType = heldTetromino->getType();
                delete currentTetromino;
                currentTetromino = new Tetromino(heldType);
                delete heldTetromino;
                heldTetromino = nullptr;
            }
            heldTetromino = new Tetromino(currentType);
        }
        break;
    default:
        QWidget::keyPressEvent(event);
        return;
    }
    update();
}

void TetrisWidget::dropTetromino()
{
    if (!currentTetromino) return;
    while (isValidPosition(*currentTetromino))
        currentTetromino->move(0, 1);
    currentTetromino->move(0, -1);
    lockTetromino();
    update();
}

bool TetrisWidget::isValidPosition(const Tetromino &tet) const
{
    for (const QPoint &p : tet.getBlocks()) {
        int x = p.x(), y = p.y();
        if (x < 0 || x >= BOARD_WIDTH || y >= BOARD_HEIGHT) return false;
        if (y < 0) continue;
        if (board[y][x] != 0) return false;
    }
    return true;
}

int TetrisWidget::ghostDropY() const
{
    if (!currentTetromino) return 0;
    Tetromino ghost = *currentTetromino;
    while (isValidPosition(ghost))
        ghost.move(0, 1);
    ghost.move(0, -1);
    return ghost.getBlocks().first().y() - currentTetromino->getBlocks().first().y();
}

void TetrisWidget::lockTetromino()
{
    if (!currentTetromino) return;
    int type = static_cast<int>(currentTetromino->getType()) + 1;
    for (const QPoint &p : currentTetromino->getBlocks()) {
        int x = p.x(), y = p.y();
        if (y >= 0 && y < BOARD_HEIGHT && x >= 0 && x < BOARD_WIDTH)
            board[y][x] = type;
    }
    delete currentTetromino;
    currentTetromino = nullptr;
    canHold = true;
    clearLines();
}

void TetrisWidget::clearLines()
{
    flashLines.clear();
    for (int y = BOARD_HEIGHT - 1; y >= 0; --y) {
        bool full = true;
        for (int x = 0; x < BOARD_WIDTH; ++x)
            if (board[y][x] == 0) { full = false; break; }
        if (full)
            flashLines.prepend(y);
    }

    if (!flashLines.isEmpty()) {
        const int points[] = {0, 100, 300, 500, 800};
        int lines = flashLines.size();
        score += points[qMin(lines, 4)] * level;
        linesCleared += lines;
        int newLevel = linesCleared / 10 + 1;
        if (newLevel != level)
            level = newLevel;
        gameTimer->stop();
        flashStep = 0;
        flashTimer->start();
    } else {
        spawnNewTetromino();
    }
}

void TetrisWidget::spawnNewTetromino()
{
    currentTetromino = nextTetromino;
    nextTetromino    = nullptr;
    int r = QRandomGenerator::global()->bounded(7);
    nextTetromino = new Tetromino(static_cast<Tetromino::Type>(r));

    if (!isValidPosition(*currentTetromino)) {
        gameOver = true;
        gameTimer->stop();
        flashTimer->stop();
        update();
        QTimer::singleShot(1500, this, [this]() {
            emit gameFinished(score);
        });
    }
}

void TetrisWidget::drawBlock(QPainter &p, int x, int y, QColor color)
{
    QRect r(x + 1, y + 1, BLOCK_SIZE - 2, BLOCK_SIZE - 2);
    QLinearGradient grad(r.topLeft(), r.bottomRight());
    grad.setColorAt(0.0, color.lighter(160));
    grad.setColorAt(0.5, color);
    grad.setColorAt(1.0, color.darker(160));
    p.fillRect(r, grad);
    p.setPen(color.lighter(180));
    p.drawLine(r.topLeft(),    r.topRight());
    p.drawLine(r.topLeft(),    r.bottomLeft());
    p.setPen(color.darker(180));
    p.drawLine(r.bottomLeft(), r.bottomRight());
    p.drawLine(r.topRight(),   r.bottomRight());
}

void TetrisWidget::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing, false);

    QLinearGradient bg(0, 0, 0, height());
    bg.setColorAt(0.0, QColor(15, 15, 30));
    bg.setColorAt(1.0, QColor(5,   5, 15));
    p.fillRect(rect(), bg);

    static const QVector<QColor> colors = {
        QColor(0,   220, 220),
        QColor(220, 200,   0),
        QColor(180,   0, 200),
        QColor(0,   200,   0),
        QColor(220,   0,   0),
        QColor(0,    80, 220),
        QColor(220, 120,   0)
    };

    // Сетка
    p.setPen(QColor(35, 35, 55));
    for (int y = 0; y < BOARD_HEIGHT; ++y)
        for (int x = 0; x < BOARD_WIDTH; ++x)
            p.drawRect(x*BLOCK_SIZE, y*BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE);

    // Ghost piece
    if (currentTetromino) {
        int dy = ghostDropY();
        for (const QPoint &pos : currentTetromino->getBlocks()) {
            int gy = pos.y() + dy;
            if (gy >= 0) {
                QColor gc = currentTetromino->getColor();
                gc.setAlpha(60);
                p.fillRect(pos.x()*BLOCK_SIZE+2, gy*BLOCK_SIZE+2,
                           BLOCK_SIZE-4, BLOCK_SIZE-4, gc);
                p.setPen(gc.lighter(120));
                p.drawRect(pos.x()*BLOCK_SIZE+2, gy*BLOCK_SIZE+2,
                           BLOCK_SIZE-4, BLOCK_SIZE-4);
            }
        }
    }

    // Зафиксированные блоки
    for (int y = 0; y < BOARD_HEIGHT; ++y)
        for (int x = 0; x < BOARD_WIDTH; ++x)
            if (board[y][x] != 0) {
                bool isFlash = flashLines.contains(y);
                QColor c = colors[board[y][x] - 1];
                if (isFlash)
                    c = (flashStep % 2 == 0) ? Qt::white : c.lighter(180);
                drawBlock(p, x*BLOCK_SIZE, y*BLOCK_SIZE, c);
            }

    // Текущая фигура
    if (currentTetromino) {
        QColor c = currentTetromino->getColor();
        for (const QPoint &pos : currentTetromino->getBlocks())
            if (pos.y() >= 0)
                drawBlock(p, pos.x()*BLOCK_SIZE, pos.y()*BLOCK_SIZE, c);
    }

    // Панель справа
    int px = BOARD_WIDTH * BLOCK_SIZE + 10;
    int pw = 145;

    auto drawPanel = [&](int top, int h) {
        QRect pr(px, top, pw, h);
        p.fillRect(pr, QColor(25, 25, 45, 200));
        p.setPen(QColor(80, 80, 120));
        p.drawRect(pr);
    };

    auto drawLabel = [&](int top, const QString &label, const QString &value) {
        drawPanel(top, 45);
        p.setPen(QColor(150, 150, 200));
        p.setFont(QFont("Arial", 9, QFont::Bold));
        p.drawText(px+6, top+14, label);
        p.setPen(Qt::white);
        p.setFont(QFont("Arial", 13, QFont::Bold));
        p.drawText(px+6, top+36, value);
    };

    drawLabel(5,   "СЧЁТ",    QString::number(score));
    drawLabel(58,  "УРОВЕНЬ", QString::number(level));
    drawLabel(111, "ЛИНИИ",   QString::number(linesCleared));

    // ДАЛЕЕ
    drawPanel(164, 90);
    p.setPen(QColor(150, 150, 200));
    p.setFont(QFont("Arial", 9, QFont::Bold));
    p.drawText(px+6, 178, "ДАЛЕЕ");
    if (nextTetromino) {
        QVector<QPoint> blocks = nextTetromino->getBlocks();
        int minX = 999, minY = 999;
        for (const QPoint &bp : blocks) { minX = qMin(minX, bp.x()); minY = qMin(minY, bp.y()); }
        int bs = BLOCK_SIZE - 6;
        QColor c = nextTetromino->getColor();
        for (const QPoint &bp : blocks)
            drawBlock(p, px+8+(bp.x()-minX)*bs, 185+(bp.y()-minY)*bs, c);
    }

    // HOLD
    drawPanel(264, 90);
    p.setPen(QColor(150, 150, 200));
    p.setFont(QFont("Arial", 9, QFont::Bold));
    p.drawText(px+6, 278, "HOLD  [C]");
    if (heldTetromino) {
        QVector<QPoint> blocks = heldTetromino->getBlocks();
        int minX = 999, minY = 999;
        for (const QPoint &bp : blocks) { minX = qMin(minX, bp.x()); minY = qMin(minY, bp.y()); }
        int bs = BLOCK_SIZE - 6;
        QColor c = canHold ? heldTetromino->getColor() : QColor(80, 80, 80);
        for (const QPoint &bp : blocks)
            drawBlock(p, px+8+(bp.x()-minX)*bs, 285+(bp.y()-minY)*bs, c);
    }

    // Подсказка управления
    p.setPen(QColor(100, 100, 140));
    p.setFont(QFont("Arial", 8));
    int ky = 370;
    for (const QString &line : {
             QString("← → движение"),
             QString("↑  поворот"),
             QString("↓  сбросить"),
             QString("C  hold")
         }) {
        p.drawText(px+4, ky, line);
        ky += 16;
    }

    // Game Over оверлей
    if (gameOver) {
        p.fillRect(0, 0, BOARD_WIDTH*BLOCK_SIZE, BOARD_HEIGHT*BLOCK_SIZE,
                   QColor(0, 0, 0, 160));
        p.setPen(Qt::red);
        p.setFont(QFont("Arial", 28, QFont::Bold));
        p.drawText(QRect(0, 0, BOARD_WIDTH*BLOCK_SIZE, BOARD_HEIGHT*BLOCK_SIZE),
                   Qt::AlignCenter, "GAME\nOVER");
    }
}

#include "tetromino.h"

const QVector<QVector<QVector<int>>> Tetromino::shapes = {
    {{{1,1,1,1}}},
    {{{1,1},{1,1}}},
    {{{0,1,0},{1,1,1}}},
    {{{0,1,1},{1,1,0}}},
    {{{1,1,0},{0,1,1}}},
    {{{1,0,0},{1,1,1}}},
    {{{0,0,1},{1,1,1}}}
};

Tetromino::Tetromino(Type t) : type(t), position(4, 0) {}

void Tetromino::rotate() {
    rotation = (rotation + 1) % 4;
}

void Tetromino::move(int dx, int dy) {
    position += QPoint(dx, dy);
}

QVector<QPoint> Tetromino::getBlocks() const {
    QVector<QPoint> blocks;
    auto shape = shapes[static_cast<int>(type)];

    int rot = rotation % 4;
    for (int r = 0; r < rot; ++r) {
        QVector<QVector<int>> rotated(shape[0].size(),
                                      QVector<int>(shape.size(), 0));
        for (int y = 0; y < shape.size(); ++y)
            for (int x = 0; x < shape[y].size(); ++x)
                rotated[x][shape.size() - 1 - y] = shape[y][x];
        shape = rotated;
    }

    for (int y = 0; y < shape.size(); ++y)
        for (int x = 0; x < shape[y].size(); ++x)
            if (shape[y][x])
                blocks.append(position + QPoint(x, y));

    return blocks;
}

QColor Tetromino::getColor() const {
    static const QVector<QColor> colors = {
        QColor(0,   220, 220),
        QColor(220, 200,   0),
        QColor(180,   0, 200),
        QColor(0,   200,   0),
        QColor(220,   0,   0),
        QColor(0,    80, 220),
        QColor(220, 120,   0)
    };
    return colors[static_cast<int>(type)];
}

Tetromino::Type Tetromino::getType() const {
    return type;
}

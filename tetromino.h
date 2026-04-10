#ifndef TETROMINO_H
#define TETROMINO_H

#include <QVector>
#include <QPoint>
#include <QColor>

class Tetromino {
public:
    enum class Type { I, O, T, S, Z, J, L };

    Tetromino(Type type = Type::I);

    void rotate();
    void move(int dx, int dy);
    QVector<QPoint> getBlocks() const;
    QColor getColor() const;
    Type getType() const;

private:
    Type type;
    int rotation = 0;
    QPoint position;

    static const QVector<QVector<QVector<int>>> shapes;
};

#endif

#ifndef MENUWIDGET_H
#define MENUWIDGET_H

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QVector>

struct ScoreEntry {
    QString name;
    int     score;
};

class MenuWidget : public QWidget {
    Q_OBJECT
public:
    explicit MenuWidget(QWidget *parent = nullptr);
    void refreshScores(const QVector<ScoreEntry> &scores);

signals:
    void startRequested(const QString &nickname);

protected:
    void paintEvent(QPaintEvent *) override;
    void resizeEvent(QResizeEvent *) override;

private:
    void layoutWidgets();

    QLineEdit   *nameEdit    = nullptr;
    QPushButton *startBtn    = nullptr;
    QLabel      *scoresLabel = nullptr;
};

#endif

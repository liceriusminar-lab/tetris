#ifndef APICLIENT_H
#define APICLIENT_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QVector>
#include "menuwidget.h"

class ApiClient : public QObject {
    Q_OBJECT
public:
    explicit ApiClient(QObject *parent = nullptr);

    void fetchScores();
    void postScore(const QString &name, int score);

signals:
    void scoresReceived(const QVector<ScoreEntry> &scores);

private:
    QNetworkAccessManager *manager = nullptr;
   QString baseUrl = QString("https://tetris-server-production-ed55.up.railway.app");
};

#endif

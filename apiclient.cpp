#include "apiclient.h"
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QUrl>

ApiClient::ApiClient(QObject *parent) : QObject(parent)
{
    manager = new QNetworkAccessManager(this);
}

void ApiClient::fetchScores()
{
    QNetworkRequest req(QUrl(baseUrl + "/scores"));
    QNetworkReply *reply = manager->get(req);

    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        if (reply->error() != QNetworkReply::NoError) {
            reply->deleteLater();
            return;
        }
        QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
        QJsonArray arr = doc.array();

        QVector<ScoreEntry> scores;
        for (const QJsonValue &v : arr) {
            QJsonObject obj = v.toObject();
            scores.append({ obj["name"].toString(), obj["score"].toInt() });
        }
        emit scoresReceived(scores);
        reply->deleteLater();
    });
}

void ApiClient::postScore(const QString &name, int score)
{
    QNetworkRequest req(QUrl(baseUrl + "/scores"));
    req.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QJsonObject obj;
    obj["name"]  = name;
    obj["score"] = score;

    QNetworkReply *reply = manager->post(req, QJsonDocument(obj).toJson());
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        reply->deleteLater();
        fetchScores(); // обновляем таблицу после отправки
    });
}

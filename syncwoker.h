#ifndef SYNCWOKER_H
#define SYNCWOKER_H

#include <QObject>
#include <QThread>
#include <QCryptographicHash>
#include <QJsonDocument>
#include <QJsonObject>
#include <QSqlQuery>
#include <Lib/HttpClient.h>


class SyncWoker : public QThread
{
    Q_OBJECT
public:
    explicit SyncWoker(int,QObject *parent = nullptr);

    void sync();
private:
    int max;
    QString movie_id;
    QSqlDatabase db;
    bool running = true;

    QString md5(QString str);
    void run() override;
signals:
    void appendMsg(QString msg);
    void updateProgressBar(int);
    void setProgressBarMax(int);
    void clearMsg();
};

#endif // SYNCWOKER_H

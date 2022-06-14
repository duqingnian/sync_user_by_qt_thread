#pragma execution_character_set("utf-8")
#include "syncwoker.h"
#include <QCoreApplication>
#include <QElapsedTimer>
#include <QThread>

SyncWoker::SyncWoker(int _max,QObject *parent) : QThread(parent)
{
    this->movie_id = "-";

    this->max = _max;

    db = QSqlDatabase::addDatabase("QMYSQL");
    db.setHostName("127.0.0.1");
    db.setUserName("root");
    db.setPassword("123456");
    db.setDatabaseName("movie");
}

void SyncWoker::run()
{
    if (!db.open())
    {
        emit appendMsg("Error: Failed to connect database.");
    }
    else
    {
        emit appendMsg("数据库状态正常，打开成功！");
        sync();
        exec();
    }
}

void SyncWoker::sync()
{
    if(!running)
    {
        return;
    }
    emit updateProgressBar(max);
    HttpClient("https://movie.xpgo.net/api/test.html").success([=](const QString &response) {
        QJsonParseError err_rpt;
        QJsonDocument  jsonDoc = QJsonDocument::fromJson(response.toUtf8(), &err_rpt);
        if(err_rpt.error != QJsonParseError::NoError)
        {
            emit appendMsg("服务器返回了错误");
        }
        else
        {
            QJsonObject rootObj = jsonDoc.object();
            if(!rootObj.contains("code"))
            {
                emit appendMsg("服务器未返回code");
            }
            else
            {
                int code = rootObj.value("code").toInt();
                QString msg = rootObj.value("msg").toString();
                if(0 == code)
                {
                    if("COMPLETE" == msg)
                    {
                        emit clearMsg();
                        QThread::usleep(100);
                        emit appendMsg("完成了！");
                        this->movie_id = "-";
                        emit setProgressBarMax(300);
                        for (int i=300; i >= 0; i--) {
                            if(running)
                            {
                                qDebug() << i;
                                emit updateProgressBar(i);
                                if(i <= 0)
                                {
                                    emit updateProgressBar(0);
                                    QThread::usleep(100);
                                    sync();
                                }
                                QThread::sleep(1);
                            }
                        }
                    }
                    QString data = rootObj.value("data").toString();
                    QJsonDocument  DataDoc = QJsonDocument::fromJson(data.toUtf8(), &err_rpt);
                    if(err_rpt.error == QJsonParseError::NoError)
                    {
                        QJsonObject dataObj = DataDoc.object();

                        //a.id,a.cn_name,a.show_year,a.slug_name,a.area
                        QString cn_name = dataObj.value("cn_name").toString();
                        QString show_year = dataObj.value("show_year").toString();
                        QString slug_name = dataObj.value("slug_name").toString();
                        QString area = dataObj.value("area").toString();
                        movie_id = QString::number(dataObj.value("movie_id").toInt());

                        if(cn_name.length() > 0)
                        {
                            QString sql = QString("SELECT id from movie WHERE movie_id='" + movie_id + "'");

                            QSqlQuery q(this->db);
                            q.exec(sql);
                            if(q.first())
                            {
                                emit appendMsg("["+area+"] "+cn_name+" 存在");
                                QString update_sql("update `movie` set `show_year`='"+show_year+"' where movie_id = "+movie_id);
                                QSqlQuery update_q(this->db);
                                update_q.exec(update_sql);
                                QThread::usleep(100);
                                sync();
                            }
                            else
                            {
                                emit appendMsg( "["+area+"] "+cn_name+" 不存在就插入");
                                QString add_sql("INSERT INTO `movie`.`movie`(`id`, `movie_id`,  `cn_name`, `area`, `slug_name`, `show_year`) VALUES (NULL, "+movie_id+", '"+cn_name+"', '"+area+"', '"+slug_name+"', '"+show_year+"')");

                                QSqlQuery add_q(this->db);
                                add_q.exec(add_sql);
                                for (int i=max; i >= 0; i--) {
                                    if(running)
                                    {
                                        emit updateProgressBar(i);
                                        if(i <= 0)
                                        {
                                            emit updateProgressBar(0);
                                            QThread::usleep(50);
                                            sync();
                                        }
                                        QThread::sleep(1);
                                    }
                                }
                            }
                        }
                        else
                        {
                            emit appendMsg("名称为空");
                        }
                    }
                }
                else
                {
                    emit appendMsg("code不等于0");
                }
            }
        }

    })
            .fail([](const QString &response,int code){
                qDebug() << "HttpClient request faild! code=" << QString::number(code) << ",res=" << response;
            })
            .param("movie_id", this->movie_id).param("token", md5("DUQINGNIAN10985$" + this->movie_id)).header("content-type", "application/x-www-form-urlencoded").post();
}

//md5加密
QString SyncWoker::md5(QString str)
{
    QString MD5;
    QByteArray _str;
    _str = QCryptographicHash::hash(str.toLatin1(),QCryptographicHash::Md5);
    MD5.append(_str.toHex());
    return MD5;
}




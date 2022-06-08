#pragma execution_character_set("utf-8")
#include "syncwoker.h"
#include <QCoreApplication>
#include <QElapsedTimer>
#include <QThread>

SyncWoker::SyncWoker(int _max,QObject *parent) : QThread(parent)
{
    this->userid = "-";

    this->max = _max;

    db = QSqlDatabase::addDatabase("QMYSQL");
    db.setHostName("127.0.0.1");
    db.setUserName("root");
    //db.setPassword("123456");
    db.setPassword("xiali623745");
    db.setDatabaseName("disk");
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
    HttpClient("https://dd.czmylike.com/api/user_sync_api").success([=](const QString &response) {
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
                        this->userid = "-";
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

                        //a.id,a.active,a.depid,a.depname,a.groupname,a.groupid,a.avatar,a.name,a.shortname,a.pinyin,a.title,a.job_number,a.userid,a.unionid,a.join_time,a.leave_time
                        QString id = dataObj.value("id").toString();
                        QString active = QString::number(dataObj.value("active").toInt());
                        QString depid = QString::number(dataObj.value("depid").toInt());
                        QString depname = dataObj.value("depname").toString();
                        QString groupid = QString::number(dataObj.value("groupid").toInt());
                        QString groupname = dataObj.value("groupname").toString();
                        QString avatar = dataObj.value("avatar").toString();
                        QString name = dataObj.value("name").toString();
                        QString shortname = dataObj.value("shortname").toString();
                        QString pinyin = dataObj.value("pinyin").toString();
                        QString title = dataObj.value("title").toString();
                        QString job_number = dataObj.value("job_number").toString();
                        QString _userid = dataObj.value("userid").toString();
                        QString unionid = dataObj.value("unionid").toString();
                        QString join_time = QString::number(dataObj.value("join_time").toInt());
                        QString leave_time = QString::number(dataObj.value("leave_time").toInt());

                        if("" == join_time)
                        {
                            join_time = "0";
                        }
                        if("" == leave_time)
                        {
                            leave_time = "0";
                        }

                        if(_userid.length() > 5)
                        {
                            this->userid = _userid;
                            //查询用户是不是存在
                            QString sql = QString("SELECT id from ding_user WHERE userid = '" + userid + "'");

                            QSqlQuery q(this->db);
                            q.exec(sql);
                            if(q.first())
                            {
                                emit appendMsg("["+name+"]存在");
                                QString update_sql = "update `ding_user` set `depid`='" + depid + "', `depname`='" + depname + "', `groupname`='" + groupname + "', `groupid`='" + groupid + "',`active`=" + active + ",`leave_time`=" + leave_time + "  where `userid` = '" + userid + "'";
                                QSqlQuery update_q(this->db);
                                update_q.exec(update_sql);
                                QThread::usleep(500);
                                sync();
                            }
                            else
                            {
                                emit appendMsg( "["+name+"]不存在就插入");
                                QString add_sql = "INSERT INTO `ding_user`(`id`, `active`, `depid`, `depname`, `groupname`, `groupid`, `avatar`, `name`, `shortname`, `pinyin`, `title`, `job_number`, `userid`, `unionid`, `join_time`, `leave_time`) VALUES (NULL, " + active + ", " + depid + ", '" + depname + "', '" + groupname + "', " + groupid + ", '" + avatar + "', '" + name + "', '" + shortname + "', '" + pinyin + "', '" + title + "', '" + job_number + "', '" + userid + "', '" + unionid + "', " + join_time + ", " + leave_time + ")";
                                QSqlQuery add_q(this->db);
                                add_q.exec(add_sql);
                                for (int i=max; i >= 0; i--) {
                                    if(running)
                                    {
                                        emit updateProgressBar(i);
                                        if(i <= 0)
                                        {
                                            emit updateProgressBar(0);
                                            QThread::usleep(10);
                                            sync();
                                        }
                                        QThread::sleep(1);
                                    }
                                }
                            }
                        }
                        else
                        {
                            emit appendMsg("userid ["+_userid+"] lenth < 5");
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
            .param("userid", this->userid).param("token", md5("DUQINGNIAN10985$" + this->userid)).header("content-type", "application/x-www-form-urlencoded").post();
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




#pragma execution_character_set("utf-8")
#ifndef BASECONTROLLER_H
#define BASECONTROLLER_H

#include <QObject>
#include <QDialog>


class BaseController : public QDialog
{
    Q_OBJECT
public:
    explicit BaseController(QWidget *parent = nullptr);
    ~BaseController();

    int border_width = 8;
    void wait( int ms );
signals:

};

#endif // BASECONTROLLER_H

#pragma execution_character_set("utf-8")
#include "basecontroller.h"
#include <QCoreApplication>
#include <QElapsedTimer>

BaseController::BaseController(QWidget *parent) : QDialog(parent)
{
}

BaseController::~BaseController()
{
}

void BaseController::wait( int ms )
{
    QElapsedTimer timer;
    timer.start();

    while ( timer.elapsed() < ms )
        QCoreApplication::processEvents();
}



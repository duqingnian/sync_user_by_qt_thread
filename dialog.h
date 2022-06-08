#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <QSqlDatabase>
#include <Common/BaseWindow.h>
#include "syncwoker.h"

QT_BEGIN_NAMESPACE
namespace Ui { class Dialog; }
QT_END_NAMESPACE

class Dialog : public BaseWindow
{
    Q_OBJECT

public:
    Dialog(QWidget *parent = nullptr);
    ~Dialog();

private slots:
    void on_btn_start_clicked();

private:
    Ui::Dialog *ui;
    QWidget* bg;
    SyncWoker* worker;
};
#endif // DIALOG_H

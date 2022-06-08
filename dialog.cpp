#pragma execution_character_set("utf-8")
#include "dialog.h"
#include "ui_dialog.h"
#include <QDebug>
#include <QThread>

Dialog::Dialog(QWidget *parent) : BaseWindow(parent) , ui(new Ui::Dialog)
{
    ui->setupUi(this);
    this->setStyleSheet("font-family: \"Microsoft Yahei\";font-size:14px;");

    int max = 5;
    worker = new SyncWoker(max);

    connect(worker,&SyncWoker::updateProgressBar,this,[=](int i){
        ui->progressBar->setValue(i);
    });
    connect(worker,&SyncWoker::appendMsg,this,[=](QString msg){
        ui->plainTextEdit->appendPlainText(msg);
    });
    connect(worker,&SyncWoker::setProgressBarMax,this,[=](int maxNum){
        ui->progressBar->setMaximum(maxNum);
    });
    connect(worker,&SyncWoker::clearMsg,this,[=](){
        ui->plainTextEdit->clear();
    });


    //标题栏
    this->m_TitleBar->setButtonType(MIN_BUTTON);
    this->m_TitleBar->set_width(this->width());
    this->m_TitleBar->show_windows();
    this->m_TitleBar->setTitle("文件服务器 - 用户同步工具");
    connect(m_TitleBar, &TitleBar::signalButtonCloseClicked, this, [=](){
        worker->exit();
        QCoreApplication::quit();
    });

    bg = new QWidget(this);
    bg->setObjectName("bg");
    bg->resize(this->width() - 16,this->height()-16 - this->m_TitleBar->height());
    bg->move(8,8+this->m_TitleBar->height());
    bg->setStyleSheet("#bg{background:#F4F5F7;}");
    bg->lower();

    ui->progressBar->setMaximum(max);
    ui->progressBar->setValue(max);
}

Dialog::~Dialog()
{
    delete ui;
}

//点击了开始按钮
void Dialog::on_btn_start_clicked()
{
    ui->btn_start->setEnabled(false);
    worker->start();
}


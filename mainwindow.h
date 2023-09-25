//
// Created by lixiang on 2023/9/12.
//

#ifndef SOCKET_MAINWINDOW_H
#define SOCKET_MAINWINDOW_H

#include <QMainWindow>
#include "def.h"
#include "util.h"

// 静态加入库文件，但是没用，在build的命令处添加了动态链接库的指令
#pragma comment(lib, "ws2_32")

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    void Init_ui();

    explicit MainWindow(QWidget *parent = nullptr);

    ~MainWindow() override;

    signals:
            void sendMsg(int
    code,
    QString msg
    );
private
    slots:
            void Slot_File_Path_Select_pressed();

    void Slot_Save2log_pressed();

    void Slot_Upload_pressed();

private:
    Ui::MainWindow *ui;

    // functions
    int sendPkt(const char *buf, int len);

    int recvPkt(char *buf, int len);

    // data
    sockaddr_in server_ip, client_ip; // 定义服务器和客户端ip
    SOCKET sock; //定义客户端套接字
    int bytesRecv, bytesSend, lastBytesRecv, lastBytesSend;
};


#endif //SOCKET_MAINWINDOW_H

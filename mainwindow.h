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

    // 定义一些信号函数
    signals:

    // 向Output窗口输出信息
    void Write2Output(int code, const QString &msg, bool raw = false);

    // 设置ProcessBar的数值为val
    void Set_ProcessBar_Val(int val);

    // 设置上传速度
    void Set_Upload_Speed(const QString &speed);

    // 设置下载速度
    void Set_Download_Speed(const QString &speed);


public slots:
    /****** 槽函数，用于响应按钮的点击事件 ******/
    // 选择文件路径
    void Slot_File_Path_Select_pressed();

    // 选择下载时存放的文件路径
    void Slot_Download_Path_Select_pressed();

    // 如果Output区非空则将Output中的输出保存到log文件中
    void Slot_Save2log_pressed();

    // 上传文件按钮按下
    void Slot_Upload_pressed();

    // 下载文件按钮按下
    void Slot_Download_pressed();
    /****** 槽函数，用于响应按钮的点击事件 ******/

    // 定时器超时，用于更新吞吐量
    void onTimeout();

private:
    Ui::MainWindow *ui;

    // functions
    int open_file();
    int sendPkt(const char *buf, int len);
    int recvPkt(char *buf, int len);
    int Wait_PKT(int TimeOut_Ms, int &Rcvd_Size);

    int Wait_ACK_DAT(int TimeOut_Ms, int &Rcvd_Size);
    int Wait_Specific_PKT(uint16_t block, int TimeOut_Ms, int &Rcvd_Size, uint16_t Pkt_Type);
    void Terminate(bool is_success);

    // data
    sockaddr_in server_ip, client_ip; // 定义服务器和客户端ip
    SOCKET sock; //定义客户端套接字
    char *RemoteFile = nullptr, *LocalFile = nullptr; // 定义远程文件和本地文件
    char filename[TFTP_MAX_BLOCK_SIZE]; // 文件名
    int Bytes_Recv = 0, Bytes_Send = 0, Last_Bytes_Recv = 0, Last_Bytes_Send = 0; // 用于计算吞吐量
    int TotalRetransmitCount = 0, FileSize, Mode, OP, Total_Size; // 总重传次数，文件大小，模式，操作码，总大小
    int server_ip_len = sizeof(server_ip); // 服务器地址长度
    uint16_t Cur_Block_Num, BlockSize = 512, Timeout = 1; // 当前块号，块大小，超时时间
    FILE *fp = nullptr; // 文件指针
    double percent; // 上传进度
    clock_t StartTime, EndTime; // 计时器
    QTimer *timer = new QTimer(this); // 定时器

    // 需要用到的几个Packet
    PKG_DATA_ERROR Rcvd_Pkt = {0}, dataPkt = {htons(CMD_DAT), {0}};
    PKG_ACK Ack_Pkt = {htons(CMD_ACK), 0};
    PKG_REQUEST Req_Pkt = {0, {0}};
};


#endif //SOCKET_MAINWINDOW_H

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

    // 信号函数，用于向Output窗口输出信息
    void Write2Output(int code, const QString msg, bool raw = false);

    void Set_ProcessBar_Val(int val);

private slots:
    /****** 槽函数，用于响应按钮的点击事件 ******/
    // 选择文件路径
    void Slot_File_Path_Select_pressed();

    // 如果Output区非空则将Output中的输出保存到log文件中
    void Slot_Save2log_pressed();

    // 上传文件按钮被按
    void Slot_Upload_pressed();
    /****** 槽函数，用于响应按钮的点击事件 ******/

private:
    Ui::MainWindow *ui;

    // functions
    int open_file();
    int sendPkt(const char *buf, int len);
    int recvPkt(char *buf, int len);

    int Wait_PKT(int TimeOut_Ms, int &Rcvd_Size);

    int Wait_ACK(int TimeOut_Ms, int &Rcvd_Size);

    int Wait_Specific_PKT(uint16_t block, int TimeOut_Ms, int &Rcvd_Size, uint16_t Pkt_Type);

    void Terminate(bool is_success);

    // data
    sockaddr_in server_ip, client_ip; // 定义服务器和客户端ip
    SOCKET sock; //定义客户端套接字
    const char *RemoteFile = nullptr, *LocalFile = nullptr;
    char filename[TFTP_MAX_BLOCK_SIZE];
    int Bytes_Recv, Bytes_Send, lastBytesRecv, lastBytesSend;
    int TotalRetransmitCount, FileSize, Mode, OP, Total_Size;
    int server_ip_len = sizeof(server_ip); // 服务器地址长度
    uint16_t Cur_Block_Num, BlockSize = 512, Timeout = 2;
    FILE *fp = nullptr; // 文件指针
    double percent; // 上传进度
    clock_t StartTime, EndTime; // 计时器

    // 需要用到的几个Packet
    PKG_DATA_ERROR Rcvd_Pkt = {0}, dataPkt = {htons(CMD_DAT), {0}};
    PKG_ACK Ack_Pkt = {htons(CMD_ACK), 0};
    PKG_REQUEST Req_Pkt = {0, {0}};
};


#endif //SOCKET_MAINWINDOW_H

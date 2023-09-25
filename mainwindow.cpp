//
// Created by lixiang on 2023/9/12.
//

// You may need to build the project (run Qt uic code generator) to get "ui_MainWindow.h" resolved

#include "mainwindow.h"
#include "ui_MainWindow.h"


MainWindow::MainWindow(QWidget *parent)
        : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    connect(this, &MainWindow::sendMsg, this, [=](int code, QString msg) {
        QString fullMsg = QDateTime::currentDateTime().toString("[MM/dd hh:mm:ss]");
        // OK设置成绿色 ERROR设置成红色
        fullMsg += (code == TFTP_CORRECT ? " <font color=\"#00FF00\">OK</font> "
                                         : " <font color=\"#FF0000\">ERROR</font> ") + msg;
        ui->Output->append(fullMsg);
    });
    connect(ui->up_selectpath, SIGNAL(clicked()), this, SLOT(Slot_File_Path_Select_pressed()));
    connect(ui->Clear, SIGNAL(clicked()), ui->Output, SLOT(clear()));
    connect(ui->Save, SIGNAL(clicked()), this, SLOT(Slot_Save2log_pressed()));
    connect(ui->uploadconfirm, SIGNAL(clicked()), this, SLOT(Slot_Upload_pressed()));
    Init_ui();
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::Init_ui() {
    // 完成一系列的初始化功能
    WSADATA wsaData;
    int nRc = WSAStartup(0x0101, &wsaData);
    if (nRc) {
        emit sendMsg(ERROR_SOCKET_ERROR, QString("WSAStartup failed with error: %1").arg(nRc));
    }
    if (wsaData.wVersion != 0x0101) {
        //报告错误给用户，清除Winsock
        emit sendMsg(ERROR_SOCKET_ERROR, QString("Winsock version is not correct: %1").arg(wsaData.wVersion));
        WSACleanup();
    }
    // emit sendMsg(TFTP_CORRECT, QString("Winsock version is not correct: %1").arg(wsaData.wVersion));
    // ui->Output->append("<font color=\"#FF0000\">Socket creation failed!\n</font>"); // 设置红色的字体
}

void MainWindow::Slot_File_Path_Select_pressed() {
    // 选择文件路径
    QDir dir;
    // 选择想要上传的文件
    QString PathName = QFileDialog::getOpenFileName(this, tr("Select Local File"), "", tr("file(*)"));
    // 设置文件路径于屏幕上
    ui->up_LocalFileName->setText(PathName);
}

void MainWindow::Slot_Save2log_pressed() {
    // 如果Output区非空则将Output中的输出保存到log文件中
    if (!ui->Output->toPlainText().isEmpty()) {
        QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"), "",
                                                        tr("Log Files (*.log);;Text Files (*.txt)"));
        QFile file(fileName);
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
            return;
        QTextStream out(&file);
        out << ui->Output->toPlainText();
        file.close();
        QMessageBox::information(this, "Save Log Successfully", "Save log successfully as: \"" + fileName + "\"");
    } else {
        QMessageBox::information(this, "Fail to Save Log", "Log is empty.");
    }
}

void MainWindow::Slot_Upload_pressed() {

    // 从用户输入处获取信息并将QString转换为C风格字符串
    QByteArray filepath = ui->up_LocalFileName->text().toLatin1();
    QByteArray server_IP = ui->up_Server_IP->text().toLatin1();
    QByteArray client_IP = ui->up_Local_IP->text().toLatin1();
    // 若信息为空则弹出提示框
    if (filepath.isEmpty() || server_IP.isEmpty() || client_IP.isEmpty()) {
        QMessageBox::critical(this, tr("Error"), tr("Incomplete Information!"), QMessageBox::Ok);
        return;
    }
    // 检查ip地址是否符合格式要求
    // 正则表达式匹配^(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)$
    QRegularExpressionMatch match_server = QRegularExpression(
            R"(^(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)$)").match(
            server_IP);
    QRegularExpressionMatch match_client = QRegularExpression(
            R"(^(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)$)").match(
            client_IP);
    if (!match_server.hasMatch() || !match_client.hasMatch()) {
        QMessageBox::critical(this, tr("Error"), tr("IP Address Format Error!"), QMessageBox::Ok);
        return;
    }
    // 检查相应路径文件是否存在
    if (!QFile::exists(ui->up_LocalFileName->text())) {
        QMessageBox::critical(this, tr("Error"), tr("File Not Found!"), QMessageBox::Ok);
        return;
    }

    // 将QByteArray转换为C风格字符串
    const char *FileName = filepath.data();
    const char *Server_ip = server_IP.data();
    const char *Client_ip = client_IP.data();
    /* 测试用户输入模块*/
    // 转化为Qstring，输出到Output中
    emit sendMsg(TFTP_CORRECT, QString("Upload Process Begins!"));
    ui->Output->append("File Path: " + QString::fromLatin1(FileName));
    ui->Output->append("Server IP: " + QString::fromLatin1(Server_ip));
    ui->Output->append("Client IP: " + QString::fromLatin1(Client_ip));

    // 从路径中提取文件名
    char filename[TFTP_DEFAULT_BLOCK_SIZE];
    int temp = 0;
    for (int i = 0; FileName[i] != '\0'; i++, temp++) {
        if (FileName[i] == '/') {
            i++;
            temp = 0;
            filename[temp] = FileName[i];
        } else {
            filename[temp] = FileName[i];
        }
    }
    filename[temp] = '\0';

    // 设置服务器端ipv4
    server_ip.sin_family = AF_INET;
    server_ip.sin_port = htons(SERV_PORT);
    InetPtonA(AF_INET, Server_ip, (void *) &server_ip.sin_addr);
    // 设置客户端ipv4
    client_ip.sin_family = AF_INET;
    client_ip.sin_port = htons(0);
    InetPtonA(AF_INET, Client_ip, (void *) &client_ip.sin_addr);
    client_ip.sin_addr.S_un.S_addr = inet_addr(Client_ip);

    // 创建客户端套接字,使用UDP协议即type为数据报套接字
    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock == INVALID_SOCKET) {
        emit sendMsg(ERROR_SOCKET_ERROR, QString("Socket creation failed with error: %1").arg(WSAGetLastError()));
        return;
    }
    // 提示套接字创建成功的信息
    emit sendMsg(TFTP_CORRECT, QString("Socket creation successful!"));
    // 绑定客户端套接字
    if (bind(sock, (sockaddr *) &client_ip, sizeof(client_ip)) == SOCKET_ERROR) {
        emit sendMsg(ERROR_SOCKET_ERROR, QString("Bind socket failed with code : %1.").arg((WSAGetLastError())));
        return;
    }
    // 提示套接字绑定成功的信息
    emit sendMsg(TFTP_CORRECT, QString("Socket Binding Successful!"));

    // 定义传输的Mode
    int Mode = ui->up_comboBox->currentIndex();
    // 向Output显示传输方式
    ui->Output->append("Current Transmission Mode: " + ui->up_comboBox->currentText());
    // 定义数据包
    PKG_REQUEST request_packet, receive_packet;
    // 组装请求报文
    request_packet.opcode = htons(CMD_WRQ);

    // 编码成netascii，正常模式下就是以二进制bin方式传输
    if (Mode == MODE_NETASCII)
        encodeNetascii(FileName);
    // 填充filename和mode
    int reqMsgLen = 0;
    appendMsg(request_packet.reqMsg, REQ_SIZE, &reqMsgLen, "%s", filename);
    appendMsg(request_packet.reqMsg, REQ_SIZE, &reqMsgLen, Mode == MODE_NETASCII ? "netascii" : "octet");

    // 发送请求报文
    int res = sendPkt((char *) &request_packet, OP_SIZE + reqMsgLen);
    if (res < 0) {
        emit sendMsg(ERROR_SEND_REQ_FAIL,
                     QString("REQUEST : Send request packet failed with code : %1.").arg((WSAGetLastError())));
        return;
    } else if (res < OP_SIZE + reqMsgLen) {
        emit sendMsg(TFTP_CORRECT, QString("REQUEST : Send request packet partially with length : %1.").arg(res));
    } else {
        emit sendMsg(TFTP_CORRECT, QString("REQUEST : Send request packet successfully with length : %1.").arg(res));
    }

}

/* 发送报文
 * @param
 * buf : 待发送报文buf区
 * len : 报文长度
 * @return : 发送的字节数
 * */
int MainWindow::sendPkt(const char *buf, int len) {
    int ret = sendto(sock, buf, len, 0, (SOCKADDR *) &server_ip, sizeof(server_ip));
    if (ret > 0) { bytesSend += ret; }
    return ret;
}

/* 接收报文
 * @param
 * buf : 待接收报文buf区
 * len : 报文长度
 * @return : 接收的字节数
 *
int MainWindow::recvPkt(char *buf, int len) {
    int ret = recvfrom(sock, buf, len, 0, (SOCKADDR*)&server_ip, );
    if (ret > 0) { bytesRecv += ret; }
    return ret;
}
*/





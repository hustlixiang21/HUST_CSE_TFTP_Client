#include "mainwindow.h"
#include "ui_MainWindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    /****** 链接信号和槽函数 ******/
    /* 连接Write2Output信号和槽函数,用于向ui中的Output区输出信息
     * 默认错误信息为红色，正确信息为绿色,若只是正常输出，raw设置为True即可
     * @param
     * code : 信息代码
     * msg : 信息内容
     * raw : 是否输出为原始信息
     * */
    connect(this, &MainWindow::Write2Output, this, [=](int code, const QString &msg, bool raw) {
        QString fullMsg = QDateTime::currentDateTime().toString("[MM/dd hh:mm:ss]");
        // OK设置成绿色 ERROR设置成红色
        if (raw) {
            fullMsg += msg;
        } else {
            fullMsg += (code == TFTP_CORRECT ? " <font color=\"#00FF00\">OK</font> "
                                             : " <font color=\"#FF0000\">ERROR</font> ") + msg;
        }
        ui->Output->append(fullMsg);
    });
    // 链接一些按钮的点击事件
    connect(ui->up_selectpath, SIGNAL(clicked()), this, SLOT(Slot_File_Path_Select_pressed()));
    connect(ui->down_selectpath, SIGNAL(clicked()), this, SLOT(Slot_Download_Path_Select_pressed()));
    connect(ui->Clear, SIGNAL(clicked()), ui->Output, SLOT(clear()));
    connect(ui->Save, SIGNAL(clicked()), this, SLOT(Slot_Save2log_pressed()));
    connect(ui->uploadconfirm, SIGNAL(clicked()), this, SLOT(Slot_Upload_pressed()));
    connect(ui->downloadconfirm, SIGNAL(clicked()), this, SLOT(Slot_Download_pressed()));
    // 连接进度条信号
    connect(this, &MainWindow::Set_ProcessBar_Val, this, [=](int val) { ui->ProgressBar->setValue(val); });
    // 连接实时吞吐量并按照一定时间间隔更新
    connect(timer, &QTimer::timeout, this, &MainWindow::update_speed);
    connect(this, &MainWindow::Set_Upload_Speed, this, [=](const QString &speed) { ui->UploadLabel->setText(speed); });
    connect(this, &MainWindow::Set_Download_Speed, this,
            [=](const QString &speed) { ui->DownloadLabel->setText(speed); });
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
        emit Write2Output(ERROR_SOCKET_ERROR, QString("WSAStartup failed with error: %1").arg(nRc));
    }
    if (wsaData.wVersion != 0x0101) {
        //报告错误给用户，清除Winsock
        emit Write2Output(ERROR_SOCKET_ERROR, QString("Winsock version is not correct: %1").arg(wsaData.wVersion));
        WSACleanup();
    }
    /* output窗口测试
    emit Write2Output(TFTP_CORRECT, QString("Winsock version is not correct: %1").arg(wsaData.wVersion));
    ui->Output->append("<font color=\"#FF0000\">Socket creation failed!\n</font>"); // 设置红色的字体
    */
}

void MainWindow::Slot_File_Path_Select_pressed() {
    // 选择想要上传的文件
    QString PathName = QFileDialog::getOpenFileName(this, tr("Select Local File"), "", tr("file(*)"));
    // 设置文件路径于屏幕上
    ui->up_LocalFileName->setText(PathName);
}

void MainWindow::Slot_Download_Path_Select_pressed() {
    // 选择保存下载文件存放的文件夹
    QString PathName = QFileDialog::getExistingDirectory(this, tr("Select Local File"), "");
    // 设置文件路径于屏幕上
    ui->down_LocalFilePath->setText(PathName);
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
    /****** 获取信息并检查参数，重置进度条 ******/
    // 重置进度条和一些信息
    emit Set_ProcessBar_Val(0);
    FileSize = 0;
    Bytes_Send = 0;
    // 从用户输入处获取信息并将QString转换为QByteArray类型
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
    LocalFile = filepath.data();
    const char *Server_ip = server_IP.data();
    const char *Client_ip = client_IP.data();

    // 转化为Qstring，输出到Output中
    emit Write2Output(TFTP_CORRECT, QString("Upload Process Begins!"), true);
    // 全用Write2Output输出，第二个参数为true表示输出为原始信息
    emit Write2Output(TFTP_CORRECT, QString("Local FilePath: %1").arg(LocalFile), true);
    emit Write2Output(TFTP_CORRECT, QString("Server IP: %1").arg(Server_ip), true);
    emit Write2Output(TFTP_CORRECT, QString("Client IP: %1").arg(Client_ip), true);

    // 从路径中提取文件名
    int temp = 0;
    for (int i = 0; LocalFile[i] != '\0'; i++, temp++) {
        if (LocalFile[i] == '/') {
            i++;
            temp = 0;
            filename[temp] = LocalFile[i];
        } else {
            filename[temp] = LocalFile[i];
        }
    }
    filename[temp] = '\0';
    /****** 获取信息并检查参数 ******/

    /****** 初始化操作，创建绑定套接字和打开文件 ******/
    // 开始计时
    StartTime = clock();
    // 打开文件
    OP = CMD_WRQ;
    if (open_file() != TFTP_CORRECT) {
        Terminate(false);
        return;
    }

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
        emit Write2Output(ERROR_SOCKET_ERROR, QString("Socket creation failed with error: %1").arg(WSAGetLastError()));
        return;
    }
    // 提示套接字创建成功的信息
    emit Write2Output(TFTP_CORRECT, QString("Socket creation successful!"));
    // 绑定客户端套接字
    if (bind(sock, (sockaddr *) &client_ip, sizeof(client_ip)) == SOCKET_ERROR) {
        emit Write2Output(ERROR_SOCKET_ERROR, QString("Bind socket failed with code : %1.").arg((WSAGetLastError())));
        return;
    }
    // 提示套接字绑定成功的信息
    emit Write2Output(TFTP_CORRECT, QString("Socket Binding Successful!"));
    /****** 初始化操作，绑定套接字并打开文件 ******/

    /****** 构造请求包并发送请求报文 ******/
    // 定义传输的Mode
    Mode = ui->up_comboBox->currentIndex();
    // 向Output显示传输方式
    emit Write2Output(TFTP_CORRECT, QString("Mode : %1").arg(Mode == MODE_NETASCII ? "netascii" : "octet"));
    // 组装请求报文
    Req_Pkt.opcode = htons(OP);
    // 编码成netascii，正常模式下就是以二进制bin方式传输
    if (Mode == MODE_NETASCII)
        encodeNetascii(LocalFile);
    // 填充filename和mode
    int reqMsgLen = 0;
    appendMsg(Req_Pkt.reqMsg, REQ_SIZE, &reqMsgLen, "%s", filename);
    appendMsg(Req_Pkt.reqMsg, REQ_SIZE, &reqMsgLen, Mode == MODE_NETASCII ? "netascii" : "octet");

    // 设置定时器，用于更新吞吐量
    timer->start(TFTP_REFRESH_INTERVAL);
    emit Write2Output(TFTP_CORRECT,
                      QString("<font color='blue'>********************** Start Write **********************</font>"),
                      true);
    // 发送请求报文
    int res = sendPkt((char *) &Req_Pkt, OP_SIZE + reqMsgLen);
    if (res < 0) {
        emit Write2Output(ERROR_SEND_REQ_FAIL,
                          QString("REQUEST : Send request packet failed with code : %1.").arg((WSAGetLastError())));
        return;
    } else if (res < OP_SIZE + reqMsgLen) {
        emit Write2Output(TFTP_CORRECT, QString("Send request packet partially with length : %1.").arg(res));
    } else {
        emit Write2Output(TFTP_CORRECT, QString("Send request packet successfully with length : %1.").arg(res));
    }
    /****** 构造请求包并发送请求报文 ******/

    /****** 等待服务器的响应，若完成之后就可以进行数据的发送 ******/
    bool GetFirstAck = false, Finished = false;
    int Rcvd_Size, Data_Size = 0, Retransmit_Count = 0, Wait_DataRet;
    Cur_Block_Num = 0, TotalRetransmitCount = 0;

    // 等待第一个ack的到达
    while (Retransmit_Count <= TFTP_MAX_RETRANSMIT) {
        int wait_for_ack = Wait_ACK_DAT(Timeout * 1000, Rcvd_Size);
        // 收到第一个ack
        if (wait_for_ack == CMD_ACK && ntohs(Rcvd_Pkt.block) == 0) {
            emit Write2Output(TFTP_CORRECT, QString("Get ACK 0, Start Sending Data!"));
            GetFirstAck = true;
            break;
        }
            // 如果收到错误包
        else if (wait_for_ack == CMD_ERR) {
            // 如果收到的错误包不完整
            if (Rcvd_Size < OP_SIZE + ERRCODE_SIZE) {
                emit Write2Output(ERROR_WRONG_PKT, "Wrong error packet.");
            }
                // 如果收到的错误包完整，就显示错误包的信息并终止传输
            else {
                emit Write2Output(ntohs(Rcvd_Pkt.errCode), QString(Rcvd_Pkt.errMsg));
            }
            Terminate(false);
            return;
        }
            // 重传
        else {
            Retransmit_Count++;
            TotalRetransmitCount++;
            emit Write2Output(ERROR_RETRANSMIT_EQU, "Retransmit request packet.");
            if (sendPkt((char *) &Req_Pkt, OP_SIZE + reqMsgLen) < 0) {
                emit Write2Output(ERROR_SEND_REQ_FAIL, "Send request packet fail.");
                Terminate(false);
                return;
            }
        }
    }
    // 如果重传次数过多，就终止传输
    if (Retransmit_Count > TFTP_MAX_RETRANSMIT) {
        emit Write2Output(ERROR_RETRANSMIT_TOO_MUCH, "Retransmission times too much.");
        Terminate(false);
        return;
    }

    //读取本地文件并且发送DATA报文
    Retransmit_Count = 0;
    while (Retransmit_Count <= TFTP_MAX_RETRANSMIT) {
        // 如果是第一个区块，则是ACK0
        if (Cur_Block_Num == 0 && GetFirstAck) {
            Wait_DataRet = CMD_ACK;
        }
            // 否则就是等待特定的ACK报文，才能继续发送下一个数据包
        else {
            Wait_DataRet = Wait_Specific_PKT(Cur_Block_Num, Timeout * 1000, Rcvd_Size, CMD_ACK);
        }
        // 接收到ACK报文
        if (Wait_DataRet == CMD_ACK) {
            if (Cur_Block_Num) emit Write2Output(TFTP_CORRECT,
                                                 QString("Get ACK %1, Data Packet Sent Successfully!").arg(
                                                         Cur_Block_Num));
            if (!Finished) {
                // 读取本地文件
                Data_Size = (int) fread(dataPkt.data, 1, TFTP_MAX_BLOCK_SIZE, fp);
                // 如果小于512Bytes，就说明是最后一个包
                if (Data_Size < BlockSize) {
                    Finished = true;
                }
                // 发送DATA报文
                dataPkt.block = htons(++Cur_Block_Num);
                // 若发送失败，就终止传输
                if (sendPkt((char *) &dataPkt, Data_Size + OP_SIZE + BLOCK_SIZE) < 0) {
                    emit Write2Output(ERROR_SEND_DATA_FAIL,
                                      QString("DATA %1 : Send DATA packet fail.").arg(Cur_Block_Num - 1));
                    Terminate(false);
                    return;
                }
                // 更新进度条和一些需要统计的数据
                Retransmit_Count = 0;
                FileSize += Data_Size;
                if ((double) FileSize / Total_Size > percent) {
                    percent = (double) FileSize / Total_Size + 0.01;
                    emit Set_ProcessBar_Val((int) (percent * 100));
                }
            } else {
                Terminate(true);
                break;
            }
        }
            // 接收到ERROR报文
        else if (Wait_DataRet == CMD_ERR) {
            if (Rcvd_Size < OP_SIZE + ERRCODE_SIZE) {
                emit Write2Output(ERROR_WRONG_PKT, "Wrong error packet.");
            } else {
                emit Write2Output(ntohs(Rcvd_Pkt.errCode), QString(Rcvd_Pkt.errMsg));
            }
            Terminate(false);
            return;
        }
            // 接收到其他报文
        else {
            Retransmit_Count++;
            TotalRetransmitCount++;
            if (Cur_Block_Num == 0) {
                emit Write2Output(ERROR_RETRANSMIT_EQU, "Retransmit request packet.");
                if (sendPkt((char *) &Req_Pkt, OP_SIZE + reqMsgLen) < 0) {
                    emit Write2Output(ERROR_SEND_REQ_FAIL, "REQUEST : Send request packet fail.");
                    Terminate(false);
                    return;
                }
            } else {
                emit Write2Output(ERROR_RETRANSMIT_ACK, QString("Retransmit DATA packet %1.").arg(Cur_Block_Num));
                if (sendPkt((char *) &dataPkt, Data_Size + OP_SIZE + BLOCK_SIZE) < 0) {
                    emit Write2Output(ERROR_SEND_DATA_FAIL,
                                      QString("DATA %1 : Send DATA packet fail.").arg(Cur_Block_Num));
                    Terminate(false);
                    return;
                }
            }
        }
    }

    if (Retransmit_Count > TFTP_MAX_RETRANSMIT) {
        emit Write2Output(ERROR_RETRANSMIT_TOO_MUCH, "Retransmission times too much.");
        Terminate(false);
    }
}


void MainWindow::Slot_Download_pressed() {
    /****** 获取信息并检查参数，重置进度条 ******/
    // 重置进度条和一些信息
    emit Set_ProcessBar_Val(0);
    FileSize = 0;
    Bytes_Recv = 0;
    // 从用户输入处获取信息并将QString转换为QByteArray类型
    QByteArray FileName = ui->down_Server_FileName->text().toLatin1();
    QByteArray filepath = ui->down_LocalFilePath->text().toLatin1();
    QByteArray server_IP = ui->down_Server_IP->text().toLatin1();
    QByteArray client_IP = ui->down_Local_IP->text().toLatin1();
    // 若信息为空则弹出提示框
    if (filepath.isEmpty() || server_IP.isEmpty() || client_IP.isEmpty() || FileName.isEmpty()) {
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

    // 将QByteArray转换为C风格字符串
    RemoteFile = FileName.data();
    // 将文件路径和文件名组合
    QByteArray QLocalFile = filepath + "/" + FileName;
    LocalFile = QLocalFile.data();
    const char *Server_ip = server_IP.data();
    const char *Client_ip = client_IP.data();

    // 判断本地文件是否存在
    if (QFile::exists(QLocalFile)) {
        // 如果存在就弹出提示框，问是否覆盖，如果是则继续进行，如果否则退出
        if (QMessageBox::No == QMessageBox::warning(this, tr("File Already Exists"), tr("Do you want to overlay it?"),
                                                    QMessageBox::Yes | QMessageBox::No)) {
            return;
        }
    }

    // 转化为Qstring，输出到Output中
    emit Write2Output(TFTP_CORRECT, QString("Download Process Begins!"), true);
    // 全用Write2Output输出，第二个参数为true表示输出为原始信息
    emit Write2Output(TFTP_CORRECT, QString("Remote FileName: %1").arg(FileName), true);
    emit Write2Output(TFTP_CORRECT, QString("Local FilePath: %1").arg(LocalFile), true);
    emit Write2Output(TFTP_CORRECT, QString("Server IP: %1").arg(Server_ip), true);
    emit Write2Output(TFTP_CORRECT, QString("Client IP: %1").arg(Client_ip), true);

    /****** 初始化操作，绑定套接字 ******/
    // 开始计时
    StartTime = clock();
    // 打开文件
    OP = CMD_RRQ;
    if (open_file() != TFTP_CORRECT) {
        Terminate(false);
        return;
    }
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
        emit Write2Output(ERROR_SOCKET_ERROR, QString("Socket creation failed with error: %1").arg(WSAGetLastError()));
        return;
    }
    // 提示套接字创建成功的信息
    emit Write2Output(TFTP_CORRECT, QString("Socket creation successful!"));
    // 绑定客户端套接字
    if (bind(sock, (sockaddr * ) & client_ip, sizeof(client_ip)) == SOCKET_ERROR) {
        emit Write2Output(ERROR_SOCKET_ERROR, QString("Bind socket failed with code : %1.").arg((WSAGetLastError())));
        return;
    }
    // 提示套接字绑定成功的信息
    emit Write2Output(TFTP_CORRECT, QString("Socket Binding Successful!"));
    /****** 初始化操作，绑定套接字 ******/

    /****** 构造请求包并发送请求报文 ******/
    // 定义传输的Mode
    Mode = ui->down_comboBox->currentIndex();
    // 向Output显示传输方式
    emit Write2Output(TFTP_CORRECT, QString("Mode : %1").arg(Mode == MODE_NETASCII ? "netascii" : "octet"));
    // 组装请求报文
    int reqMsgLen = 0;
    Req_Pkt.opcode = htons(OP);
    appendMsg(Req_Pkt.reqMsg, REQ_SIZE, &reqMsgLen, "%s", RemoteFile);
    appendMsg(Req_Pkt.reqMsg, REQ_SIZE, &reqMsgLen, Mode == MODE_NETASCII ? "netascii" : "octet");

    emit Write2Output(TFTP_CORRECT,
                      QString("<font color='blue'>********************** Start Read **********************</font>"),
                      true);
    // 发送请求报文
    int res = sendPkt((char *) &Req_Pkt, OP_SIZE + reqMsgLen);
    if (res < 0) {
        emit Write2Output(ERROR_SEND_REQ_FAIL,
                          QString("REQUEST : Send request packet failed with code : %1.").arg((WSAGetLastError())));
        return;
    } else if (res < OP_SIZE + reqMsgLen) {
        emit Write2Output(TFTP_CORRECT, QString("Send request packet partially with length : %1.").arg(res));
    } else {
        emit Write2Output(TFTP_CORRECT, QString("Send request packet successfully with length : %1.").arg(res));
    }
    /****** 构造请求包并发送请求报文 ******/

    /****** 等待数据包的到达 ******/
    int Rcvd_Size = 0, Data_Size = 0, Retransmit_Count = 0, Wait_DataRet;
    Cur_Block_Num = 1, TotalRetransmitCount = 0;

    // 等待第一个数据包的到达，如果没有服务器没有反应就重发
    while (Retransmit_Count <= TFTP_MAX_RETRANSMIT) {
        int wait_first_data = Wait_ACK_DAT(Timeout * 1000, Rcvd_Size);
        if (wait_first_data == CMD_DAT && ntohs(Rcvd_Pkt.block) == 1) {
            emit Write2Output(TFTP_CORRECT, QString("Get DATA Packet, Sent ACK 1."));
            break;
        } else if (wait_first_data == CMD_ERR) {
            if (Rcvd_Size < OP_SIZE + ERRCODE_SIZE) {
                emit Write2Output(ERROR_WRONG_PKT, "Wrong error packet.");
            } else {
                emit Write2Output(ntohs(Rcvd_Pkt.errCode), QString(Rcvd_Pkt.errMsg));
            }
            Terminate(false);
            return;
        } else {
            Retransmit_Count++;
            TotalRetransmitCount++;
            emit Write2Output(ERROR_RETRANSMIT_EQU, "Retransmit request packet.");
            if (sendPkt((char *) &Req_Pkt, OP_SIZE + reqMsgLen) < 0) {
                emit Write2Output(ERROR_SEND_REQ_FAIL, "Send request packet fail.");
                Terminate(false);
                return;
            }
        }
    }

    // 如果重传次数过多，就终止传输
    if (Retransmit_Count > TFTP_MAX_RETRANSMIT) {
        emit Write2Output(ERROR_RETRANSMIT_TOO_MUCH, "Retransmission times too much.");
        Terminate(false);
        return;
    }

    // 开始继续接受并写入文件
    Retransmit_Count = 0;
    while (Retransmit_Count <= TFTP_MAX_RETRANSMIT) {
        // 如果是第一个区块，则是ACK0
        if (Cur_Block_Num == 1) {
            Wait_DataRet = CMD_DAT;
        } else {
            Wait_DataRet = Wait_Specific_PKT(Cur_Block_Num, Timeout * 1000, Rcvd_Size, CMD_DAT);
        }

        // 接收到DATA报文
        if (Wait_DataRet == CMD_DAT) {
            if (Cur_Block_Num > 1)
                emit Write2Output(TFTP_CORRECT, QString("Get DATA Packet, Sent ACK %1").arg(Cur_Block_Num));
            // 写入文件
            Data_Size = Rcvd_Size - OP_SIZE - BLOCK_SIZE;
            if (fwrite(Rcvd_Pkt.data, 1, Data_Size, fp) < Data_Size) {
                emit Write2Output(ERROR_FWRITE_FAIL, "Write file fail.");
                Terminate(false);
                return;
            }
            // 发送ack报文
            Ack_Pkt.opcode = htons(CMD_ACK);
            Ack_Pkt.block = htons(Cur_Block_Num);
            if (sendPkt((char *) &Ack_Pkt, OP_SIZE + BLOCK_SIZE) < 0) {
                emit Write2Output(ERROR_SEND_ACK_FAIL,
                                  QString("ACK %1 : Send ACK packet fail.").arg(Cur_Block_Num));
                Terminate(false);
                return;
            }
            // 更新进度条和一些需要统计的数据
            Cur_Block_Num++;
            Retransmit_Count = 0;
            FileSize += Data_Size;
            if ((double) FileSize / Total_Size > percent) {
                percent = (double) FileSize / Total_Size + 0.01;
                emit Set_ProcessBar_Val((int) (percent * 100));
            }
            // 如果是最后一个数据包，就终止传输
            if (Data_Size < BlockSize) {
                Terminate(true);
                break;
            }
        }
            // 接收到ERROR报文
        else if (Wait_DataRet == CMD_ERR) {
            if (Rcvd_Size < OP_SIZE + ERRCODE_SIZE) {
                emit Write2Output(ERROR_WRONG_PKT, "Wrong error packet.");
            } else {
                emit Write2Output(ntohs(Rcvd_Pkt.errCode), QString(Rcvd_Pkt.errMsg));
            }
            Terminate(false);
        } else {
            Retransmit_Count++;
            TotalRetransmitCount++;
            emit Write2Output(ERROR_RETRANSMIT_ACK, QString("Retransmit ACK packet <%1>.").arg(Cur_Block_Num - 1));
            Ack_Pkt.opcode = htons(CMD_ACK);
            Ack_Pkt.block = htons(Cur_Block_Num - 1);
            if (sendPkt((char *) &Ack_Pkt, OP_SIZE + BLOCK_SIZE) < 0) {
                emit Write2Output(ERROR_SEND_ACK_FAIL,
                                  QString("Send ACK packet %1 fail.").arg(Cur_Block_Num - 1));
                Terminate(false);
                return;
            }
        }
    }
    if (Retransmit_Count > TFTP_MAX_RETRANSMIT) {
        emit Write2Output(ERROR_RETRANSMIT_TOO_MUCH, "Retransmission times too much.");
        Terminate(false);
        return;
    }
    /****** 等待数据包的到达 ******/
    // 如果是ASCII码模式需要解码
    if (Mode == MODE_NETASCII) { decodeNetascii(LocalFile, PLATFORM_WINDOWS); }
}

/* 打开文件
 * @param
 * @return : 0表示成功，-1表示失败
 * */
int MainWindow::open_file() {
    if (fopen_s(&fp, LocalFile, OP == CMD_RRQ ? "wb" : "rb") == 0) {
        emit Write2Output(TFTP_CORRECT, QString("Open file successfully : %1.").arg(QString::fromLocal8Bit(LocalFile)));
        Total_Size = getFileSize(LocalFile);
        return TFTP_CORRECT;
    } else {
        emit Write2Output(ERROR_OPENFILE_FAIL,
                          QString("Open file : Fail : %1.").arg(QString::fromLocal8Bit(LocalFile)));
        return ERROR_OPENFILE_FAIL;
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
    if (ret > 0) { Bytes_Send += ret; }
    return ret;
}

/* 接收报文
 * @param
 * buf : 待接收报文buf区
 * len : 报文长度
 * @return : 接收的字节数
 * */
int MainWindow::recvPkt(char *buf, int len) {
    int ret = recvfrom(sock, buf, len, 0, (SOCKADDR *) &server_ip, &server_ip_len);
    if (ret > 0) { Bytes_Recv += ret; }
    return ret;
}

/**
 * 尝试试在TIMEOUT时间内接收一个报文
 * @TimeOut_Ms : time out interval (ms) 超时时间(毫秒)
 * @Rcvd_Size  : used to return the size of packet received 用来返回接收报文的大小
 * #return    : status code 状态码
 */
int MainWindow::Wait_PKT(int TimeOut_Ms, int &Rcvd_Size) {
    // 设置超时时间
    FD_SET readFds;
    TIMEVAL tv = {TimeOut_Ms / 1000, TimeOut_Ms % 1000 * 1000};
    FD_ZERO(&readFds);
    FD_SET(sock, &readFds);
    // 调用select函数
    int selectRet = select(sock + 1, &readFds, nullptr, nullptr, &tv);

    // 根据返回值进行判断
    if (selectRet == SOCKET_ERROR) {
        return ERROR_SOCKET_ERROR;
    } else if (selectRet == 0) {
        return TFTP_ERROR_TIMEOUT;
    } else {
        // 接收报文
        Rcvd_Size = recvPkt((char *) &Rcvd_Pkt, sizeof(PKG_DATA_ERROR));
        if (Rcvd_Size == SOCKET_ERROR) {
            emit Write2Output(ERROR_SOCKET_ERROR, "RECV : SOCKET_ERROR.");
            return ERROR_SOCKET_ERROR;
        } else if (Rcvd_Size == 0) {
            emit Write2Output(ERROR_CONNECT_CLOSE, "RECV : The connection has been closed.");
            return ERROR_CONNECT_CLOSE;
        }
    }
    if (ntohs(Rcvd_Pkt.opcode) == CMD_DAT || ntohs(Rcvd_Pkt.opcode) == CMD_ACK || ntohs(Rcvd_Pkt.opcode) == CMD_ERR) {
        if (Rcvd_Size < OP_SIZE + MODE_SIZE) { return TFTP_ERROR_WRONG_PKT; }
    }
    return TFTP_CORRECT;
}


/* 等待ACK报文
 * @param
 * TimeOut_Ms : 超时时间
 * Rcvd_Size : 接收的字节数
 * @return : 接收到的报文类型
 * */
int MainWindow::Wait_ACK_DAT(int TimeOut_Ms, int &Rcvd_Size) {
    int Rest_Time = TimeOut_Ms, Wait_Pkt, Start_Time;

    do {
        Start_Time = clock();
        Wait_Pkt = Wait_PKT(Rest_Time, Rcvd_Size);
        Rest_Time -= (clock() - Start_Time);
    } while (Wait_Pkt == TFTP_CORRECT &&
             (OP == CMD_RRQ ? (ntohs(Rcvd_Pkt.opcode) == CMD_DAT && ntohs(Rcvd_Pkt.block) != 1) :
              (ntohs(Rcvd_Pkt.opcode) == CMD_ACK && ntohs(Rcvd_Pkt.block) != 0)));

    if (Rest_Time < 0) {
        emit Write2Output(TFTP_ERROR_TIMEOUT, QString("ACK : Timeout."));
    }

    return Wait_Pkt != TFTP_CORRECT ? Wait_Pkt : ntohs(Rcvd_Pkt.opcode);
}

/* 等待特定类型特定block号的报文
 * @param
 * block : 块号
 * TimeOut_Ms : 超时时间
 * Rcvd_Size : 接收的字节数
 * cmd : 报文类型
 * */
int MainWindow::Wait_Specific_PKT(uint16_t block, int TimeOut_Ms, int &Rcvd_Size, uint16_t Pkt_Type) {
    int Rest_Time = TimeOut_Ms, Wait_PktRet, Start_Time;
    do {
        Start_Time = clock();
        Wait_PktRet = Wait_PKT(Rest_Time, Rcvd_Size);
        Rest_Time -= (clock() - Start_Time);
    } while (Wait_PktRet == TFTP_CORRECT && (ntohs(Rcvd_Pkt.opcode) != Pkt_Type || ntohs(Rcvd_Pkt.block) != block));

    if (Rest_Time < 0) {
        emit Write2Output(TFTP_ERROR_TIMEOUT,
                          (Pkt_Type == CMD_ACK ? "ACK " : "DATA") + QString(" %1 : Timeout.").arg(block));
    }
    return Wait_PktRet != TFTP_CORRECT ? Wait_PktRet : ntohs(Rcvd_Pkt.opcode);
}

/* 传输终止显示相关信息
 * @param
 * is_success : 传输是否成功
 * */
void MainWindow::Terminate(bool is_success) {
    timer->stop();
    emit Set_ProcessBar_Val(is_success ? 100 : 0);
    if (is_success) {
        EndTime = clock();
        double timeSec = (double) (EndTime - StartTime) / CLOCKS_PER_SEC;
        if (OP == CMD_RRQ) {
            emit Write2Output(TFTP_CORRECT,
                              QString("Block      : %1 blocks ").arg(Cur_Block_Num - (OP == CMD_RRQ ? 1 : 0)), true);
            emit Write2Output(TFTP_CORRECT, QString("Time       : %1 s ").arg(timeSec), true);
            emit Write2Output(TFTP_CORRECT, QString("File Size  : %1 kB ").arg((double) FileSize / 1024), true);
            emit Write2Output(TFTP_CORRECT, QString("Down       : %1 kB ").arg((double) Bytes_Recv / 1024), true);
            emit Write2Output(TFTP_CORRECT,
                              QString("Down Speed : %1 kB/s ").arg(((double) Bytes_Recv / 1024 / timeSec)), true);
            emit Write2Output(TFTP_CORRECT, QString("Read Success  : Remote: %1 ==> Local: %2").arg(
                    QString::fromLocal8Bit(RemoteFile), QString::fromLocal8Bit(LocalFile)), true);
        } else {
            emit Write2Output(TFTP_CORRECT,
                              QString("Block      : %1 blocks ").arg(Cur_Block_Num - (OP == CMD_RRQ ? 1 : 0)), true);
            emit Write2Output(TFTP_CORRECT, QString("Time       : %1 s ").arg(timeSec), true);
            emit Write2Output(TFTP_CORRECT, QString("File Size  : %1 kB ").arg((double) FileSize / 1024), true);
            emit Write2Output(TFTP_CORRECT, QString("Up         : %1 kB ").arg((double) Bytes_Send / 1024), true);
            emit Write2Output(TFTP_CORRECT,
                              QString("Up Speed   : %1 kB/s ").arg(((double) Bytes_Send / 1024 / timeSec)), true);
            emit Write2Output(TFTP_CORRECT, QString("Retransmit : %1 times ").arg(TotalRetransmitCount), true);
            emit Write2Output(TFTP_CORRECT,
                              QString("Write Success : Local: %1 ==> Remote: %2").arg(QString::fromLocal8Bit(LocalFile),
                                                                                      QString::fromLocal8Bit(filename)),
                              true);
        }
        emit Write2Output(TFTP_CORRECT,
                          QString("<font color='blue'>********************** END SUCCESS **********************</font>"),
                          true);
        ui->Output->append("");
    } else {
        emit Write2Output(TFTP_ERROR_WRONG_PKT,
                          QString("<font color='blue'>********************** END FAIL **********************</font>"),
                          true);
        ui->Output->append("");
    }
    if (fp) { fclose(fp); }
}

void MainWindow::update_speed() {
    emit
    Set_Upload_Speed(QString::number(((double) Bytes_Send - Last_Bytes_Send) / TFTP_REFRESH_INTERVAL * 1000 / 1024));
    emit
    Set_Download_Speed(QString::number(((double) Bytes_Recv - Last_Bytes_Recv) / TFTP_REFRESH_INTERVAL * 1000 / 1024));
    Last_Bytes_Recv = Bytes_Recv;
    Last_Bytes_Send = Bytes_Send;
}








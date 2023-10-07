#ifndef SOCKET_DEF_H
#define SOCKET_DEF_H

#include <cstdio>
#include <winsock2.h>
#include <windows.h>
#include <WS2tcpip.h>
#include <sys/stat.h>
#include <io.h>
#include <QTimer>
#include <QDebug>
#include "QString"
#include "QDir"
#include "QFileDialog"
#include "QMessageBox"
#include "QRegularExpressionMatch"
#include "QDateTime"

#define SERV_PORT 69

// 定义TFTP操作码
#define CMD_RRQ (uint16_t)1
#define CMD_WRQ (uint16_t)2
#define CMD_DAT (uint16_t)3
#define CMD_ACK (uint16_t)4
#define CMD_ERR (uint16_t)5

// 定义模式码
#define MODE_NETASCII 0
#define MODE_OCTET 1

// 定义一些默认大小
#define OP_SIZE 2
#define MODE_SIZE 2
#define BLOCK_SIZE 2
#define ERRCODE_SIZE 2
#define REQ_SIZE 512
#define ERROR_SIZE 512
#define TFTP_MAX_BLOCK_SIZE 512
#define TFTP_MAX_RETRANSMIT 10
#define TFTP_REFRESH_INTERVAL 1000  // 吞吐量刷新间隔(ms)

// 定义正确和错误码
#define TFTP_CORRECT 0
#define ERROR_OPENFILE_FAIL -13
#define ERROR_SEND_REQ_FAIL -14
#define ERROR_SOCKET_ERROR -15
#define ERROR_CONNECT_CLOSE -16
#define ERROR_WRONG_PKT -17
#define ERROR_SEND_ACK_FAIL -18
#define ERROR_FWRITE_FAIL -19
#define ERROR_RETRANSMIT_EQU -21
#define ERROR_RETRANSMIT_ACK -22
#define ERROR_RETRANSMIT_TOO_MUCH -23
#define TFTP_ERROR_TIMEOUT -24
#define ERROR_SEND_DATA_FAIL -25
#define TFTP_ERROR_WRONG_PKT -61


// 请求包（包括写请求和读请求）数据结构的定义
typedef struct {
    WORD opcode;           // 操作码
    char reqMsg[REQ_SIZE]; // 请求信息
} PKG_REQUEST;

// DATA包和ERROR包数据结构的定义
typedef struct {
    WORD opcode;
    union {
        WORD block;    // 块号
        WORD errCode;  // 错误码
    };
    union {
        char data[TFTP_MAX_BLOCK_SIZE]; // 数据
        char errMsg[ERROR_SIZE];        // 错误信息
    };
} PKG_DATA_ERROR;

// ACK包数据结构的定义
typedef struct {
    WORD opcode; // 操作码
    WORD block;  // 确认的块号
} PKG_ACK;


#endif //SOCKET_DEF_H

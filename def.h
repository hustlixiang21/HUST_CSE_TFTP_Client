#ifndef SOCKET_DEF_H
#define SOCKET_DEF_H

#include <cstdio>
#include <winsock2.h>
#include <windows.h>
#include <WS2tcpip.h>
#include <sys/stat.h>
#include <io.h>
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

#define MODE_NETASCII 0
#define MODE_OCTET 1

#define OP_SIZE 2
#define MODE_SIZE 2
#define BLOCK_SIZE 2
#define ERRCODE_SIZE 2
#define REQ_SIZE 512
#define ERROR_SIZE 512
#define TFTP_MAX_BLOCK_SIZE 512
#define TFTP_MIN_BLOCK_SIZE 8
#define TFTP_MAX_RETRANSMIT 10

#define TFTP_CORRECT 0
#define ERROR_INVALID_ARG -1
#define ERROR_TOO_LONG_REQUEST -11
#define ERROR_UNDEFINED_MODE -12
#define ERROR_OPENFILE_FAIL -13
#define ERROR_SEND_REQ_FAIL -14
#define ERROR_SOCKET_ERROR -15
#define ERROR_CONNECT_CLOSE -16
#define ERROR_WRONG_PKT -17
#define ERROR_SEND_ACK_FAIL -18
#define ERROR_FWRITE_FAIL -19
#define ERROR_UNEXPECTED_PKT -20
#define ERROR_RETRANSMIT_EQU -21
#define ERROR_RETRANSMIT_ACK -22
#define ERROR_RETRANSMIT_TOO_MUCH -23
#define TFTP_ERROR_TIMEOUT -24
#define ERROR_SEND_DATA_FAIL -25
#define ERROR_SELECT_SOCKET_ERROR -41
#define ERROR_SELECT_TIMEOUT -42
#define ERROR_SELECT_CONNECT_CLOSE -43
#define TFTP_ERROR_WRONG_PKT -61


// 请求包（包括写请求和读请求）数据结构的定义
typedef struct {
    WORD opcode;
    char reqMsg[REQ_SIZE];
} PKG_REQUEST;

// DATA包和ERROR包数据结构的定义
typedef struct {
    WORD opcode;
    union {
        WORD block;    // 块号
        WORD errCode;  // 错误码
        char opMsg[2]; //
    };
    union {
        char data[TFTP_MAX_BLOCK_SIZE];
        char errMsg[ERROR_SIZE];
    };
} PKG_DATA_ERROR;

// ACK包数据结构的定义
typedef struct {
    WORD opcode;
    WORD block;
} PKG_ACK;


#endif //SOCKET_DEF_H

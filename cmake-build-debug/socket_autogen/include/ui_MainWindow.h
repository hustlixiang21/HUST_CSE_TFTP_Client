/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.4.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QIcon>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QTextBrowser>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QTabWidget *tabWidget;
    QWidget *Upload;
    QPushButton *uploadconfirm;
    QLabel *up_local_ip;
    QLabel *up_server_ip;
    QLabel *up_local_filename;
    QLineEdit *up_Server_IP;
    QComboBox *up_comboBox;
    QToolButton *up_selectpath;
    QLineEdit *up_LocalFileName;
    QLabel *up_mode;
    QLineEdit *up_Local_IP;
    QWidget *Download;
    QLabel *down_local_filename;
    QLineEdit *down_LocalFilePath;
    QLineEdit *down_Local_IP;
    QComboBox *down_comboBox;
    QPushButton *downloadconfirm;
    QLineEdit *down_Server_IP;
    QLabel *down_local_ip_;
    QToolButton *down_selectpath;
    QLabel *down_mode;
    QLabel *down_server_ip;
    QLabel *down_server_filename;
    QLineEdit *down_Server_FileName;
    QGroupBox *info;
    QVBoxLayout *verticalLayout_4;
    QHBoxLayout *horizontalLayout_9;
    QLabel *label;
    QLabel *UploadLabel;
    QLabel *label_4;
    QLabel *label_2;
    QLabel *DownloadLabel;
    QLabel *label_5;
    QProgressBar *ProgressBar;
    QTextBrowser *Output;
    QHBoxLayout *horizontalLayout_10;
    QSpacerItem *horizontalSpacer;
    QPushButton *Clear;
    QPushButton *Save;
    QSpacerItem *horizontalSpacer_2;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(560, 460);
        MainWindow->setMinimumSize(QSize(560, 460));
        MainWindow->setMaximumSize(QSize(560, 460));
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName("centralwidget");
        tabWidget = new QTabWidget(centralwidget);
        tabWidget->setObjectName("tabWidget");
        tabWidget->setGeometry(QRect(10, 0, 541, 141));
        Upload = new QWidget();
        Upload->setObjectName("Upload");
        uploadconfirm = new QPushButton(Upload);
        uploadconfirm->setObjectName("uploadconfirm");
        uploadconfirm->setGeometry(QRect(370, 80, 101, 21));
        up_local_ip = new QLabel(Upload);
        up_local_ip->setObjectName("up_local_ip");
        up_local_ip->setGeometry(QRect(10, 40, 54, 21));
        up_server_ip = new QLabel(Upload);
        up_server_ip->setObjectName("up_server_ip");
        up_server_ip->setGeometry(QRect(10, 0, 54, 21));
        up_local_filename = new QLabel(Upload);
        up_local_filename->setObjectName("up_local_filename");
        up_local_filename->setGeometry(QRect(230, 20, 101, 21));
        up_Server_IP = new QLineEdit(Upload);
        up_Server_IP->setObjectName("up_Server_IP");
        up_Server_IP->setGeometry(QRect(70, 0, 151, 20));
        up_comboBox = new QComboBox(Upload);
        up_comboBox->addItem(QString());
        up_comboBox->addItem(QString());
        up_comboBox->setObjectName("up_comboBox");
        up_comboBox->setGeometry(QRect(200, 80, 71, 21));
        up_comboBox->setContextMenuPolicy(Qt::PreventContextMenu);
        up_comboBox->setAutoFillBackground(false);
        up_selectpath = new QToolButton(Upload);
        up_selectpath->setObjectName("up_selectpath");
        up_selectpath->setGeometry(QRect(510, 20, 21, 21));
        up_selectpath->setPopupMode(QToolButton::InstantPopup);
        up_LocalFileName = new QLineEdit(Upload);
        up_LocalFileName->setObjectName("up_LocalFileName");
        up_LocalFileName->setGeometry(QRect(330, 20, 171, 20));
        up_LocalFileName->setClearButtonEnabled(true);
        up_mode = new QLabel(Upload);
        up_mode->setObjectName("up_mode");
        up_mode->setGeometry(QRect(60, 80, 121, 21));
        up_Local_IP = new QLineEdit(Upload);
        up_Local_IP->setObjectName("up_Local_IP");
        up_Local_IP->setGeometry(QRect(70, 40, 151, 20));
        tabWidget->addTab(Upload, QString());
        Download = new QWidget();
        Download->setObjectName("Download");
        down_local_filename = new QLabel(Download);
        down_local_filename->setObjectName("down_local_filename");
        down_local_filename->setGeometry(QRect(240, 40, 81, 21));
        down_LocalFilePath = new QLineEdit(Download);
        down_LocalFilePath->setObjectName("down_LocalFilePath");
        down_LocalFilePath->setGeometry(QRect(330, 40, 171, 20));
        down_Local_IP = new QLineEdit(Download);
        down_Local_IP->setObjectName("down_Local_IP");
        down_Local_IP->setGeometry(QRect(70, 40, 151, 20));
        down_comboBox = new QComboBox(Download);
        down_comboBox->addItem(QString());
        down_comboBox->addItem(QString());
        down_comboBox->setObjectName("down_comboBox");
        down_comboBox->setGeometry(QRect(200, 80, 71, 21));
        down_comboBox->setContextMenuPolicy(Qt::PreventContextMenu);
        down_comboBox->setAutoFillBackground(false);
        downloadconfirm = new QPushButton(Download);
        downloadconfirm->setObjectName("downloadconfirm");
        downloadconfirm->setGeometry(QRect(370, 80, 101, 21));
        down_Server_IP = new QLineEdit(Download);
        down_Server_IP->setObjectName("down_Server_IP");
        down_Server_IP->setGeometry(QRect(70, 0, 151, 20));
        down_local_ip_ = new QLabel(Download);
        down_local_ip_->setObjectName("down_local_ip_");
        down_local_ip_->setGeometry(QRect(10, 40, 54, 21));
        down_selectpath = new QToolButton(Download);
        down_selectpath->setObjectName("down_selectpath");
        down_selectpath->setGeometry(QRect(510, 40, 21, 21));
        down_mode = new QLabel(Download);
        down_mode->setObjectName("down_mode");
        down_mode->setGeometry(QRect(60, 80, 121, 21));
        down_server_ip = new QLabel(Download);
        down_server_ip->setObjectName("down_server_ip");
        down_server_ip->setGeometry(QRect(10, 0, 54, 21));
        down_server_filename = new QLabel(Download);
        down_server_filename->setObjectName("down_server_filename");
        down_server_filename->setGeometry(QRect(230, 0, 101, 21));
        down_Server_FileName = new QLineEdit(Download);
        down_Server_FileName->setObjectName("down_Server_FileName");
        down_Server_FileName->setGeometry(QRect(330, 0, 171, 20));
        tabWidget->addTab(Download, QString());
        info = new QGroupBox(centralwidget);
        info->setObjectName("info");
        info->setGeometry(QRect(10, 140, 551, 309));
        verticalLayout_4 = new QVBoxLayout(info);
        verticalLayout_4->setObjectName("verticalLayout_4");
        horizontalLayout_9 = new QHBoxLayout();
        horizontalLayout_9->setObjectName("horizontalLayout_9");
        label = new QLabel(info);
        label->setObjectName("label");

        horizontalLayout_9->addWidget(label);

        UploadLabel = new QLabel(info);
        UploadLabel->setObjectName("UploadLabel");

        horizontalLayout_9->addWidget(UploadLabel);

        label_4 = new QLabel(info);
        label_4->setObjectName("label_4");

        horizontalLayout_9->addWidget(label_4);

        label_2 = new QLabel(info);
        label_2->setObjectName("label_2");

        horizontalLayout_9->addWidget(label_2);

        DownloadLabel = new QLabel(info);
        DownloadLabel->setObjectName("DownloadLabel");

        horizontalLayout_9->addWidget(DownloadLabel);

        label_5 = new QLabel(info);
        label_5->setObjectName("label_5");

        horizontalLayout_9->addWidget(label_5);

        ProgressBar = new QProgressBar(info);
        ProgressBar->setObjectName("ProgressBar");
        ProgressBar->setValue(24);

        horizontalLayout_9->addWidget(ProgressBar);

        horizontalLayout_9->setStretch(0, 1);
        horizontalLayout_9->setStretch(1, 2);
        horizontalLayout_9->setStretch(2, 1);
        horizontalLayout_9->setStretch(3, 1);
        horizontalLayout_9->setStretch(4, 2);
        horizontalLayout_9->setStretch(5, 1);
        horizontalLayout_9->setStretch(6, 15);

        verticalLayout_4->addLayout(horizontalLayout_9);

        Output = new QTextBrowser(info);
        Output->setObjectName("Output");
        Output->setFrameShape(QFrame::Box);
        Output->setFrameShadow(QFrame::Raised);
        Output->setLineWidth(1);

        verticalLayout_4->addWidget(Output);

        horizontalLayout_10 = new QHBoxLayout();
        horizontalLayout_10->setObjectName("horizontalLayout_10");
        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_10->addItem(horizontalSpacer);

        Clear = new QPushButton(info);
        Clear->setObjectName("Clear");
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/resources/images/clear.png"), QSize(), QIcon::Normal, QIcon::Off);
        Clear->setIcon(icon);

        horizontalLayout_10->addWidget(Clear);

        Save = new QPushButton(info);
        Save->setObjectName("Save");
        QIcon icon1;
        icon1.addFile(QString::fromUtf8(":/resources/images/save.png"), QSize(), QIcon::Normal, QIcon::Off);
        Save->setIcon(icon1);

        horizontalLayout_10->addWidget(Save);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_10->addItem(horizontalSpacer_2);


        verticalLayout_4->addLayout(horizontalLayout_10);

        MainWindow->setCentralWidget(centralwidget);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName("statusbar");
        MainWindow->setStatusBar(statusbar);

        retranslateUi(MainWindow);

        tabWidget->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "MainWindow", nullptr));
        uploadconfirm->setText(QCoreApplication::translate("MainWindow", "Upload", nullptr));
        up_local_ip->setText(QCoreApplication::translate("MainWindow", "Local   IP", nullptr));
        up_server_ip->setText(QCoreApplication::translate("MainWindow", "Server IP", nullptr));
        up_local_filename->setText(QCoreApplication::translate("MainWindow", "Local   FileName", nullptr));
        up_Server_IP->setText(QString());
        up_Server_IP->setPlaceholderText(QCoreApplication::translate("MainWindow", "Input Server IP", nullptr));
        up_comboBox->setItemText(0, QCoreApplication::translate("MainWindow", "netascii", nullptr));
        up_comboBox->setItemText(1, QCoreApplication::translate("MainWindow", "octet", nullptr));

        up_comboBox->setCurrentText(QCoreApplication::translate("MainWindow", "netascii", nullptr));
        up_selectpath->setText(QCoreApplication::translate("MainWindow", "...", nullptr));
        up_LocalFileName->setText(QString());
        up_LocalFileName->setPlaceholderText(QCoreApplication::translate("MainWindow", "Input  Local  FileName", nullptr));
        up_mode->setText(QCoreApplication::translate("MainWindow", "Transmission Mode", nullptr));
        up_Local_IP->setText(QString());
        up_Local_IP->setPlaceholderText(QCoreApplication::translate("MainWindow", "Input  Local  IP", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(Upload), QCoreApplication::translate("MainWindow", "Upload", nullptr));
        down_local_filename->setText(QCoreApplication::translate("MainWindow", "Local  FilePath", nullptr));
        down_LocalFilePath->setText(QString());
        down_LocalFilePath->setPlaceholderText(QCoreApplication::translate("MainWindow", "Input  Local   FilePath", nullptr));
        down_Local_IP->setText(QString());
        down_Local_IP->setPlaceholderText(QCoreApplication::translate("MainWindow", "Input  Local  IP", nullptr));
        down_comboBox->setItemText(0, QCoreApplication::translate("MainWindow", "netascii", nullptr));
        down_comboBox->setItemText(1, QCoreApplication::translate("MainWindow", "octet", nullptr));

        down_comboBox->setCurrentText(QCoreApplication::translate("MainWindow", "netascii", nullptr));
        downloadconfirm->setText(QCoreApplication::translate("MainWindow", "DownLoad", nullptr));
        down_Server_IP->setText(QString());
        down_Server_IP->setPlaceholderText(QCoreApplication::translate("MainWindow", "Input Server IP", nullptr));
        down_local_ip_->setText(QCoreApplication::translate("MainWindow", "Local   IP", nullptr));
        down_selectpath->setText(QCoreApplication::translate("MainWindow", "...", nullptr));
        down_mode->setText(QCoreApplication::translate("MainWindow", "Transmission Mode", nullptr));
        down_server_ip->setText(QCoreApplication::translate("MainWindow", "Server IP", nullptr));
        down_server_filename->setText(QCoreApplication::translate("MainWindow", "Server FileName", nullptr));
        down_Server_FileName->setText(QString());
        down_Server_FileName->setPlaceholderText(QCoreApplication::translate("MainWindow", "Input  Server  FileName", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(Download), QCoreApplication::translate("MainWindow", "DownLoad", nullptr));
        info->setTitle(QCoreApplication::translate("MainWindow", "Info", nullptr));
        label->setText(QCoreApplication::translate("MainWindow", "\342\206\221:", nullptr));
        UploadLabel->setText(QCoreApplication::translate("MainWindow", "0.00", nullptr));
        label_4->setText(QCoreApplication::translate("MainWindow", "kB/s", nullptr));
        label_2->setText(QCoreApplication::translate("MainWindow", "\342\206\223:", nullptr));
        DownloadLabel->setText(QCoreApplication::translate("MainWindow", "0.00", nullptr));
        label_5->setText(QCoreApplication::translate("MainWindow", "kB/s", nullptr));
        Clear->setText(QCoreApplication::translate("MainWindow", "Clear", nullptr));
        Save->setText(QCoreApplication::translate("MainWindow", "Save", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include "Global.h"
#include "xlsxchart.h"
#include "xlsxcellrange.h"
#include "xlsxdocument.h"
#include "xlsxconditionalformatting.h"
#include "canqthread.h"
//#define AUTO_RUN_KEY "HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run"
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

typedef struct TablData
{
    int     speed_l;
    int     speed_r;
    float   cur_l;
    float   cur_r;
    float   pressure;
    float   force_l;
    float   force_r;
    int     speed_p;
    float   commsp;
    float   commal;
}sTab_data_;


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    //void setMyAppAutoRun(bool isStart);         // 设置自启动
    void insertLogReList(QString op_info);
    void cannCan();

    bool mconnect = false;
private slots:
    void deal_my_string(QString str);             // 子线程结束槽函数
    //void timerEvent(QTimerEvent *event);      //

private:
    void            init();
    void            log();
    bool            DirExist(QString fullPath);
    bool            createxlsx(QString fullPath,QString strDate,QString strTime);
    void            xlsxFile(QString fullPath);
    short           flag,num = 0;
    int             curRow_Login;
    bool            isOK = false;
    int             i_num = 0;
    //int             SystemId;
    sTab_data_      tab_Dta = {0,0,0,0,0,0,0,0,0,0};
    Ui::MainWindow  *ui;
    CanQthread      *MyCANControlThread = nullptr;
    QFile           *file = nullptr;
    //QXlsx::Document *xlsx = nullptr;

};
#endif // MAINWINDOW_H

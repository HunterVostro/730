#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
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

    void insertLogReList(QString op_info);
    void cannCan();
    bool mconnect = false;
private slots:
    void deal_my_string(QString str);             // 子线程结束槽函数

private:

    void            exitRun(bool start);
    void            DirExist(QString fullPath);
    void            log(bool tip);
    void            saveCurrentFile();
    void            createNewFile();
    QString         generateFilePath() const;
    int             m_maxRowsPerFile;       // 每个文件的最大行数
    int             m_currentFileNumber;    // 当前文件编号
    QString         m_currentFilePath;      // 当前文件地址
    QString         m_baseFilePath;
    QXlsx::Document *m_currentDocument = nullptr;

    short           flag,num = 0;
    int             curRow_Login;
    sTab_data_      tab_Dta = {0,0,0,0,0,0,0,0,0,0};

    Ui::MainWindow  *ui;
    CanQthread      *MyCANControlThread = nullptr;
};
#endif // MAINWINDOW_H

#ifndef CANQTHREAD_H
#define CANQTHREAD_H
#include <QObject>
#include "Global.h"

class CanQthread : public QThread
{
    Q_OBJECT
public:
    CanQthread();

    void stop();

    void OpenCANThread(byte baud, byte mode, byte Filter);      // 打开can线程
    void CloseCANThread();              // 关闭can线程

    void ReceiveCANThread();             // 接收数据线程
    void TransmitCANThread(unsigned int id, unsigned char *ch,bool EXT,bool RTR, unsigned int len); // 发送数据线程

public:
    typedef DWORD(__stdcall OpenDevice)(DWORD,DWORD,DWORD);
    typedef DWORD(__stdcall ResetCAN)(DWORD DeviceType,DWORD DeviceInd,DWORD CANInd);
    typedef DWORD(__stdcall CloseDevice)(DWORD DeviceType,DWORD DeviceInd);
    typedef DWORD(__stdcall InitCAN)(DWORD DeviceType, DWORD DeviceInd, DWORD CANInd, P_INIT_CONFIG pInitConfig);
    typedef DWORD(__stdcall StartCAN)(DWORD DeviceType,DWORD DeviceInd,DWORD CANInd);
    typedef ULONG(__stdcall Transmit)(DWORD DeviceType,DWORD DeviceInd,DWORD CANInd,P_CAN_OBJ pSend,ULONG Len);

    typedef ULONG(__stdcall Receive)(DWORD DeviceType,DWORD DeviceInd,DWORD CANInd,P_CAN_OBJ pReceive,ULONG Len,INT WaitTime/*=-1*/);
    typedef ULONG(__stdcall GetReceiveNum)(DWORD DeviceType,DWORD DeviceInd,DWORD CANInd);
    typedef DWORD(__stdcall ClearBuffer)(DWORD DeviceType,DWORD DeviceInd,DWORD CANInd);
    typedef DWORD(__stdcall ReadErrInfo)(DWORD DeviceType,DWORD DeviceInd,DWORD CANInd,P_ERR_INFO pErrInfo);
    typedef DWORD(__stdcall ReadCANStatus)(DWORD DeviceType,DWORD DeviceInd,DWORD CANInd,P_CAN_STATUS pCANStatus);
    typedef DWORD(__stdcall SetReference)(DWORD DeviceType,DWORD DeviceInd,DWORD CANInd,DWORD RefType,PVOID pData);
    typedef DWORD(__stdcall ReadBoardInfo)(DWORD DeviceType,DWORD DeviceInd,P_BOARD_INFO pInfo);

    int devtype = USBCAN2;//设备类型号//
    int devind = 0; //设备索引号
    int res = 0;        //保留参数，通常为0
    int canind = 0; //第几路can
    int reftype;    //参数类型
    bool musbcanstart; //是否打开设备
//Widget *ui;

protected:
    //QThread类的虚函数//就是子线程处理函数
    void run();//子线程处理函数//不能直接调用该函数//只能通过start间接调用

private:
    volatile bool stopped;
    OpenDevice *pOpenDevice;        // 打开驱动
    ResetCAN *pResetCAN;            // 复位can
    CloseDevice *pCloseDevice;      // 关闭驱动
    InitCAN *pInitCAN;              // 初始化can
    StartCAN *pStartCAN;            // 启动can
    Transmit *pTransmitCAN;         // can发送
    Receive *pReceive;              // can接收
    GetReceiveNum *pGetReceiveNum;  // 获取接收编号
    ClearBuffer *pClearBuffer;      // 清楚缓存区
    ReadErrInfo *pReadErrInfoCAN;   // 读取错误信息
    ReadCANStatus *pReadCANStatus;  // 读取can状态
    SetReference *pSetReference;    //
    ReadBoardInfo *pReadBoardInfo;  //
    long  mrec = 0;

signals:
    void my_signal(QString str);//信号函数
    //void Data_signal(QString str);
};

#endif // CANQTHREAD_H

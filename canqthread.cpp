#include "canqthread.h"

CanQthread::CanQthread()
{
    QLibrary lib("ECanVci64.dll");
    if(true == lib.load())
       qDebug()<<"ECanVci64.dll load Ok";

    pOpenDevice = (OpenDevice *)lib.resolve("OpenDevice");
    pCloseDevice = (CloseDevice *)lib.resolve("CloseDevice");
    pInitCAN = (InitCAN *)lib.resolve("InitCAN");
    pStartCAN = (StartCAN *)lib.resolve("StartCAN");
    pTransmitCAN = (Transmit *)lib.resolve("Transmit");
    pReceive = (Receive *)lib.resolve("Receive");
    pGetReceiveNum = (GetReceiveNum *)lib.resolve("GetReceiveNum");
    pClearBuffer = (ClearBuffer *)lib.resolve("ClearBuffer");
    pReadErrInfoCAN = (ReadErrInfo*)lib.resolve("ReadErrInfo");
    pResetCAN = (ResetCAN *)lib.resolve("ResetCAN");
    pSetReference = (SetReference *)lib.resolve("SetReference");
    pReadBoardInfo = (ReadBoardInfo *)lib.resolve("ReadBoardInfo");
    pReadCANStatus = (ReadCANStatus *)lib.resolve("ReadCANStatus");
}

void CanQthread::run()
{
    if (musbcanstart == true)
    {
        while(!stopped) {//死循环//
            ReceiveCANThread();//接收数据
        }
        stopped = false;
    }
}

void CanQthread::stop()
{
    stopped = true;
}

void CanQthread::ReceiveCANThread()
{
    //bool ok;
    ERR_INFO vei;
    CAN_OBJ preceive[1000];
    //CAN_OBJ psend;
    //int baud=0x10000000;//参数有关数据缓冲区地址首指针
    //接收
    ULONG res = 10;
    res = pReceive(devtype,devind,canind,preceive,50,0);

    //qDebug()<<"读取帧数："<<res;
    for(unsigned int i = 0 ;i < res; i++) {
        if(res == 4294967295) {
            if(pReadErrInfoCAN(devtype,devind,canind,&vei)!=STATUS_ERR) {
                qDebug()<<"读取数据失败!"<<"错误码为："<<QString::number(vei.ErrCode,16);
            }
        }
        QString receive_str = "接收";
        mrec++;
        receive_str.append(QString::number(mrec,10));
        receive_str.append("帧ID: ");
        receive_str.append(QString::number(preceive[i].ID,16));
        receive_str.append(" 数据: ");
        for (int a = 0; a < preceive[i].DataLen; a++) {
            receive_str.append(QString::asprintf("%02X",preceive[i].Data[a]));
            //receive_str.append(QString::number(preceive[i].Data[a],16));
            receive_str.append(" ");
        }

        emit my_signal(receive_str);//子线程处理完毕//触发自定义的信号

    }
    QThread::msleep(1);
}

void CanQthread::TransmitCANThread(unsigned int id, unsigned char *ch, bool EXT, bool RTR, unsigned int len)
{
    CAN_OBJ psend;
    //发送
    ULONG Tr;
    psend.ID=id;//ID号//需要实际填写//
    psend.SendType=0;
    psend.RemoteFlag=RTR;
    psend.ExternFlag=EXT;
    psend.DataLen=len;
    for (unsigned int i=0; i < len; i++) {
        psend.Data[i]=ch[i];
    }

    Tr = pTransmitCAN(devtype,devind,canind,&psend,1);//返回实际发送的帧数
    //devtype设备类型号//devind设备索引号//canind第几路CAN//&psend要发送的数据帧数组的首指针//1要发送的数据帧数组的长度//
    if(Tr!=STATUS_ERR){
    }
}

void CanQthread::OpenCANThread(byte baud, byte mode, byte filter)
{
    mrec=0;
    //bool ok;
    ERR_INFO vei;
    //CAN_OBJ preceive[100];
    //CAN_OBJ psend;
    //int baud=0x10000000;//参数有关数据缓冲区地址首指针
    //int pdata=0x00;//参数有关数据缓冲区地址首指针
    //打开设备
    musbcanstart = false;

    if(pOpenDevice(devtype, devind, res) == STATUS_ERR ) {//为1表示操作成功，0表示操作失败。
        QThread::msleep(500);
        if(pReadErrInfoCAN(devtype, devind, canind, &vei) != STATUS_ERR) {
          qDebug()<<u8"打开失败："<<QString::number(vei.ErrCode,16);
        }
        IsOpenFlag = false;
        return;
    } else {
      IsOpenFlag = true;
      qDebug() << "open successed";
    }

    INIT_CONFIG init_config;//INIT_CONFIG
    init_config.Mode = mode;//0正常模式//1为只听模式//2为自发自收模式//
    init_config.Filter = filter;//滤波方式，单滤波
    switch (baud) {
        case 0: //1000
            init_config.Timing0 = 0;
            init_config.Timing1 = 0x14;
            break;
        case 1: //800
            init_config.Timing0 = 0;
            init_config.Timing1 = 0x16;
            break;
        case 2: //666
            init_config.Timing0 = 0x80;
            init_config.Timing1 = 0xb6;
            break;
        case 3: //500
            init_config.Timing0 = 0;
            init_config.Timing1 = 0x1c;
            break;
        case 4://400
            init_config.Timing0 = 0x80;
            init_config.Timing1 = 0xfa;
            break;
        case 5://250
            init_config.Timing0 = 0x01;
            init_config.Timing1 = 0x1c;
            break;
        case 6://200
            init_config.Timing0 = 0x81;
            init_config.Timing1 = 0xfa;
            break;
        case 7://125
            init_config.Timing0 = 0x03;
            init_config.Timing1 = 0x1c;
            break;
        case 8://100
            init_config.Timing0 = 0x04;
            init_config.Timing1 = 0x1c;
            break;
        case 9://80
            init_config.Timing0 = 0x83;
            init_config.Timing1 = 0xff;
            break;
        case 10://50
            init_config.Timing0 = 0x09;
            init_config.Timing1 = 0x1c;
            break;
    }

    init_config.AccCode=0x000000;//验收码
    init_config.AccMask=0xFFFFFF;//屏蔽码
    QThread::msleep(100);
    if(pInitCAN(devtype, devind, canind, &init_config) == STATUS_ERR) {
        qDebug("Init Error");
        pCloseDevice(devtype, devind);
        return;
    } else {
        qDebug() << "Init successed";
        qDebug() << u8"定时器0：" << init_config.Timing0;
        qDebug() << u8"定时器1：" << init_config.Timing1;
    }

    if(pStartCAN(devtype, devind, canind) == STATUS_ERR) {
        qDebug()<<"start fail";
        pCloseDevice(devtype, devind);
        return;
    } else {
        qDebug() << "start successed";
        musbcanstart = true;
    }
}

void CanQthread::CloseCANThread()
{
    pCloseDevice(devtype,devind);
    IsOpenFlag = false;
    qDebug() << "closed";
}


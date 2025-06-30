#include "mainwindow.h"
#include <QProcessEnvironment>
#include "ui_mainwindow.h"
#include <QTextCodec>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->show();
    this->activateWindow();

    // 先进行最简单的初始化
    log();

    // 延迟其他初始化
    QTimer::singleShot(2000, this, [this]() {
        MyCANControlThread = new CanQthread;
        connect(MyCANControlThread,SIGNAL(my_signal(QString)),this,SLOT(deal_my_string(QString)));
        cannCan();
        init();
        ui->textEdit->document()->setMaximumBlockCount(50); // 设置最大行数
        //SystemId = startTimer(500);
    });
}

MainWindow::~MainWindow() {
    if(mconnect == true) {
        MyCANControlThread->CloseCANThread();
        MyCANControlThread->stop();         // 停止子线程
    }
    file->close();
    delete xlsx;
    delete file;

    delete ui;
}

/*void MainWindow::timerEvent(QTimerEvent *event) {
    if (event->timerId() == SystemId) {
        QString strDate = currenttime.toString("yyyy-MM-dd");
        QString strTime = currenttime.toString("hh");
    }
}*/

void MainWindow::init() {
    ui->textEdit->setReadOnly(true);
    QDateTime currenttime = QDateTime::currentDateTime();
    QString strDate = currenttime.toString("yyyy-MM-dd");
    QString strTime = currenttime.toString(" hh "); // -mm-ss
    //i_num = QString::toInt(strTime);
    QString path = QCoreApplication::applicationDirPath() + "/Log";
    bool textIsOk = false;
    curRow_Login = 0;
    if (DirExist(path)) {
        textIsOk = createxlsx(path,strDate,strTime);//createText(path,strDate,strTime);
    }

    QTextStream stream(file);
    stream.setCodec("utf-8");
    //fileStream.setCodec(QTextCodec::codecForName("utf-8"));//必须加，否则含有中文时乱码
    //stream.setCodec(QTextCodec::codecForName("utf-8"));//必须加，否则含有中文时乱码
    stream<<QString("左轮速度")<<" , "<<QString("右轮速度")<<" , "<<QString("左轮转矩")<<" , "<<QString("右轮转矩")<<" , "
          <<QString("左销轴力")<<" , "<<QString("右销轴力")<<" , "<<QString("系统压力")<<" , "
          <<QString("油泵转速")<<" , "<<QString("指令线速")<<" , "<<QString("指令角速")<<"\r\n";
}

bool MainWindow::DirExist(QString fullPath) {
    QDir dir(fullPath);
    if (dir.exists()) {
            qDebug()<<QString::fromUtf8(u8"目录已存在");
            ui->textEdit->append("目录已存在");
            return true;
        } else {
            bool ok = dir.mkdir(fullPath);
            qDebug()<<QString::fromUtf8(u8"创建一级子目录");
            ui->textEdit->append("创建一级子目录");
            return ok;
    }
    return false;
}

/*********************************************************************************************************/

bool MainWindow::createxlsx(QString fullPath,QString strDate,QString strTime) {
    const QString strPath = fullPath + "/" + strDate + strTime + " log.xlsx";
    //file = new QFile("/log.txt");//("C:/FilesData/QtFiles/730/build-730-Desktop_Qt_5_12_12_MinGW_64_bit-Debug/debug/Log/ log.txt");
    //file = new QFile("log.txt");
    file = new QFile(fullPath + "/" + strDate + strTime + " log.txt");
    isOK = file->open(QIODevice::WriteOnly|QIODevice::Append); // QIODevice::WriteOnly| QIODevice::Text|
    xlsx = new QXlsx::Document(strPath,this);
    QFileInfo fi(strPath);
    if(fi.isFile()) { // 判断文件是否创建成功
        ui->textEdit->append("xlsx已存在");
        int last = 1;
        while (!xlsx->read(last,1).isNull()) {
            last++;
        }
        curRow_Login = last - 2;
     } else {
        xlsx->write("A1", "左轮速度");
        xlsx->write("B1", "右轮速度");
        xlsx->write("C1", "左轮转矩");
        xlsx->write("D1", "右轮转矩");
        xlsx->write("E1", "左销轴力");
        xlsx->write("F1", "右销轴力");
        xlsx->write("G1", "系统压力");
        xlsx->write("H1", "油泵转速");
        xlsx->write("I1", "指令线速");
        xlsx->write("J1", "指令角速");
        xlsx->save();
        ui->textEdit->append("xlsx创建成功");
     }

    return fi.isFile();
}

/************************************************************************************/
void MainWindow::cannCan() {
    if (MyCANControlThread == nullptr)
        return;

    if (mconnect == false) {
        MyCANControlThread->canind = 0; //  设置can通道
        MyCANControlThread->OpenCANThread((byte)3,0,0);
        MyCANControlThread->start();//启动子线程//间接调用了run()函数//即接收数据
        if(IsOpenFlag == false) {//启动设备失败
          MyCANControlThread->CloseCANThread();
          MyCANControlThread->stop();//停止子线程
          QMessageBox::information(this,u8"错误",u8"打开设备失败");
          ui->textEdit->append("启动can设备失败");
        } else {//启动设备成功
          mconnect = true;
          ui->textEdit->append("启动can设备成功");
        }
    } else {
        MyCANControlThread->CloseCANThread();
        MyCANControlThread->stop();//停止子线程
        mconnect = false;
    }
}

void MainWindow::deal_my_string(QString str)//槽函数//子线程(run()函数)结束后由子线程的信号函数(my_signal(QString))触发该函数
{
    if (num == 100) {
        insertLogReList(str);
        num = 0;
    } else {
        num += 1;
    }
}

void MainWindow::insertLogReList(QString op_info) {
    bool ok;
    QStringList list = op_info.split(" ");
    QString id_ = list[1];
    int spe_l = 0,cur_l = 0,spe_r = 0,cur_r = 0;
    switch (id_.toInt(&ok,16)) {
        case 0X100:
                cur_l = (list[4].append(list[3])).toInt(&ok,16);
                spe_l = (list[6].append(list[5])).toInt(&ok,16);
                cur_r = (list[8].append(list[7])).toInt(&ok,16);
                spe_r = (list[10].append(list[9])).toInt(&ok,16);
                tab_Dta.speed_l  = spe_l - 30000;
                tab_Dta.cur_l    = cur_l * 0.1 - 1000;
                tab_Dta.speed_r  = spe_r - 30000;
                tab_Dta.cur_r    = cur_r * 0.1 - 1000;
                flag = 1;
            break;
        case 0X101:
                cur_l = (list[4].append(list[3])).toInt(&ok,16);
                spe_l = (list[6].append(list[5])).toInt(&ok,16);
                cur_r = (list[8].append(list[7])).toInt(&ok,16);
                spe_r = (list[10].append(list[9])).toInt(&ok,16);
                tab_Dta.pressure        = cur_l * 0.01 - 100;
                tab_Dta.force_l         = spe_l * 0.01 - 100;
                tab_Dta.force_r         = cur_r * 0.01 - 100;
                tab_Dta.speed_p         = spe_r - 30000;
                flag = 2;
            break;
        case 0X102:
                cur_l = (list[4].append(list[3])).toInt(&ok,16);
                spe_l = (list[6].append(list[5])).toInt(&ok,16);
                tab_Dta.commsp          = cur_l * 0.01 - 100;
                tab_Dta.commal          = spe_l * 0.01 - 100;
                flag = 3;
            break;
    }

    if(flag == 3) {
        curRow_Login += 1;
        //  逐行写入
        xlsx->write(curRow_Login+1,1,tab_Dta.speed_l);
        xlsx->write(curRow_Login+1,2,tab_Dta.speed_r);
        xlsx->write(curRow_Login+1,3,tab_Dta.cur_l);
        xlsx->write(curRow_Login+1,4,tab_Dta.cur_r);
        xlsx->write(curRow_Login+1,5,tab_Dta.force_l);
        xlsx->write(curRow_Login+1,6,tab_Dta.force_r);
        xlsx->write(curRow_Login+1,7,tab_Dta.pressure);
        xlsx->write(curRow_Login+1,8,tab_Dta.speed_p);
        xlsx->write(curRow_Login+1,9,tab_Dta.commsp);
        xlsx->write(curRow_Login+1,10,tab_Dta.commal);

        QDateTime currenttime = QDateTime::currentDateTime();
        QString strTime =currenttime.toString("[hh:mm:ss]:");
        // xlsx->dimension().columnCount(); 计算总列数
        if ( xlsx->dimension().rowCount() >= 1800) { // 计算总行数

        }

        if (!xlsx->save()) {
            ui->textEdit->append(strTime + "动态保存失败");
        } else {
            ui->textEdit->append(strTime + "动态保存成功");
        }

        //xlsx.dimension().rowCount();


        if(isOK) {
            QTextStream stream(file);
            stream<<tab_Dta.speed_l<<","<<tab_Dta.speed_r<<","<<tab_Dta.cur_l<<","<<tab_Dta.cur_r<<","
                  <<tab_Dta.force_l<<","<<tab_Dta.force_r<<","<<tab_Dta.pressure<<","
                  <<tab_Dta.speed_p<<","<<tab_Dta.commsp<<","<<tab_Dta.commal<<"\r\n";
        }  else  { //文件打开失败
            qDebug()<<QString::fromLocal8Bit("open txt err");
        }

       /* if (curRow_Login == 3600) {
            curRow_Login = 0;
            QDateTime currenttime = QDateTime::currentDateTime();
            //QString strDate = currenttime.toString("yyyy-MM-dd");
            //QString strTime =currenttime.toString(" hh "); // -mm-ss
            QString path = QCoreApplication::applicationDirPath() + "/Log.qlsx";
            xlsx->setObjectName(path);//(path);
            xlsx->save();
        }*/

        flag = 0;
    }
}

void MainWindow::log() {
    QFile logFile("startup.log");
    if(logFile.open(QIODevice::WriteOnly | QIODevice::Append)) {
        QTextStream stream(&logFile);
        stream << QDateTime::currentDateTime().toString() << " - Program started\n";
        stream << "Application path: " << QCoreApplication::applicationFilePath() << "\n";
        stream << "Working directory: " << QDir::currentPath() << "\n";
        stream << "Arguments: " << QCoreApplication::arguments().join(" ") << "\n";
        stream << "Environment: " << QProcessEnvironment::systemEnvironment().toStringList().join(";") << "\n";
        logFile.close();
    }
}




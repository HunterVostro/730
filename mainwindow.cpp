#include "mainwindow.h"
#include <QProcessEnvironment>
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->show();
    this->activateWindow();


    // 延迟其他初始化
    QTimer::singleShot(2000, this, [this]() {
        m_currentFileNumber = 0;
        curRow_Login = 0;
        m_maxRowsPerFile = 600;
        ui->textEdit->setReadOnly(true);
        ui->textEdit->document()->setMaximumBlockCount(50); // 设置最大行数
        m_baseFilePath = QCoreApplication::applicationDirPath() + "/Log";   //  安装地址
        DirExist(m_baseFilePath);
        MyCANControlThread = new CanQthread;
        connect(MyCANControlThread,SIGNAL(my_signal(QString)),this,SLOT(deal_my_string(QString)));
        cannCan();
        exitRun(mconnect);
    });
}

MainWindow::~MainWindow() {
    if(mconnect == true) {
        MyCANControlThread->CloseCANThread();
        MyCANControlThread->stop();         // 停止子线程
    }
    delete m_currentDocument;
    delete ui;
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

    return;
}

void MainWindow::exitRun(bool start) {
   log(start);
   if (!start) {
       qApp->quit();
       qApp->exit(0);
   }
   return;
}

void MainWindow::DirExist(QString fullPath) {
    QDir dir(fullPath);
    if (dir.exists()) {
            ui->textEdit->append("目录已存在");
            return;
        } else {
            bool ok = dir.mkdir(fullPath);
            if (ok) {
                ui->textEdit->append("创建一级子目录");
            } else {
                ui->textEdit->append("目录创建失败");
            }
            return;
    }
    return;
}

void MainWindow::deal_my_string(QString str)//槽函数//子线程(run()函数)结束后由子线程的信号函数(my_signal(QString))触发该函数
{
    if (num == 100) {
        insertLogReList(str);
        num = 0;
    } else {
        num += 1;
    }

    return;
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
        if (curRow_Login > m_maxRowsPerFile || m_currentDocument == nullptr) {
            saveCurrentFile();
            m_currentFileNumber++;
            curRow_Login = 0;
            createNewFile();
            m_currentDocument->write("A1", "动作时间");
            m_currentDocument->write("B1", "左轮速度");
            m_currentDocument->write("C1", "右轮速度");
            m_currentDocument->write("D1", "左轮转矩");
            m_currentDocument->write("E1", "右轮转矩");
            m_currentDocument->write("F1", "左销轴力");
            m_currentDocument->write("G1", "右销轴力");
            m_currentDocument->write("H1", "系统压力");
            m_currentDocument->write("I1", "油泵转速");
            m_currentDocument->write("J1", "指令线速");
            m_currentDocument->write("K1", "指令角速");
            //m_currentDocument->save();
            ui->textEdit->append(QString("创建新文件: %1").arg(generateFilePath()));
        }
        curRow_Login += 1;
        //  逐行写入
        QDateTime currenttime = QDateTime::currentDateTime();                       //  系统时间
        QString strTime = currenttime.toString("hh:mm:ss");
        m_currentDocument->write(curRow_Login+1,1,strTime);
        m_currentDocument->write(curRow_Login+1,2,tab_Dta.speed_l);
        m_currentDocument->write(curRow_Login+1,3,tab_Dta.speed_r);
        m_currentDocument->write(curRow_Login+1,4,tab_Dta.cur_l);
        m_currentDocument->write(curRow_Login+1,5,tab_Dta.cur_r);
        m_currentDocument->write(curRow_Login+1,6,tab_Dta.force_l);
        m_currentDocument->write(curRow_Login+1,7,tab_Dta.force_r);
        m_currentDocument->write(curRow_Login+1,8,tab_Dta.pressure);
        m_currentDocument->write(curRow_Login+1,9,tab_Dta.speed_p);
        m_currentDocument->write(curRow_Login+1,10,tab_Dta.commsp);
        m_currentDocument->write(curRow_Login+1,11,tab_Dta.commal);

        if (!m_currentDocument->save()) {
            ui->textEdit->append(QString::number(curRow_Login) + "动态保存失败");
        } else {
            ui->textEdit->append(QString::number(curRow_Login) + "动态保存成功");
        }
        flag = 0;
    }

    return;
}

void MainWindow::saveCurrentFile() {
    if (m_currentDocument && curRow_Login > 1) {
        m_currentDocument->save();
        delete m_currentDocument;
        m_currentDocument = nullptr;
    }

    return;
}

void MainWindow::createNewFile() {
    m_currentFilePath = generateFilePath();
    m_currentDocument = new QXlsx::Document(m_currentFilePath);

    return;
}

QString MainWindow::generateFilePath() const
{
    QDateTime currenttime = QDateTime::currentDateTime();                       //  系统时间
    QString strDate = currenttime.toString("yyyy-MM-dd");
    QString strTime = currenttime.toString(" hh ");
    const QString strPath = m_baseFilePath + "/" + strDate + strTime + " log.xlsx";
    QFileInfo fileInfo(strPath);
    QString baseName = fileInfo.baseName();
    QString suffix = fileInfo.completeSuffix();
    QString path = fileInfo.path();

    if (!path.isEmpty()) {
        path += "/";
    }

    return QString("%1%2_%3.%4")
            .arg(path)
            .arg(baseName)
            .arg(m_currentFileNumber)
            .arg(suffix);
}

void MainWindow::log(bool tip) {
    QFile logFile(m_baseFilePath + "/startup.log");
    if (logFile.open(QIODevice::ReadWrite | QIODevice::Text | QIODevice::Append)) {  // 使用 Text 模式自动处理换行符
        QTextStream stream(&logFile);
        stream.setCodec("utf-8");// Qt6 使用 setEncoding，Qt5 用 setCodec("utf-8")

        QStringList lines;
        stream.seek(0);
        while (!stream.atEnd()) {
            lines.append(stream.readLine());
        }

        if (lines.size() >= 30) {
            lines = lines.mid(3);
            logFile.resize(0);
            stream.seek(0);
            for (const QString &line : lines) {
                stream << line << "\n";
            }
        }

        QDateTime timeData = QDateTime::currentDateTime();
        stream << timeData.toString("yyyy-MM-dd hh:mm:ss") << QString("   启动计划\n");
        stream << QString("应用目录: ") << QCoreApplication::applicationFilePath() << "\n";
        stream << QString("启动情况: ") << (tip ? QString("程序正常启动。") : QString("CAN设备启动失败，程序关闭。")) << "\n";
        logFile.close();  // 关闭文件
    }

    return;
}






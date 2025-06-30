#include "mainwindow.h"

#include <QApplication>
#include <QDebug>
#include <QProcessEnvironment>
void setMyAppAutoRun(bool isStart)
{

    /*QSettings settings("HKEY_CURRENT_USER\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run", QSettings::NativeFormat);
    //设置注册表路径
    QString appName = QApplication::applicationName();//应用程序名称
    QString appPath = QApplication::applicationFilePath();//应用程序绝对路径
    appPath = QDir::toNativeSeparators(appPath);
    if (isStart) {
        settings.setValue(appName, appPath);
    } else {
        settings.remove(appName);
    }*/

    QString taskName = QApplication::applicationName();
    QString appPath = QDir::toNativeSeparators(QApplication::applicationFilePath());

    QProcess process;
    if (isStart) {
        // 创建计划任务（管理员权限 + 用户登录时触发）
        QString command = QString(
            "schtasks /create /tn \"%1\" /tr \"%2\" /sc onlogon /rl highest /f"
        ).arg(taskName).arg(appPath);
        process.start(command);
    } else {
        // 删除计划任务
        process.start(QString("schtasks /delete /tn \"%1\" /f").arg(taskName));
    }
    process.waitForFinished();
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    setMyAppAutoRun(true);
    system("chcp 65001");
    MainWindow w;
    w.setWindowTitle(str_Title);
    w.setMinimumSize(QSize(462, 398));
    w.setMaximumSize(QSize(462, 398));

    w.show();
    return a.exec();
}

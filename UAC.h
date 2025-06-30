#ifndef UAC_H
#define UAC_H
#include <ShlObj_core.h>
#include <QCoreApplication>
#pragma comment (lib, "Shell32.lib")

class UAC {
    public:
        static bool runAsAdmin() {
            if (IsUserAnAdmin()) {
                return false;
            }
            QStringList args = QCoreApplication::arguments();
            if (args.count() < 2 || args[1] != "runas") {
                QString filePath = QCoreApplication::applicationFilePath();
                HINSTANCE ins = ShellExecuteA(nullptr, "runas", filePath.toStdString().c_str(), "runas", nullptr, SW_SHOWNORMAL);
                    if (ins > (HINSTANCE)32) {
                        return true;
                    }
                }
            return false;
        }
};
#endif // UAC_H

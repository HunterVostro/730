#ifndef XLSXMULTIFILEWRITER_H
#define XLSXMULTIFILEWRITER_H

#include "xlsxchart.h"
#include "xlsxcellrange.h"
#include "xlsxdocument.h"
#include "xlsxconditionalformatting.h"
#include "Global.h"
class XlsxMultiFileWriter : QObject
{
    Q_OBJECT
public:
    //XlsxMultiFileWriter();
    explicit XlsxMultiFileWriter(const QString& baseFilePath,
                                   int maxRowsPerFile = 100,
                                   QObject *parent = nullptr);

    bool writeRow(const QString rowData);
    void close();
    void setMaxRowsPerFile(int maxRows);
    int maxRowsPerFile() const;
    QString currentFilePath() const;
    //bool writeRow(const QVector<QVariant>& rowData);

signals:
    void fileCreated(const QString& filePath);
    void errorOccurred(const QString& error);
private:
    QScopedPointer<QXlsx::Document> m_currentDocument;
    QString m_baseFilePath;
    int m_maxRowsPerFile;
    int m_currentFileNumber;
    int m_currentRowInFile;
    QString m_currentFilePath;

    void createNewFile();
    void saveCurrentFile();
    QString generateFilePath() const;
};

#endif // XLSXMULTIFILEWRITER_H

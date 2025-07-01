#include "xlsxmultifilewriter.h"

XlsxMultiFileWriter::XlsxMultiFileWriter(const QString& baseFilePath,
                                       int maxRowsPerFile,
                                       QObject *parent)
    : QObject(parent)
    , m_baseFilePath(baseFilePath)
    , m_maxRowsPerFile(maxRowsPerFile)
    , m_currentFileNumber(1)
    , m_currentRowInFile(1)
{
    createNewFile();
}

bool XlsxMultiFileWriter::writeRow(const struct rowData data) {
    if (!m_currentDocument) {
        emit errorOccurred("Document is not initialized");
        return false;
    }

    // 检查是否需要创建新文件
    if (m_currentRowInFile > m_maxRowsPerFile) {
        saveCurrentFile();
        m_currentFileNumber++;
        m_currentRowInFile = 1;
        createNewFile();
    }

    // 写入数据
    /*for (int col = 0; col < rowData.size(); ++col) {
        m_currentDocument->write(m_currentRowInFile, col + 1, rowData[col]);
    }
    for (int col = 0; col < rowData.size(); ++col) {

    }*/

    m_currentRowInFile++;
    return true;
}

void XlsxMultiFileWriter::close()
{
    saveCurrentFile();
    m_currentDocument.reset();
}

void XlsxMultiFileWriter::setMaxRowsPerFile(int maxRows)
{
    if (maxRows > 0) {
        m_maxRowsPerFile = maxRows;
    }
}

int XlsxMultiFileWriter::maxRowsPerFile() const
{
    return m_maxRowsPerFile;
}

QString XlsxMultiFileWriter::currentFilePath() const
{
    return m_currentFilePath;
}

void XlsxMultiFileWriter::createNewFile()
{
    m_currentDocument.reset(new QXlsx::Document());
    m_currentFilePath = generateFilePath();
    emit fileCreated(m_currentFilePath);
}

void XlsxMultiFileWriter::saveCurrentFile()
{
    if (m_currentDocument && m_currentRowInFile > 1) {
        if (!m_currentDocument->saveAs(m_currentFilePath)) {
            emit errorOccurred(QString("Failed to save file: %1").arg(m_currentFilePath));
        }
    }
}

QString XlsxMultiFileWriter::generateFilePath() const
{
    QFileInfo fileInfo(m_baseFilePath);
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

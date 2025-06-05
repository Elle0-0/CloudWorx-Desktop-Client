#ifndef FILESAPI_H
#define FILESAPI_H

#include <QString>
#include <QList>

struct DEKData {
    QString key_id;
    QString iv_dek;
    QString encrypted_dek;
    QString assoc_data_dek;
};

struct FileInfo {
    QString file_id;
    QString file_name;
    QString iv_file;
    QString assoc_data_file;
    QString created_at;
    DEKData dek_data;
};


class FilesApi
{
public:
    FilesApi();
    static bool getFiles(const QString& authToken, QList<FileInfo>& filesOut, int& countOut, QString& errorOut);
    static bool uploadEncryptedFile(const QString& authToken,
                                    const QString& filePath,
                             const QString& ivFile,
                             const QString& fileType,
                             qint64 fileSize,
                             const QString& ivDEK,
                             const QString& encryptedDEK,
                             QString& responseOut,
                             const QString& customFileName = "");
    static bool getAllFiles(QString& responseOut, const QString& fieldsMask);
    static bool resolveFileId(const QString& fileName, QString& fileIdOut);
    static bool deleteFileByName(const QString& fileName, const QString& authToken, QString& responseOut);
    static bool downloadEncryptedFileToMemory(const QString& authToken, const QString& fileName, QByteArray& fileDataOut, QString& errorOut);
};

#endif // FILESAPI_H

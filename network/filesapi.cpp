#include "filesapi.h"

#include "../utils/apihelper.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>
#include <QFile>
#include <QFileInfo>
#include <QDebug>
#include <curl/curl.h>

FilesApi::FilesApi() {}



bool FilesApi::getFiles(const QString& authToken, QList<FileInfo>& filesOut, int& countOut, QString& errorOut)
{
    CURL* curl = curl_easy_init();
    if (!curl)
        return false;

    QString url = "http://gobbler.info:6174/api/files";
    struct curl_slist* headers = nullptr;
    headers = curl_slist_append(headers, "Content-Type: application/json");
    headers = curl_slist_append(headers, QString("Authorization: Bearer " + authToken).toStdString().c_str());

    std::string responseString;
    curl_easy_setopt(curl, CURLOPT_URL, url.toStdString().c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseString);

    CURLcode res = curl_easy_perform(curl);
    bool success = false;

    if (res == CURLE_OK) {
        QJsonDocument doc = QJsonDocument::fromJson(QByteArray::fromStdString(responseString));
        if (!doc.isObject()) {
            errorOut = "Invalid JSON response";
        } else {
            QJsonObject root = doc.object();
            countOut = root["count"].toInt();

            QJsonArray filesArray = root["files"].toArray();
            for (const QJsonValue& val : filesArray) {
                QJsonObject obj = val.toObject();
                FileInfo file;
                file.file_id = obj["file_id"].toString();
                file.file_name = obj["file_name"].toString();
                file.iv_file = obj["iv_file"].toString();
                file.assoc_data_file = obj["assoc_data_file"].toString();
                file.created_at = obj["created_at"].toString();

                QJsonObject dek = obj["dek_data"].toObject();
                file.dek_data.key_id = dek["key_id"].toString();
                file.dek_data.iv_dek = dek["iv_dek"].toString();
                file.dek_data.encrypted_dek = dek["encrypted_dek"].toString();
                file.dek_data.assoc_data_dek = dek["assoc_data_dek"].toString();

                filesOut.append(file);
            }

            success = true;
        }
    } else {
        errorOut = QString("CURL error: ") + curl_easy_strerror(res);
    }

    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);
    return success;
}

bool FilesApi::uploadEncryptedFile(const QString& filePath,
                                   const QString& ivFile,
                                   const QString& fileType,
                                   qint64 fileSize,
                                   const QString& ivDEK,
                                   const QString& encryptedDEK,
                                   QString& responseOut,
                                   const QString& customFileName)
{
    CURL *curl = curl_easy_init();
    if (!curl)
        return false;

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Failed to open file:" << filePath;
        return false;
    }

    // Use MIME instead of deprecated formadd
    curl_mime* mime = curl_mime_init(curl);
    curl_mimepart* part = curl_mime_addpart(mime);

    // File part
    curl_mime_name(part, "encrypted_file");
    curl_mime_filedata(part, filePath.toStdString().c_str());

    // Prepare headers
    struct curl_slist* headers = nullptr;
    QFileInfo fileInfo(filePath);
    QString finalFileName = customFileName.isEmpty() ? fileInfo.fileName() : customFileName;

    headers = curl_slist_append(headers, ("X-File-Name: " + finalFileName).toUtf8().constData());
    headers = curl_slist_append(headers, ("X-IV-File: " + ivFile).toUtf8().constData());
    if (!fileType.isEmpty())
        headers = curl_slist_append(headers, ("X-File-Type: " + fileType).toUtf8().constData());
    headers = curl_slist_append(headers, ("X-File-Size: " + QString::number(fileSize)).toUtf8().constData());
    headers = curl_slist_append(headers, ("X-IV-DEK: " + ivDEK).toUtf8().constData());
    headers = curl_slist_append(headers, ("X-Encrypted-DEK: " + encryptedDEK).toUtf8().constData());

    std::string responseString;

    curl_easy_setopt(curl, CURLOPT_URL, "http://gobbler.info:6174/api/files");
    curl_easy_setopt(curl, CURLOPT_MIMEPOST, mime);  // modern replacement
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseString);

    CURLcode res = curl_easy_perform(curl);
    bool success = false;

    if (res == CURLE_OK) {
        responseOut = QString::fromStdString(responseString);
        success = true;
    } else {
        qWarning() << "CURL error:" << curl_easy_strerror(res);
    }

    curl_mime_free(mime);                 // Clean up MIME
    curl_slist_free_all(headers);        // Clean up headers
    curl_easy_cleanup(curl);
    file.close();

    return success;
}


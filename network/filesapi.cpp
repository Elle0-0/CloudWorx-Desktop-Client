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

    QString url = "https://networkninjas.gobbler.info/api/files";
    struct curl_slist* headers = nullptr;
    headers = curl_slist_append(headers, "Content-Type: application/json");
    headers = curl_slist_append(headers, QString("Authorization: Bearer " + authToken).toStdString().c_str());

    std::string responseString;
    curl_easy_setopt(curl, CURLOPT_URL, url.toStdString().c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseString);

    if (!setCurlCACert(curl)) {
        errorOut = "Failed to set CA cert.";
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
        return false;
    }

    CURLcode res = curl_easy_perform(curl);
    long http_code = 0;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
    bool success = false;

    if (res == CURLE_OK && http_code == 200) {
        QJsonDocument doc = QJsonDocument::fromJson(QByteArray::fromStdString(responseString));
        if (doc.isObject()) {
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
        } else {
            errorOut = "Invalid JSON response";
        }
    } else {
        errorOut = QString("CURL error: %1, HTTP code: %2")
                       .arg(curl_easy_strerror(res))
                       .arg(http_code);
    }

    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);
    return success;
}


bool FilesApi::uploadEncryptedFile(const QString& authToken,
                                   const QString& filePath,
                                   const QString& ivFile,
                                   const QString& fileType,
                                   qint64 fileSize,
                                   const QString& ivDEK,
                                   const QString& encryptedDEK,
                                   QString& responseOut,
                                   const QString& customFileName)
{
    CURL* curl = curl_easy_init();
    if (!curl)
        return false;

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Failed to open file:" << filePath;
        return false;
    }

    curl_mime* mime = curl_mime_init(curl);
    curl_mimepart* part = curl_mime_addpart(mime);
    curl_mime_name(part, "encrypted_file");
    curl_mime_filedata(part, filePath.toStdString().c_str());

    struct curl_slist* headers = nullptr;
    QFileInfo fileInfo(filePath);
    QString finalFileName = customFileName.isEmpty() ? fileInfo.fileName() : customFileName;

    headers = curl_slist_append(headers, QString("Authorization: Bearer " + authToken).toStdString().c_str());
    headers = curl_slist_append(headers, ("X-File-Name: " + finalFileName).toUtf8().constData());
    headers = curl_slist_append(headers, ("X-IV-File: " + ivFile).toUtf8().constData());
    if (!fileType.isEmpty())
        headers = curl_slist_append(headers, ("X-File-Type: " + fileType).toUtf8().constData());
    headers = curl_slist_append(headers, ("X-File-Size: " + QString::number(fileSize)).toUtf8().constData());
    headers = curl_slist_append(headers, ("X-IV-DEK: " + ivDEK).toUtf8().constData());
    headers = curl_slist_append(headers, ("X-Encrypted-DEK: " + encryptedDEK).toUtf8().constData());

    std::string responseString;
    curl_easy_setopt(curl, CURLOPT_URL, "https://networkninjas.gobbler.info/api/files");
    curl_easy_setopt(curl, CURLOPT_MIMEPOST, mime);
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseString);

    if (!setCurlCACert(curl)) {
        qDebug() << "Failed to set CA cert.";
        curl_mime_free(mime);
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
        return false;
    }

    CURLcode res = curl_easy_perform(curl);
    long http_code = 0;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
    bool success = false;

    if (res == CURLE_OK && http_code == 200) {
        responseOut = QString::fromStdString(responseString);
        success = true;
        qDebug() << "[uploadEncryptedFile] Upload succeeded.";
        qDebug() << "[uploadEncryptedFile] Server response:" << responseOut;
    } else {
        responseOut = QString("CURL error: %1, HTTP code: %2")
                          .arg(curl_easy_strerror(res))
                          .arg(http_code);
        qWarning() << responseOut;
    }

    curl_mime_free(mime);
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);
    file.close();

    return success;
}

bool FilesApi::downloadEncryptedFileToMemory(const QString& authToken, const QString& fileName, QByteArray& fileDataOut, QString& errorOut)
{
    CURL* curl = curl_easy_init();
    if (!curl)
        return false;

    QString url = "https://networkninjas.gobbler.info/api/files/" + QUrl::toPercentEncoding(fileName);

    struct curl_slist* headers = nullptr;
    headers = curl_slist_append(headers, QString("Authorization: Bearer " + authToken).toStdString().c_str());

    std::string responseBuffer;

    curl_easy_setopt(curl, CURLOPT_URL, url.toStdString().c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, +[](char* ptr, size_t size, size_t nmemb, void* userdata) -> size_t {
            std::string* buffer = static_cast<std::string*>(userdata);
            buffer->append(ptr, size * nmemb);
            return size * nmemb;
        });
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseBuffer);

    if (!setCurlCACert(curl)) {
        errorOut = "Failed to set CA cert.";
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
        return false;
    }

    CURLcode res = curl_easy_perform(curl);
    long http_code = 0;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);

    bool success = false;

    if (res == CURLE_OK && http_code == 200) {
        fileDataOut = QByteArray::fromStdString(responseBuffer);
        success = true;
    } else {
        errorOut = QString("CURL error: %1 | HTTP code: %2").arg(curl_easy_strerror(res)).arg(http_code);
    }

    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);
    return success;
}
bool FilesApi::resolveFileId(const QString& fileName, QString& fileIdOut)
{
    CURL* curl = curl_easy_init();
    if (!curl)
        return false;

    QString url = QString("https://networkninjas.gobbler.info/api/files/resolve-id/%1")
                      .arg(QString::fromUtf8(QUrl::toPercentEncoding(fileName)));

    curl_easy_setopt(curl, CURLOPT_URL, url.toUtf8().constData());

    std::string responseStr;
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseStr);

    if (!setCurlCACert(curl)) {
        qDebug() << "Failed to set CA cert.";
        curl_easy_cleanup(curl);
        return false;
    }

    CURLcode res = curl_easy_perform(curl);
    long http_code = 0;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);

    bool success = false;

    if (res == CURLE_OK && http_code == 200) {
        QJsonDocument doc = QJsonDocument::fromJson(QByteArray::fromStdString(responseStr));
        if (doc.isObject()) {
            QJsonObject obj = doc.object();
            if (obj.contains("file_id")) {
                fileIdOut = obj["file_id"].toString();
                success = true;
            } else {
                qWarning() << "Response JSON does not contain 'file_id'.";
            }
        } else {
            qWarning() << "Invalid JSON received.";
        }
    } else {
        qWarning() << QString("CURL error: %1 | HTTP code: %2")
                          .arg(curl_easy_strerror(res))
                          .arg(http_code);
    }

    curl_easy_cleanup(curl);
    return success;
}

bool FilesApi::deleteFileByName(const QString& fileName,const QString& authToken, QString& responseOut)
{
    CURL* curl = curl_easy_init();
    if (!curl)
        return false;

    QString url = QString("https://networkninjas.gobbler.info/api/files/%1")
                      .arg(QString::fromUtf8(QUrl::toPercentEncoding(fileName)));

    struct curl_slist* headers = nullptr;
    QString authHeader = "Authorization: Bearer " + authToken;
    headers = curl_slist_append(headers, authHeader.toStdString().c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

    curl_easy_setopt(curl, CURLOPT_URL, url.toUtf8().constData());
    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "DELETE");

    std::string responseStr;
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseStr);

    if (!setCurlCACert(curl)) {
        qDebug() << "Failed to set CA cert.";
        curl_easy_cleanup(curl);
        return false;
    }

    CURLcode res = curl_easy_perform(curl);
    long http_code = 0;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);

    bool success = false;
    if (res == CURLE_OK && http_code == 200) {
        responseOut = QString::fromStdString(responseStr);
        success = true;
    } else {
        qWarning() << "CURL error:" << curl_easy_strerror(res) << "HTTP code:" << http_code;
        responseOut = QString("HTTP Error %1").arg(http_code);
    }

    curl_easy_cleanup(curl);
    return success;
}


#include "sharesapi.h"
#include "../utils/apihelper.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonParseError>
#include <QDebug>
#include <curl/curl.h>

SharesApi::SharesApi() {}

bool SharesApi::shareEncryptedFile(const QString& fileId,
                                   const QString& authToken,
                                   const QString& sharedWithUsername,
                                   const QString& encryptedFile,
                                   const QString& nonce,
                                   const QString& ephemeralPublicKey,
                                   const QString& signature,
                                   QString& responseOut,
                                   QString& errorOut)
{
    CURL* curl = curl_easy_init();
    if (!curl)
        return false;

    QJsonObject json;
    json["shared_with_username"] = sharedWithUsername;
    json["encrypted_file"] = encryptedFile;
    json["nonce"] = nonce;
    json["ephemeral_public_key"] = ephemeralPublicKey;
    json["signature"] = signature;

    QByteArray jsonData = QJsonDocument(json).toJson();

    struct curl_slist* headers = nullptr;
    headers = curl_slist_append(headers, "Content-Type: application/json");
    headers = curl_slist_append(headers, QString("Authorization: Bearer " + authToken).toStdString().c_str());

    std::string responseString;

    QString url = "https://networkninjas.gobbler.info/api/shares/" + fileId + "/share";
    curl_easy_setopt(curl, CURLOPT_URL, url.toUtf8().constData());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, jsonData.constData());
    curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, jsonData.size());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseString);

    if (!setCurlCACert(curl)) {
        errorOut = "Failed to set CA cert.";
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
        return false;
    }

    CURLcode res = curl_easy_perform(curl);
    bool success = false;

    if (res == CURLE_OK) {
        long http_code = 0;
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);

        if (http_code == 201) {
            responseOut = QString::fromStdString(responseString);
            success = true;
        } else {
            errorOut = QString("Unexpected HTTP code: %1\nResponse: %2")
                           .arg(http_code)
                           .arg(QString::fromStdString(responseString));
        }
    } else {
        errorOut = QString("CURL error: %1").arg(curl_easy_strerror(res));
    }

    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);
    return success;
}

bool SharesApi::downloadSharedFile(const QString& shareId,const QString& authToken, QString& responseOut, QString& errorOut)
{
    CURL* curl = curl_easy_init();
    if (!curl)
        return false;

    struct curl_slist* headers = nullptr;
    headers = curl_slist_append(headers, "Content-Type: application/json");
    headers = curl_slist_append(headers, QString("Authorization: Bearer " + authToken).toStdString().c_str());

    std::string responseString;
    QString url = "https://networkninjas.gobbler.info/api/shares/download/" + shareId;

    curl_easy_setopt(curl, CURLOPT_URL, url.toUtf8().constData());
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
    bool success = false;

    if (res == CURLE_OK) {
        long http_code = 0;
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);

        if (http_code == 200) {
            responseOut = QString::fromStdString(responseString);
            success = true;
        } else if (http_code == 403) {
            errorOut = "Access denied (403)";
        } else if (http_code == 404) {
            errorOut = "Shared file not found (404)";
        } else {
            errorOut = QString("Unexpected HTTP code: %1\nResponse: %2")
                           .arg(http_code)
                           .arg(QString::fromStdString(responseString));
        }
    } else {
        errorOut = QString("CURL error: %1").arg(curl_easy_strerror(res));
    }

    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);
    return success;
}

bool SharesApi::getPublicKeysForUser(const QString& username,const QString& authToken, QString& responseOut, QString& errorOut)
{
    CURL* curl = curl_easy_init();
    if (!curl)
        return false;

    struct curl_slist* headers = nullptr;
    headers = curl_slist_append(headers, "Content-Type: application/json");
    headers = curl_slist_append(headers, QString("Authorization: Bearer " + authToken).toStdString().c_str());

    std::string responseString;
    QString url = "https://networkninjas.gobbler.info/api/shares/public-key/" + username;

    curl_easy_setopt(curl, CURLOPT_URL, url.toUtf8().constData());
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
    bool success = false;

    if (res == CURLE_OK) {
        long http_code = 0;
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);

        if (http_code == 200) {
            responseOut = QString::fromStdString(responseString);
            success = true;
        } else if (http_code == 404) {
            errorOut = "User not found (404)";
        } else {
            errorOut = QString("Unexpected HTTP code: %1\nResponse: %2")
                           .arg(http_code)
                           .arg(QString::fromStdString(responseString));
        }
    } else {
        errorOut = QString("CURL error: %1").arg(curl_easy_strerror(res));
    }

    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);
    return success;
}

bool SharesApi::getFilesSharedWithMe(QList<SharedFileModel>& filesOut,const QString& authToken, QString& errorOut)
{
    CURL* curl = curl_easy_init();
    if (!curl)
        return false;

    struct curl_slist* headers = nullptr;
    headers = curl_slist_append(headers, "Content-Type: application/json");
    headers = curl_slist_append(headers, QString("Authorization: Bearer " + authToken).toStdString().c_str());

    std::string responseString;
    const char* url = "https://networkninjas.gobbler.info/api/shares/shared-with-me";

    curl_easy_setopt(curl, CURLOPT_URL, url);
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
    bool success = false;

    if (res == CURLE_OK) {
        long http_code = 0;
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);

        if (http_code == 200) {
            QJsonDocument doc = QJsonDocument::fromJson(QByteArray::fromStdString(responseString));
            QJsonObject obj = doc.object();

            QJsonArray filesArray = obj["files"].toArray();
            for (const QJsonValue& val : filesArray) {
                QJsonObject f = val.toObject();
                SharedFileModel file;
                file.share_id = f["share_id"].toString();
                file.file_id = f["file_id"].toString();
                file.file_name = f["file_name"].toString();
                file.file_type = f["file_type"].toString();
                file.file_size = f["file_size"].toInt();
                file.shared_by = f["shared_by"].toString();
                file.shared_by_username = f["shared_by_username"].toString();
                file.created_at = f["created_at"].toString();

                filesOut.append(file);
            }

            success = true;
        } else {
            errorOut = QString("Unexpected HTTP code: %1").arg(http_code);
        }
    } else {
        errorOut = QString("CURL error: %1").arg(curl_easy_strerror(res));
    }

    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);
    return success;
}

bool SharesApi::getSharesForFile(const QString& file_id,const QString& authToken, QList<ShareDetailModel>& sharesOut, QString& errorOut)
{
    CURL* curl = curl_easy_init();
    if (!curl)
        return false;

    struct curl_slist* headers = nullptr;
    headers = curl_slist_append(headers, "Content-Type: application/json");
    headers = curl_slist_append(headers, QString("Authorization: Bearer " + authToken).toStdString().c_str());

    std::string responseString;

    std::string url = "https://networkninjas.gobbler.info/api/shares/" + file_id.toStdString() + "/share";

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
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
    bool success = false;

    if (res == CURLE_OK) {
        long http_code = 0;
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);

        if (http_code == 200) {
            QJsonDocument doc = QJsonDocument::fromJson(QByteArray::fromStdString(responseString));
            QJsonObject obj = doc.object();

            QJsonArray sharesArray = obj["shares"].toArray();
            for (const QJsonValue& val : sharesArray) {
                QJsonObject s = val.toObject();
                ShareDetailModel share;
                share.share_id = s["share_id"].toString();
                share.shared_with = s["shared_with"].toString();
                share.shared_with_username = s["shared_with_username"].toString();
                share.created_at = s["created_at"].toString();
                share.file_id = s["file_id"].toString();
                share.file_name = s["file_name"].toString();
                share.file_size = s["file_size"].toInt();
                share.file_type = s["file_type"].toString();

                sharesOut.append(share);
            }

            success = true;
        } else {
            errorOut = QString("Unexpected HTTP code: %1").arg(http_code);
        }
    } else {
        errorOut = QString("CURL error: %1").arg(curl_easy_strerror(res));
    }

    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);
    return success;
}

bool SharesApi::revokeShareAccess(const QString& file_id,const QString& authToken, const QString& shared_with_username, QString& errorOut)
{
    CURL* curl = curl_easy_init();
    if (!curl)
        return false;

    // Prepare JSON payload
    QJsonObject json;
    json["shared_with_username"] = shared_with_username;
    QByteArray jsonData = QJsonDocument(json).toJson();

    struct curl_slist* headers = nullptr;
    headers = curl_slist_append(headers, "Content-Type: application/json");
    headers = curl_slist_append(headers, QString("Authorization: Bearer " + authToken).toStdString().c_str());

    std::string responseString;

    std::string url = "https://networkninjas.gobbler.info/api/shares/" + file_id.toStdString() + "/share";

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "DELETE");  // HTTP DELETE method
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, jsonData.constData());       // send JSON body
    curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, jsonData.size());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseString);

    if (!setCurlCACert(curl)) {
        errorOut = "Failed to set CA cert.";
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
        return false;
    }

    CURLcode res = curl_easy_perform(curl);
    bool success = false;

    if (res == CURLE_OK) {
        long http_code = 0;
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);

        if (http_code == 200) {
            success = true;
        } else if (http_code == 404) {
            errorOut = "File or share not found";
        } else {
            errorOut = QString("Unexpected HTTP code: %1").arg(http_code);
        }
    } else {
        errorOut = QString("CURL error: %1").arg(curl_easy_strerror(res));
    }

    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);
    return success;
}





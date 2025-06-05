#include "authapi.h"
#include "../utils/apihelper.h"

#include <QJsonDocument>
#include <QJsonArray>
#include <QDebug>
#include <QCoreApplication>
#include <QFile>
#include <curl/curl.h>

AuthAPI::AuthAPI() {}

bool AuthAPI::registerUser(const UserRegisterModel& model, QString& responseOut, QString& errorOut)
{
    CURL* curl = curl_easy_init();
    if (!curl)
        return false;

    // Build JSON payload
    QJsonObject json{
        {"username", model.username},
        {"auth_password", model.auth_password},
        {"email", model.email},
        {"public_key", model.public_key},
        {"signing_public_key", model.signing_public_key},
        {"iv_KEK", model.iv_KEK},
        {"encrypted_KEK", model.encrypted_KEK},
        {"salt", model.salt},
        {"p", model.p},
        {"m", model.m},
        {"t", model.t}
    };

    QByteArray jsonData = QJsonDocument(json).toJson();

    struct curl_slist* headers = curl_slist_append(nullptr, "Content-Type: application/json");

    std::string responseString;
    bool success = false;

    curl_easy_setopt(curl, CURLOPT_URL, "https://networkninjas.gobbler.info/api/auth/register");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, jsonData.constData());
    curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, jsonData.size());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseString);

    if (!setCurlCACert(curl)) {
        errorOut = "Failed to set CA cert.";
    } else {
        CURLcode res = curl_easy_perform(curl);
        if (res == CURLE_OK) {
            responseOut = QString::fromStdString(responseString);
            success = true;
        } else {
            errorOut = QString("CURL error: ") + curl_easy_strerror(res);
        }
    }

    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);
    return success;
}
bool AuthAPI::loginUser(const QString& username, const QString& entered_auth_password,
                        UserLoginModel& responseOut, QString& errorOut)
{
    CURL* curl = curl_easy_init();
    if (!curl)
        return false;

    QJsonObject json{
        {"username", username},
        {"entered_auth_password", entered_auth_password}
    };

    QByteArray jsonData = QJsonDocument(json).toJson();

    struct curl_slist* headers = curl_slist_append(nullptr, "Content-Type: application/json");

    std::string responseString;
    bool success = false;

    curl_easy_setopt(curl, CURLOPT_URL, "https://networkninjas.gobbler.info/api/auth/login");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, jsonData.constData());
    curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, jsonData.size());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseString);

    if (!setCurlCACert(curl)) {
        errorOut = "Failed to set CA cert.";
    } else {
        CURLcode res = curl_easy_perform(curl);
        long http_code = 0;
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
        if (res == CURLE_OK && http_code == 200) {
            QJsonDocument doc = QJsonDocument::fromJson(QByteArray::fromStdString(responseString));
            if (!doc.isObject()) {
                errorOut = "Invalid JSON response";
            } else {
                QJsonObject obj = doc.object();
                responseOut.token = obj["token"].toString();
                responseOut.user_id = obj["user_id"].toString();

                QJsonArray filesArray = obj["files"].toArray();
                for (const QJsonValue& fileVal : filesArray) {
                    QJsonObject fileObj = fileVal.toObject();
                    FileModel file(
                        fileObj["file_id"].toString(),
                        fileObj["file_name"].toString(),
                        fileObj["file_type"].toString(),
                        fileObj["file_size"].toInt()
                        );
                    responseOut.files.append(file);
                }

                success = true;
            }
        } else {
            errorOut = QString("CURL error: ") + curl_easy_strerror(res);
        }
    }

    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);
    return success;
}


bool AuthAPI::changeAuthPassword(const QString username, const QString oldPassword, const QString newPassword , QString& responseOut, QString& errorOut)
{
    CURL *curl = curl_easy_init();
    if (!curl)
        return false;

    QJsonObject json;
    json["username"] = username;
    json["old_auth_password"] = oldPassword;
    json["new_auth_password"] = newPassword;

    QByteArray jsonData = QJsonDocument(json).toJson();

    struct curl_slist *headers = nullptr;
    headers = curl_slist_append(headers, "Content-Type: application/json");

    std::string responseString;
    curl_easy_setopt(curl, CURLOPT_URL, "https://networkninjas.gobbler.info/api/auth/auth-password");
    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PUT");
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
        responseOut = QString::fromStdString(responseString);
        success = true;
    } else {
        qWarning() << "curl error:" << curl_easy_strerror(res);
    }

    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);
    return success;
}

bool AuthAPI::changeEncryptionPassword(const ChangeEncryptionPasswordModel& model, QString& responseOut, QString& errorOut)
{
    CURL *curl = curl_easy_init();
    if (!curl)
        return false;

    // Create JSON body
    QJsonObject json;
    json["username"] = model.username;
    json["old_password_derived_key"] = model.old_password_derived_key;
    json["new_password_derived_key"] = model.new_password_derived_key;
    json["new_iv_KEK"] = model.new_iv_KEK;
    json["new_encrypted_KEK"] = model.new_encrypted_KEK;

    QByteArray jsonData = QJsonDocument(json).toJson();

    struct curl_slist *headers = nullptr;
    headers = curl_slist_append(headers, "Content-Type: application/json");

    std::string responseString;
    curl_easy_setopt(curl, CURLOPT_URL, "https://networkninjas.gobbler.info/api/auth/encryption-password");
    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PUT");
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
        responseOut = QString::fromStdString(responseString);
        success = true;
    } else {
        qWarning() << "curl error:" << curl_easy_strerror(res);
    }

    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);
    return success;
}

bool AuthAPI::fetchAllUsers(QList<UserInfoModel>& usersOut, QString& errorOut)
{
    CURL *curl = curl_easy_init();
    if (!curl)
        return false;

    struct curl_slist *headers = nullptr;
    headers = curl_slist_append(headers, "Content-Type: application/json");

    std::string responseString;
    curl_easy_setopt(curl, CURLOPT_URL, "https://networkninjas.gobbler.info/api/auth/users");
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
        QJsonDocument doc = QJsonDocument::fromJson(QByteArray::fromStdString(responseString));
        if (!doc.isObject()) {
            errorOut = "Invalid JSON response";
        } else {
            QJsonObject obj = doc.object();
            QJsonArray usersArray = obj["users"].toArray();
            for (const QJsonValue& userVal : usersArray) {
                QJsonObject userObj = userVal.toObject();
                UserInfoModel user;
                user.username = userObj["username"].toString();
                user.email = userObj["email"].toString();
                usersOut.append(user);
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
bool AuthAPI::deleteUser(const QString userID, const QString password, QString& responseOut, QString& errorOut)
{
    CURL* curl = curl_easy_init();
    if (!curl)
        return false;

    // Construct the endpoint URL with the user ID
    QString endpoint = QString("https://networkninjas.gobbler.info/api/auth/%1").arg(userID);

    // JSON payload with password
    QJsonObject json;
    json["password"] = password;
    QByteArray jsonData = QJsonDocument(json).toJson();

    struct curl_slist* headers = nullptr;
    headers = curl_slist_append(headers, "Content-Type: application/json");

    std::string responseString;
    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "DELETE");
    curl_easy_setopt(curl, CURLOPT_URL, endpoint.toStdString().c_str());
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
        responseOut = QString::fromStdString(responseString);
        success = true;
    } else {
        responseOut = QString("CURL error: ") + curl_easy_strerror(res);
    }

    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);
    return success;
}

UploadResult AuthAPI::getUserInfo(const QString& authToken, QString& errorOut)
{
    UploadResult result;

    CURL *curl = curl_easy_init();
    if (!curl) {
        result.success = false;
        result.errorMessage = "Failed to initialize CURL";
        return result;
    }

    QString url = QString("https://networkninjas.gobbler.info/api/auth/user_info");

    qDebug() << "[getUserInfo] Called";

    struct curl_slist *headers = nullptr;
    headers = curl_slist_append(headers, "Accept: application/json");
    headers = curl_slist_append(headers, QString("Authorization: Bearer " + authToken).toStdString().c_str());

    std::string responseString;
    curl_easy_setopt(curl, CURLOPT_URL, url.toStdString().c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseString);

    qDebug() << "[getUserInfo] Fetching URL:" << url;

    if (!setCurlCACert(curl)) {
        errorOut = "Failed to set CA cert.";
        result.success = false;
        result.errorMessage = errorOut;
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
        return result;
    }

    CURLcode res = curl_easy_perform(curl);

    if (res != CURLE_OK) {
        result.success = false;
        result.errorMessage = QString("CURL error: %1").arg(curl_easy_strerror(res));
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
        return result;
    }

    // Parse JSON response
    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(QByteArray::fromStdString(responseString), &parseError);

    if (parseError.error != QJsonParseError::NoError || !doc.isObject()) {
        result.success = false;
        result.errorMessage = QString("JSON parse error: %1").arg(parseError.errorString());
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
        return result;
    }

    QJsonObject obj = doc.object();

    // Map JSON fields to struct
    result.userId = obj.value("user_id").toString();
    result.username = obj.value("username").toString();
    result.email = obj.value("email").toString();
    result.publicKey = obj.value("public_key").toString();
    result.createdAt = obj.value("created_at").toString();
    result.modifiedAt = obj.value("modified_at").toString();
    result.keyId = obj.value("key_id").toString();
    result.ivKEK = obj.value("iv_KEK").toString();
    result.encryptedKEK = obj.value("encrypted_KEK").toString();
    result.assocDataKEK = obj.value("assoc_data_KEK").toString();
    result.salt = obj.value("salt").toString();
    result.p = obj.value("p").toInt();
    result.m = obj.value("m").toInt();
    result.t = obj.value("t").toInt();
    result.kekCreatedAt = obj.value("kek_created_at").toString();

    result.success = true;

    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);

    return result;
}




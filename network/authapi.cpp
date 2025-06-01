#include "authapi.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>
#include <curl/curl.h>

AuthAPI::AuthAPI() {}

size_t AuthAPI::writeCallback(void* contents, size_t size, size_t nmemb, void* userp)
{
    size_t totalSize = size * nmemb;
    std::string* str = static_cast<std::string*>(userp);
    str->append(static_cast<char*>(contents), totalSize);
    return totalSize;
}


bool AuthAPI::registerUser(const UserRegisterModel& model, QString& responseOut)
{
    CURL *curl = curl_easy_init();
    if (!curl)
        return false;

    // Build JSON from the model
    QJsonObject json;
    json["username"] = model.username;
    json["auth_password"] = model.auth_password;
    json["email"] = model.email;
    json["public_key"] = model.public_key;
    json["iv_KEK"] = model.iv_KEK;
    json["encrypted_KEK"] = model.encrypted_KEK;

    QByteArray jsonData = QJsonDocument(json).toJson();

    struct curl_slist *headers = nullptr;
    headers = curl_slist_append(headers, "Content-Type: application/json");

    std::string responseString;
    curl_easy_setopt(curl, CURLOPT_URL, "http://gobbler.info:6174/api/auth/register");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, jsonData.constData());
    curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, jsonData.size());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseString);

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

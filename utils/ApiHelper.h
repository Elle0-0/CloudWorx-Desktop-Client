#ifndef APIHELPER_H
#define APIHELPER_H

#include <string>
#include <QCoreApplication>
#include <QFile>
#include <QDebug>
#include <curl/curl.h>

inline size_t writeCallback(void* contents, size_t size, size_t nmemb, void* userp)
{
    size_t totalSize = size * nmemb;
    std::string* str = static_cast<std::string*>(userp);
    str->append(static_cast<char*>(contents), totalSize);
    return totalSize;
}

inline bool setCurlCACert(CURL* curl)
{
    QString certPath = QCoreApplication::applicationDirPath() + "/certs/cacert.pem";
    if (!QFile::exists(certPath)) {
        qWarning() << "[setCurlCACert] CA cert file not found at:" << certPath;
        return false;
    }

    CURLcode result = curl_easy_setopt(curl, CURLOPT_CAINFO, certPath.toStdString().c_str());
    if (result != CURLE_OK) {
        qWarning() << "[setCurlCACert] Failed to set CAINFO:" << curl_easy_strerror(result);
        return false;
    }

    return true;
}

#endif // APIHELPER_H

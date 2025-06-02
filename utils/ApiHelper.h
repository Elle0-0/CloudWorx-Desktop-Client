#ifndef APIHELPER_H
#define APIHELPER_H

#include <string>

inline size_t writeCallback(void* contents, size_t size, size_t nmemb, void* userp)
{
    size_t totalSize = size * nmemb;
    std::string* str = static_cast<std::string*>(userp);
    str->append(static_cast<char*>(contents), totalSize);
    return totalSize;
}

#endif // APIHELPER_H

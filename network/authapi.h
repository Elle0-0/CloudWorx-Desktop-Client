#ifndef AUTHAPI_H
#define AUTHAPI_H

#include "../models/UserRegisterModel.h"

class AuthAPI
{
public:
    AuthAPI();
    bool registerUser(const UserRegisterModel& model, QString& responseOut);
    static size_t writeCallback(void* contents, size_t size, size_t nmemb, void* userp);

};

#endif // AUTHAPI_H

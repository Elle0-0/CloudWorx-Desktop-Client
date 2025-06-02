#ifndef USERLOGINMODEL_H
#define USERLOGINMODEL_H

#include <QString>
#include <QList>
#include "FileModel.h"

struct UserLoginModel {
    QString token;
    QString user_id;
    QList<FileModel> files;
};

#endif // USERLOGINMODEL_H

#ifndef FILEMODEL_H
#define FILEMODEL_H

#include <QString>


class FileModel {
public:
    QString file_id;
    QString file_name;
    QString file_type;
    int file_size;

    //default constructor ig
    FileModel() : file_id(""), file_name(""), file_type(""), file_size(0) {}

    FileModel(const QString& id, const QString& name, const QString& type, int size)
        : file_id(id), file_name(name), file_type(type), file_size(size) {}
};


#endif // FILEMODEL_H

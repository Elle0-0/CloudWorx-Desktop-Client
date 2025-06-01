#ifndef FILELISTMODEL_H
#define FILELISTMODEL_H

#include "filemodel.h"
#include <QAbstractListModel>
#include <QVector>

class FileListModel : public QAbstractListModel
{
    Q_OBJECT

public:
    explicit FileListModel(QObject *parent = nullptr)
        : QAbstractListModel(parent) {}

    int rowCount(const QModelIndex &parent = QModelIndex()) const override {
        Q_UNUSED(parent)
        return files.size();
    }

    QVariant data(const QModelIndex &index, int role) const override {
        if (!index.isValid() || index.row() < 0 || index.row() >= files.size())
            return QVariant();

        const FileModel &file = files[index.row()];
        if (role == Qt::DisplayRole)
            return QString("%1 (%2 KB, %3)").arg(file.file_name).arg(file.file_size).arg(file.file_type);
        return QVariant();
    }

    void addFile(const FileModel &file) {
        beginInsertRows(QModelIndex(), files.size(), files.size());
        files.append(file);
        endInsertRows();
    }

    const FileModel &getFile(int row) const {
        return files.at(row);
    }

private:
    QVector<FileModel> files;
};

#endif // FILELISTMODEL_H

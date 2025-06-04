#ifndef FILELISTMODEL_H
#define FILELISTMODEL_H

#include <QAbstractListModel>
#include "../envelopeencryptionmanager.h"
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

        const FileData &file = files[index.row()];
        if (role == Qt::DisplayRole)
            return QString("%1 (%2 KB, %3)").arg(file.fileName).arg(file.fileSize).arg(file.fileType);
        return QVariant();
    }

    void addFile(const FileData &file) {
        beginInsertRows(QModelIndex(), files.size(), files.size());
        files.append(file);
        endInsertRows();
    }

    void clear()
    {
        beginResetModel();
        files.clear();
        endResetModel();
    }


    const FileData &getFile(int row) const {
        return files.at(row);
    }

private:
    QVector<FileData> files;
};

#endif // FILELISTMODEL_H

#ifndef VIRTUALFILE_H
#define VIRTUALFILE_H

#pragma once
#include <QString>
#include <QFileInfo>

// Abstract Base Class
class VirtualFile {
public:
    VirtualFile(const QString& path) : filePath(path) {}
    virtual ~VirtualFile() {}  // virtual destructor

    // virtual function (for polymorphism)
    virtual QString getType() const { return "VirtualFile"; }

    // pure virtual function (must override)
    virtual qint64 getSize() const = 0;

    // operator overloading (==)
    bool operator==(const VirtualFile& other) const {
        return QFileInfo(filePath).fileName() == QFileInfo(other.filePath).fileName();
    }

    QString getFileName() const { return QFileInfo(filePath).fileName(); }
    QString getFullPath() const { return filePath; }

protected:
    QString filePath;
};

// derived class: Text File
class TextFile : public VirtualFile {
public:
    TextFile(const QString& path) : VirtualFile(path) {}

    QString getType() const override { return "TextFile"; }
    qint64 getSize() const override {
        QFileInfo info(filePath);
        return info.size();
    }
};

// derived class: PDF File
class PdfFile : public VirtualFile {
public:
    PdfFile(const QString& path) : VirtualFile(path) {}

    QString getType() const override { return "PdfFile"; }
    qint64 getSize() const override {
        QFileInfo info(filePath);
        return info.size();
    }
};

// derived class: Image File
class ImageFile : public VirtualFile {
public:
    ImageFile(const QString& path) : VirtualFile(path) {}

    QString getType() const override { return "ImageFile"; }
    qint64 getSize() const override {
        QFileInfo info(filePath);
        return info.size();
    }
};


#endif // VIRTUALFILE_H

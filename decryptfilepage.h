#ifndef DECRYPTFILEPAGE_H
#define DECRYPTFILEPAGE_H

#include "models/filemodel.h"

#include <QWidget>

namespace Ui {
class DecryptFilePage;
}

class DecryptFilePage : public QWidget
{
    Q_OBJECT

public:
    explicit DecryptFilePage(QWidget *parent = nullptr);
    ~DecryptFilePage();
    void setFileData(const FileModel &file);

private:
    Ui::DecryptFilePage *ui;
    FileModel file;
};

#endif // DECRYPTFILEPAGE_H

#ifndef DECRYPTSHAREDPAGE_H
#define DECRYPTSHAREDPAGE_H

#include <QWidget>
#include "envelopeencryptionmanager.h"

namespace Ui {
class DecryptSharedPage;
}

class DecryptSharedPage : public QWidget
{
    Q_OBJECT

public:
    explicit DecryptSharedPage(QWidget *parent = nullptr);
    void setFileData(const FileData &file);
    QString getPrivateKeyPath() const { return privSignatureKeyFilePath; }
    ~DecryptSharedPage();

private slots:
    void on_uploadKeyButton_clicked();

private:
    Ui::DecryptSharedPage *ui;
    QString privSignatureKeyFilePath;
    FileData fileData;
};

#endif // DECRYPTSHAREDPAGE_H

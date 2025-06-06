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
    void setFileData(const FileData &fileData, QString jwtToken);
    QByteArray extractRawFromPEM(const QString &pemStr);
    ~DecryptSharedPage();

private slots:
    void on_uploadKeyButton_clicked();

    void on_decryptButton_clicked();

    void on_dashboardButton_clicked();

private:
    Ui::DecryptSharedPage *ui;
    QString privSignatureKeyFilePath;
    FileData fileData;
    QString authToken;

signals:
    void backToDashboard();
};

#endif // DECRYPTSHAREDPAGE_H

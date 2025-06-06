#ifndef SHAREFILEPAGE_H
#define SHAREFILEPAGE_H

#include <QWidget>
#include "envelopeencryptionmanager.h"

namespace Ui {
class ShareFilePage;
}

class ShareFilePage : public QWidget
{
    Q_OBJECT

public:
    explicit ShareFilePage(QWidget *parent = nullptr);
    void setFileData(const FileData &fileData, QString jwtToken);
    QString getPrivateKeyFilePath() const;
    QString getPrivateSignatureKeyFilePath() const;
    QString getPublicSignatureKeyFilePath() const;
    QByteArray decodeStoredPemBase64(const QString& storedBase64Pem);
    QByteArray decodePemBase64FromString(const QString& pemStrRaw);
    void reset();
    ~ShareFilePage();

private slots:
    void on_dashboardButton_clicked();

    void on_shareButton_clicked();

    void on_shareButton_2_clicked();

    void on_privKeyButton_clicked();

    void on_signPrivButton_clicked();

    void on_signPubButton_2_clicked();

    void on_privKeyButton_2_clicked();

private:
    Ui::ShareFilePage *ui;
    FileData file;
    QString authToken;
    QString privKeyFilePath;
    QString privSignatureKeyFilePath;
    QString pubSignatureKeyFilePath;


signals:
    void backToDashboard();
};

#endif // SHAREFILEPAGE_H

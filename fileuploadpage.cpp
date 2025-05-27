#include "fileuploadpage.h"
#include "ui_fileuploadpage.h"
#include "envelopeencryption.h"

#include <QMessageBox>
#include <sodium.h>
#include <QFileDialog>

FileUploadPage::FileUploadPage(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::FileUploadPage)
{
    ui->setupUi(this);
}

FileUploadPage::~FileUploadPage()
{
    delete ui;
}

QString FileUploadPage::getSelectedFilePath() const
{
    return selectedFilePath;
}


void FileUploadPage::on_uploadFileButton_clicked()
{
    QString filePath = QFileDialog::getOpenFileName(this, "Select File to Encrypt");

    if (filePath.isEmpty()){
        QMessageBox::warning(this, "error", "File path cannot be empty");
        return;
    }

    selectedFilePath = filePath;

    QFileInfo fileInfo(filePath);
    ui->uploadedFileLabel->setTextInteractionFlags(Qt::TextBrowserInteraction); // so its clickable
    ui->uploadedFileLabel->setOpenExternalLinks(true);
    ui->uploadedFileLabel->setText(QString("<a href=\"file:///%1\">%2</a>").arg(filePath, fileInfo.fileName()));

}


void FileUploadPage::on_encryptFileButton_clicked()
{
    QString password = ui->encryptionPasswordTextField->text();

    if (password.isEmpty()) {
        QMessageBox::warning(this, "Missing Password", "Password cannot be empty");
        return;
    }

    QString filePath = getSelectedFilePath();
    if (filePath.isEmpty()) {
        QMessageBox::warning(this, "Missing File", "File path cannot be empty");
        return;
    }

    // TODO: FUNCTION HERE TO CHECK IF PASSWORD MATCHES THE ONE SAVED IN THE DB.

    try {
        QByteArray passwordBytes = password.toUtf8();

        // Encrypt the file


        // Send to server or save locally - not implemented yet
        //bool success = saveEncryptionResultToServer(result);

        // if (success) {
        //     qDebug() << "File encrypted and uploaded successfully";
        //     // Update UI to show success
        // } else {
        //     qDebug() << "Failed to upload encrypted file";
        // }

        // Secure cleanup
        sodium_memzero(passwordBytes.data(), passwordBytes.size());

    } catch (const std::exception& e) {
        QMessageBox::critical(this, "Encryption Failed", e.what());
        return;
    }

    ui->encryptionPasswordTextField->clear();
    sodium_memzero(password.data(), password.size());
}

// bool FileUploadPage::saveEncryptionResultToServer(const EnvelopeEncryptionResult& result)
// {
//     // Implement your server communication here
//     // You'll need to send:
//     // - result.salt, result.timeCost, result.memoryCost
//     // - result.kekNonce, result.wrappedKEK
//     // - result.dekNonce, result.wrappedDEK
//     // - result.msgNonce, result.ciphertext

//     // Example pseudo-code:
//     // QNetworkRequest request(QUrl("https://your-server.com/api/upload-encrypted"));
//     // QJsonObject json;
//     // json["salt"] = QString(result.salt.toBase64());
//     // json["timeCost"] = static_cast<int>(result.timeCost);
//     // ... etc
//     // return sendPostRequest(request, json);

//     return true; // Placeholder
// }


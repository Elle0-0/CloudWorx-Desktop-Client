#include "fileuploadpage.h"
#include "ui_fileuploadpage.h"
#include "envelopeencryptionmanager.h"

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

        bool passwordVerification = EnvelopeEncryptionManager::verifyUserFilePassword(jwtToken, passwordBytes);

        if (!passwordVerification) {
            QMessageBox::critical(this, "Wrong Password", "password incorrect");
            return;
        }

        QString fileEncryptedSuccess = EnvelopeEncryptionManager::encryptAndStoreFile(userId, jwtToken, filePath, passwordBytes);

        QMessageBox::information(this, "Success", "File encrypted successfully");

        // Secure cleanup
        sodium_memzero(passwordBytes.data(), passwordBytes.size());

    } catch (const std::exception& e) {
        QMessageBox::critical(this, "Encryption Failed", e.what());
        return;
    }

    ui->encryptionPasswordTextField->clear();

}

void FileUploadPage::setIdAndToken(QString userId, QString jwtToken)
{
    this->jwtToken = jwtToken;
    this->userId = userId;
}


void FileUploadPage::on_dashboardButton_clicked()
{
    emit backToDashboard();
}


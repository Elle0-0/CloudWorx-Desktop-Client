#include "fileuploadpage.h"
#include "ui_fileuploadpage.h"
#include "argon2id_utils.h"

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


void FileUploadPage::on_uploadFileButton_clicked()
{
    QString filePath = QFileDialog::getOpenFileName(this, "Select File to Encrypt");

    ui->uploadedFileLabel->setTextInteractionFlags(Qt::TextBrowserInteraction);  // So its clickable
    ui->uploadedFileLabel->setOpenExternalLinks(true);  // opens file in file explorer
    ui->uploadedFileLabel->setText(QString("<a href=\"file:///%1\">%2</a>").arg(filePath, filePath));

}


void FileUploadPage::on_encryptFileButton_clicked()
{
    QString password = ui->encryptionPasswordTextField->text();
    QByteArray passwordBytes = password.toUtf8();

    QString passwordDecryptionKey = hashPassword(passwordBytes);

    // function call here for envelope encryption

    sodium_memzero(password.data(), password.size());
    sodium_memzero(passwordBytes.data(), passwordBytes.size());
}


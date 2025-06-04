#include "dashboard.h"
#include "ui_dashboard.h"
#include "models/filelistmodel.h"
#include "network/filesapi.h"


#include <QMessageBox>
#include <curl/curl.h>

Dashboard::Dashboard(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Dashboard)
    , model(new FileListModel(this))
{
    ui->setupUi(this);

    ui->tabWidget->setTabText(0, "All Files");
    ui->tabWidget->setTabText(1, "My files");
    ui->tabWidget->setTabText(2, "Shared");
    ui->tabWidget->setTabText(3, "Shared with me");

    ui->listView->setModel(model);

}


Dashboard::~Dashboard()
{
    delete ui;
}


void Dashboard::on_decryptFileButton_clicked()
{
    QModelIndex index = ui->listView->currentIndex();
    if (!index.isValid()) {
        QMessageBox::warning(this, "No File Selected", "Please select a file first.");
        return;
    }

    FileData file = model->getFile(index.row());

    QByteArray encryptedFile;
    QString error;
    if (!FilesApi::downloadEncryptedFileToMemory(jwtToken, file.fileName, encryptedFile, error)) {
        QMessageBox::critical(this, "Download Failed", error);
        return;
    }

    file.encryptedFile = encryptedFile;

    emit goToFileDecryption(file, jwtToken);
}




void Dashboard::on_shareFileButton_clicked()
{
    QModelIndex index = ui->listView->currentIndex();
    if (!index.isValid()) {
        QMessageBox::warning(this, "No File Selected", "Please select a file first.");
        return;
    }

    const FileData &file = model->getFile(index.row());

    emit goToFileShare(file.fileid);
}

void Dashboard::setIdAndToken(QString userId, QString jwtToken)
{
    this->jwtToken = jwtToken;
    this->userId = userId;
    loadFiles();
}

void Dashboard::loadFiles()
{
    QList<FileInfo> files;
    int count = 0;
    QString error;

    if (!FilesApi::getFiles(jwtToken, files, count, error)) {
        QMessageBox::critical(this, "Failed to Load Files", error);
        return;
    }

    model->clear();

    for (const FileInfo &info : files) {
        FileData data;
        data.fileName = info.file_name;
        data.fileType = QFileInfo(info.file_name).suffix();
        data.fileSize = 0; // You don’t know size yet (encrypted file not downloaded)
        data.ivFile = info.iv_file;
        data.ivDEK = info.dek_data.iv_dek;
        data.encryptedDEK = info.dek_data.encrypted_dek;
        data.assocDataFile = info.assoc_data_file;
        // Leave data.encryptedFile empty for now

        model->addFile(data);
    }
}



void Dashboard::on_uploadFileButton_clicked()
{
    emit goToFileUpload(userId, jwtToken);
}


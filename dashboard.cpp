#include "dashboard.h"
#include "ui_dashboard.h"
#include "models/filelistmodel.h"
#include "network/filesapi.h"
#include "network/sharesapi.h"


#include <QTabWidget>
#include <QMessageBox>
#include <curl/curl.h>

Dashboard::Dashboard(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Dashboard)
    , model(new FileListModel(this))
{
    ui->setupUi(this);

    connect(ui->tabWidget, &QTabWidget::currentChanged,
            this, &Dashboard::onTabChanged);

    ui->tabWidget->setTabText(0, "My Files");
    ui->tabWidget->setTabText(1, "Shared");

    ui->listView->setModel(model);

}


Dashboard::~Dashboard()
{
    delete ui;
}

void Dashboard::on_logOutButton_clicked()
{
    emit logOutRequested(); // Ask MainWindow to handle logout
}


void Dashboard::on_decryptFileButton_clicked()
{
    int activeTab = ui->tabWidget->currentIndex(); // 0 = My Files, 1 = Shared Files
    QModelIndex index;
    FileData file;

    if (activeTab == 0) {
        index = ui->listView->currentIndex(); // My Files
        if (!index.isValid()) {
            QMessageBox::warning(this, "No File Selected", "Please select a file to decrypt.");
            return;
        }

        file = model->getFile(index.row());

        QByteArray encryptedFile;
        QString error;
        if (!FilesApi::downloadEncryptedFileToMemory(jwtToken, file.fileName, encryptedFile, error)) {
            QMessageBox::critical(this, "Download Failed", error);
            return;
        }

        file.encryptedFile = encryptedFile;

        emit goToFileDecryption(file, jwtToken);
    }
    else if (activeTab == 1) {
        index = ui->listView_2->currentIndex(); // Shared Files
        if (!index.isValid()) {
            QMessageBox::warning(this, "No File Selected", "Please select a shared file to decrypt.");
            return;
        }

        FileData selectedRow = model->getFile(index.row()); // Assumes `fileId` is in there

        SharedFileData sharedFile;
        QString error;
        if (!SharesApi::downloadSharedFile(selectedRow.shareid, jwtToken, sharedFile, error)) {
            QMessageBox::critical(this, "Download Failed", error);
            return;
        }

        // Populate FileData with what's needed for decryption view
        file.fileName = sharedFile.fileName;
        file.encryptedFile = sharedFile.encryptedFile;
        file.sharedBy = sharedFile.sharedByUsername;  // Optional display
        file.nonce = sharedFile.nonce.toBase64();
        file.ephemeralPublicKey = sharedFile.ephemeralPublicKey.toBase64();
        file.senderSignature = sharedFile.senderSignature.toBase64();
        file.senderX25519PublicKeyPem = sharedFile.senderX25519PublicKeyPem;
        file.senderEd25519PublicKeyPem = sharedFile.senderEd25519PublicKeyPem;

        emit goToDecryptSharedFile(file, jwtToken);
    }
}





void Dashboard::on_shareFileButton_clicked()
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

    emit goToFileShare(file, jwtToken);
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
        data.fileid = info.file_id;
        data.fileName = info.file_name;
        data.fileType = QFileInfo(info.file_name).suffix();
        data.fileSize = 0; //(encrypted file not downloaded yet)
        data.ivFile = info.iv_file;
        data.ivDEK = info.dek_data.iv_dek;
        data.encryptedDEK = info.dek_data.encrypted_dek;
        data.assocDataFile = info.assoc_data_file;

        model->addFile(data);
    }
}

FileData convertSharedToFileData(const SharedFileModel& shared) {
    FileData file;
    file.fileid = shared.file_id;
    file.shareid = shared.share_id;
    file.fileName = shared.file_name;
    file.fileType = shared.file_type;
    file.fileSize = shared.file_size;

    file.ivFile = QString();
    file.ivDEK = QString();
    file.encryptedDEK = QString();
    file.assocDataFile = QString();

    qDebug() << "Converted:" << file.fileName;

    return file;
}

void Dashboard::loadSharedFiles()
{
    QList<SharedFileModel> sharedFiles;
    QString error;

    if (!SharesApi::getFilesSharedWithMe(sharedFiles, jwtToken, error)) {
        QMessageBox::critical(this, "Failed to Load Shared Files", error);
        return;
    }

    qDebug() << "Received shared files count:" << sharedFiles.size();

    model->clear();

    for (const SharedFileModel& shared : sharedFiles) {
        qDebug() << "File name:" << shared.file_name
                 << " | file_id:" << shared.file_id
                 << "file share id" << shared.share_id
                 << " | file_type:" << shared.file_type
                 << " | file_size:" << shared.file_size;

        ui->listView_2->setModel(model);

        model->addFile(convertSharedToFileData(shared));
    }
}



void Dashboard::on_uploadFileButton_clicked()
{
    emit goToFileUpload(userId, jwtToken);
}


void Dashboard::on_refreshButton_clicked()
{
    loadFiles();

}

void Dashboard::on_deleteFileButton_clicked()
{
    QModelIndex index = ui->listView->currentIndex();
    if (!index.isValid()) {
        QMessageBox::warning(this, "No File Selected", "Please select a file first.");
        return;
    }

    FileData file = model->getFile(index.row());

    // Optional: Ask user to confirm deletion
    if (QMessageBox::question(this, "Confirm Delete",
                              "Are you sure you want to delete \"" + file.fileName + "\"?") != QMessageBox::Yes) {
        return;
    }

    QString response;
    bool success = FilesApi::deleteFileByName(file.fileName,jwtToken, response);

    if (!success) {
        QMessageBox::critical(this, "Delete Failed", "Could not delete the file.\n" + response);
        return;
    }

    // Remove from model and show success
    model->removeFile(index.row());
    QMessageBox::information(this, "Success", "File deleted successfully.");
}

void Dashboard::onTabChanged(int index)
{
    qDebug() << "Tab changed to index:" << index;
    if (index == 0) {
        loadFiles();
    } else if (index == 1) {
        qDebug() << "Loading shared files...";
        loadSharedFiles();
    }
}

void Dashboard::reset()
{
    // Clear list model
    model->clear();

    // Reset views (deselect any selection)
    ui->listView->clearSelection();
    ui->listView_2->clearSelection();

    ui->tabWidget->setCurrentIndex(0);

    // Clear session data
    jwtToken.clear();
    userId.clear();
}






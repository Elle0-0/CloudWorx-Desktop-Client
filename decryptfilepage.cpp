#include "decryptfilepage.h"
#include "ui_decryptfilepage.h"

#include "envelopeencryptionmanager.h"
#include "network/filesapi.h"
#include <QFileDialog>
#include <QMessageBox>

DecryptFilePage::DecryptFilePage(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::DecryptFilePage)
{
    ui->setupUi(this);
}

DecryptFilePage::~DecryptFilePage()
{
    delete ui;
}
void DecryptFilePage::setFileData(const FileData &fileData, QString jwtToken)
{
    this->file = fileData;
    this->authToken = jwtToken;

    qDebug() << "file recieved";
}

void DecryptFilePage::on_dashboardButton_clicked()
{
    emit backToDashboard();
}

void DecryptFilePage::on_pushButton_clicked()
{
    // 1. Prompt user for output path
    QString outputPath = QFileDialog::getSaveFileName(this, "Save Decrypted File", file.fileName);

    if (outputPath.isEmpty()) {
        QMessageBox::warning(this, "Cancelled", "You must choose a location to save the decrypted file.");
        return;
    }

    // 2. Get password
    QByteArray password = ui->encryptionPasswordTextField->text().toUtf8();

    // 3. Download encrypted file contents
    QByteArray encryptedData;
    QString error;

    if (!FilesApi::downloadEncryptedFileToMemory(authToken, file.fileName, encryptedData, error)) {
        QMessageBox::critical(this, "Download Failed", "Failed to download encrypted file: " + error);
        return;
    }

    // 4. Build FileData object for decryption
    FileData fileData;
    fileData.ivDEK = file.ivDEK;             // assuming these getters exist
    fileData.ivFile = file.ivFile;
    fileData.encryptedDEK = file.encryptedDEK;
    fileData.encryptedFile = encryptedData;


    try {
        // 5. Decrypt it
        QByteArray decryptedData = EnvelopeEncryptionManager::decryptAndSaveFile(fileData, authToken, password, outputPath);

        if (decryptedData.isEmpty()) {
            QMessageBox::critical(this, "Decryption Failed", "Decryption returned no data.");
            return;
        }

        // 6. Save to file
        QFile fileOut(outputPath);
        if (fileOut.open(QIODevice::WriteOnly)) {
            fileOut.write(decryptedData);
            fileOut.close();
            QMessageBox::information(this, "Success", "File decrypted and saved successfully.");
        } else {
            QMessageBox::critical(this, "Save Failed", "Could not write decrypted data to file.");
        }

    } catch (const std::exception& e) {
        QMessageBox::critical(this, "Decryption Failed", e.what());
    }
}

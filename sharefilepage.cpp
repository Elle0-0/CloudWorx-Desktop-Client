#include "sharefilepage.h"
#include "ui_sharefilepage.h"

#include "envelopeencryptionmanager.h"
#include "network/filesapi.h"
#include "network/sharesapi.h"
#include "cryptography/hybridencryptionmanager.h"
#include <QFileDialog>
#include <QStandardPaths>
#include <QMessageBox>

ShareFilePage::ShareFilePage(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ShareFilePage)
{
    ui->setupUi(this);
}

ShareFilePage::~ShareFilePage()
{
    delete ui;
}

void ShareFilePage::setFileData(const FileData &fileData, QString jwtToken) {
    this->file = fileData;
    this->authToken = jwtToken;

    qDebug() << "file recieved";
}

void ShareFilePage::on_dashboardButton_clicked()
{
    emit backToDashboard();
}


void ShareFilePage::on_shareButton_clicked()
{
    // 1. Prepare temporary output file path
    QString outputPath = QStandardPaths::writableLocation(QStandardPaths::TempLocation) + "/" + file.fileName;

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
    fileData.ivDEK = file.ivDEK;
    fileData.ivFile = file.ivFile;
    fileData.encryptedDEK = file.encryptedDEK;
    fileData.encryptedFile = encryptedData;

    try {
        // 5. Decrypt the file
        QByteArray decryptedData = EnvelopeEncryptionManager::decryptAndSaveFile(fileData, authToken, password, outputPath); // custom or adjusted method

        if (decryptedData.isEmpty()) {
            QMessageBox::critical(this, "Decryption Failed", "Decryption returned no data.");
            return;
        }

        // 6. Save decrypted file to temporary disk location
        QFile fileOut(outputPath);
        if (!fileOut.open(QIODevice::WriteOnly)) {
            QMessageBox::critical(this, "Save Failed", "Could not write decrypted data to file.");
            return;
        }
        fileOut.write(decryptedData);
        fileOut.close();

        // 7. Get sender's keys
        QByteArray senderX25519PrivKey   = decodeStoredPemBase64(getPrivateKeyFilePath());
        QByteArray senderEd25519PrivKey  = decodeStoredPemBase64(getPrivateSignatureKeyFilePath());
        QByteArray senderEd25519PubKey   = decodeStoredPemBase64(getPublicSignatureKeyFilePath());

        // 8. Get recipient username and fetch their public keys
        QString username = ui->userTextField->text();
        QString response;
        if (!SharesApi::getPublicKeysForUser(username, authToken, response, error)) {
            QMessageBox::critical(this, "Public Key Error", "Failed to get public keys: " + error);
            return;
        }

        // 9. Parse recipient public keys from JSON
        QJsonParseError parseError;
        QJsonDocument jsonDoc = QJsonDocument::fromJson(response.toUtf8(), &parseError);
        if (parseError.error != QJsonParseError::NoError || !jsonDoc.isObject()) {
            QMessageBox::critical(this, "JSON Error", "Failed to parse public key response: " + parseError.errorString());
            return;
        }

        QJsonObject jsonObj = jsonDoc.object();
        QByteArray recipientX25519PubKey = decodeStoredPemBase64(jsonObj.value("x25519PublicKey").toString());
        QByteArray recipientEd25519PubKey = decodeStoredPemBase64(jsonObj.value("ed25519PublicKey").toString());


        if (recipientX25519PubKey.isEmpty() || recipientEd25519PubKey.isEmpty()) {
            QMessageBox::critical(this, "Missing Key", "Recipient public keys are missing in the response.");
            return;
        }


        // 10. Encrypt and sign the file for sharing
        auto optionalPayload = HybridEncryptionManager::encryptAndSign(
            outputPath,
            recipientX25519PubKey,
            senderX25519PrivKey,
            senderEd25519PrivKey,
            senderEd25519PubKey
            );

        if (!optionalPayload.has_value()) {
            QMessageBox::critical(this, "Encryption Failed", "File encryption and signing failed.");
            return;
        }

        EncryptedPayload payload = optionalPayload.value(); // Now safe


        // 11. Share the encrypted file
        if (!SharesApi::shareEncryptedFile(
                fileData.fileid,
                authToken,
                username,
                payload.ciphertext,
                payload.nonce,
                payload.ephemeralPublicKey,
                payload.senderSigningPublicKey,
                response,
                error)) {
            QMessageBox::critical(this, "Share Failed", "Failed to share file: " + error);
            return;
        }

        QMessageBox::information(this, "Success", "Encrypted file successfully shared!");
        QFile::remove(outputPath);


    } catch (const std::exception& e) {
        QMessageBox::critical(this, "Error", e.what());
    }
}


QString ShareFilePage::getPrivateKeyFilePath() const
{
    return privKeyFilePath;
}

QString ShareFilePage::getPrivateSignatureKeyFilePath() const
{
    return privSignatureKeyFilePath;
}
QString ShareFilePage::getPublicSignatureKeyFilePath() const
{
    return pubSignatureKeyFilePath;
}



void ShareFilePage::on_privKeyButton_clicked()
{

}

void ShareFilePage::on_shareButton_2_clicked() {

}


void ShareFilePage::on_signPrivButton_clicked()
{
    QString filePath = QFileDialog::getOpenFileName(this, "Select File to Encrypt");

    if (filePath.isEmpty()){
        QMessageBox::warning(this, "error", "File path cannot be empty");
        return;
    }

    privSignatureKeyFilePath = filePath;

    QFileInfo fileInfo(filePath);
    ui->signKey->setTextInteractionFlags(Qt::TextBrowserInteraction); // so its clickable
    ui->signKey->setOpenExternalLinks(true);
    ui->signKey->setText(QString("<a href=\"file:///%1\">%2</a>").arg(filePath, fileInfo.fileName()));
}


void ShareFilePage::on_signPubButton_2_clicked()
{
    QString filePath = QFileDialog::getOpenFileName(this, "Select File to Encrypt");

    if (filePath.isEmpty()){
        QMessageBox::warning(this, "error", "File path cannot be empty");
        return;
    }

    pubSignatureKeyFilePath = filePath;

    QFileInfo fileInfo(filePath);
    ui->signPubKey->setTextInteractionFlags(Qt::TextBrowserInteraction); // so its clickable
    ui->signPubKey->setOpenExternalLinks(true);
    ui->signPubKey->setText(QString("<a href=\"file:///%1\">%2</a>").arg(filePath, fileInfo.fileName()));
}

QByteArray ShareFilePage::decodeStoredPemBase64(const QString& filePath) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Failed to open key file:" << filePath;
        return {};
    }

    QString pemStr = QString::fromUtf8(file.readAll()).trimmed();
    file.close();

    // Extract base64 payload (skip header/footer lines)
    QStringList lines = pemStr.split('\n', Qt::SkipEmptyParts);
    QString base64Body;
    for (const QString& line : lines) {
        if (line.startsWith("-----")) continue;  // skip PEM headers
        base64Body += line.trimmed();
    }

    // Decode the base64-encoded payload
    return QByteArray::fromBase64(base64Body.toUtf8());
}




void ShareFilePage::on_privKeyButton_2_clicked()
{
    QString filePath = QFileDialog::getOpenFileName(this, "Select File to Encrypt");

    if (filePath.isEmpty()){
        QMessageBox::warning(this, "error", "File path cannot be empty");
        return;
    }

    privKeyFilePath = filePath;

    QFileInfo fileInfo(filePath);
    ui->privKey->setTextInteractionFlags(Qt::TextBrowserInteraction); // so its clickable
    ui->privKey->setOpenExternalLinks(true);
    ui->privKey->setText(QString("<a href=\"file:///%1\">%2</a>").arg(filePath, fileInfo.fileName()));

}


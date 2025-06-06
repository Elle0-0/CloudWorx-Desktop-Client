#include "decryptsharedpage.h"
#include "cryptography/hybridencryptionmanager.h"
#include "ui_decryptsharedpage.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QByteArray>


DecryptSharedPage::DecryptSharedPage(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::DecryptSharedPage)
{
    ui->setupUi(this);
}

DecryptSharedPage::~DecryptSharedPage()
{
    delete ui;
}

void DecryptSharedPage::setFileData(const FileData &fileData, QString jwtToken)
{
    this->fileData = fileData;
    this->authToken = jwtToken;

    qDebug() << "file recieved";
}

void DecryptSharedPage::on_uploadKeyButton_clicked()
{
    QString filePath = QFileDialog::getOpenFileName(this, "Select Private Key File");

    if (filePath.isEmpty()){
        QMessageBox::warning(this, "error", "File path cannot be empty");
        return;
    }

    privSignatureKeyFilePath = filePath;

    QFileInfo fileInfo(filePath);
    ui->filePath->setTextInteractionFlags(Qt::TextBrowserInteraction); // so its clickable
    ui->filePath->setOpenExternalLinks(true);
    ui->filePath->setText(QString("<a href=\"file:///%1\">%2</a>").arg(filePath, fileInfo.fileName()));
}
void DecryptSharedPage::setFileData(const FileData &file)
{
    this->fileData = file;
}

#include <QDebug>  // Make sure this is included at the top of your file

void DecryptSharedPage::on_decryptButton_clicked()
{
    qDebug() << "Starting decryption process";

    qDebug() << "Raw ciphertext size:" << fileData.encryptedFile.size();
    qDebug() << "Sender Signature (base64):" << fileData.senderSignature;
    qDebug() << "Nonce (base64):" << fileData.nonce;
    qDebug() << "Ephemeral Key (base64):" << fileData.ephemeralPublicKey;
    qDebug() << "Sender X25519 PubKey (base64):" << fileData.senderX25519PublicKeyPem;
    qDebug() << "Sender Ed25519 PubKey PEM:\n" << fileData.senderEd25519PublicKeyPem;



    EncryptedPayload payload;
    // payload.ciphertext = fileData.encryptedFile;
    // payload.nonce = decodeBase64(fileData.nonce);
    // payload.ephemeralPublicKey = decodeBase64(fileData.ephemeralPublicKey);
    // payload.signature = decodeBase64(fileData.senderSignature);

    payload.ciphertext = fileData.encryptedFile;  // already raw bytes
    payload.nonce = QByteArray::fromBase64(fileData.nonce.toUtf8());
    payload.ephemeralPublicKey = QByteArray::fromBase64(fileData.ephemeralPublicKey.toUtf8());
    payload.signature = QByteArray::fromBase64(fileData.senderSignature.toUtf8());


    QByteArray senderX25519Raw = extractRawFromPEM(fileData.senderX25519PublicKeyPem);
    QByteArray senderEd25519Raw = extractRawFromPEM(fileData.senderEd25519PublicKeyPem);


    qDebug() << "Decoded ciphertext size:" << payload.ciphertext.size();
    qDebug() << "Decoded nonce size:" << payload.nonce.size();
    qDebug() << "Decoded ephemeral public key size:" << payload.ephemeralPublicKey.size();
    qDebug() << "Decoded signature size:" << payload.signature.size();

    QByteArray senderSigningRaw = extractRawFromPEM(fileData.senderEd25519PublicKeyPem);
    if (senderSigningRaw.isEmpty()) {
        QMessageBox::critical(this, "Error", "Invalid PEM format in senderEd25519PublicKeyPem.");
        qDebug() << "Failed to extract sender signing key from PEM";
        return;
    }
    qDebug() << "Extracted sender signing public key raw bytes size:" << senderSigningRaw.size();
    payload.senderSigningPublicKey = senderSigningRaw;

    QFile privKeyFile(privSignatureKeyFilePath);
    if (!privKeyFile.open(QIODevice::ReadOnly)) {
        QMessageBox::critical(this, "Error", "Failed to open private signature key file.");
        qDebug() << "Failed to open private key file at path:" << privSignatureKeyFilePath;
        return;
    }
    qDebug() << "Opened private key file successfully";

    QByteArray encodedPem = privKeyFile.readAll();
    qDebug() << "Read private key file size (encoded):" << encodedPem.size();

    QByteArray pemText = QByteArray::fromBase64(encodedPem);
    QString pemStr = QString::fromUtf8(encodedPem);  // directly

    QByteArray privKeyRaw = extractRawFromPEM(pemStr);
    if (privKeyRaw.isEmpty()) {
        QMessageBox::critical(this, "Error", "Invalid PEM format in private key.");
        qDebug() << "Failed to extract private key raw bytes from PEM";
        return;
    }
    qDebug() << "Extracted private key raw bytes size:" << privKeyRaw.size();

    bool signatureValid = true;
    qDebug() << "Starting decryption and signature verification";
    auto decrypted = HybridEncryptionManager::decryptAndVerify(
        payload,
        privKeyRaw,
        senderX25519Raw,
        senderEd25519Raw,
        signatureValid
        );

    qDebug() << "Decryption result pointer:" << decrypted;
    qDebug() << "Signature valid?" << signatureValid;

    if (!signatureValid) {
        QMessageBox::warning(this, "Signature Invalid", "The signature could not be verified.");
        qDebug() << "Signature verification failed";
        return;
    }

    if (!decrypted) {
        QMessageBox::critical(this, "Decryption Failed", "Unable to decrypt file contents.");
        qDebug() << "Decryption failed: no decrypted data";
        return;
    }

    QString outputPath = QFileDialog::getSaveFileName(this, "Save Decrypted File", fileData.fileName);
    qDebug() << "User selected output file path:" << outputPath;
    if (!outputPath.isEmpty()) {
        QFile outFile(outputPath);
        if (outFile.open(QIODevice::WriteOnly)) {
            outFile.write(*decrypted);
            outFile.close();
            QMessageBox::information(this, "Success", "File decrypted and saved successfully.");
            qDebug() << "File saved successfully";
        } else {
            QMessageBox::critical(this, "File Error", "Could not save decrypted file.");
            qDebug() << "Failed to open output file for writing:" << outputPath;
        }
    } else {
        qDebug() << "Save file dialog canceled by user";
    }

    qDebug() << "Decryption process finished";
}



QByteArray DecryptSharedPage::extractRawFromPEM(const QString &pemStr) {
    QRegularExpression re("-----BEGIN .*?-----\\s*(.*?)\\s*-----END .*?-----", QRegularExpression::DotMatchesEverythingOption);
    QRegularExpressionMatch match = re.match(pemStr);
    if (match.hasMatch()) {
        QString base64Block = match.captured(1).remove(QRegularExpression("\\s"));
        return QByteArray::fromBase64(base64Block.toUtf8());
    }
    return QByteArray();  // empty if invalid
}



void DecryptSharedPage::on_dashboardButton_clicked()
{
    emit backToDashboard();
}


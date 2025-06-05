#include "keygenpage.h"
#include "ui_keygenpage.h"

#include <QFileDialog>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>

KeyGenPage::KeyGenPage(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::KeyGenPage)
{
    ui->setupUi(this);
}

KeyGenPage::~KeyGenPage()
{
    delete ui;
}

void KeyGenPage::on_contiButton_clicked()
{
    emit goToLogin();
}

void KeyGenPage::setPubAndPriv(QString pub, QString priv, QString signPub, QString signPriv) {
    this->publicKey = pub;
    this->privateKey = priv;
    this->signPublicKey = signPub;
    this->signPrivateKey = signPriv;

    ui->publickey->setText(publicKey);
    ui->privatekey->setText(privateKey);
    ui->signpub->setText(signPublicKey);
    ui->signpriv->setText(signPrivateKey);
}


void KeyGenPage::on_downloadPubButton_clicked()
{
    QString fileName = QFileDialog::getSaveFileName(this,
                                                    tr("Save Public Key"), "public_key.txt", tr("All Files (*)"));

    if (!fileName.isEmpty()) {
        QFile file(fileName);
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream out(&file);
            out << publicKey;
            file.close();
        } else {
            QMessageBox::warning(this, tr("Error"), tr("Could not save the public key file."));
        }
    }
}



void KeyGenPage::on_downloadPrivButton_clicked()
{
    QString fileName = QFileDialog::getSaveFileName(this,
                                                    tr("Save Private Key"), "private_key.txt", tr("All Files (*)"));

    if (!fileName.isEmpty()) {
        QFile file(fileName);
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream out(&file);
            out << publicKey;
            file.close();
        } else {
            QMessageBox::warning(this, tr("Error"), tr("Could not save the private key file."));
        }
    }
}


void KeyGenPage::on_downloadSignPub_clicked()
{
    QString fileName = QFileDialog::getSaveFileName(this,
                                                    tr("Save Public Signature Key"), "signature_public_key.txt", tr("All Files (*)"));

    if (!fileName.isEmpty()) {
        QFile file(fileName);
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream out(&file);
            out << signPublicKey;
            file.close();
        } else {
            QMessageBox::warning(this, tr("Error"), tr("Could not save the private key file."));
        }
    }
}


void KeyGenPage::on_downloadSignPriv_clicked()
{
    QString fileName = QFileDialog::getSaveFileName(this,
                                                    tr("Save Private Signature Key"), "signature_private_key.txt", tr("All Files (*)"));

    if (!fileName.isEmpty()) {
        QFile file(fileName);
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream out(&file);
            out << signPrivateKey;
            file.close();
        } else {
            QMessageBox::warning(this, tr("Error"), tr("Could not save the private key file."));
        }
    }
}


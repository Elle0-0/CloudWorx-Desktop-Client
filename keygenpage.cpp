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

void KeyGenPage::setPubAndPriv(QString pub, QString priv) {
    this->publicKey = pub;
    this->privateKey = priv;

    ui->publickey->setText(publicKey);
    ui->privatekey->setText(privateKey);
}


void KeyGenPage::on_downloadPubButton_clicked()
{
    QString fileName = QFileDialog::getSaveFileName(this,
                                                    tr("Save Public Key"), "public_key.pem", tr("PEM Files (*.pem);;All Files (*)"));

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
                                                    tr("Save Private Key"), "private_key.pem", tr("PEM Files (*.pem);;All Files (*)"));

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


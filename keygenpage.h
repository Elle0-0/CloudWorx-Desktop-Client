#ifndef KEYGENPAGE_H
#define KEYGENPAGE_H

#include <QWidget>

namespace Ui {
class KeyGenPage;
}

class KeyGenPage : public QWidget
{
    Q_OBJECT

public:
    explicit KeyGenPage(QWidget *parent = nullptr);
    void setPubAndPriv(QString pub, QString priv, QString signPub, QString signPriv);
    ~KeyGenPage();

private slots:
    void on_contiButton_clicked();

    void on_downloadPubButton_clicked();

    void on_downloadPrivButton_clicked();

    void on_downloadSignPub_clicked();

    void on_downloadSignPriv_clicked();

private:
    Ui::KeyGenPage *ui;
    QString publicKey;
    QString privateKey;
    QString signPublicKey;
    QString signPrivateKey;

signals:
    void goToLogin();
};

#endif // KEYGENPAGE_H

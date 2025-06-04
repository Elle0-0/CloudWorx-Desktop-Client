#ifndef DECRYPTFILEPAGE_H
#define DECRYPTFILEPAGE_H


#include <QWidget>

#include "envelopeencryptionmanager.h"

namespace Ui {
class DecryptFilePage;
}

class DecryptFilePage : public QWidget
{
    Q_OBJECT

public:
    explicit DecryptFilePage(QWidget *parent = nullptr);
    ~DecryptFilePage();
    void setFileData(const FileData &fileData, QString jwtToken);
    void setToken(QString token);

private slots:
    void on_dashboardButton_clicked();

    void on_pushButton_clicked();

private:
    Ui::DecryptFilePage *ui;
    FileData file;
    QString authToken;

signals:
    void backToDashboard();
};

#endif // DECRYPTFILEPAGE_H

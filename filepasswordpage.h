#ifndef FILEPASSWORDPAGE_H
#define FILEPASSWORDPAGE_H

#include <QWidget>

namespace Ui {
class FilePasswordPage;
}

class FilePasswordPage : public QWidget
{
    Q_OBJECT

public:
    explicit FilePasswordPage(QWidget *parent = nullptr);
    void setUserData(const QString &username, const QString &email, const QString &hashedPassword);
    ~FilePasswordPage();

private slots:
    void on_createPasswordButton_clicked();

private:
    Ui::FilePasswordPage *ui;
    QString username;
    QString email;
    QString hashedPassword;
};

#endif // FILEPASSWORDPAGE_H

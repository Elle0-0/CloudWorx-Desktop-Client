#ifndef SIGNUPPAGE_H
#define SIGNUPPAGE_H

#include <QWidget>

namespace Ui {
class SignUpPage;
}

class SignUpPage : public QWidget
{
    Q_OBJECT

public:
    explicit SignUpPage(QWidget *parent = nullptr);
    void reset();
    ~SignUpPage();

private:
    Ui::SignUpPage *ui;

signals:
    void backToMenu();
    void signUpSuccessful(const QString &username, const QString &email, const QString &hashedPassword);

private slots:
    void on_backButton_clicked();
    void on_loginButton_clicked();
    void on_signUpButton_clicked();
};

#endif // SIGNUPPAGE_H

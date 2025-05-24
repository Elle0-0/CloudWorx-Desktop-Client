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
    ~SignUpPage();

private:
    Ui::SignUpPage *ui;

signals:
    void backToMenu();

private slots:
    void on_backButton_clicked();
    void on_loginButton_clicked();
    void on_signUpButton_clicked();
};

#endif // SIGNUPPAGE_H

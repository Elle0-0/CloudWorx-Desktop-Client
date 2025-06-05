#ifndef ACCOUNTSETTINGSPAGE_H
#define ACCOUNTSETTINGSPAGE_H

#include <QWidget>

namespace Ui {
class AccountSettingsPage;
}

class AccountSettingsPage : public QWidget
{
    Q_OBJECT

public:
    explicit AccountSettingsPage(QWidget *parent = nullptr);
    ~AccountSettingsPage();

private:
    Ui::AccountSettingsPage *ui;
};

#endif // ACCOUNTSETTINGSPAGE_H

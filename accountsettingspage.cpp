#include "accountsettingspage.h"
#include "ui_accountsettingspage.h"

AccountSettingsPage::AccountSettingsPage(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::AccountSettingsPage)
{
    ui->setupUi(this);
}

AccountSettingsPage::~AccountSettingsPage()
{
    delete ui;
}

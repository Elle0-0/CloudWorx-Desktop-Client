#include "decryptfilepage.h"
#include "ui_decryptfilepage.h"

DecryptFilePage::DecryptFilePage(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::DecryptFilePage)
{
    ui->setupUi(this);
}

DecryptFilePage::~DecryptFilePage()
{
    delete ui;
}
void DecryptFilePage::setFileData(const FileModel &fileData)
{
    this->file = fileData;

    qDebug() << "file recieved";
}

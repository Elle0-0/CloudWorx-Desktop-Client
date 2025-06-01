#include "sharefilepage.h"
#include "ui_sharefilepage.h"

ShareFilePage::ShareFilePage(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ShareFilePage)
{
    ui->setupUi(this);
}

ShareFilePage::~ShareFilePage()
{
    delete ui;
}

void ShareFilePage::setFileId(const QString file_id) {
    this->file_id = file_id;
}

#include "fileuploadpage.h"
#include "ui_fileuploadpage.h"

FileUploadPage::FileUploadPage(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::FileUploadPage)
{
    ui->setupUi(this);
}

FileUploadPage::~FileUploadPage()
{
    delete ui;
}

#include "decryptsharedpage.h"
#include "ui_decryptsharedpage.h"

#include <QFileDialog>
#include <QMessageBox>

DecryptSharedPage::DecryptSharedPage(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::DecryptSharedPage)
{
    ui->setupUi(this);
}

DecryptSharedPage::~DecryptSharedPage()
{
    delete ui;
}

void DecryptSharedPage::on_uploadKeyButton_clicked()
{
    QString filePath = QFileDialog::getOpenFileName(this, "Select Private Key File");

    if (filePath.isEmpty()){
        QMessageBox::warning(this, "error", "File path cannot be empty");
        return;
    }

    privSignatureKeyFilePath = filePath;

    QFileInfo fileInfo(filePath);
    ui->filePath->setTextInteractionFlags(Qt::TextBrowserInteraction); // so its clickable
    ui->filePath->setOpenExternalLinks(true);
    ui->filePath->setText(QString("<a href=\"file:///%1\">%2</a>").arg(filePath, fileInfo.fileName()));
}
void DecryptSharedPage::setFileData(const FileData &file)
{
    this->fileData = file;
}



void DecryptSharedPage::on_decryptButton_clicked()
{

}


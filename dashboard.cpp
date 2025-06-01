#include "dashboard.h"
#include "ui_dashboard.h"
#include "models/filelistmodel.h"

#include <QMessageBox>
#include <curl/curl.h>

Dashboard::Dashboard(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Dashboard)
    , model(new FileListModel(this))
{
    ui->setupUi(this);

    ui->tabWidget->setTabText(0, "All Files");
    ui->tabWidget->setTabText(1, "My files");
    ui->tabWidget->setTabText(2, "Shared");
    ui->tabWidget->setTabText(3, "Shared with me");

    model->addFile(FileModel("uuid1", "Report.pdf", "PDF", 2048));
    model->addFile(FileModel("uuid2", "Photo.jpg", "Image", 5120));
    model->addFile(FileModel("uuid3", "Notes.txt", "Text", 128));

    ui->listView->setModel(model);

}
void Dashboard::on_decryptFileButton_clicked()
{
    QModelIndex index = ui->listView->currentIndex();
    if (!index.isValid()) {
        QMessageBox::warning(this, "No File Selected", "Please select a file first.");
        return;
    }

    const FileModel &file = model->getFile(index.row());

    emit goToFileDecryption(file);
}


Dashboard::~Dashboard()
{
    delete ui;
}



void Dashboard::on_shareFileButton_clicked()
{
    QModelIndex index = ui->listView->currentIndex();
    if (!index.isValid()) {
        QMessageBox::warning(this, "No File Selected", "Please select a file first.");
        return;
    }

    const FileModel &file = model->getFile(index.row());

    emit goToFileShare(file.file_id);
}


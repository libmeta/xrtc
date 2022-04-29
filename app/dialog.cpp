#include "dialog.hpp"
#include "./ui_dialog.h"


#include "xrtc.hpp"

Dialog::Dialog(QWidget* parent)
    : QDialog(parent)
    , ui(new Ui::Dialog)
{
    ui->setupUi(this);
    XRTC();
}

Dialog::~Dialog()
{
    delete ui;
}

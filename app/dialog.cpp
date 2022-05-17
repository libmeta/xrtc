#include "dialog.hpp"
#include "./ui_dialog.h"


#include "xrtc.hpp"

#include "srt_net/test_srt_net.hpp"


Dialog::Dialog(QWidget* parent)
    : QDialog(parent)
    , ui(new Ui::Dialog)
{
    ui->setupUi(this);
    this->showMaximized();
    XRTC();
//    test_srt_net_main();
    auto f = fopen("/sdcard/sdk.log","w");
    fwrite("1111",4,1,f);
    fclose(f);
}

Dialog::~Dialog()
{
    delete ui;
}

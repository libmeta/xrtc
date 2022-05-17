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
    test_srt_net_thread = std::thread([this]{
        xrtc::test_srt_net_main();
    });

    xrtc::XRTC();
}

Dialog::~Dialog()
{
    if(test_srt_net_thread.joinable()) {
        test_srt_net_thread.join();
    }

    delete ui;
}

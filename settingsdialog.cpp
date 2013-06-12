#include "settingsdialog.h"
#include "ui_settingsdialog.h"

Settings::Settings()
{
    //initial settings
    aspect_ratio = true;
    recursive = false;
    format = 0;
    quality = 85;
    height = 768;
    width = 1024;
}

Settings::Settings(bool _aspect_ratio, bool _recursive, int _format, int _quality, int _height, int _width)
{
    aspect_ratio = _aspect_ratio;
    recursive = _recursive;
    format = _format;
    quality = _quality;
    height = _height;
    width = _width;
}

SettingsDialog::SettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);

    connect(this,SIGNAL(accepted()),this,SLOT(saveSettings()));
}

SettingsDialog::~SettingsDialog()
{
    delete ui;
}

void SettingsDialog::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void SettingsDialog::setSettings(Settings _s)
{
    ui->checkBox_Aspect_Ratio->setChecked(_s.aspect_ratio);
    ui->checkBox_Recursive->setChecked(_s.recursive);
    ui->comboBox_Format->setCurrentIndex(_s.format);
    ui->spinBox_Height->setValue(_s.height);
    ui->spinBox_Width->setValue(_s.width);
    ui->spinBox_Quality->setValue(_s.quality);
}

void SettingsDialog::saveSettings()
{
    Settings s(ui->checkBox_Aspect_Ratio->isChecked(),ui->checkBox_Recursive->isChecked(),ui->comboBox_Format->currentIndex(),ui->spinBox_Quality->value(),ui->spinBox_Height->value(),ui->spinBox_Width->value());

    emit settingsChanged(s);
}

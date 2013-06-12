#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>

namespace Ui {
    class SettingsDialog;
}

struct Settings
{
    bool aspect_ratio;
    bool overwrite;
    bool recursive;
    int format;
    int quality;
    int height;
    int width;

    Settings();
    Settings(bool _aspect_ratio, bool _recursive, int _format, int _quality, int _height, int _width);
};

class SettingsDialog : public QDialog {
    Q_OBJECT
public:
    SettingsDialog(QWidget *parent = 0);
    ~SettingsDialog();

public slots:
    void setSettings(Settings _s);

signals:
    void settingsChanged(Settings _s);

protected:
    void changeEvent(QEvent *e);

private:
    Ui::SettingsDialog *ui;

private slots:

    void saveSettings();
};

#endif // SETTINGSDIALOG_H

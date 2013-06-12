#include "mainwindow.h"
#include "ui_mainwindow.h"

Settings MainWindow::s;

QString MainWindow::output_path;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    QTextCodec *codec = QTextCodec::codecForName("Windows-1251");
    QTextCodec::setCodecForTr(codec);

    ui->setupUi(this);

    QFile file("settings.dat");

    if(file.open(QIODevice::ReadOnly))
    {
        //read settings from file
        QDataStream stream(&file);

        stream>>s.aspect_ratio>>s.overwrite>>s.recursive>>s.format>>s.height>>s.width>>s.quality;

        file.close();
    }
}

MainWindow::~MainWindow()
{
    delete ui;

    QFile file("settings.dat");

    if(file.open(QIODevice::WriteOnly))
    {
        //write settings to file
        QDataStream stream(&file);

        stream<<s.aspect_ratio<<s.overwrite<<s.recursive<<s.format<<s.height<<s.width<<s.quality;

        file.close();
    }
}

void MainWindow::changeEvent(QEvent *e)
{
    QMainWindow::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void MainWindow::dragEnterEvent(QDragEnterEvent* event)
{
    if (event->mimeData()->hasUrls())
    {
        event->acceptProposedAction();
    }
}

void MainWindow::dropEvent(QDropEvent* event)
{
    QList<QUrl> filenames = event->mimeData()->urls();

    foreach(QUrl f, filenames)
    {
        QString filename = f.toLocalFile();

        if(filename.endsWith(".jpg",Qt::CaseInsensitive) ||
                filename.endsWith(".jpeg",Qt::CaseInsensitive) ||
                filename.endsWith(".png",Qt::CaseInsensitive) ||
                filename.endsWith(".gif",Qt::CaseInsensitive) ||
                filename.endsWith(".tif",Qt::CaseInsensitive) ||
                filename.endsWith(".tiff",Qt::CaseInsensitive) ||
                filename.endsWith(".bmp",Qt::CaseInsensitive))
        {
            if(ui->listWidget->findItems(filename,Qt::MatchExactly).isEmpty())
            {
                ui->listWidget->addItem(filename);
                event->acceptProposedAction();
            }
            else
            {
                event->ignore();
            }

        }
        else
        {
            event->ignore();
        }
    }
}

void MainWindow::on_pushButton_AddFiles_clicked()
{
    QStringList files = QFileDialog::getOpenFileNames(this,tr("Відкрити"),QDir::homePath(),tr("Зображення (*.bmp *.png *.jpg)"));

    if(!files.isEmpty())
    {
        ui->listWidget->addItems(files);
    }
}

void MainWindow::on_pushButton_AddDir_clicked()
{
    QString dir_path = QFileDialog::getExistingDirectory(this,tr("Відкрити"),QDir::homePath());

    if(!dir_path.isNull())
    {
        this->addDirectory(dir_path);
    }
}

void MainWindow::addDirectory(QString directory)
{
    QDir dir(directory);
    QStringList ls;

    //TODO: should do it multithreading
    if(s.recursive)
    {
        ls = dir.entryList(QDir::AllDirs | QDir::NoDotAndDotDot);

        for(int i = 0; i < ls.length(); ++i)
        {
            addDirectory(dir.absoluteFilePath(ls[i]));
        }
    }

    QStringList file_types;

    file_types<<"*.bmp"<<"*.png"<<"*.jpg";

    dir.setNameFilters(file_types);

    ls = dir.entryList();

    for(int i = 0; i < ls.length(); ++i)
    {
        ui->listWidget->addItem(dir.absoluteFilePath(ls[i]));
    }
}

void MainWindow::scaleImage(QString image_path)
{
    QImage source_image(image_path);

    if(source_image.isNull())
    {
        return;
    }

    const char* format;

    switch(s.format)
    {
    case 1:
        format = "BMP";
        break;
    case 2:
        format = "JPG";
        break;
    case 3:
        format = "PNG";
        break;
    default:
        QString type = image_path.mid(image_path.lastIndexOf('.')+1).toUpper();
        format = strcpy(new char[type.length()],type.toAscii().data());
    }

    QString str = image_path.left(image_path.length()-4).mid(image_path.lastIndexOf('/')+1)+"_Resized";
    QString filename = str;

    for(int i=0; QFile::exists(QString("%1/%2.%3").arg(output_path,filename).arg(format)); ++i)
    {
        filename = QString("%1(%2)").arg(str).arg(i+1);
    }

    int height;
    int width;

    if( ((source_image.width() > source_image.height()) && (s.width > s.height)) ||
        ((source_image.width() < source_image.height()) && (s.width < s.height)) )
    {
        height = s.height;
        width = s.width;
    }
    else
    {
        height = s.width;
        width = s.height;
    }

    if(s.aspect_ratio)
    {
        QImage scaled = source_image.scaled(width,height,Qt::KeepAspectRatio,Qt::SmoothTransformation);

        scaled.save(QString("%1/%2.%3").arg(output_path,filename).arg(format),format,s.quality);
    }
    else
    {
        QImage scaled = source_image.scaled(width,height,Qt::IgnoreAspectRatio,Qt::SmoothTransformation);

        scaled.save(QString("%1/%2.%3").arg(output_path,filename).arg(format),format,s.quality);
    }

    delete format;
}

void MainWindow::on_pushButton_Delete_clicked()
{
    QList<QListWidgetItem*> items = ui->listWidget->selectedItems();

    foreach(QListWidgetItem* item, items)
    {
        delete item;
    }
}

void MainWindow::on_pushButton_DeleteAll_clicked()
{
    ui->listWidget->clear();
}

void MainWindow::on_toolButton_OutputDir_clicked()
{
    QString dir_path = QFileDialog::getExistingDirectory(this,tr("Відкрити"),QDir::homePath());

    ui->lineEdit->setText(dir_path);
}

void MainWindow::on_pushButton_Settings_clicked()
{
    SettingsDialog* dialog = new SettingsDialog(this);

    dialog->setSettings(s);

    connect(dialog,SIGNAL(settingsChanged(Settings)),this,SLOT(setSettings(Settings)));

    dialog->exec();

    dialog->deleteLater();
}

void MainWindow::on_pushButton_Run_clicked()
{
    if( ui->listWidget->count() == 0)
    {
        QMessageBox::warning(this,tr("Помилка!"),tr("Вкажіть вхідні файли!"));

        return;
    }

    if( (ui->lineEdit->text().isEmpty()) || (!QDir(ui->lineEdit->text()).exists()) )
    {
        QMessageBox::warning(this,tr("Помилка!"),tr("Вкажіть існуючу вихідну теку!"));

        return;
    }
    else
    {
        output_path = ui->lineEdit->text();
    }

    QList<QString> image_paths;

    for(int i=0; i < ui->listWidget->count(); ++i)
    {
        QListWidgetItem* item = ui->listWidget->item(i);

        image_paths.append(item->text());
    }

    QFutureWatcher<void> watcher;
    QProgressDialog* progress_dialog = new QProgressDialog(this);

    progress_dialog->setWindowTitle(tr("Обробка зображень"));
    progress_dialog->setLabelText(tr("Триває обробка зображень, зачекайте..."));
    progress_dialog->setWindowModality(Qt::WindowModal);
    progress_dialog->setCancelButtonText(tr("Відмінити"));
    progress_dialog->setFixedSize(progress_dialog->sizeHint());

    connect(&watcher,SIGNAL(progressRangeChanged(int,int)),progress_dialog,SLOT(setRange(int,int)));
    connect(&watcher,SIGNAL(progressValueChanged(int)),progress_dialog,SLOT(setValue(int)));
    connect(progress_dialog,SIGNAL(canceled()),&watcher,SLOT(cancel()));

    watcher.setFuture(QtConcurrent::map(image_paths,&MainWindow::scaleImage));

    progress_dialog->exec();

    watcher.waitForFinished();
}

void MainWindow::setSettings(Settings _s)
{
    s = _s;
}

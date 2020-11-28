#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_KeyComboBox_currentIndexChanged(const QString &arg1)
{
    if(arg1==ui->KeyComboBox->itemText(1)){
        ui->KeyToolButton->setEnabled(false);
        ui->InputKey->setText(MainWindow::generate_file_name(0));
    }else{
        ui->InputKey->clear();
        ui->KeyToolButton->setEnabled(true);
    }
}

void MainWindow::on_TxtComboBox_currentIndexChanged(const QString &arg1)
{
    if(arg1==ui->TxtComboBox->itemText(1)){
        ui->TxtSaveToolButton->setEnabled(false);
        ui->InputSaveTxt->setText(MainWindow::generate_file_name(1));
    }else{
        ui->InputSaveTxt->clear();
        ui->TxtSaveToolButton->setEnabled(true);
    }
}

QString MainWindow::generate_file_name(int type){

   QDir(".");
   QString s=QDir(".").absolutePath();

   if(type==0){
       return s+"/"+"file.key";
   }else{
       return s+"/"+"file.txt";
    }
}

void MainWindow::generate_parameters(){

    QFile keyFile(ui->InputKey->text());
    keyFile.open(QIODevice::WriteOnly);
    QByteArray buff;

    QString time;
    QStringList timeSep;
    long timeInt[3];

    QString date;
    QStringList dateSep;
    long dateInt[3];

    long c0;
    long a;
    long b;

    /*значение даты и времени для генерации ПСЧ*/
    time=QTime::currentTime().toString();
    date=QDate::currentDate().toString("dd.MM.yyyy");

    timeSep=time.split(':');
    dateSep=date.split('.');

    for(int i=0; i<3; i++){
        timeInt[i]=timeSep.at(i).toInt();
        dateInt[i]=dateSep.at(i).toInt();
    }


    /*получаем значение параметры m в ЛКГ*/
    long m=1;
    for (int i=0;i<MainWindow::n;i++){
        m*=2;
    }

    /*генерируем параметры ЛКГ*/
    c0=(m/(timeInt[0]+timeInt[1]-1))/(timeInt[2]+1);

    long buffer=m/(1024+timeInt[2]);
    a=(buffer%4==1 ? buffer : (buffer-(buffer%4))+1);

    buffer=(m/(dateInt[1]+timeInt[1]+1))/(1024+timeInt[0]+1);
    while(true){
        if (buffer%2==0)
            buffer/=2;
        else
            break;
    }
    b=buffer;

    buff=QString::number(a).toUtf8()+" "+QString::number(b).toUtf8()+" "+QString::number(c0).toUtf8();
    keyFile.write(buff);
    keyFile.close();
}

void MainWindow::generate_sequence(long len,long seq[]){

    /*получаем значение параметры m в ЛКГ*/
    long m=1;
    for(int i=0;i<MainWindow::n;i++){
        m*=2;
    }

    QFile keysFile(ui->InputKey->text());
    keysFile.open(QIODevice::ReadOnly);
    QByteArray buffer1=keysFile.readLine();
    QList<QByteArray> buffer2= buffer1.split(' ');
    keysFile.close();

    long parametres[3];
    for(int i=0;i<3;i++){
       parametres[i]=buffer2.at(i).toLong();
    }

    long previous_number=parametres[2];
    long current_number;

    seq[0]=previous_number;

    /*генерируем новое ПСЧ,выводим в окно главного меню и записываем в файл*/
    for(int i=1;i<len;i++){
        current_number=(parametres[0] * previous_number+parametres[1])%m;

        seq[i]=current_number;
        previous_number=current_number;
    }

}

void MainWindow::on_ButtonGenerate_released()
{

    int type=0;
    QFile keyFile(ui->InputKey->text());

    if(keyFile.size()!=0){
        QMessageBox msgBox;
        msgBox.setText("Файл не пуст.");
        msgBox.setInformativeText("Хотите использовать его содержимое в качестве параметров для ГПСЧ?\nЕсли \"Нет\", то файл будет перезаписан.");
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
        int ret = msgBox.exec();

        switch (ret) {
            case QMessageBox::No:
                type=0;
                break;
            case QMessageBox::Yes:
                type=1;
                break;
            case QMessageBox::Cancel:
                return;
        }
    }

    if(type==0)
        MainWindow::generate_parameters();

    ui->statusbar->showMessage("Генерация параметров выполнена");
}

void MainWindow::on_ButtonGamming_released()
{
    /*читаем всё содержимое файла, которое хотим зашифровать*/
    MainWindow::encodedData.clear();
    QFile encodeFile(ui->InputTxt->text());
    encodeFile.open(QIODevice::ReadOnly);
    QByteArray buffer1=encodeFile.readAll();
    encodeFile.close();

    /*инициализируем область памяти для хранения ПСЧ*/
    long sequence[buffer1.length()];
    memset(sequence,0,sizeof(long));

    /*генерируем последовательность ПСЧ для гаммирования файла*/
    MainWindow::generate_sequence(buffer1.length(),sequence);


    ushort chrUncd;
    ushort chrNewUncd;
    QElapsedTimer timer;
    timer.start();

    /*выполняем гаммирование*/
    for(int i=0;i<buffer1.length();i++){
       chrUncd=buffer1[i];
       chrNewUncd=chrUncd^sequence[i];
       MainWindow::encodedData.append(chrNewUncd);
    }

    int elapsed=timer.nsecsElapsed();
    ui->time->setText(QString::number(elapsed/pow(10,9))+" seconds");
    ui->statusbar->showMessage("Гаммирование данных выполнено.");
}

void MainWindow::on_ButtonSave_released()
{
    QFile saveFile(ui->InputSaveTxt->text());

    if(saveFile.size()!=0){
        QMessageBox msgBox;
        msgBox.setText("Файл не пуст.");
        msgBox.setInformativeText("Хотите перезаписать его содержимое?");
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::Cancel);
        int ret = msgBox.exec();

        switch (ret) {
            case QMessageBox::Yes:
                break;
            case QMessageBox::Cancel:
                return;
        }
    }

   saveFile.open(QIODevice::WriteOnly);
   saveFile.write(MainWindow::encodedData);
   saveFile.close();

   ui->statusbar->showMessage("Запись в файл выполнена.");
}

void MainWindow::on_KeyToolButton_released()
{
    QString fileKEYname=QFileDialog::getOpenFileName(this,"Выберите файл *.key",QDir::homePath(),"Key файл (*.key)");
    ui->InputKey->setText(fileKEYname);
}

void MainWindow::on_TxtToolButton_released()
{
    QString fileKEYname=QFileDialog::getOpenFileName(this,"Выберите файл *.txt",QDir::homePath(),"Text файл (*.txt)");
    ui->InputTxt->setText(fileKEYname);
}

void MainWindow::on_TxtSaveToolButton_released()
{
    QString fileKEYname=QFileDialog::getOpenFileName(this,"Выберите файл *.txt",QDir::homePath(),"Text файл (*.txt)");
    ui->InputSaveTxt->setText(fileKEYname);
}

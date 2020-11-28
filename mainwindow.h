#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDir>
#include <QFileDialog>
#include <QMessageBox>
#include <QFile>
#include <QTime>
#include <QDate>
#include <QElapsedTimer>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    QByteArray encodedData;
    const int n=16;

private slots:
    void on_KeyComboBox_currentIndexChanged(const QString &arg1);

    void on_TxtComboBox_currentIndexChanged(const QString &arg1);

    void on_ButtonGenerate_released();

    void on_ButtonGamming_released();

    void on_ButtonSave_released();

    void on_KeyToolButton_released();

    void on_TxtToolButton_released();

    void on_TxtSaveToolButton_released();

    void generate_sequence(long len,long seq[]);

    /**
     * @brief generate_parameters Генерируем параметры для линейного конгруэнтного
     * генератора
     */
    void generate_parameters();

    QString generate_file_name(int type);

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H

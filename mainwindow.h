#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QPixmap>
#include <QPainter>
#include <QApplication>
#include <QDebug>
#include <QFileDialog>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QMediaMetaData>
#include <QTime>
#include <QtMath>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void stateChanged(QMediaPlayer::PlaybackState state);
    void positionChanged(qint64 position);

private slots:


    void on_pauseButton_clicked();

    void on_playButton_clicked();

    void on_stopButton_clicked();

    void on_progressSlider_sliderMoved(int position);

    void on_volumeSlider_sliderMoved(int position);

    void on_actionOpen_File_triggered();

    void on_volumeButton_clicked();

private:
    Ui::MainWindow *ui;
    QMediaPlayer* audio_player;
    QAudioOutput* audio_output;

    bool volume_button_clicked;
    float cached_volume;
};

#endif // MAINWINDOW_H

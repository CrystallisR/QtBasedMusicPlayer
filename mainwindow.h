#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QInputDialog>
#include <QPixmap>
#include <QGraphicsScene>
#include <QPainter>
#include <QApplication>
#include <QCoreApplication>
#include <QDebug>
#include <QFileDialog>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QMediaMetaData>
#include <QTime>
#include <QtMath>
#include <QSettings>
#include <QCloseEvent>
#include <QListWidgetItem>

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

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:

    void on_playButton_clicked();

    void on_stopButton_clicked();

    void on_progressSlider_sliderMoved(int position);

    void on_volumeSlider_sliderMoved(int position);

    void on_actionOpen_File_triggered();

    void on_volumeButton_clicked();

    void on_actionImport_Music_Resources_triggered();

    void on_actionSet_Appearance_triggered();

    void on_musicList_itemDoubleClicked(QListWidgetItem *item);

    void on_forwardButton_clicked();

    void on_backwardButton_clicked();

    void on_actionReset_Music_List_triggered();

private:
    Ui::MainWindow *ui;
    QMediaPlayer* audio_player;
    QAudioOutput* audio_output;

    // file settings
    QString default_file_dir;
    QString default_import_dir;
    int last_position;

    // ui settings
    QPixmap default_music_image;

    // temporary state/ control variable
    bool volume_button_clicked;
    bool play_button_clicked;
    bool music_manually_stopped;
    float cached_volume;

    float volumeConvert(int value);
    void startPlaying(QFileInfo file_info);
    void writeSettings();
    void readSettings();
    void saveList(QSettings& settings);
    void loadList(QSettings& settings);
};

#endif // MAINWINDOW_H

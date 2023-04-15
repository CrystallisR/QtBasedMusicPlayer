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
#include <QMessageBox>
#include <QShortcut>
#include <memory>
#include <QSystemTrayIcon>
#include "playqueue.h"

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

    void on_musicList_itemDoubleClicked(QListWidgetItem* item);

    void on_forwardButton_clicked();

    void on_backwardButton_clicked();

    void on_actionReset_Music_List_triggered();

    void on_modeButton_clicked();

private:
    Ui::MainWindow *ui;
    std::unique_ptr<QMediaPlayer> audio_player;
    std::unique_ptr<QAudioOutput> audio_output;
    std::unique_ptr<PlayQueue> play_queue;
    std::unique_ptr<QSystemTrayIcon> tray_icon;

    std::unique_ptr<QMenu> music_list_menu;
    std::unique_ptr<QMenu> tray_menu;
    std::unique_ptr<QMenu> mode_menu;

    // menu actions
    std::unique_ptr<QAction> quit_action;
    std::unique_ptr<QAction> add_to_queue_action;
    std::unique_ptr<QAction> remove_from_list_action;
    std::unique_ptr<QAction> play_next_action;
    std::unique_ptr<QAction> play_prev_action;
    std::unique_ptr<QAction> play_action;
    std::unique_ptr<QAction> order_loop_action;
    std::unique_ptr<QAction> random_loop_action;
    std::unique_ptr<QAction> single_loop_action;

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

    // usesr interaction settings
    void setShortCutsForAll();

    // signal and slot connection control
    void initConnect();

    // play control
    void startPlayingNew(QFileInfo file_info);
    inline void playListItem(QListWidgetItem* item);
    void addToPlayQueue();
    void removeFromPlayList();
    void setOrderLoopMode();
    void setSingleLoopMode();
    void setRandomLoopMode();

    // ui update
    void showMusicInfo(QFileInfo file_info);
    inline void updateItemSelectedUI(QListWidgetItem* cur_item, QListWidgetItem* new_item);

    // save/load settings
    void writeSettings();
    void readSettings();
    void saveList(QSettings& settings);
    void loadList(QSettings& settings);

    // manage menu actions
    void initActions();

    // set tool button
    void setModeButton();

    // manage context Menu
    void setListWidgetContextMenu();
    void showListWidgetContextMenu(const QPoint &pos);

    void setTrayIcon(const QIcon& appIcon);
    void setTrayIconMenu();
    void trayIconActivated(QSystemTrayIcon::ActivationReason reason);

    // helper functions
    float volumeConvert(int value);
    int setYesOrNoMessageBox(QString message, QString window_title);
};

#endif // MAINWINDOW_H

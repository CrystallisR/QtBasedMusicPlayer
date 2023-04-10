#ifndef MUSICPLAYER_H
#define MUSICPLAYER_H

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

QT_BEGIN_NAMESPACE
namespace MP { class musicPlayer; }
QT_END_NAMESPACE

class musicPlayer
{


public:
    musicPlayer();
    ~musicPlayer();
    void writeSettings();
    void readSettings();

    QMediaPlayer *getAudio_player();
    void setAudio_player(QMediaPlayer *newAudio_player);

    QAudioOutput *getAudio_output();
    void setAudio_output(QAudioOutput *newAudio_output);

    bool getVolume_button_clicked() const;
    void setVolume_button_clicked(bool newVolume_button_clicked);

    bool getPlay_button_clicked() const;
    void setPlay_button_clicked(bool newPlay_button_clicked);

    float getCached_volume() const;
    void setCached_volume(float newCached_volume);

    const QString &getDefault_file_dir() const;
    void setDefault_file_dir(const QString &newDefault_file_dir);

    int getLast_position() const;
    void setLast_position(int newLast_position);

private:
    QMediaPlayer* audio_player;
    QAudioOutput* audio_output;

    // temporary state cache
    bool volume_button_clicked;
    bool play_button_clicked;
    float cached_volume;

    // permanent state for settings
    QString default_file_dir;
    int last_position;

    float volumeConvert(int value);
};

#endif // MUSICPLAYER_H

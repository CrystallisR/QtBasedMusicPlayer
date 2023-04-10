#include "musicPlayer.h"

musicPlayer::musicPlayer() :
    volume_button_clicked(false),
    play_button_clicked(false),
    cached_volume(0.0f)
{
    // settings
    QApplication::setApplicationName("myMusicPlayer");
    QApplication::setOrganizationName("CrystallisR");
    QSettings::setDefaultFormat(QSettings::IniFormat);

    readSettings();

    // player initialization
    audio_player = new QMediaPlayer();
    audio_output = new QAudioOutput();
    audio_player->setAudioOutput(audio_output);

    // use settings
    audio_output->setVolume(volumeConvert(last_position));
}

musicPlayer::~musicPlayer()
{
    delete audio_output;
    delete audio_player;
}

QMediaPlayer *musicPlayer::getAudio_player()
{
    return audio_player;
}

void musicPlayer::setAudio_player(QMediaPlayer *newAudio_player)
{
    audio_player = newAudio_player;
}

QAudioOutput *musicPlayer::getAudio_output()
{
    return audio_output;
}

void musicPlayer::setAudio_output(QAudioOutput *newAudio_output)
{
    audio_output = newAudio_output;
}

bool musicPlayer::getVolume_button_clicked() const
{
    return volume_button_clicked;
}

void musicPlayer::setVolume_button_clicked(bool newVolume_button_clicked)
{
    volume_button_clicked = newVolume_button_clicked;
}

bool musicPlayer::getPlay_button_clicked() const
{
    return play_button_clicked;
}

void musicPlayer::setPlay_button_clicked(bool newPlay_button_clicked)
{
    play_button_clicked = newPlay_button_clicked;
}

float musicPlayer::getCached_volume() const
{
    return cached_volume;
}

void musicPlayer::setCached_volume(float newCached_volume)
{
    cached_volume = newCached_volume;
}

const QString &musicPlayer::getDefault_file_dir() const
{
    return default_file_dir;
}

void musicPlayer::setDefault_file_dir(const QString &newDefault_file_dir)
{
    default_file_dir = newDefault_file_dir;
}

int musicPlayer::getLast_position() const
{
    return last_position;
}

void musicPlayer::setLast_position(int newLast_position)
{
    last_position = newLast_position;
}

void musicPlayer::writeSettings()
{
    QSettings settings;
    settings.setValue("file/default_dir", default_file_dir);
    settings.setValue("file/last_volume_pos", last_position);
}

void musicPlayer::readSettings()
{
    QSettings settings;
    default_file_dir = settings.value("file/default_dir", "").toString();
    last_position = settings.value("file/last_volume_pos", 50).toInt();
}

// private

float musicPlayer::volumeConvert(int value)
{// using exponential on volume percent to produce linear changes in perceived loudness
    if (value < 0) return 0.0f;
    if (value > 100) return 1.0f;
    float percent = static_cast<float>(value) / 100.0f;
    float converted_volume = (qExp<float>(percent) - 1.0f) / (qExp<float>(1.0f) - 1.0f);
    return converted_volume;
}

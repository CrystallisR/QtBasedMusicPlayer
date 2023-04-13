#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , audio_player(new QMediaPlayer(this))
    , audio_output(new QAudioOutput(this))
    , volume_button_clicked(false)
    , play_button_clicked(false)
    , music_manually_stopped(false)
    , cached_volume(0.0f)
{  
    QApplication::setApplicationName("myMusicPlayer");
    QApplication::setOrganizationName("CrystallisR");
    QSettings::setDefaultFormat(QSettings::IniFormat);

    ui->setupUi(this);

    // load settings
    // you should read settings after ui is set up
    // since you may want to initialize some components in ui
    readSettings();
    play_queue = new PlayQueue(ui->musicList);

    // player initialization
    audio_player->setAudioOutput(audio_output);
    audio_output->setVolume(volumeConvert(last_position));

    // connect
    connect(audio_player, &QMediaPlayer::playbackStateChanged, this, &MainWindow::stateChanged);
    connect(audio_player, &QMediaPlayer::positionChanged, this, &MainWindow::positionChanged);

    // ui setting
    ui->volumeSlider->setValue(last_position);
    ui->volumeDisplay->setText(QString::number(ui->volumeSlider->value()) + "%");

    setWindowIcon(QIcon(":icons/res/musical_notec.png"));
    default_music_image = QPixmap(":icons/res/musical_notec.png");
    this->setProperty("windowOpacity", 1.0);

    // set stylesheet
    // ...

    ui->playButton->setEnabled(true);
    ui->stopButton->setEnabled(false);
}

MainWindow::~MainWindow()
{
    delete ui;
    delete audio_output;
    delete audio_player;
    delete play_queue;
}

void MainWindow::positionChanged(qint64 position)
{
    if (audio_player->duration() != ui->progressSlider->maximum())
        ui->progressSlider->setMaximum(audio_player->duration());

    ui->progressSlider->setValue(position);

    const int base {1000};
    auto seconds = (position / base) % 60;
    auto minutes = (position/(60 * base)) % 60;
    auto hours = (position/(3600 * base)) % 24;
    QTime time(hours, minutes, seconds);
    ui->durationDisplay->setText(time.toString());
}

void MainWindow::stateChanged(QMediaPlayer::PlaybackState state)
{
    if (state == QMediaPlayer::PlayingState)
    {
        ui->playButton->setEnabled(true);
        ui->stopButton->setEnabled(true);
    }
    else if (state == QMediaPlayer::PausedState)
    {
        ui->playButton->setEnabled(true);
        ui->stopButton->setEnabled(true);
    }
    else if (state == QMediaPlayer::StoppedState)
    {
        ui->playButton->setEnabled(true);
        ui->stopButton->setEnabled(false);
        if (!music_manually_stopped)
            ui->forwardButton->click();
    }
}

// protected
void MainWindow::closeEvent(QCloseEvent *event)
{
    QMessageBox exit_box;
    QIcon window_icon = QIcon(QPixmap(":icons/res/musical_notec.png"));
    QPixmap display_icon = QPixmap(":icons/res/question_markr1.png");
    exit_box.setWindowTitle("Exit");
    exit_box.setWindowIcon(window_icon);
    exit_box.setIconPixmap(display_icon.scaledToHeight(40));
    exit_box.setText("<p align='center'>Are You Sure to Exit?");
    exit_box.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    exit_box.setDefaultButton(QMessageBox::Yes);
    auto ret = exit_box.exec();

    if (ret == QMessageBox::Yes)
    {
        writeSettings();
        event->accept();
    }
    else
    {
        event->ignore();
    }
}

// private slot

void MainWindow::on_actionOpen_File_triggered()
{
    QString prompt = "Please Select Your Audio File";
    QString file_format {"ALL (*.mp3 *.wav *.flac *.acc);;"
                         "MP3 (*.mp3);;WAV (*.wav);;FLAC (*.flac);;AAC (*.acc)"};
    QString file_dir = default_file_dir == "" ? qApp->applicationDirPath() : default_file_dir;
    QString file_path = QFileDialog::getOpenFileName(this, prompt, file_dir, file_format);

    QFileInfo file_info(file_path);
    if (file_info.absolutePath() != "") default_file_dir = file_info.absolutePath();
    if (file_info.fileName() != "")
        startPlayingNew(file_info);

}

void MainWindow::on_actionImport_Music_Resources_triggered()
{
    QString prompt = "Please Select Your Audio File Directory";
    QString import_dir = QFileDialog::getExistingDirectory(this, prompt,\
               default_import_dir, QFileDialog::DontUseNativeDialog);
    QDir dir(import_dir);

    // Match designated format
    // QString wildcardExp = QRegularExpression::wildcardToRegularExpression("*.(flac)");
    static QRegularExpression re(".*.(flac|mp3|wav)", QRegularExpression::CaseInsensitiveOption);

    // register imported files
    for (const QFileInfo &file : dir.entryInfoList(QDir::Files))
    {
        if (!re.match(file.fileName()).hasMatch()) continue;
        auto items = ui->musicList->findItems(file.fileName(), Qt::MatchExactly);
        if (!items.isEmpty()) continue;

        QListWidgetItem* item = new QListWidgetItem;
        item->setIcon(QIcon(":/icons/res/music_notec2.png"));
        item->setText(file.fileName());
        item->setData(Qt::UserRole, file.absoluteFilePath());
        ui->musicList->addItem(item);
    }
    default_import_dir = import_dir;
}

void MainWindow::on_actionReset_Music_List_triggered()
{
    ui->musicList->clear();
}

void MainWindow::on_actionSet_Appearance_triggered()
{
    this->setProperty("windowOpacity", 1.0);
}

void MainWindow::on_progressSlider_sliderMoved(int position)
{
    audio_player->setPosition(position);
}

void MainWindow::on_volumeSlider_sliderMoved(int position)
{
    audio_output->setVolume(volumeConvert(position));
    ui->volumeDisplay->setText(QString::number(position) + "%");
    last_position = position;
}

void MainWindow::on_playButton_clicked()
{
    play_button_clicked = !play_button_clicked;
    music_manually_stopped = false;
    if (play_button_clicked)
    {
        audio_player->play();
        ui->playButton->setIcon(QIcon(":/icons/res/pause_w.png"));
    }
    else
    {
        audio_player->pause();
        ui->playButton->setIcon(QIcon(":/icons/res/play_w.png"));
    }
}

void MainWindow::on_stopButton_clicked()
{
    // when state changes to stop, <music_manually_stopped>'ll be checked, set it before stop()
    music_manually_stopped = true;
    play_button_clicked = false;
    audio_player->stop();
    ui->playButton->setIcon(QIcon(":/icons/res/play_w.png"));
}

void MainWindow::on_volumeButton_clicked()
{
    volume_button_clicked = !volume_button_clicked;
    if (volume_button_clicked)
    {
        cached_volume = audio_output->volume();
        audio_output->setVolume(0);
        ui->volumeButton->setIcon(QIcon(":/icons/res/volume_mute_w.png"));
        ui->volumeDisplay->setText("0%");
    }
    else
    {
        audio_output->setVolume(cached_volume);
        ui->volumeButton->setIcon(QIcon(":/icons/res/volume_w.png"));
        ui->volumeDisplay->setText(QString::number(static_cast<int>(cached_volume * 100)) + "%");
    }
}

void MainWindow::on_musicList_itemDoubleClicked(QListWidgetItem* item)
{
    int new_item_row = ui->musicList->row(item);
    play_queue->updatePlayingQueue(new_item_row);
    play_queue->setCurrent_item_row(new_item_row);
    ui->forwardButton->click();
}

void MainWindow::on_forwardButton_clicked()
{
    auto* current_item = play_queue->current();
    auto* next_item = play_queue->next();
    if (!next_item || !current_item) return;
    updateItemSelectedUI(current_item, next_item);
    playListItem(next_item);
}

void MainWindow::on_backwardButton_clicked()
{
    auto* current_item = play_queue->current();
    auto* pre_item = play_queue->previous();
    if (!pre_item || !current_item) return;
    updateItemSelectedUI(current_item, pre_item);
    playListItem(pre_item);
}

// play control
void MainWindow::startPlayingNew(QFileInfo file_info)
{ // do not modify this function, it's under inspection
    ui->stopButton->click();
    audio_player->setSource(QUrl::fromLocalFile(file_info.absoluteFilePath()));
    if (play_button_clicked) ui->playButton->click();
    ui->playButton->click();
    showMusicInfo(file_info);
}

inline void MainWindow::playListItem(QListWidgetItem* item)
{
    QString file_path = item->data(Qt::UserRole).toString();
    QFileInfo file_info(file_path);
    startPlayingNew(file_info);
}

// ui update
void MainWindow::showMusicInfo(QFileInfo file_info)
{
    if (1) // metadata?
    {// attempt to read meta data
        ui->musicNameDisplay->setText("Playing <"+ file_info.fileName() + ">...");
    }
    else
    {
        ui->musicNameDisplay->setText("Playing <"+ file_info.fileName() + ">...");
    }
}

inline void MainWindow::updateItemSelectedUI(QListWidgetItem* cur_item, QListWidgetItem* new_item)
{
    cur_item->setSelected(false);
    new_item->setSelected(true);
}

// save/load settings
void MainWindow::writeSettings()
{
    QSettings settings;
    settings.setValue("file/default_dir", default_file_dir);
    settings.setValue("file/default_import_dir", default_import_dir);
    settings.setValue("file/last_volume_pos", last_position);
    saveList(settings);
}

void MainWindow::readSettings()
{
    QSettings settings;
    default_file_dir = settings.value("file/default_dir", "").toString();
    default_import_dir = settings.value("file/default_import_dir", default_file_dir).toString();
    last_position = settings.value("file/last_volume_pos", 25).toInt();
    loadList(settings);
}

void MainWindow::saveList(QSettings& settings)
{
    settings.beginWriteArray("musicList");
    for (int row = 0; row < ui->musicList->count(); row++)
    {
        settings.setArrayIndex(row);
        QListWidgetItem* cur_item = ui->musicList->item(row);
        settings.setValue("musicName", cur_item->text());
        settings.setValue("musicPath", cur_item->data(Qt::UserRole));
    }
    settings.endArray();
}

void MainWindow::loadList(QSettings& settings)
{
    int size = settings.beginReadArray("musicList");
    for (int row = 0; row < size; row++)
    {
        settings.setArrayIndex(row);
        QString file_name = settings.value("musicName").toString();
        QString file_path = settings.value("musicPath").toString();

        QListWidgetItem* cur_item = new QListWidgetItem;
        cur_item->setIcon(QIcon(":/icons/res/music_notec2.png"));
        cur_item->setText(file_name);
        cur_item->setData(Qt::UserRole, file_path);
        ui->musicList->addItem(cur_item);
    }
    settings.endArray();
}

// helper functions
float MainWindow::volumeConvert(int value)
{// using exponential on volume percent to produce linear changes in perceived loudness
    if (value < 0) return 0.0f;
    if (value > 100) return 1.0f;
    float percent = static_cast<float>(value) / 100.0f;
    float converted_volume = (qExp<float>(percent) - 1.0f) / (qExp<float>(1.0f) - 1.0f);
    return converted_volume;
}


#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , volume_button_clicked(false)
    , play_button_clicked(false)
    , music_manually_stopped(false)
    , cached_volume(0.0f)
{
    ui->setupUi(this);
    // init widgetlist first since we need to read settings
    music_list = std::unique_ptr<ManageList>(new ManageList(ui->musicList));
    // load settings
    // you should read settings after ui is set up
    // since you may want to initialize some components in ui
    readSettings();

    // player initialization
    audio_player = std::unique_ptr<QMediaPlayer>(new QMediaPlayer(this));
    audio_output = std::unique_ptr<QAudioOutput>(new QAudioOutput(this));
    play_queue = std::unique_ptr<PlayQueue>(new PlayQueue(ui->musicList));
    audio_player->setAudioOutput(audio_output.get());
    audio_output->setVolume(volumeConvert(last_position));

    // set key shortcuts
    setShortCutsForAll();

    // set Menu & actions
    initActions();
    // initMenus();

    // ui setting
    ui->volumeSlider->setValue(last_position);
    ui->volumeDisplay->setText(QString::number(ui->volumeSlider->value()) + "%");

    auto appIcon = QIcon(":icons/res/musical_notec.png");
    setTrayIcon(appIcon);
    setWindowIcon(appIcon);
    default_music_image = QPixmap(":icons/res/musical_notec.png");
    this->setProperty("windowOpacity", 1.0);

    // other ui componet settings
    ui->playButton->setEnabled(true);
    ui->stopButton->setEnabled(false);
    setMusicListMenu();
    connectMusicListMenu();
    setModeButton();
    // set stylesheet
    // ...

    // signal&slot connecttion
    initConnect();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::initConnect()
{
    // connect audio_player's state with GUI
    connect(audio_player.get(), &QMediaPlayer::playbackStateChanged, this, &MainWindow::stateChanged);
    connect(audio_player.get(), &QMediaPlayer::positionChanged, this, &MainWindow::positionChanged);
    // after media fully loaded, read its metadata and show infos
    connect(audio_player.get(), &QMediaPlayer::mediaStatusChanged, this, &MainWindow::showMusicInfo);

    // if not using auto connection by ui designer, use below connection
    // connect(ui->playButton, &QPushButton::clicked, this, &MainWindow::on_playButton_clicked); //...
}

void MainWindow::setShortCutsForAll()
{
    ui->playButton->setShortcut(QKeySequence("Space"));
    ui->stopButton->setShortcut(QKeySequence("Ctrl+C"));
    ui->forwardButton->setShortcut(QKeySequence("Ctrl+Right"));
    ui->backwardButton->setShortcut(QKeySequence("Ctrl+Left"));
    ui->volumeButton->setShortcut(QKeySequence("Ctrl+O"));
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
    auto ret = setYesOrNoMessageBox("Are You Sure to Exit?", "Exit");

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
    QString file_format {"ALL (*.mp3 *.wav *.flac);;"
                         "MP3 (*.mp3);;WAV (*.wav);;FLAC (*.flac)"};
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

    music_list->importToList(dir, ".*.(flac|mp3|wav)");
    default_import_dir = import_dir;
}

void MainWindow::on_actionReset_Music_List_triggered()
{
    auto ret = setYesOrNoMessageBox("Are You Sure To Clear All Imported Files?", "Reset");
    if (ret == QMessageBox::Yes)
    {
        play_queue->clear();
        music_list->clear();
    }
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
        play_action->setIcon(QIcon(":icons/res/pause.png"));
        play_action->setText("&Pause");
    }
    else
    {
        audio_player->pause();
        ui->playButton->setIcon(QIcon(":/icons/res/play_w.png"));
        play_action->setIcon(QIcon(":icons/res/play.png"));
        play_action->setText("&Play");
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
    int new_item_row = music_list->getRow(item);
    play_queue->updatePlayingQueue(new_item_row);
    play_queue->setCurrent_item_row(new_item_row);

    auto cur_play_mode = play_queue->getPlayMode();
    play_queue->setPlayMode(PQ::PlayMode::Order);
    ui->forwardButton->click();
    play_queue->setPlayMode(cur_play_mode);
}

void MainWindow::on_forwardButton_clicked()
{
    auto* current_item = play_queue->current();
    auto* next_item = play_queue->next();
    if (!next_item || !current_item) return;
    playListItem(next_item);
    music_list->updateUIonItemChange(current_item, next_item);
}

void MainWindow::on_backwardButton_clicked()
{
    auto* current_item = play_queue->current();
    auto* pre_item = play_queue->previous();
    if (!pre_item || !current_item) return;
    playListItem(pre_item);
    music_list->updateUIonItemChange(current_item, pre_item);
}

void MainWindow::on_modeButton_clicked()
{
    ui->modeButton->showMenu();
}

// play control
void MainWindow::startPlayingNew(QFileInfo file_info)
{ // do not modify this function, it's under inspection
    ui->stopButton->click();
    audio_player->setSource(QUrl::fromLocalFile(file_info.absoluteFilePath()));
    if (play_button_clicked) ui->playButton->click();
    ui->playButton->click();
    cur_file_info = file_info;
}

inline void MainWindow::playListItem(QListWidgetItem* item)
{
    QString file_path = item->data(Qt::UserRole).toString();
    QFileInfo file_info(file_path);
    startPlayingNew(file_info);
}

void MainWindow::addToPlayQueue()
{
    play_queue->addToUserQueue();
}

void MainWindow::removeFromPlayList()
{
    auto ret = setYesOrNoMessageBox("Are You Sure To Remove The Selected File(s) From Play List?"
                                    "<br>(Local Files Won't Be Affected)"
                                    "<br>But Play Queue Will Be Reset"
                                    ,"Remove Music");
    if (ret == QMessageBox::Yes)
    {
        play_queue->clear();
        music_list->removeSelectedFromList();
    }
}

void MainWindow::setOrderLoopMode()
{
    ui->modeButton->setIcon(QIcon(":icons/res/loopmodec.png"));
    play_queue->setPlayMode(PQ::PlayMode::Order);
}

void MainWindow::setSingleLoopMode()
{
    ui->modeButton->setIcon(QIcon(":icons/res/loopb.png"));
    play_queue->setPlayMode(PQ::PlayMode::Single);
}

void MainWindow::setRandomLoopMode()
{
    ui->modeButton->setIcon(QIcon(":icons/res/shuffle.png"));
    play_queue->setPlayMode(PQ::PlayMode::Shuffle);
}

// ui update
void MainWindow::showMusicInfo(QMediaPlayer::MediaStatus status)
{
    if (status != QMediaPlayer::LoadedMedia) return;
    QMediaMetaData file_meta_data = audio_player->metaData();
    // set image
    QVariant raw_image = file_meta_data.value(QMediaMetaData::ThumbnailImage);
    QImage cover_image = raw_image.value<QImage>();

    if (!cover_image.isNull())
        ui->musicGraphics->setPixmap(QPixmap::fromImage(cover_image));
    else
        ui->musicGraphics->setPixmap(QPixmap(":icons/res/musical_notec.png"));

    // set music title & author infos
    QString title = file_meta_data.value(QMediaMetaData::Title).toString();
    QString author = file_meta_data.value(QMediaMetaData::Author).toString();

    if (!title.isEmpty() && !author.isEmpty())
        ui->musicNameDisplay->setText(\
         "Playing " + title + "...<br>Musician ("+ author\
         + ")<br>File (" + cur_file_info.fileName() + ")");
    else
        ui->musicNameDisplay->setText("Playing <"+ cur_file_info.fileName() + ">...");

    // also show music info in tray icon tooltips
    tray_icon->setToolTip("Playing <"+ cur_file_info.fileName() + ">...");
}


// save/load settings
void MainWindow::writeSettings()
{
    QSettings settings;
    settings.setValue("file/default_dir", default_file_dir);
    settings.setValue("file/default_import_dir", default_import_dir);
    settings.setValue("file/last_volume_pos", last_position);
    music_list->saveList(settings, "musicList");
}

void MainWindow::readSettings()
{
    QSettings settings;
    default_file_dir = settings.value("file/default_dir", "").toString();
    default_import_dir = settings.value("file/default_import_dir", default_file_dir).toString();
    last_position = settings.value("file/last_volume_pos", 25).toInt();
    music_list->loadList(settings, "musicList");
}

void MainWindow::initActions()
{
    quit_action = std::unique_ptr<QAction>(new QAction("&Quit", this));
    quit_action->setIcon(QIcon(":icons/res/quit.png"));
    connect(quit_action.get(), &QAction::triggered, qApp, &QCoreApplication::quit);

    add_to_queue_action = std::unique_ptr<QAction>(new QAction("&Add To Play Queue", this));
    add_to_queue_action->setIcon(QIcon(":icons/res/add_p1.png"));
    connect(add_to_queue_action.get(), &QAction::triggered, this, &MainWindow::addToPlayQueue);

    remove_from_list_action = std::unique_ptr<QAction>(new QAction("&Remove From List", this));
    remove_from_list_action->setIcon(QIcon(":icons/res/remove_cyan1.png"));
    connect(remove_from_list_action.get(), &QAction::triggered, this, &MainWindow::removeFromPlayList);

    play_next_action = std::unique_ptr<QAction>(new QAction("&Play Next", this));
    play_next_action->setIcon(QIcon(":icons/res/next.png"));
    connect(play_next_action.get(), &QAction::triggered, this, &MainWindow::on_forwardButton_clicked);

    play_prev_action = std::unique_ptr<QAction>(new QAction("&Play Last", this));
    play_prev_action->setIcon(QIcon(":icons/res/back.png"));
    connect(play_prev_action.get(), &QAction::triggered, this, &MainWindow::on_backwardButton_clicked);

    play_action = std::unique_ptr<QAction>(new QAction(("&Play"), this));
    play_action->setIcon(QIcon(":icons/res/play.png"));
    connect(play_action.get(), &QAction::triggered, this, &MainWindow::on_playButton_clicked);

    order_loop_action = std::unique_ptr<QAction>(new QAction(("&Order Loop"), this));
    order_loop_action->setIcon(QIcon(":icons/res/loopmodec.png"));
    connect(order_loop_action.get(), &QAction::triggered, this, &MainWindow::setOrderLoopMode);

    single_loop_action= std::unique_ptr<QAction>(new QAction(("&Repeat Once"), this));
    single_loop_action->setIcon(QIcon(":icons/res/loopb.png"));
    connect(single_loop_action.get(), &QAction::triggered, this, &MainWindow::setSingleLoopMode);

    random_loop_action = std::unique_ptr<QAction>(new QAction(("&Shuffle"), this));
    random_loop_action->setIcon(QIcon(":icons/res/shuffle.png"));
    connect(random_loop_action.get(), &QAction::triggered, this, &MainWindow::setRandomLoopMode);
}

void MainWindow::setModeButton()
{
    mode_menu = std::unique_ptr<QMenu>(new QMenu(this));
    mode_menu->setWindowFlag(Qt::FramelessWindowHint);
    mode_menu->setAttribute(Qt::WA_TranslucentBackground);
    mode_menu->addAction(order_loop_action.get());
    mode_menu->addAction(single_loop_action.get());
    mode_menu->addAction(random_loop_action.get());
    ui->modeButton->setMenu(mode_menu.get());
}

void MainWindow::setMusicListMenu()
{
    music_list_menu = std::unique_ptr<QMenu>(new QMenu(this));
    music_list_menu->setWindowFlag(Qt::FramelessWindowHint);
    music_list_menu->setAttribute(Qt::WA_TranslucentBackground);
    music_list_menu->addAction(add_to_queue_action.get());
    music_list_menu->addAction(remove_from_list_action.get());
}

void MainWindow::connectMusicListMenu()
{
    music_list->getItem_list()->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(music_list->getItem_list(), &QWidget::customContextMenuRequested,\
            this, &MainWindow::showMusicListMenu);
}

void MainWindow::showMusicListMenu(const QPoint &pos)
{
    // get global position
    QPoint global_pos = music_list->getItem_list()->mapToGlobal(pos);
    music_list_menu->exec(global_pos);
}

void MainWindow::setTrayIcon(const QIcon& appIcon)
{
    tray_icon = std::unique_ptr<QSystemTrayIcon>(new QSystemTrayIcon(this));
    tray_icon->setIcon(appIcon);
    tray_icon->setToolTip("Not Playing Now");
    setTrayIconMenu();
    tray_icon->show();
    // set System Tray Icon Interaction
    connect(tray_icon.get(), &QSystemTrayIcon::activated, this, &MainWindow::trayIconActivated);
}

void MainWindow::setTrayIconMenu()
{
    // init & set tray menu
    tray_menu = std::unique_ptr<QMenu>(new QMenu(this));
    tray_menu->setWindowFlag(Qt::FramelessWindowHint);
    tray_menu->setAttribute(Qt::WA_TranslucentBackground);
    tray_menu->addAction(play_next_action.get());
    tray_menu->addAction(play_action.get());
    tray_menu->addAction(play_prev_action.get());
    tray_menu->addAction(quit_action.get());
    tray_icon->setContextMenu(tray_menu.get());
}

void MainWindow::trayIconActivated(QSystemTrayIcon::ActivationReason reason)
{
    switch (reason)
    {
        case QSystemTrayIcon::Trigger:
          this->tray_icon->showMessage("Playing Now", "To Be Done", QIcon(":icons/res/star_shining.png"));
          break;
        default: ;
    }
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

int MainWindow::setYesOrNoMessageBox(QString message, QString window_title)
{
    QMessageBox exit_box;

    exit_box.setWindowTitle(window_title);
    QIcon window_icon = QIcon(QPixmap(":icons/res/musical_notec.png"));
    exit_box.setWindowIcon(window_icon);

    QPixmap display_icon = QPixmap(":icons/res/question_markr1.png");
    exit_box.setIconPixmap(display_icon.scaledToHeight(40));

    exit_box.setText(message);
    exit_box.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    exit_box.setDefaultButton(QMessageBox::Yes);
    return exit_box.exec();
}

#ifndef PLAYQUEUE_H
#define PLAYQUEUE_H

#include <QObject>
#include <QQueue>
#include <QStack>
#include <QListWidget>
#include <QListWidgetItem>
#include <random>

QT_BEGIN_NAMESPACE
namespace PQ { class PlayQueue;}
QT_END_NAMESPACE

namespace PQ
{
    enum PlayMode {Order, Single, Shuffle};
}

class PlayQueue : public QObject
{
    Q_OBJECT
    #define HISTORYSIZE 200
    #define QUEUESIZE 200
    #define AUTO_QUEUE_BATCH 10
    #define AUTO_STACK_BATCH 10

public:
    explicit PlayQueue(QListWidget* init_play_list, QObject *parent = nullptr);
    ~PlayQueue();

    // queue setttings
    void setPlayList(QListWidget*);
    void updatePlayingQueue(int row = 0);
    void setHistoryStack(int row = 0);
    void clear();
    void setPlayMode(PQ::PlayMode);
    PQ::PlayMode getPlayMode() const;

    void addToUserQueue();

    QListWidgetItem* current();
    QListWidgetItem* next();
    QListWidgetItem* previous();

    void setCurrent_item_row(int newCurrent_item_row);

private slots:

private:

    std::random_device rand_dev;
    std::mt19937 generator;

    int current_item_row;
    PQ::PlayMode play_mode;

    QListWidget* play_list;
    QQueue<QListWidgetItem*> default_queue;
    QQueue<QListWidgetItem*> user_added_queue;
    QStack<QListWidgetItem*> history_stack;

    QListWidgetItem* nextOrder();
    QListWidgetItem* nextRandom();
    QListWidgetItem* nextSame();
};

#endif // PLAYQUEUE_H

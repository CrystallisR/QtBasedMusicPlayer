#ifndef PLAYQUEUE_H
#define PLAYQUEUE_H

#include <QObject>
#include <QQueue>
#include <QStack>
#include <QListWidget>
#include <QListWidgetItem>


class PlayQueue : public QObject
{
    Q_OBJECT
    #define HISTORYSIZE 200
    #define QUEUESIZE 200
    #define AUTO_QUEUE_BATCH 20
    #define AUTO_STACK_BATCH 20

public:
    explicit PlayQueue(QListWidget* init_play_list, QObject *parent = nullptr);
    ~PlayQueue();

    void setPlayList(QListWidget*);
    void updatePlayingQueue(int row = 0);
    void setHistoryStack(int row = 0);

    QListWidgetItem* current();
    QListWidgetItem* next();
    QListWidgetItem* previous();
    QListWidgetItem* nextRand();

    void setCurrent_item_row(int newCurrent_item_row);

private:

    int current_item_row;

    QListWidget* play_list;
    QQueue<QListWidgetItem*> default_queue;
    QQueue<QListWidgetItem*> user_added_queue;
    QStack<QListWidgetItem*> history_stack;

signals:

};

#endif // PLAYQUEUE_H

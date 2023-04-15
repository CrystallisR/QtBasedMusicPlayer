#include "playqueue.h"

PlayQueue::PlayQueue(QListWidget* init_play_list, QObject *parent)
    :
      QObject{parent}
    ,current_item_row(0)
    ,play_list(init_play_list)
{
    generator = std::mt19937(rand_dev());
}

PlayQueue::~PlayQueue()
{

}

// public

void PlayQueue::setPlayList(QListWidget* new_play_list)
{
    play_list = new_play_list;
    // other operations
}


void PlayQueue::updatePlayingQueue(int row)
{
    if (play_list->count() == 0) return;
    default_queue.clear();
    user_added_queue.clear();
    for (int cnt = 0; cnt < AUTO_QUEUE_BATCH; cnt++)
    {
        if (row >= play_list->count()) row = 0;
        if (default_queue.size() >= QUEUESIZE) default_queue.dequeue();
        default_queue.enqueue(play_list->item(row++));
    }
}

void PlayQueue::setHistoryStack(int row)
{
    if (play_list->count() == 0) return;
    history_stack.clear();
    row -= AUTO_STACK_BATCH;
    while(row < 0) row += play_list->count();
    for (int cnt = 0; cnt < AUTO_STACK_BATCH; cnt++)
    {
        if (row >= play_list->count()) row = 0;
        if (history_stack.size() >= HISTORYSIZE) history_stack.pop_front();
        history_stack.push(play_list->item(row++));
    }
}

void PlayQueue::clear()
{
    current_item_row = 0;
    default_queue.clear();
    user_added_queue.clear();
    history_stack.clear();
}

void PlayQueue::setPlayMode(PQ::PlayMode new_mode)
{
    play_mode = new_mode;
}

PQ::PlayMode PlayQueue::getPlayMode() const
{
    return play_mode;
}

void PlayQueue::addToUserQueue()
{
    QList<QListWidgetItem*> selected_items = play_list->selectedItems();
    for (auto item: selected_items)
    {
        if (user_added_queue.size() >= QUEUESIZE) break;
        user_added_queue.enqueue(item);
    }
}

QListWidgetItem *PlayQueue::current()
{ // return current item being selected
    if (play_list->count() <= 0) return nullptr;
    return play_list->item(current_item_row);
}

QListWidgetItem *PlayQueue::next()
{ // return next item in play queue and update current item to next item
    // and put current item into history stack
    QListWidgetItem* next_item {nullptr};

    if (play_list->count() <= 0) return next_item;

    switch (play_mode) {
    case PQ::PlayMode::Shuffle:
        next_item = nextRandom();
        break;
    case PQ::PlayMode::Single:
        next_item = nextSame();
        break;
    default:
        next_item = nextOrder();
        break;
    }

    history_stack.push(play_list->item(current_item_row));
    current_item_row = play_list->row(next_item);

    return next_item;
}

QListWidgetItem *PlayQueue::previous()
{
    QListWidgetItem* pre_item {nullptr};

    if (play_list->count() <= 0) return pre_item;

    if (!history_stack.empty() && play_list->row(history_stack.top()) == current_item_row)
        history_stack.pop();

    if (history_stack.empty())
        setHistoryStack(current_item_row);

    if (!history_stack.empty())
        pre_item = history_stack.pop();

    // maybe some other operations
    if (pre_item)
        current_item_row = play_list->row(pre_item);

    return pre_item;
}


void PlayQueue::setCurrent_item_row(int newCurrent_item_row)
{
    current_item_row = newCurrent_item_row;
}

QListWidgetItem *PlayQueue::nextOrder()
{
    QListWidgetItem* next_item {nullptr};

    // if (play_list->count() <= 0) return next_item;
    if (!user_added_queue.empty())
    {
        next_item = user_added_queue.dequeue();
    }
    else
    {
        if (default_queue.empty()) updatePlayingQueue(current_item_row+1);
        next_item = default_queue.dequeue();
    }
    return next_item;
}

QListWidgetItem *PlayQueue::nextRandom()
{
    default_queue.clear();
    std::uniform_int_distribution<int> distr(0, play_list->count()-1);
    int next_row = distr(generator);
    return play_list->item(next_row);
}

QListWidgetItem *PlayQueue::nextSame()
{
    return play_list->item(current_item_row);
}




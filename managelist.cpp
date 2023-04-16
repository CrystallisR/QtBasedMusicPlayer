#include "managelist.h"

ManageList::ManageList(QListWidget* init_list, QObject *parent)
    : QObject{parent}
    , item_list(init_list)
{

}

ManageList::~ManageList()
{

}

void ManageList::importToList(const QDir &dir, QString format)
{
    // Match designated format
    static QRegularExpression re(format, QRegularExpression::CaseInsensitiveOption);

    // register imported files
    for (const QFileInfo &file : dir.entryInfoList(QDir::Files))
    {
        if (!re.match(file.fileName()).hasMatch()) continue;
        auto items = item_list->findItems(file.fileName(), Qt::MatchExactly);
        if (!items.isEmpty()) continue;

        QListWidgetItem* item = new QListWidgetItem;
        item->setIcon(QIcon(":/icons/res/music_notec2.png"));
        item->setText(file.fileName());
        item->setData(Qt::UserRole, file.absoluteFilePath());
        item_list->addItem(item);
    }
}

void ManageList::removeSelectedFromList()
{
    QList<QListWidgetItem*> removed_items = item_list->selectedItems();
    for (QListWidgetItem* item: removed_items)
    {
        item_list->takeItem(item_list->row(item));
        delete item;
    }
}

void ManageList::clear()
{
    item_list->clear();
}

int ManageList::getRow(QListWidgetItem *item)
{
    return item_list->row(item);
}

void ManageList::updateUIonItemChange(QListWidgetItem *cur_item, QListWidgetItem *new_item)
{
    cur_item->setSelected(false);
    new_item->setSelected(true);
    item_list->scrollToItem(new_item);
}

void ManageList::saveList(QSettings &settings, QString list_name)
{
    settings.beginWriteArray(list_name);
    for (int row = 0; row < item_list->count(); row++)
    {
        settings.setArrayIndex(row);
        QListWidgetItem* cur_item = item_list->item(row);
        settings.setValue("fileName", cur_item->text());
        settings.setValue("filePath", cur_item->data(Qt::UserRole));
    }
    settings.endArray();
}

void ManageList::loadList(QSettings &settings, QString list_name)
{
    int size = settings.beginReadArray(list_name);
    for (int row = 0; row < size; row++)
    {
        settings.setArrayIndex(row);
        QString file_name = settings.value("fileName").toString();
        QString file_path = settings.value("filePath").toString();

        QListWidgetItem* cur_item = new QListWidgetItem;
        cur_item->setIcon(QIcon(":/icons/res/music_notec2.png"));
        cur_item->setText(file_name);
        cur_item->setData(Qt::UserRole, file_path);
        item_list->addItem(cur_item);
    }
    settings.endArray();
}

void ManageList::setItem_list(QListWidget *newMusic_list)
{
    item_list = newMusic_list;
}

QListWidget *ManageList::getItem_list() const
{
    return item_list;
}



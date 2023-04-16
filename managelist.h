#ifndef MANAGELIST_H
#define MANAGELIST_H

#include <QObject>
#include <QDir>
#include <QSettings>
#include <QListWidget>

QT_BEGIN_NAMESPACE
namespace ML { class ManageList;}
QT_END_NAMESPACE

class ManageList : public QObject
{
    Q_OBJECT

public:
    explicit ManageList(QListWidget* init_list, QObject *parent = nullptr);
    ~ManageList();

    // operations
    void importToList(const QDir& dir, QString format);
    void removeSelectedFromList();
    void clear();
    int getRow(QListWidgetItem* item);

    // ui update
    void updateUIonItemChange(QListWidgetItem* cur_item, QListWidgetItem* new_item);

    // save/load
    void saveList(QSettings& settings, QString list_name);
    void loadList(QSettings& settings, QString list_name);

    // getters & setters
    void setItem_list(QListWidget *newMusic_list);
    QListWidget *getItem_list() const;

private:
    QListWidget* item_list;

};

#endif // MANAGELIST_H

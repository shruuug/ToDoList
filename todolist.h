#pragma once
#pragma execution_character_set("utf-8")
    
#include <QtWidgets/QWidget>
#include <QInputDialog>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonValue>
#include <QFile>
#include "grouptoolbutton.h"
#include "ui_todolist.h"

enum class Priority
{
    No = 0,
    Low = 1,
    Medium = 2,
    High = 3
};

struct ToDo
{
    int id;
    bool check;
    QString title;
    Priority priority;
    QString deadLine;
    QString reminderDate;
    QString description;
    QString location;
    QString remark;
};
Q_DECLARE_METATYPE(ToDo);

class ToDoList : public QWidget
{
    Q_OBJECT

public:
    ToDoList(QWidget* parent = Q_NULLPTR);
    ~ToDoList();
    int m_groupIndex = 1;
    int m_id = 0;
    int m_selectedItemId;

private:
    Ui::ToDoListClass ui;
    void initList();

private slots:
    void onGoHomePage();
    void onGoToDoPage();
    void onCreateToDoItem();
    void onDeleteToDoItem();
    void onAddGroup();
    void onChooseGroup(QString groupname);
    void onShowDetail(QTreeWidgetItem* item, int column);
    std::vector<QTreeWidgetItem*> searchItem(int id);
    void onChangeTitle();
    void onChangeCheckState(int state);
    void onChangePriority(int index);
    void onChangeDeadLine();
    void onChangeReminderDate();
    void onChangeDescription();
    void onChangeLocation();
    void onChangeRemark();
    void onClearDeadLine();
    void onClearReminder();
    void onSort();
};

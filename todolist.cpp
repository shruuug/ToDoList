#include "todolist.h"

ToDoList::ToDoList(QWidget* parent)
    : QWidget(parent)
{
    ui.setupUi(this);
    ui.wdSide->setVisible(false);
    ui.btnSort->setVisible(false);
    ui.tabWidget->setVisible(false);
    ui.cwDeadLine->setSelectedDate(QDate());
    ui.cwReminder->setSelectedDate(QDate());
    ui.wdSideDeadLine->setVisible(false);
    ui.wdSideReminder->setVisible(false);
    ui.widget_3->setVisible(false);
    ui.treeWidget->expandAll();
    ui.treeWidget_2->expandAll();

    ui.treeWidget->setColumnWidth(0, 400);
    ui.treeWidget_2->setColumnWidth(0, 400);

    connect(ui.pushButton_3, &QPushButton::clicked, this, [=]() {ui.tabWidget->setVisible(false); });
    connect(ui.pushButton_5, &QPushButton::clicked, this, [=]() {ui.tabWidget->setVisible(false); });
    connect(ui.btnSort, &QPushButton::clicked, this, [=]() {ui.widget_3->setVisible(ui.widget_3->isHidden()); });
    connect(ui.btnSetDeadLine, &QPushButton::clicked, this, [=]() {ui.wdSideDeadLine->setVisible(ui.wdSideDeadLine->isHidden()); });
    connect(ui.btnSetReminder, &QPushButton::clicked, this, [=]() {ui.wdSideReminder->setVisible(ui.wdSideReminder->isHidden()); });
    connect(ui.btnHideSide, &QPushButton::clicked, this, [=]() {ui.wdSide->setVisible(false); });
    connect(ui.btnShowCalendar, &QPushButton::clicked, this, [=]() {ui.tabWidget->setVisible(ui.tabWidget->isHidden()); });
    connect(ui.treeWidget, &QTreeWidget::itemActivated, this, &ToDoList::onShowDetail);
    connect(ui.treeWidget_2, &QTreeWidget::itemActivated, this, &ToDoList::onShowDetail);

    initList();
}

void ToDoList::initList()
{
    QFile file("./todolist_data.json");
    QByteArray jsonData;
    if (file.open(QIODevice::ReadOnly)) {
        jsonData = file.readAll();
        file.close();
    }

    QJsonDocument jsonDocu = QJsonDocument::fromJson(jsonData);
    if (jsonDocu.isObject()) {
        QJsonObject root = jsonDocu.object();
        QStringList keys = root.keys();
        for (auto& key : keys) {
            QTreeWidgetItem* item = new QTreeWidgetItem();
            ToDo data;
            data.id = ++m_id;

            QJsonValue value = root.value(key);
            QJsonObject subObj = value.toObject();
            QStringList subKeys = subObj.keys();
            for (auto& subKey : subKeys) {
                QJsonValue subValue = subObj.value(subKey);
                if (subKey == "deadLine")
                {
                    data.deadLine = subValue.toString();
                }
                else if (subKey == "description")
                {
                    data.description = subValue.toString();
                }
                else if (subKey == "location")
                {
                    data.location = subValue.toString();
                }
                else if (subKey == "priority")
                {
                    data.priority = (Priority)subValue.toInt();
                }
                else if (subKey == "remark")
                {
                    data.remark = subValue.toString();
                }
                else if (subKey == "reminderDate")
                {
                    data.reminderDate = subValue.toString();
                }
                else if (subKey == "title")
                {
                    data.title = subValue.toString();
                }
            }
            QString topItemString;
            item->setData(0, Qt::UserRole, QVariant::fromValue(data));
            item->setCheckState(0, Qt::Unchecked);
            item->setText(0, item->data(0, Qt::UserRole).value<ToDo>().title);
            item->setText(1, item->data(0, Qt::UserRole).value<ToDo>().deadLine);

            if (data.priority == Priority::High)
                item->setIcon(0, QIcon(QString("images/High.png")));
            else if (data.priority == Priority::Medium)
                item->setIcon(0, QIcon(QString("images/Mid.png")));
            else if (data.priority == Priority::Low)
                item->setIcon(0, QIcon(QString("images/Low.png")));
            else if (data.priority == Priority::No)
                item->setIcon(0, QIcon(QString()));

            qint64 diffDays = QDate::currentDate().daysTo(QDate::fromString(QDate::currentDate().toString("yyyy") + "-" + data.deadLine, "yyyy-MM-dd dddd"));

            if (diffDays > 0 && diffDays <= 7)
            {
                topItemString = QString("未来七天");
            }
            else if (diffDays > 7)
            {
                topItemString = QString("以后");
            }
            else
            {
                topItemString = QString("未安排");
            }
            for (int i = 0; i < ui.treeWidget->topLevelItemCount(); ++i)
            {
                QTreeWidgetItem* currItem = ui.treeWidget->topLevelItem(i);
                if (currItem->text(0) == topItemString)
                {
                    currItem->addChild(item);
                }
            }

            QTime time = QTime::currentTime();

            //在我的待办添加相应待办
            QTreeWidgetItem* cloneItem = item->clone();
            ui.treeWidget_2->topLevelItem(0)->addChild(cloneItem);
        }
    }
}

//json文件保存数据
ToDoList::~ToDoList()
{
    QJsonObject listRoot;

    for (int i = 0; i < ui.treeWidget_2->topLevelItem(0)->childCount(); ++i)
    {
        QJsonObject todo;
        todo.insert("title", ui.treeWidget_2->topLevelItem(0)->child(i)->data(0, Qt::UserRole).value<ToDo>().title);
        todo.insert("priority", static_cast<int>(ui.treeWidget_2->topLevelItem(0)->child(i)->data(0, Qt::UserRole).value<ToDo>().priority));
        todo.insert("deadLine", ui.treeWidget_2->topLevelItem(0)->child(i)->data(0, Qt::UserRole).value<ToDo>().deadLine);
        todo.insert("reminderDate", ui.treeWidget_2->topLevelItem(0)->child(i)->data(0, Qt::UserRole).value<ToDo>().reminderDate);
        todo.insert("description", ui.treeWidget_2->topLevelItem(0)->child(i)->data(0, Qt::UserRole).value<ToDo>().description);
        todo.insert("location", ui.treeWidget_2->topLevelItem(0)->child(i)->data(0, Qt::UserRole).value<ToDo>().location);
        todo.insert("remark", ui.treeWidget_2->topLevelItem(0)->child(i)->data(0, Qt::UserRole).value<ToDo>().remark);
        listRoot.insert(QDate::currentDate().toString("yyyy-MM-dd") + "_" + QString::number(i), todo);
    }

    QFile file("./todolist_data.json");
    QJsonDocument jsonDocu(listRoot);
    QByteArray jsonData = jsonDocu.toJson();
    if (file.open(QIODevice::WriteOnly)) 
    {
        file.write(jsonData);
        file.close();
    }

}

void ToDoList::onGoHomePage()
{
    ui.stackedWidget->setCurrentIndex(0);
    ui.lblTitle->setText("我的首页");
    ui.btnSort->setVisible(false);
    ui.tabWidget->setVisible(false);
    ui.widget_3->setVisible(false);
}

void ToDoList::onGoToDoPage()
{
    ui.stackedWidget->setCurrentIndex(1);
    ui.lblTitle->setText("我的待办");
    ui.btnSort->setVisible(true);
    ui.tabWidget->setVisible(false);
    ui.widget_3->setVisible(false);
}

void ToDoList::onCreateToDoItem()
{
    if (ui.leCreateToDo->text() == "") return;
    QTreeWidgetItem* item = new QTreeWidgetItem(QStringList(ui.leCreateToDo->text()));
    ToDo data;
    data.id = ++m_id;
    data.title = item->text(0);
    data.deadLine = ui.cwDeadLine->selectedDate().toString("MM-dd dddd");
    data.reminderDate = ui.cwReminder->selectedDate().toString("MM-dd") + " " + ui.timeEdit->time().toString("hh:mm");
    data.priority = Priority::No;
    QTreeWidgetItem* currentItem;
    QString topItemString;
    item->setData(0, Qt::UserRole, QVariant::fromValue(data));
    item->setCheckState(0, Qt::Unchecked);
    item->setText(1, item->data(0, Qt::UserRole).value<ToDo>().deadLine);
    qint64 diffDays = QDate::currentDate().daysTo(ui.cwDeadLine->selectedDate());
    if (diffDays > 0 && diffDays <= 7)
    {
        topItemString = QString("未来七天");
    }
    else if (diffDays > 7)
    {
        topItemString = QString("以后");
    }
    else
    {
        topItemString = QString("未安排");
    }
    for (int i = 0; i < ui.treeWidget->topLevelItemCount(); ++i)
    {
        currentItem = ui.treeWidget->topLevelItem(i);
        if (currentItem->text(0) == topItemString)
        {
            currentItem->addChild(item);
        }
    }

    ui.leCreateToDo->clear();
    ui.leCreateToDo->clearFocus();

    //在我的待办添加相应待办
    QTreeWidgetItem* cloneItem = item->clone();
    ui.treeWidget_2->topLevelItem(0)->addChild(cloneItem);
}

void ToDoList::onDeleteToDoItem()
{
    std::vector<QTreeWidgetItem*> items = searchItem(m_selectedItemId);
    if (items.empty())
        return;

    for (int i = 0; i < 2; ++i)
    {
        QTreeWidgetItem* selectedItem = items[i];
        delete selectedItem;
    }

    ui.wdSide->hide();
}

void ToDoList::onShowDetail(QTreeWidgetItem* item, int column)
{
    m_selectedItemId = item->data(0, Qt::UserRole).value<ToDo>().id;
    ui.wdSide->setVisible(true);
    ui.leTitle->setText(item->data(0, Qt::UserRole).value<ToDo>().title);
    ui.checkBox->setCheckState(item->checkState(0));
    ui.cmbPriority->setCurrentIndex((int)item->data(0, Qt::UserRole).value<ToDo>().priority);
    ui.wdSideDeadLine->setVisible(false);
    ui.cwSideDeadLine->setSelectedDate(QDate::fromString(item->data(0, Qt::UserRole).value<ToDo>().deadLine));
    ui.leDeadLine->setText(item->data(0, Qt::UserRole).value<ToDo>().deadLine);
    ui.wdSideReminder->setVisible(false);
    ui.cwSideReminder->setSelectedDate(QDate::fromString(item->data(0, Qt::UserRole).value<ToDo>().reminderDate));
    ui.leReminder->setText(item->data(0, Qt::UserRole).value<ToDo>().reminderDate);
    ui.leDescription->setText(item->data(0, Qt::UserRole).value<ToDo>().description);
    ui.leLocation->setText(item->data(0, Qt::UserRole).value<ToDo>().location);
    ui.leRemark->setText(item->data(0, Qt::UserRole).value<ToDo>().remark);
}

std::vector<QTreeWidgetItem*> ToDoList::searchItem(int id)
{
    std::vector<QTreeWidgetItem*> itemList;

    for (int i = 0; i < 3; ++i)
    {
        for (int j = 0; j < ui.treeWidget->topLevelItem(i)->childCount(); ++j)
        {
            if (ui.treeWidget->topLevelItem(i)->child(j)->data(0, Qt::UserRole).value<ToDo>().id == m_selectedItemId)
            { 
                itemList.push_back(ui.treeWidget->topLevelItem(i)->child(j));
            }
        }
    }

    for (int i = 0; i < ui.treeWidget_2->topLevelItem(0)->childCount(); ++i)
    {
        if (ui.treeWidget_2->topLevelItem(0)->child(i)->data(0, Qt::UserRole).value<ToDo>().id == m_selectedItemId)
        {
            itemList.push_back(ui.treeWidget_2->topLevelItem(0)->child(i));
        }
    }

    return itemList;
}

void ToDoList::onChangeTitle()
{
    std::vector<QTreeWidgetItem*> items = searchItem(m_selectedItemId);
    if (items.empty())
    {
        ui.leTitle->clear();
        ui.leTitle->clearFocus();
        return;
    }
    for (int i = 0; i < 2; ++i)
    {
        QTreeWidgetItem* selectedItem = items[i];
        ToDo data = selectedItem->data(0, Qt::UserRole).value<ToDo>();
        data.title = ui.leTitle->text();
        selectedItem->setData(0, Qt::UserRole, QVariant::fromValue(data));
        selectedItem->setText(0, ui.leTitle->text());
    }
    ui.leTitle->clearFocus();
}

void ToDoList::onChangePriority(int index)
{   
    std::vector<QTreeWidgetItem*> itemVector = searchItem(m_selectedItemId);
    if (itemVector.empty()) return;
    for (int i = 0; i < 2; ++i)
    {
        QTreeWidgetItem* selectedItem = itemVector[i];
        ToDo data = selectedItem->data(0, Qt::UserRole).value<ToDo>();
        data.priority = (Priority)index;
        selectedItem->setData(0, Qt::UserRole, QVariant::fromValue(data));
        if ((Priority)index == Priority::High)
            selectedItem->setIcon(0, QIcon(QString("images/High.png")));
        if ((Priority)index == Priority::Medium)
            selectedItem->setIcon(0, QIcon(QString("images/Mid.png")));
        if ((Priority)index == Priority::Low)
            selectedItem->setIcon(0, QIcon(QString("images/Low.png")));
        if ((Priority)index == Priority::No)
            selectedItem->setIcon(0, QIcon(QString()));
    }
}

void ToDoList::onChangeDeadLine()
{
    std::vector<QTreeWidgetItem*> itemVector = searchItem(m_selectedItemId);
    if (itemVector.empty())
    {
        ui.cwSideDeadLine->setSelectedDate(QDate::currentDate());
        return;
    }
    QString deadLine = ui.cwSideDeadLine->selectedDate().toString("MM-dd dddd");
    for (int i = 0; i < 2; ++i)
    {
        QTreeWidgetItem* selectedItem = itemVector[i];
        ToDo data = selectedItem->data(0, Qt::UserRole).value<ToDo>();
        data.deadLine = deadLine;
        selectedItem->setData(0, Qt::UserRole, QVariant::fromValue(data));
        selectedItem->setText(1, deadLine);
        ui.leDeadLine->setText(deadLine);
    }

    QTreeWidgetItem* selectedItem = itemVector[0];
    QTreeWidgetItem* cloneItem = selectedItem->clone();
    delete selectedItem;
    QString topItemString;
    qint64 diffDays = QDate::currentDate().daysTo(ui.cwSideDeadLine->selectedDate());
    if (diffDays > 0 && diffDays <= 7)
    {
        topItemString = QString("未来七天");
    }
    else if (diffDays > 7)
    {
        topItemString = QString("以后");
    }
    else
    {
        topItemString = QString("未安排");
    }

    for (int i = 0; i < ui.treeWidget->topLevelItemCount(); ++i)
    {
        QTreeWidgetItem* currTopItem = ui.treeWidget->topLevelItem(i);
        if (currTopItem->text(0) == topItemString)
        {
            currTopItem->addChild(cloneItem);
        }
    }

    ui.wdSideDeadLine->hide();
}

void ToDoList::onChangeReminderDate()
{
    std::vector<QTreeWidgetItem*> itemVector = searchItem(m_selectedItemId);
    if (itemVector.empty())
    {
        ui.cwSideReminder->setSelectedDate(QDate::currentDate());
        return;
    }
    QString reminder = ui.cwSideReminder->selectedDate().toString("MM-dd") + " " + ui.timeEdit_2->time().toString("hh:mm");
    for (int i = 0; i < 2; ++i)
    {
        QTreeWidgetItem* selectedItem = itemVector[i];
        ToDo data = selectedItem->data(0, Qt::UserRole).value<ToDo>();
        data.reminderDate = reminder;
        selectedItem->setData(0, Qt::UserRole, QVariant::fromValue(data));
    }
    ui.leReminder->setText(reminder);
    ui.wdSideReminder->hide();
}

void ToDoList::onChangeCheckState(int state)
{
    std::vector<QTreeWidgetItem*> itemVector = searchItem(m_selectedItemId);
    if (itemVector.empty()) return;
    for (int i = 0; i < 2; ++i)
    {
        QTreeWidgetItem* selectedItem = itemVector[i];
        selectedItem->setCheckState(0, Qt::CheckState(state));
    }
}

void ToDoList::onChangeDescription()
{
    std::vector<QTreeWidgetItem*> itemVector = searchItem(m_selectedItemId);
    if (itemVector.empty())
    {
        ui.leDescription->clear();
        ui.leDescription->clearFocus();
        return;
    }
    for (int i = 0; i < 2; ++i)
    {
        
        QTreeWidgetItem* selectedItem = itemVector[i];
        ToDo data = selectedItem->data(0, Qt::UserRole).value<ToDo>();
        QString description = ui.leDescription->text();
        data.description = description;
        selectedItem->setData(0, Qt::UserRole, QVariant::fromValue(data));
    }
    ui.leDescription->clearFocus();
}

void ToDoList::onChangeLocation()
{
    std::vector<QTreeWidgetItem*> itemVector = searchItem(m_selectedItemId);
    if (itemVector.empty())
    {
        ui.leLocation->clear();
        ui.leLocation->clearFocus();
        return;
    }
    for (int i = 0; i < 2; ++i)
    {
        QTreeWidgetItem* selectedItem = itemVector[i];
        ToDo data = selectedItem->data(0, Qt::UserRole).value<ToDo>();
        QString location = ui.leLocation->text();
        data.location = location;
        selectedItem->setData(0, Qt::UserRole, QVariant::fromValue(data)); 
    }
    ui.leLocation->clearFocus();
}

void ToDoList::onChangeRemark()
{
    std::vector<QTreeWidgetItem*> itemVector = searchItem(m_selectedItemId);
    if (itemVector.empty())
    {
        ui.leRemark->clear();
        ui.leRemark->clearFocus();
        return;
    }
    for (int i = 0; i < 2; ++i)
    {
        QTreeWidgetItem* selectedItem = itemVector[i];
        ToDo data = selectedItem->data(0, Qt::UserRole).value<ToDo>();
        QString remark = ui.leRemark->text();
        data.remark = remark;
        selectedItem->setData(0, Qt::UserRole, QVariant::fromValue(data));
    }
    ui.leRemark->clearFocus();
}

void ToDoList::onClearDeadLine()
{
    std::vector<QTreeWidgetItem*> itemVector = searchItem(m_selectedItemId);
    if (itemVector.empty()) return;
    for (int i = 0; i < 2; ++i)
    {
        QTreeWidgetItem* selectedItem = itemVector[i];
        ToDo data = selectedItem->data(0, Qt::UserRole).value<ToDo>();
        data.deadLine = QDate().toString("MM-dd dddd");
        selectedItem->setData(0, Qt::UserRole, QVariant::fromValue(data));
        selectedItem->setText(1, data.deadLine);
    }
    ui.leDeadLine->setText("");
}

void ToDoList::onClearReminder()
{
    for (int i = 0; i < 2; ++i)
    {
        std::vector<QTreeWidgetItem*> itemVector = searchItem(m_selectedItemId);
        if (itemVector.empty()) return;
        QTreeWidgetItem* selectedItem = itemVector[i];
        ToDo data = selectedItem->data(0, Qt::UserRole).value<ToDo>();
        data.reminderDate = QDate().toString("MM-dd") + " " + QTime().toString("hh:mm");
        selectedItem->setData(0, Qt::UserRole, QVariant::fromValue(data));
    }
    ui.leReminder->setText("");
}

void ToDoList::onAddGroup()
{
    bool ok = false;
    QString groupName = QInputDialog::getText(this, "新建待办分组", "请输入分组名", QLineEdit::Normal, "", &ok);
    if (groupName == "") return;
    GroupToolButton* btn = new GroupToolButton(++m_groupIndex, groupName, this);
    
    ui.verticalLayout->addWidget(btn);
    (void)connect(btn, &GroupToolButton::myClicked, this, &ToDoList::onChooseGroup);

    QWidget* w = new QWidget;
    QTreeWidget* tw = new QTreeWidget;
    QTreeWidgetItem* item = new QTreeWidgetItem(QStringList(groupName + "组的待办"));
    QHBoxLayout* layout = new QHBoxLayout;
    w->resize(650, 650);
    tw->resize(650, 650);
    tw->setHeaderHidden(true);
    tw->addTopLevelItem(item);
    layout->addWidget(tw);
    ui.stackedWidget->addWidget(w);
    w->move(0, 0);
    w->setLayout(layout);
    tw->move(0, 0);
}

void ToDoList::onChooseGroup(QString groupName)
{
    //GroupToolButton* button = qobject_cast<GroupToolButton*>(sender());  
    ui.lblTitle->setText(groupName);                       
    ui.stackedWidget->setCurrentIndex(m_groupIndex);       
}

void ToDoList::onSort()
{
    QList<QTreeWidgetItem*> items;
    QTreeWidgetItem* temp;
    for (int i = 0; i < ui.treeWidget_2->topLevelItem(0)->childCount(); ++i)
    {
        items.push_back(ui.treeWidget_2->topLevelItem(0)->child(i));
    }
    ui.treeWidget_2->topLevelItem(0)->takeChildren();
    if (ui.btnByDate->isChecked())
    {
        for (int i = 0; i < items.size() - 1; i++)
        {
            for (int j = 0; j < items.size() - i - 1; j++)
            {
                if (items[j + 1]->data(0, Qt::UserRole).value<ToDo>().deadLine < items[j]->data(0, Qt::UserRole).value<ToDo>().deadLine)
                {
                    temp = items[j];
                    items[j] = items[j + 1];
                    items[j + 1] = temp;
                }
            }
        }
    }
    else if (ui.btnByPriority->isChecked())
    {
        for (int i = 0; i < items.size() - 1; i++)
        {
            for (int j = 0; j < items.size() - i - 1; j++)
            {
                if (items[j + 1]->data(0, Qt::UserRole).value<ToDo>().priority > items[j]->data(0, Qt::UserRole).value<ToDo>().priority)
                {
                    temp = items[j];
                    items[j] = items[j + 1];
                    items[j + 1] = temp;
                }
            }
        }
    }
    ui.treeWidget_2->topLevelItem(0)->addChildren(items);
    //ui.widget_3->setVisible(false);
}
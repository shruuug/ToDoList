#include "todolist.h"
#include "grouptoolbutton.h"
#include <QtWidgets/QApplication>
#include <QSharedMemory>

int main(int argc, char* argv[])
{
    QApplication a(argc, argv);

    //��ֹ����࿪
    QSharedMemory sharedMemory(qApp->applicationName());    // ���ð󶨵Ĺ����ڴ�ε�keyֵ
    if (sharedMemory.attach()) {
        return 1;
    }
    else {
        sharedMemory.create(1); // ����1byte��С�Ĺ����ڴ��
    }

    ToDoList w;
    w.show();
    return a.exec();
}

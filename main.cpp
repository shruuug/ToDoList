#include "todolist.h"
#include "grouptoolbutton.h"
#include <QtWidgets/QApplication>
#include <QSharedMemory>

int main(int argc, char* argv[])
{
    QApplication a(argc, argv);

    //禁止程序多开
    QSharedMemory sharedMemory(qApp->applicationName());    // 设置绑定的共享内存段的key值
    if (sharedMemory.attach()) {
        return 1;
    }
    else {
        sharedMemory.create(1); // 创建1byte大小的共享内存段
    }

    ToDoList w;
    w.show();
    return a.exec();
}

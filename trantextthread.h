#ifndef TRANTEXTTHREAD_H
#define TRANTEXTTHREAD_H
#include <QThread>
#include <QString>
#include <QClipboard>
#include <QApplication>
#include "mydb.h"
#include <windows.h>
#include <iostream>
#include <string>

class tranTextThread: public QThread{
    Q_OBJECT
public:
    tranTextThread(QObject *parent=nullptr);
    ~tranTextThread();
protected:
    void run();
private:
    HHOOK keyHook;
    static LRESULT CALLBACK keyHookEvent(int code, WPARAM wParam, LPARAM lParam);
};
void toClipboard(const std::string &s);
bool check(const QString& input, MyDB* db);
void keyDown(int vk);
void keyUp(int vk);
void CtrlV();
void Del(int n = 1);
#endif // TRANTEXTTHREAD_H

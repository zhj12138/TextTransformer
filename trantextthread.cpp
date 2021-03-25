#include "trantextthread.h"
void toClipboard(const std::string &s) {
    OpenClipboard(nullptr);
    EmptyClipboard();
    HANDLE hg = GlobalAlloc(GMEM_MOVEABLE, s.size()+1);
    if (!hg) {
        CloseClipboard();
        return;
    }
    memcpy(GlobalLock(hg), s.c_str(), s.size()+1);
    SetClipboardData(CF_TEXT, hg);//dfasheloo asdfhell asdf:pensil2:
    GlobalUnlock(hg);
    CloseClipboard();
    GlobalFree(hg);
}
void keyDown(int vk){
    keybd_event(vk,0,0,0);
}
void keyUp(int vk){
    keybd_event(vk, 0, KEYEVENTF_KEYUP, 0);
}
void CtrlV(){
    keyDown(VK_CONTROL);
    keyDown('V');
    keyUp(VK_CONTROL);
    keyUp('V');
}
void Del(int n){
    for(int i = 0; i < n; i++){
        keyDown(VK_BACK);
        keyUp(VK_BACK);
    }
}
tranTextThread::tranTextThread(QObject *parent): QThread(parent){

}
tranTextThread::~tranTextThread(){
    UnhookWindowsHookEx(keyHook);
}
bool check(const QString& input, MyDB* db){
//    qDebug() << "checking" << input;
    auto piece_abbrs = db->getAllActivePieceNames();
    for(const auto& abbr: piece_abbrs){
//        qDebug() << "abbr" << abbr;
        if(input.endsWith(abbr)){
            int size = abbr.size();
            auto piece = db->getPieceByAbbr(abbr);
            auto content = piece.getContent();
//            qDebug() << "content: " << content;//     connect();
//            qApp->clipboard()->setText(content);
            toClipboard(content.toStdString());
            Del(size);//
            CtrlV();//
            return true;//sdsf
        }
    }
    return false;
}

LRESULT CALLBACK tranTextThread::keyHookEvent(int code, WPARAM wParam, LPARAM lParam) {
//    qDebug() << "call back" << code << endl;//
    static QString input;
    static auto db = new MyDB();//jrdlfkjsldkfjlsdkfjklmfpdfa
    static bool shiftKey = false;//
    bool mod = false;
    if (code < 0)
        return CallNextHookEx(nullptr, code, wParam, lParam);
//    qDebug() << wParam << " " << WM_KEYDOWN << endl;
    if(wParam == WM_KEYUP){
        auto i = ((KBDLLHOOKSTRUCT *) lParam)->vkCode;
        if(i == 160) shiftKey = false;
    }
    if (wParam == WM_KEYDOWN) {
        auto i = ((KBDLLHOOKSTRUCT *) lParam)->vkCode;
//        qDebug() << i;hj
        bool active = false;
        if(i == VK_TAB && db->Tab()){
            active = true;
        }else if(i == VK_SPACE && db->Space()){
            active = true;
        }else if(i == VK_RETURN && db->Enter()){
            active = true;
        }else if(i >= 48 && i <= 57){//数字
            if(!shiftKey){
                input.append(char((i - 48) + '0'));
            }else{
                input.append(char(")!@#$%^&*("[i-48]));
            }
        }else if(i >= 96 && i <= 105){
            input.append(char((i - 96) + '0'));
        }else if(i >= 65 && i <= 90){//字母
            if(shiftKey){
                input.append(char((i-65) + 'A'));
            }else{
                input.append(char((i-65) + 'a'));
            }
        }else if(i == 192){
            if(shiftKey){
                input.append('~');
            }else{
                input.append('`');
            }
        }else if(i == 189){
            if(shiftKey){
                input.append('_');
            }else{
                input.append('-');
            }
        }else if(i == 187){
            if(shiftKey){
                input.append('+');
            }else{
                input.append('=');
            };
        }else if(i == 219){
            if(shiftKey){
                input.append('{');
            }else{
                input.append('[');
            }
        }else if(i == 221){
            if(shiftKey){
                input.append('}');
            }else{
                input.append(']');
            }
        }else if(i == 220){
            if(shiftKey){
                input.append('|');
            }else{
                input.append(char(92));
            }
        }else if(i == 186){
            if(shiftKey){
                input.append(':');
            }else{
                input.append(';');
            }
        }else if(i == 222){
            if(shiftKey){
                input.append('"');
            }else{
                input.append('\'');
            }
        }else if(i == 188){
            if(shiftKey){
                input.append('<');
            }else{
                input.append(',');
            }
        }else if(i == 190){
            if(shiftKey){
                input.append('>');
            }else{
                input.append('.');
            }
        }else if(i == 191){
            if(shiftKey){
                input.append('?');
            }else{
                input.append('/');
            }
        }else if(i == 160){
            shiftKey = true;
        }
        if(input.size() > 160){
            input = input.mid(80);
        }
//        qDebug() << input;
        if(active){
            mod = check(input, db);

        }
    }
    return mod ? 1 : CallNextHookEx(nullptr, code, wParam, lParam);
}

void tranTextThread::run(){
    keyHook = SetWindowsHookEx(WH_KEYBOARD_LL, keyHookEvent, nullptr, 0);
    MSG msg;
//    qDebug() << "run" << endl;
    while (GetMessage(&msg, nullptr, 0, 0)){

    }
}

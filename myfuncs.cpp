#include "myfuncs.h"

QString setToString(QSet<QString> s){
    QString ret;
    for(const auto& i: s){
        ret += QString(i);
        ret += ", ";
    }
    if(!ret.size()) return ret;
    return ret.mid(0, ret.size()-2);
}

QString setToString(QSet<int> s){
    QString ret;
    for(const auto& i: s){
        ret += QString::number(i);
        ret += ", ";
    }
    if(!ret.size()) return ret;
    return ret.mid(0, ret.size()-2);
}
void stringToSet(const QString& s, QSet<QString>& sset){
    auto split_list = s.split(",");
    for(auto i: split_list){
        if(i != ""){
            sset.insert(i.trimmed());
        }
    }
}
void stringToSet(const QString &s, QSet<int> &sset){
    sset.clear();
    auto split_list = s.split(",");
    for(auto i: split_list){
        if(i != ""){
            sset.insert(i.trimmed().toInt());
        }
    }
}
QString getShortenString(const QString &str, int n){
    if(str.size() > n){
        QString temp = str.mid(0, n);
        temp.append("...");
        return temp;
    }
    return str;
}
bool isLegal(const QString &str){
    if(std::find(str.begin(), str.end(), ' ') != str.end()){
        return false;
    }else if(std::find(str.begin(), str.end(), ',') != str.end()){
        return false;
    }
    return true;
}

#include <QString>
#include <QSet>
#include <algorithm>

QString setToString(QSet<QString> s);
QString setToString(QSet<int> s);
void stringToSet(const QString& s, QSet<QString>& sset);
void stringToSet(const QString &s, QSet<int> &sset);
QString getShortenString(const QString& str, int n);
bool isLegal(const QString& str);

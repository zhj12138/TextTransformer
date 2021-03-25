#ifndef TRANSFORMPIECE_H
#define TRANSFORMPIECE_H
#include <QString>
#include <QSet>

class MyDB;
class TransformPiece
{
    friend class MyDB;
public:
    TransformPiece() = default;
    TransformPiece(int id, const QString& abbr, const QString& con, const QSet<int>& vec = {}, const QString& desp = "", bool active = true):
        ID(id), abbreviation(abbr), content(con), groups(vec), description(desp), is_active(active) { }
    void setAbbreviation(const QString& abbr) { abbreviation = abbr; }
    void setActive(bool active) { is_active = active; }
    void setContent(const QString& con) { content = con; }
    void setDiscription(const QString& desp) { description = desp; }
    void setGroups(QSet<int> vec) { groups = vec; }
    void addGroup(int group_id) { groups.insert(group_id); }
    void delGroup(int group_id) { groups.erase(std::find(groups.begin(), groups.end(), group_id)); }
    int getID() const { return ID; }
    QString getContent() const { return content; }
    QString getAbbreviation() const { return abbreviation; }
    QString getDiscription() const { return description; }
    QSet<int> getGroups() const { return groups; }
    bool isActive() const { return is_active; }
private:
    int ID;
    QString abbreviation;
    QString content;
    QSet<int> groups;
    QString description;
    bool is_active;
};

#endif // TRANSFORMPIECE_H

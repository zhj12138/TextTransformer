#ifndef TRANSFORMGROUP_H
#define TRANSFORMGROUP_H
#include <QString>
#include <QSet>
#include <algorithm>

class MyDB;
class TransformGroup
{
    friend class MyDB;
public:
    TransformGroup() = default;
    TransformGroup(int id, const QString& name, const QSet<int> &vec = {}, bool active = false):
        ID(id), group_name(name), pieces(vec), is_active(active) { }
    TransformGroup(int id): TransformGroup(id, "") { }
    void setName(const QString& name){ group_name = name;}
    void setPieces(const QSet<int>& vec){ pieces = vec; }
    void setActive(bool active) { is_active = active; }
    void addPiece(int piece) { pieces.insert(piece); }
    void deletePiece(int piece);
    int getID() const { return ID; }
    QString getName() const { return group_name; }
    QSet<int> getPieces() const { return pieces; }
    bool isActive() const { return is_active; }


private:
    int ID;
    QString group_name;
    QSet<int> pieces;
    bool is_active;
};

#endif // TRANSFORMGROUP_H

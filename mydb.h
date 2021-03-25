#ifndef MYDB_H
#define MYDB_H
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QString>
#include <QDebug>
#include <QVector>
#include <QSet>
#include "transformgroup.h"
#include "transformpiece.h"
#include "myfuncs.h"

class MyDB
{
public:
    MyDB();
    QVector<TransformPiece> getAllPieces();
    QVector<TransformGroup> getAllGroups();
    TransformPiece getPieceByAbbr(const QString& piece_abbr);
    TransformGroup getGroupByName(const QString& group_name);
    TransformPiece getPieceByID(int ID);
    TransformGroup getGroupByID(int ID);
    QSet<QString> getAllGroupNames();
    QSet<QString> getAllPieceNames();
    QSet<QString> getPieceNamesByGroupID(int ID);
    QSet<QString> getGroupNamesByPieceID(int ID);
    QString getGroupNamesByGroupID(int ID);
    QString getPieceNamesByPieceID(int ID);
    QVector<TransformPiece> getPiecesByGroupID(int ID);
    QVector<TransformPiece> getPiecesByGroupName(const QString& gname);
    TransformPiece addPiece(const QString& abbr, const QString& con, const QSet<int> &vec = {}, const QString& desp = "", bool active = true);
    TransformGroup addGroup(const QString& name, const QSet<int> &vec = {}, bool active = false);
    QSet<QString> getAllActiveGroups();
    QSet<QString> getAllInActiveGroups();
    QSet<QString> getAllActivePieceNames();
    bool hasPieceName(const QString& pname);
    bool hasGroupName(const QString& gname);
    bool isAllActive();
    bool Enter();
    bool Space();
    bool Tab();
    void setAllActive(bool active);
    void setEnter(bool enter);
    void setSpace(bool space);
    void setTab(bool tab);
    void updatePiece(const TransformPiece& piece);
    void updateGroup(const TransformGroup& group);
    void deletePieceByAbbr(const QString& abbr);
    void deleteGroupByName(const QString& name);
    void deletePieceByID(int ID);
    void deleteGroupByID(int ID);
private:
    QString db_name = "info.db";
    QString piece_tb_name = "piece";
    QString group_tb_name = "mygroup";
    QString id_tb_name = "myids";
    QString arg_tb_name = "args";
    QSqlDatabase db;
    void connect();
    void close();
    void createPieceTable();
    void createGroupTable();
    void createIDTable();
    void createArgTable();
    int allocPieceID();
    int allocGroupID();
    void setArgTable(bool ok, QString col_name);
};

#endif // MYDB_H

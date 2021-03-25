#include "mydb.h"

MyDB::MyDB(){
//    if (!QSqlDatabase::contains("qt_sql_default_connection")){//不存在则新建一个数据库
//        db = QSqlDatabase::addDatabase("QSQLITE");
//        db.setDatabaseName(db_name);/hkjdfa
//    }
    if (QSqlDatabase::contains("qt_sql_default_connection")){
        db = QSqlDatabase::database("qt_sql_default_connection");
    }else{
        db = QSqlDatabase::addDatabase("QSQLITE");
        db.setDatabaseName(db_name);
    }
    createPieceTable();
    createGroupTable();
    createIDTable();
    createArgTable();
}
void MyDB::connect(){
    if(!db.open()){
        qDebug() << "Error: Failed to open db" << db.lastError();
    }
}

void MyDB::close(){
    db.close();
}
void MyDB::createPieceTable(){
    connect();
    QSqlQuery query;
    query.prepare(QString("CREATE TABLE IF NOT EXISTS %1("
                  "id integer NOT NULL, "
                  "abbr text NOT NULL, "
                  "content text NOT NULL, "
                  "groups text, "
                  "description text, "
                  "is_active BOOLEAN NOT NULL, "
                  "PRIMARY KEY(id))").arg(piece_tb_name));
//    query.addBindValue(piece_tb_name);
    if(!query.exec()){
        qDebug() << "Error: FAIL TO CREATE TABLE piece " << query.lastError();
    }
    close();
}
void MyDB::createGroupTable(){
    connect();
    QSqlQuery query;
    query.prepare(QString("CREATE TABLE IF NOT EXISTS %1 ("
                  "id integer NOT NULL, "
                  "name text, "
                  "pieces text, "
                  "is_active BOOLEAN NOT NULL, "
                  "PRIMARY KEY(id))").arg(group_tb_name));
//    query.addBindValue(group_tb_name);
    if(!query.exec()){
        qDebug() << "Error: FAIL TO CREATE TABLE group " << query.lastError();
    }
    close();
}
void MyDB::createIDTable(){
    connect();
    QSqlQuery query;
    query.prepare(QString("CREATE TABLE IF NOT EXISTS %1 ("
                  "piece_id integer NOT NULL, "
                  "group_id integer NOT NULL)").arg(id_tb_name));
//    query.addBindValue(id_tb_name);
    if(!query.exec()){
        qDebug() << "Error: FAIL TO CREATE TABLE myids " << query.lastError();
    }
    query.exec(QString("SELECT COUNT(*) FROM %1").arg(id_tb_name));
    while(query.next()){
        int num = query.value(0).toInt();
        if(!num){
            query.exec(QString("INSERT INTO %1 (piece_id, group_id) VALUES (%2, %3)").arg(id_tb_name, "1", "1"));
        }
    }
    close();
}
void MyDB::createArgTable(){
    connect();
    QSqlQuery query;
    query.prepare(QString("CREATE TABLE IF NOT EXISTS %1 ("
                  "all_active BOOLEAN NOT NULL, "
                  "tab BOOLEAN NOT NULL, "
                  "space BOOLEAN NOT NULL, "
                  "enter BOOLEAN NOT NULL)").arg(arg_tb_name));
//    query.addBindValue(id_tb_name);
    if(!query.exec()){
        qDebug() << "Error: FAIL TO CREATE TABLE arg " << query.lastError();
    }
    query.exec(QString("SELECT COUNT(*) FROM %1").arg(arg_tb_name));
    while(query.next()){
        int num = query.value(0).toInt();
        if(!num){
            query.prepare(QString("INSERT INTO %1 (all_active, tab, space, enter) VALUES (?, ?, ?, ?)").arg(arg_tb_name));
            query.addBindValue(true);
            query.addBindValue(true);
            query.addBindValue(true);
            query.addBindValue(false);
            if(!query.exec()){
                qDebug() << "Error: Fail to init table arg" << query.lastError();
            }
        }
    }
    close();
}
bool MyDB::isAllActive(){
    connect();
    QSqlQuery query;
    bool active;
    query.exec(QString("SELECT all_active FROM %1").arg(arg_tb_name));
    while(query.next()){
        active = query.value(0).toBool();
    }
    if(!query.exec()){
        qDebug() << "Error: Fail to get whether all_active " << query.lastError();
    }
    close();
    return active;
}
bool MyDB::Tab(){
    connect();
    QSqlQuery query;
    bool tab;
    query.exec(QString("SELECT tab FROM %1").arg(arg_tb_name));
    while(query.next()){
        tab = query.value(0).toBool();
    }
    if(!query.exec()){
        qDebug() << "Error: Fail to get whether tab " << query.lastError();
    }
    close();
    return tab;
}
bool MyDB::Space(){
    connect();
    QSqlQuery query;
    bool space;
    query.exec(QString("SELECT space FROM %1").arg(arg_tb_name));
    while(query.next()){
        space = query.value(0).toBool();
    }
    if(!query.exec()){
        qDebug() << "Error: Fail to get whether space " << query.lastError();
    }
    close();
    return space;
}
bool MyDB::Enter(){
    connect();
    QSqlQuery query;
    bool enter;
    query.exec(QString("SELECT enter FROM %1").arg(arg_tb_name));
    while(query.next()){
        enter = query.value(0).toBool();
    }
    if(!query.exec()){
        qDebug() << "Error: Fail to get whether enter " << query.lastError();
    }
    close();
    return enter;
}
void MyDB::setArgTable(bool ok, QString col_name){
    connect();
    QSqlQuery query;
    query.prepare(QString("UPDATE %1 SET %2=?").arg(arg_tb_name, col_name));
    query.addBindValue(ok);
    if(!query.exec()){
        qDebug() << "Error: Fail to get set col" << col_name << query.lastError();
    }
    close();
}

void MyDB::setAllActive(bool active){
    setArgTable(active, "all_active");
}
void MyDB::setEnter(bool enter){
    setArgTable(enter, "enter");
}
void MyDB::setSpace(bool space){
    setArgTable(space, "space");
}
void MyDB::setTab(bool tab){
    setArgTable(tab, "tab");
}

int MyDB::allocPieceID(){
    connect();
    QSqlQuery query;
    int old_piece_id = 0, new_piece_id = 0, group_id = 0;
    query.exec(QString("SELECT * FROM %1").arg(id_tb_name));
    while(query.next()){
        old_piece_id = query.value(0).toInt();
        group_id = query.value(1).toInt();
    }
    new_piece_id = old_piece_id + 1;
//    qDebug() << old_piece_id << " " << new_piece_id << " " << group_id << "\n";
    query.exec(QString("UPDATE %1 SET piece_id=%2 WHERE group_id=%3").arg(id_tb_name, QString::number(new_piece_id), QString::number(group_id)));
    if(!query.exec()){
        qDebug() << "Error: Fail to update piece_id: " << query.lastError();
    }
    close();
    return old_piece_id;
}
int MyDB::allocGroupID(){
    connect();
    QSqlQuery query;
    int old_group_id = 0, new_group_id = 0, piece_id;
    query.exec(QString("SELECT * FROM %1").arg(id_tb_name));
    while(query.next()){
        piece_id = query.value(0).toInt();
        old_group_id = query.value(1).toInt();
    }
    new_group_id = old_group_id + 1;
//    qDebug() << old_group_id << " " << new_group_id << " " << piece_id << "\n";
    query.exec(QString("UPDATE %1 SET group_id=%2 WHERE piece_id=%3").arg(id_tb_name, QString::number(new_group_id), QString::number(piece_id)));
    if(!query.exec()){
        qDebug() << "Error: Fail to update group_id: " << query.lastError();
    }
    close();
    return old_group_id;
}
bool MyDB::hasGroupName(const QString &gname){
    connect();
    QSqlQuery query;
    bool has = false;
    query.prepare(QString("SELECT COUNT(*) FROM %1 WHERE name=?").arg(group_tb_name));
    query.addBindValue(gname);
    if(!query.exec()){
        qDebug() << "Error: Fail to get if has group name " << query.lastError();
    }
    while(query.next()){
        int num = query.value(0).toInt();
        if(num) has = true;
        else has = false;
    }
    close();
    return has;
}
bool MyDB::hasPieceName(const QString &pname){
    connect();
    QSqlQuery query;
    bool has = false;
    query.prepare(QString("SELECT COUNT(*) FROM %1 WHERE abbr=?").arg(piece_tb_name));
    query.addBindValue(pname);
    if(!query.exec()){
        qDebug() << "Error: Fail to get if has piece name " << query.lastError();
    }
    while(query.next()){
        int num = query.value(0).toInt();
        if(num) has = true;
        else has = false;
    }
    close();
    return has;
}

QVector<TransformPiece> MyDB::getAllPieces(){
    QVector<TransformPiece> ret;
    connect();
    QSqlQuery query;
    QSet<int> groups;
    int id;
    QString abbr, description, con;
    bool is_active;
    query.exec(QString("SELECT * FROM %1").arg(piece_tb_name));
    while(query.next()){
        id = query.value(0).toInt();
        abbr = query.value(1).toString();
        con = query.value(2).toString();
        stringToSet(query.value(3).toString(), groups);
        description = query.value(4).toString();
        is_active = query.value(5).toBool();
        TransformPiece temp(id, abbr, con, groups, description, is_active);
        ret.push_back(temp);
    }
    close();
    return ret;
}
QVector<TransformGroup> MyDB::getAllGroups(){
    QVector<TransformGroup> ret;
    connect();
    QSqlQuery query;
    QSet<int> pieces;
    int id;
    QString name;
    bool is_active;
    query.exec(QString("SELECT * FROM %1").arg(group_tb_name));
    while(query.next()){
        id = query.value(0).toInt();
        name = query.value(1).toString();
        stringToSet(query.value(2).toString(), pieces);
        is_active = query.value(3).toBool();
        TransformGroup temp(id, name, pieces, is_active);
        ret.push_back(temp);
    }
    close();
    return ret;
}
TransformPiece MyDB::getPieceByAbbr(const QString &piece_abbr){
    TransformPiece ret;
    connect();
    QSqlQuery query;
    QSet<int> groups;
    int id;
    QString abbr, description, con;
    bool is_active;
//    qDebug() << "db get: " << piece_abbr;
    query.prepare(QString("SELECT * FROM %1 WHERE abbr=?").arg(piece_tb_name));
    query.addBindValue(piece_abbr);
    if(!query.exec()){
        qDebug() << "Error: Fail to get piece by abbr " << query.lastError();
    }
    while(query.next()){
        id = query.value(0).toInt();
        abbr = query.value(1).toString();
        con = query.value(2).toString();
        stringToSet(query.value(3).toString(), groups);
        description = query.value(4).toString();
        is_active = query.value(5).toBool();
        TransformPiece temp(id, abbr, con, groups, description, is_active);
        ret = temp;
    }
    close();
    return ret;
}
TransformGroup MyDB::getGroupByName(const QString &group_name){
    TransformGroup ret;
    connect();
    QSqlQuery query;
    QSet<int> pieces;
    int id;
    QString name;
    bool is_active;
    query.prepare(QString("SELECT * FROM %1 WHERE name=?").arg(group_tb_name));
    query.addBindValue(group_name);
    if(!query.exec()){
        qDebug() << "Error: Fail to get group by name " << query.lastError();
    }
    while(query.next()){
        id = query.value(0).toInt();
        name = query.value(1).toString();
        stringToSet(query.value(2).toString(), pieces);
        is_active = query.value(3).toBool();
        TransformGroup temp(id, name, pieces, is_active);
        ret = temp;
    }
    close();
    return ret;
}
TransformPiece MyDB::getPieceByID(int ID){
    TransformPiece ret;
    connect();
    QSqlQuery query;
    QSet<int> groups;
    int id;
    QString abbr, description, con;
    bool is_active;
    query.prepare(QString("SELECT * FROM %1 WHERE id=?").arg(piece_tb_name));
    query.addBindValue(ID);
    if(!query.exec()){
         qDebug() << "Error: Fail to get piece by ID" << query.lastError();
    }
    while(query.next()){
        id = query.value(0).toInt();
        abbr = query.value(1).toString();
        con = query.value(2).toString();
        stringToSet(query.value(3).toString(), groups);
        description = query.value(4).toString();
        is_active = query.value(5).toBool();
        TransformPiece temp(id, abbr, con, groups, description, is_active);
        ret = temp;
    }
    close();
    return ret;
}
TransformGroup MyDB::getGroupByID(int ID){
    TransformGroup ret;
    connect();
    QSqlQuery query;
    QSet<int> pieces;
    int id;
    QString name;
    bool is_active;
    query.prepare(QString("SELECT * FROM %1 WHERE id=?").arg(group_tb_name));
    query.addBindValue(ID);
    if(!query.exec()){
        qDebug() << "Error: Fail to get group by ID " << query.lastError();
    }
    while(query.next()){
        id = query.value(0).toInt();
        name = query.value(1).toString();
        stringToSet(query.value(2).toString(), pieces);
        is_active = query.value(3).toBool();
        TransformGroup temp(id, name, pieces, is_active);
        ret = temp;
    }
    close();
    return ret;
}
TransformPiece MyDB::addPiece(const QString &abbr, const QString& con, const QSet<int> &vec, const QString &desp, bool active){
    TransformPiece piece(allocPieceID(), abbr, con, vec, desp, active);
    connect();
    QSqlQuery query;
    query.prepare(QString("INSERT INTO %1 values(?, ?, ?, ?, ?, ?)").arg(piece_tb_name));
//    query.addBindValue(piece_tb_name);
    query.addBindValue(piece.getID());
    query.addBindValue(piece.getAbbreviation());
    query.addBindValue(piece.getContent());
    query.addBindValue(setToString(piece.getGroups()));
    query.addBindValue(piece.getDiscription());
    query.addBindValue(piece.isActive());
    if(!query.exec()){
        qDebug() << "Error: Fail to insert data: " << query.lastError();
    }
    close();
    return piece;
}
TransformGroup MyDB::addGroup(const QString &name, const QSet<int> &vec, bool active){
    TransformGroup group(allocGroupID(), name, vec, active);
    connect();
    QSqlQuery query;
    query.prepare(QString("INSERT INTO %1 values(?, ?, ?, ?)").arg(group_tb_name));
//    query.addBindValue(group_tb_name);
    query.addBindValue(group.getID());
    query.addBindValue(group.getName());
    query.addBindValue(setToString(group.getPieces()));
    query.addBindValue(group.isActive());
    if(!query.exec()){
        qDebug() << "Error: Fail to insert data: " << query.lastError();
    }
    close();
    return group;
}
void MyDB::updatePiece(const TransformPiece& piece){
    connect();
    QSqlQuery query;
    query.prepare(QString("UPDATE %1 SET abbr=?, content=?, groups=?, description=?, is_active=? WHERE id=?").arg(piece_tb_name));
//    query.addBindValue(piece_tb_name);
    query.addBindValue(piece.getAbbreviation());
    query.addBindValue(piece.getContent());
    query.addBindValue(setToString(piece.getGroups()));
    query.addBindValue(piece.getDiscription());
    query.addBindValue(piece.isActive());
    query.addBindValue(piece.getID());
    if(!query.exec()){
        qDebug() << "Error: Fail to update data: " << query.lastError();
    }
    close();
}

void MyDB::updateGroup(const TransformGroup& group){
    auto s = group.getName();
    connect();
    QSqlQuery query;
    query.prepare(QString("UPDATE %1 SET name=?, pieces=?, is_active=? WHERE id=?").arg(group_tb_name));
//    query.addBindValue(group_tb_name);
    query.addBindValue(group.getName());
    query.addBindValue(setToString(group.getPieces()));
    query.addBindValue(group.isActive());
    query.addBindValue(group.getID());
    if(!query.exec()){
        qDebug() << "Error: Fail to update data: " << query.lastError();
    }
    close();
}

void MyDB::deletePieceByAbbr(const QString& abbr){
    connect();
    QSqlQuery query;
    query.prepare(QString("DELETE FROM %1 WHERE abbr=?").arg(piece_tb_name));
//    query.addBindValue(piece_tb_name);
    query.addBindValue(abbr);
    if(!query.exec()){
        qDebug() << "Error: Fail to delete data: " << query.lastError();
    }
    close();
}

void MyDB::deleteGroupByName(const QString& name){
    connect();
    QSqlQuery query;
    query.prepare(QString("DELETE FROM %1 WHERE name=?").arg(group_tb_name));
//    query.addBindValue(group_tb_name);
    query.addBindValue(name);
    if(!query.exec()){
        qDebug() << "Error: Fail to delete data: " << query.lastError();
    }
    close();
}

void MyDB::deletePieceByID(int ID){
    connect();
    QSqlQuery query;
    query.prepare(QString("DELETE FROM %1 WHERE id=?").arg(piece_tb_name));
//    query.addBindValue(piece_tb_name);
    query.addBindValue(ID);
    if(!query.exec()){
        qDebug() << "Error: Fail to delete data: " << query.lastError();
    }
    close();
}

void MyDB::deleteGroupByID(int ID){
    connect();
    QSqlQuery query;
    query.prepare(QString("DELETE FROM %1 WHERE id=?").arg(group_tb_name));
//    query.addBindValue(group_tb_name);
    query.addBindValue(ID);
    if(!query.exec()){
        qDebug() << "Error: Fail to delete data: " << query.lastError();
    }
    close();
}
QString MyDB::getGroupNamesByGroupID(int ID){
    connect();
    QSqlQuery query;
    query.exec(QString("SELECT name FROM %1 WHERE id=%2").arg(group_tb_name, QString::number(ID)));
    QString ret;
    while(query.next()){
        auto tname = query.value(0).toString();
        ret = tname;
    }
    close();
    return ret;
}
QString MyDB::getPieceNamesByPieceID(int ID){
    connect();
    QSqlQuery query;
    query.exec(QString("SELECT name FROM %1 WHERE id=?").arg(piece_tb_name));
    query.addBindValue(ID);
    QString ret;
    while(query.next()){
        auto tname = query.value(0).toString();
        ret = tname;
    }
    close();
    return ret;
}

QSet<QString> MyDB::getAllGroupNames(){
    connect();
    QSqlQuery query;
    query.exec(QString("SELECT name FROM %1").arg(group_tb_name));
    QSet<QString> ret;
    while(query.next()){
        auto tname = query.value(0).toString();
        ret.insert(tname);
    }
    close();
    return ret;
}

QSet<QString> MyDB::getAllPieceNames(){
    connect();
    QSqlQuery query;
    query.exec(QString("SELECT abbr FROM %1").arg(piece_tb_name));
    QSet<QString> ret;
    while(query.next()){
        auto tname = query.value(0).toString();
        ret.insert(tname);
    }
    close();
    return ret;
}

QSet<QString> MyDB::getPieceNamesByGroupID(int ID){
    connect();
    QSqlQuery query;
    query.prepare(QString("SELECT pieces FROM %1 WHERE id=?").arg(group_tb_name));
    query.addBindValue(ID);
    if(!query.exec()){
        qDebug() << "Error: Fail to get piece by group ID " << query.lastError();
    }
    QSet<QString> ret;
    QSet<int> pieces;
    QString piecesString;
    while(query.next()){
        auto tpieces = query.value(0).toString();
        piecesString = tpieces;
    }
//    qDebug() << piecesString;
    stringToSet(piecesString, pieces);
//    qDebug() << pieces;
    for(const auto& piece: pieces){
        auto tname = getPieceNamesByPieceID(piece);
        ret.insert(tname);
    }
    close();
    return ret;
}

QSet<QString> MyDB::getGroupNamesByPieceID(int ID){
    connect();
    QSqlQuery query;
    query.prepare(QString("SELECT groups FROM %1 WHERE id=?").arg(piece_tb_name));
    query.addBindValue(ID);
    if(!query.exec()){
        qDebug() << "Error: Fail to get group names by piece ID " << query.lastError();
    }
    QSet<QString> ret;
    QSet<int> groups;
    QString groupsString;
    while(query.next()){
        auto tgroups = query.value(0).toString();
        groupsString = tgroups;
    }
    stringToSet(groupsString, groups);
    for(const auto& group: groups){
        auto tname = getGroupNamesByGroupID(group);
        ret.insert(tname);
    }
    close();
    return ret;
}
QVector<TransformPiece> MyDB::getPiecesByGroupID(int ID){
    QVector<TransformPiece> ret;
    QSet<int> groups;
    connect();
    QSqlQuery query;
    query.prepare(QString("SELECT pieces FROM %1 WHERE id=?").arg(group_tb_name));
    query.addBindValue(ID);
    QSet<int> pieces;
    if(!query.exec()){
        qDebug() << "Error: Fail to get pieces by group ID " << query.lastError();
    }
    while(query.next()){
        QString piecesStr = query.value(0).toString();
        stringToSet(piecesStr, pieces);
    }
    for(const auto& piece: pieces){
        auto g = getPieceByID(piece);
        ret.push_back(g);
    }
    close();
    return ret;
}

QVector<TransformPiece> MyDB::getPiecesByGroupName(const QString& gname){
    auto g = getGroupByName(gname);
    return getPiecesByGroupID(g.getID());
}
QSet<QString> MyDB::getAllActiveGroups(){
    QSet<QString> ret;
    if(isAllActive()){
        ret.insert("所有指令");
    }
    connect();
    QSqlQuery query;
    query.prepare(QString("SELECT name FROM %1 WHERE is_active=?").arg(group_tb_name));
    query.addBindValue(true);
    if(!query.exec()){
        qDebug() << "Error: Fail to get active groups " << query.lastError();
    }
    while(query.next()){
        QString group_name = query.value(0).toString();
        ret.insert(group_name);
    }
    close();
    return ret;
}
QSet<QString> MyDB::getAllInActiveGroups(){
    QSet<QString> ret;
    if(!isAllActive()){
        ret.insert("所有指令");
    }
    connect();
    QSqlQuery query;
    query.prepare(QString("SELECT name FROM %1 WHERE is_active=?").arg(group_tb_name));
    query.addBindValue(false);
    if(!query.exec()){
        qDebug() << "Error: Fail to get active groups " << query.lastError();
    }
    while(query.next()){
        QString group_name = query.value(0).toString();
        ret.insert(group_name);
    }
    close();
    return ret;
}
QSet<QString> MyDB::getAllActivePieceNames(){
    QSet<QString> ret;
    QSqlQuery query;
    if(isAllActive()){
        connect();
        query.prepare(QString("SELECT abbr FROM %1 WHERE is_active=?").arg(piece_tb_name));
        query.addBindValue(true);
        if(!query.exec()){
            qDebug() << "Error: Fail to get active piece abbrs" << query.lastError();
        }//dfa dfa
        while(query.next()){
            QString piece_name = query.value(0).toString();
            ret.insert(piece_name);
        }
    }else{
        auto groups = getAllActiveGroups();
        QSet<int> pieces;
        for(const auto& group: groups){
            auto g = getGroupByName(group);
            for(const auto&p: g.getPieces()){
                pieces.insert(p);
            }
        }
        for(const auto&piece: pieces){
            auto pi = getPieceNamesByPieceID(piece);
            ret.insert(pi);
        }
    }
    close();
    return ret;
}

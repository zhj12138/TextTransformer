#ifndef MYTRAYICON_H
#define MYTRAYICON_H
#include <QtGui>
#include <QSystemTrayIcon>
#include <QIcon>
#include <QWidget>
#include <QMenu>
#include <QAction>
#include <QEvent>
#include <QTableWidget>
#include <QTabWidget>
#include <QListWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QButtonGroup>
#include <QCheckBox>
#include <QSplitter>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QRadioButton>
#include <QLabel>
#include <QFont>
#include <QSet>
#include <QVector>
#include <QString>
#include <QStringList>
#include <QHeaderView>
#include <QPoint>
#include <QCursor>
#include <QInputDialog>
#include "mydb.h"
#include "myfuncs.h"
#include "piecedialog.h"
#include "trantextthread.h"

class MyTrayIcon: public QWidget
{ 
    Q_OBJECT
public:
    MyTrayIcon(QWidget *parent = 0);
    void testDB();
    ~MyTrayIcon();
protected:
    void showWindow();
    void quit();
    void closeEvent(QCloseEvent *event) override;
private:
    QSystemTrayIcon *trayIcon;
    QMenu *menu;
    QTabWidget *tabs;
    QWidget *main_tab;
    QWidget *setting_tab;
    QListWidget *group_list;
    QLineEdit *search_line;
    QPushButton *add_piece_button;
    QRadioButton *active_button;
    QTableWidget *piece_table;
    QCheckBox *tab_check;
    QCheckBox *space_check;
    QCheckBox *enter_check;
    QButtonGroup *shortcuts_group;
    QAction *open_window_action;
    QMenu *open_group_menu;
    QMenu *close_group_menu;
    QAction *exit_action;
    tranTextThread* th;
    MyDB *db;
    QString cur_group_name = "所有指令";
    void initTrayIcon();
    void initTabs();
    void initMainWindowUI();
    void initSettingWindowUI();
    void groupAddPiece(TransformGroup& g, TransformPiece& p);
    void setGroupList(const QSet<QString>& group_names);
    void setPieceTable(const QVector<TransformPiece>& pieces);
    void updateGroupList();
    void updateTableList();
    void updateONGroups();
    void updateOFFGroups();
private slots:
    void onTrayIconActivated(QSystemTrayIcon::ActivationReason reason);
    void onAddPieceBtnClicked();
    void onEditBtnClicked(QString abbr);
    void onDelBtnClicked(QString abbr);
    void onGroupListItemClicked(QListWidgetItem* item);
    void onDataChanged();
    void onActiveBtnClicked(QString abbr, bool isActive);
    void onGroupListContextMenu(const QPoint& point);
    void onPieceTableContextMenu(const QPoint& point);
    void onChangeGroupName(const QString& gname);
    void onDeleteGroup(const QString& gname);
    void onPieceAddGroup(const QString& abbr, const QString& gname);
    void onAddToGroupMenuTriggered(QAction* action, const QString& abbr);
    void onGroupActiveBtnClicked();
    void onTabCheckClicked();
    void onEnterCheckClicked();
    void onSpaceCheckClicked();
    void onOnGroupMenuTriggered(QAction* action);
    void onOffGroupMenuTriggered(QAction* action);
    void onGroupMenuTriggered(const QString& gname, bool ok);
};
#endif // MYTRAYICON_H

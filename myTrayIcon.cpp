#include "myTrayIcon.h"

MyTrayIcon::MyTrayIcon(QWidget *parent): QWidget(parent) {
    db = new MyDB();
    initTrayIcon();
    initTabs();
    initMainWindowUI();
    initSettingWindowUI();
    auto tempLay = new QHBoxLayout(this);
    tempLay->addWidget(tabs);
    th = new tranTextThread(this);
    th->start();
    this->setLayout(tempLay);
    this->setWindowIcon(QIcon(":/new/icons/text.svg"));
    this->setFont(QFont("", 13));
    this->setGeometry(300, 100, 1280, 800);
    testDB();
}
void MyTrayIcon::initTrayIcon(){
    trayIcon = new QSystemTrayIcon(this);
    trayIcon->setIcon(QIcon(":/new/icons/text.svg"));
    trayIcon->setToolTip("文本转换");
    connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(onTrayIconActivated(QSystemTrayIcon::ActivationReason)));
    trayIcon->show();

    menu = new QMenu(this);
    open_window_action = new QAction("打开主面板");
    connect(open_window_action, SIGNAL(triggered(bool)), this, SLOT(show()));
    open_group_menu = new QMenu("启用分组");
    close_group_menu = new QMenu("禁用分组");
    connect(open_group_menu, &QMenu::triggered, this, &MyTrayIcon::onOnGroupMenuTriggered);
    connect(close_group_menu, &QMenu::triggered, this, &MyTrayIcon::onOffGroupMenuTriggered);
    exit_action = new QAction("退出程序");
    connect(exit_action, SIGNAL(triggered(bool)), qApp, SLOT(quit()));
    updateONGroups();
    updateOFFGroups();
    menu->addAction(open_window_action);
    menu->addMenu(open_group_menu);
    menu->addMenu(close_group_menu);
    menu->addAction(exit_action);
    menu->addSeparator();
    trayIcon->setContextMenu(menu);
}
void MyTrayIcon::initTabs(){
    tabs = new QTabWidget(this);
    main_tab = new QWidget();
    setting_tab = new QWidget();
    tabs->addTab(main_tab, "文本指令");
    tabs->addTab(setting_tab, "设置");
    tabs->setCurrentIndex(0);
}

void MyTrayIcon::initMainWindowUI(){
    group_list = new QListWidget(main_tab);
    search_line = new QLineEdit(main_tab);
    search_line->setPlaceholderText("搜索");
    add_piece_button = new QPushButton("+ 添加指令");
    active_button = new QRadioButton("激活");
    piece_table = new QTableWidget(main_tab);
    auto all_active = db->isAllActive();
    active_button->setChecked(all_active);
    connect(active_button, &QRadioButton::clicked, this, &MyTrayIcon::onGroupActiveBtnClicked);
    connect(add_piece_button, SIGNAL(clicked(bool)), this, SLOT(onAddPieceBtnClicked()));
    connect(group_list, &QListWidget::itemClicked, this, &MyTrayIcon::onGroupListItemClicked);
    piece_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
//    piece_table->setSelectionMode(QAbstractItemView::NoSelection);
    piece_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    piece_table->setContextMenuPolicy(Qt::CustomContextMenu);
    group_list->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(piece_table, &QTableWidget::customContextMenuRequested, this, &MyTrayIcon::onPieceTableContextMenu);
    connect(group_list, &QListWidget::customContextMenuRequested, this, &MyTrayIcon::onGroupListContextMenu);
//    auto tempWid2 = new QWidget(main_tab);
//    tempWid2->setLayout(hl);
    auto vl = new QVBoxLayout(main_tab);
    auto hl = new QHBoxLayout();
    hl->addWidget(search_line);
    hl->addWidget(add_piece_button);
    hl->addWidget(active_button);
    vl->addLayout(hl);
    vl->addWidget(piece_table);
    auto sp = new QSplitter(main_tab);
    sp->addWidget(group_list);
    auto tempWid1 = new QWidget(main_tab);
    tempWid1->setLayout(vl);
    sp->addWidget(tempWid1);
    sp->setSizes({100, 800});
    auto tempLay = new QHBoxLayout(main_tab);
    tempLay->setContentsMargins(0, 0, 0, 0);
    tempLay->addWidget(sp);
    main_tab->setLayout(tempLay);
    updateGroupList();
    updateTableList();
}
void MyTrayIcon::initSettingWindowUI(){
    shortcuts_group = new QButtonGroup(this);
    tab_check = new QCheckBox("tab键触发");
    space_check = new QCheckBox("空格触发");
    enter_check = new QCheckBox("回车触发");
    shortcuts_group->addButton(space_check);
    shortcuts_group->addButton(tab_check);
    shortcuts_group->addButton(enter_check);
    shortcuts_group->setExclusive(false);
    tab_check->setChecked(db->Tab());
    space_check->setChecked(db->Space());
    enter_check->setChecked(db->Enter());
    connect(tab_check, &QCheckBox::clicked, this, &MyTrayIcon::onTabCheckClicked);
    connect(space_check, &QCheckBox::clicked, this, &MyTrayIcon::onSpaceCheckClicked);
    connect(enter_check, &QCheckBox::clicked, this, &MyTrayIcon::onEnterCheckClicked);
    auto tempLay = new QHBoxLayout(this);
    tempLay->addWidget(space_check);
    tempLay->addWidget(tab_check);
    tempLay->addWidget(enter_check);
    setting_tab->setLayout(tempLay);
    piece_table->setColumnCount(7);
    QStringList headers;
    headers << "缩写词" << "说明" << "分组" << "文本" << "状态" << "编辑" << "删除";
    piece_table->setHorizontalHeaderLabels(headers);
//    piece_table->resizeColumnsToContents();
//    piece_table->resizeRowsToContents();
    piece_table->setShowGrid(false);
    piece_table->verticalHeader()->setVisible(false);
    piece_table->verticalHeader()->setSectionsClickable(false);
    auto pieces = db->getAllPieces();
    setPieceTable(pieces);
}
void MyTrayIcon::setGroupList(const QSet<QString> &group_names){
    group_list->clear();
    group_list->addItem(new QListWidgetItem(QIcon(":/new/icons/tags.svg"), "所有指令"));
    for(const auto& name: group_names){
        group_list->addItem(new QListWidgetItem(QIcon(":/new/icons/tags.svg"), name));
    }
}
void MyTrayIcon::setPieceTable(const QVector<TransformPiece> &pieces){
    auto len = pieces.size();
    piece_table->clearContents();
    piece_table->setRowCount(len);
//    qDebug() << len;
    int row = 0;
    for(const auto& piece: pieces){
        auto abbr = piece.getAbbreviation();
        auto active = piece.isActive();
        piece_table->setItem(row, 0, new QTableWidgetItem(abbr));
//        qDebug() << piece.getAbbreviation();
        piece_table->setItem(row, 1, new QTableWidgetItem(getShortenString(piece.getDiscription(), 19)));
//        qDebug() << getShortenString(piece.getDiscription());
        piece_table->setItem(row, 2, new QTableWidgetItem(getShortenString(setToString(db->getGroupNamesByPieceID(piece.getID())), 19)));
//        qDebug() << db->getGroupNamesByPieceID(piece.getID());
        piece_table->setItem(row, 3, new QTableWidgetItem(getShortenString(piece.getContent(), 19)));
//        qDebug() << piece.getContent();
        auto btn = new QPushButton();
//        piece_table->item(row, 4)->setTextAlignment(Qt::AlignVCenter);
        auto dBtn = new QPushButton(), eBtn = new QPushButton();
//        connect(dBtn, &QPushButton::clicked, );
        connect(btn, &QPushButton::clicked, this, [this, abbr, active](){
            this->onActiveBtnClicked(abbr, active);
        });
        connect(eBtn, &QPushButton::clicked, this, [this, abbr](){
            this->onEditBtnClicked(abbr);
        });
        connect(dBtn, &QPushButton::clicked, this, [this, abbr](){
            this->onDelBtnClicked(abbr);
        });
        if(active){
            btn->setIcon(QIcon(":new/icons/on.svg"));
        }else{
            btn->setIcon(QIcon(":new/icons/off.svg"));
        }
        dBtn->setIcon(QIcon(":/new/icons/delete.svg"));
        eBtn->setIcon(QIcon(":/new/icons/edit.svg"));
        piece_table->setCellWidget(row, 4, btn);
        piece_table->setCellWidget(row, 5, eBtn);
        piece_table->setCellWidget(row, 6, dBtn);
        row++;
//        qDebug() << "add a row";
    }
}

MyTrayIcon::~MyTrayIcon(){
    delete trayIcon;
    delete menu;
    delete tabs;
    delete main_tab;
    delete setting_tab;
    delete group_list;
    delete search_line;
    delete add_piece_button;
    delete active_button;
    delete piece_table;
    delete shortcuts_group;
    delete open_window_action;
    delete open_group_menu;
    delete close_group_menu;
    delete exit_action;
    delete th;
    delete db;
}
void MyTrayIcon::closeEvent(QCloseEvent *event){
    if(this->isVisible()){
        this->hide();
        event->ignore();
    }
}
void MyTrayIcon::onGroupActiveBtnClicked(){
    if(cur_group_name == "所有指令"){
        db->setAllActive(!db->isAllActive());
    }else{
        auto group = db->getGroupByName(cur_group_name);
        group.setActive(!group.isActive());
        db->updateGroup(group);
    }
    updateONGroups();
    updateOFFGroups();
}
void MyTrayIcon::onEnterCheckClicked(){
    db->setEnter(!db->Enter());
}
void MyTrayIcon::onTabCheckClicked(){
    db->setTab(!db->Tab());
}
void MyTrayIcon::onSpaceCheckClicked(){
    db->setSpace(!db->Space());
}

void MyTrayIcon::onGroupListItemClicked(QListWidgetItem *item){
//    qDebug() << item->text();
    cur_group_name = item->text();
    updateTableList();
}
void MyTrayIcon::onGroupListContextMenu(const QPoint &point){
    auto item = group_list->itemAt(point);
    if(!item) return;
    auto gname = item->text();
    if(gname == "所有指令") return;
    auto menu = new QMenu();
    auto changeGroupNameAction = new QAction("修改组名");
    auto deleteGroupAction = new QAction("删除该组");
    connect(changeGroupNameAction, &QAction::triggered, this, [this, gname](){
        this->onChangeGroupName(gname);
    });
    connect(deleteGroupAction, &QAction::triggered, this, [this, gname](){
        this->onDeleteGroup(gname);
    });
    menu->addActions({changeGroupNameAction, deleteGroupAction});
    menu->exec(QCursor::pos());
}
void MyTrayIcon::onPieceAddGroup(const QString &abbr, const QString &gname){
    auto piece = db->getPieceByAbbr(abbr);
    auto group = db->getGroupByName(gname);
    groupAddPiece(group, piece);
    updateTableList();
}
void MyTrayIcon::onDeleteGroup(const QString &gname){
    QMessageBox box(QMessageBox::Warning, "提醒", "您确定要删除该分组吗?");
    auto yes = box.addButton("是", QMessageBox::YesRole);
    box.addButton("否", QMessageBox::NoRole);
    box.exec();
    if(box.clickedButton() == yes){
        auto group = db->getGroupByName(gname);
        auto id = group.getID();
        auto pieces = db->getPiecesByGroupID(id);
        for(auto& piece: pieces){
            piece.delGroup(id);
            db->updatePiece(piece);
        }
        db->deleteGroupByName(gname);
        if(cur_group_name == gname){
            cur_group_name = "所有指令";
        }
        updateGroupList();
        updateTableList();
        updateONGroups();
        updateOFFGroups();
    }
}
void MyTrayIcon::onChangeGroupName(const QString &gname){
    auto group = db->getGroupByName(gname);
    auto new_name = QInputDialog::getText(this, "提醒", "请输入新名字");
    if(new_name != gname && db->hasGroupName(new_name)){
        QMessageBox::warning(this, "警告", "已含有相同的组名");
    }else if(new_name == "所有指令"){
        QMessageBox::warning(this, "警告", "组名不合法");
    }
    if(cur_group_name == gname){
        cur_group_name = new_name;
    }
    group.setName(new_name);
    db->updateGroup(group);
    updateGroupList();
    updateTableList();
    updateONGroups();
    updateOFFGroups();
}
void MyTrayIcon::onGroupMenuTriggered(const QString& gname, bool ok){
    if(gname == "所有指令"){
        db->setAllActive(ok);
    }else{
        auto group = db->getGroupByName(gname);
        group.setActive(ok);
        db->updateGroup(group);
    }
    updateONGroups();
    updateOFFGroups();
    updateGroupList();
    updateTableList();
}

void MyTrayIcon::onOnGroupMenuTriggered(QAction *action){
    auto gname = action->text();
    onGroupMenuTriggered(gname, true);
}
void MyTrayIcon::onOffGroupMenuTriggered(QAction *action){
    auto gname = action->text();
    onGroupMenuTriggered(gname, false);
}

void MyTrayIcon::onPieceTableContextMenu(const QPoint &point){
    auto item = piece_table->itemAt(point);
    if(!item) return;
    auto abbr = piece_table->item(item->row(), 0)->text();
    auto piece = db->getPieceByAbbr(abbr);
    QString activeStr;
    auto pieceAc = piece.isActive();
    if(pieceAc){
        activeStr = "禁用";
    }else{
        activeStr = "激活";
    }
    auto menu = new QMenu();
    auto editPieceAction= new QAction("编辑");
    connect(editPieceAction, &QAction::triggered, this, [this, abbr](){
        this->onEditBtnClicked(abbr);
    });
    auto activePieceAction = new QAction(activeStr);
    connect(activePieceAction, &QAction::triggered, this, [this, abbr, pieceAc](){
        this->onActiveBtnClicked(abbr, pieceAc);
    });
    auto addToGroupMenu = new QMenu("添加到分组");
    auto nGAC = new QAction("添加到新分组");
    addToGroupMenu->addAction(nGAC);
    auto group_names = db->getAllGroupNames();
    group_names -= db->getGroupNamesByPieceID(piece.getID());
    for(const auto& group_name: group_names){
        auto gAc = new QAction(group_name);
        addToGroupMenu->addAction(gAc);
    }
    connect(addToGroupMenu, &QMenu::triggered, this, [this, abbr](QAction* action){
        onAddToGroupMenuTriggered(action, abbr);
    });
    auto deletePieceAction = new QAction("删除");
    connect(deletePieceAction, &QAction::triggered, this, [this, abbr](){
        this->onDelBtnClicked(abbr);
    });
    menu->addActions({editPieceAction, activePieceAction});
    menu->addMenu(addToGroupMenu);
    menu->addAction(deletePieceAction);
    menu->exec(QCursor::pos());
}

void MyTrayIcon::onTrayIconActivated(QSystemTrayIcon::ActivationReason reason){
    switch (reason) {
    case QSystemTrayIcon::Trigger:
        this->show();
        break;
    default:
        break;
    }
}
void MyTrayIcon::onAddToGroupMenuTriggered(QAction *action, const QString& abbr){
    auto gname = action->text();
    TransformGroup group;
    if(gname == "添加到新分组"){
        gname = QInputDialog::getText(this, "输入", "请输入新的分组名");
        if(db->hasGroupName(gname)){
            QMessageBox::warning(this, "警告", "组名已存在");
            return;
        }
        group = db->addGroup(gname);
        updateONGroups();
        updateOFFGroups();
    }else{
        group = db->getGroupByName(gname);
    }
    auto piece = db->getPieceByAbbr(abbr);
    groupAddPiece(group, piece);
    updateGroupList();
    updateTableList();
}

void MyTrayIcon::onEditBtnClicked(QString abbr){
//    qDebug() << "edit clicked: " << abbr;
    auto dig = new pieceDialog(db->getPieceByAbbr(abbr), db, this);
    connect(dig, &pieceDialog::saveBtnClicked, this, &MyTrayIcon::onDataChanged);
}
void MyTrayIcon::onDelBtnClicked(QString abbr){
    QMessageBox box(QMessageBox::Warning, "提醒", "您确定要删除吗？");
    auto yes = box.addButton("是", QMessageBox::YesRole);
    box.addButton("否", QMessageBox::NoRole);
    box.exec();
    if(box.clickedButton() == yes){
        auto piece = db->getPieceByAbbr(abbr);
        for(const auto& group: db->getGroupNamesByPieceID(piece.getID())){
            TransformGroup tmpG;
            if(!db->hasGroupName(group)){
                tmpG = db->addGroup(group);
                qDebug() << "something went wrong on save btn clicked";
            }else{
                tmpG = db->getGroupByName(group);
            }
            tmpG.deletePiece(piece.getID());
            db->updateGroup(tmpG);
        }
        db->deletePieceByAbbr(abbr);
        updateTableList();
    }
}
void MyTrayIcon::onActiveBtnClicked(QString abbr, bool isActive){
    QString msg;
//    qDebug() << "clicked";
    msg = isActive ? "您确定要禁用这条指令吗?" : "您确定要激活这条指令吗?";
    QMessageBox box(QMessageBox::Information, "提醒", msg);
    auto yes = box.addButton("是", QMessageBox::YesRole);
    box.addButton("否", QMessageBox::NoRole);
    box.exec();
    auto piece = db->getPieceByAbbr(abbr);
    if(box.clickedButton() == yes){
        piece.setActive(!piece.isActive());
        db->updatePiece(piece);
        updateTableList();
    }
}

void MyTrayIcon::groupAddPiece(TransformGroup &g, TransformPiece &p){
    g.addPiece(p.getID());
    p.addGroup(g.getID());
    db->updateGroup(g);
    db->updatePiece(p);
}
void MyTrayIcon::onAddPieceBtnClicked(){
    auto dig = new pieceDialog(db, this);
    connect(dig, &pieceDialog::saveBtnClicked, this, &MyTrayIcon::onDataChanged);
}
void MyTrayIcon::onDataChanged(){
    updateTableList();
    updateGroupList();
    updateONGroups();
    updateOFFGroups();
}

void MyTrayIcon::updateGroupList(){
    auto group_names = db->getAllGroupNames();
    setGroupList(group_names);
    auto items = group_list->findItems(cur_group_name, Qt::MatchExactly);
    if(items.size() != 1){
        qDebug() << "something wrong in update group list";
    }else{
        for(const auto& item: items){
            group_list->setCurrentItem(item);
        }
    }
}
void MyTrayIcon::updateTableList(){
    QVector<TransformPiece> pieces;
    if(cur_group_name == "所有指令"){
        pieces = db->getAllPieces();
        active_button->setChecked(db->isAllActive());
    }else{
        pieces = db->getPiecesByGroupName(cur_group_name);
        active_button->setChecked((db->getGroupByName(cur_group_name)).isActive());
//        qDebug() << db->getGroupByName(cur_group_name).getID();
//        qDebug() << db->getPieceNamesByGroupID((db->getGroupByName(cur_group_name)).getID());
    }
    setPieceTable(pieces);
}
void MyTrayIcon::updateONGroups(){
    open_group_menu->clear();
    auto groups = db->getAllInActiveGroups();
//    qDebug() << groups;
    for(const auto& group: groups){
        auto action = new QAction(group);
        open_group_menu->addAction(action);
    }
}
void MyTrayIcon::updateOFFGroups(){
    close_group_menu->clear();
    auto groups = db->getAllActiveGroups();
//    qDebug() << groups;
    for(const auto& group: groups){
        auto action = new QAction(group);
        close_group_menu->addAction(action);
    }
}

void MyTrayIcon::testDB(){
//    auto g1 = db->addGroup("g1");
//    auto g2 = db->addGroup("g2");
//    auto p1 = db->addPiece("cts", "casdfasdf");
//    auto p2 = db->addPiece("acm", "penrasjfkda");
//    auto p3 = db->addPiece("mdp", "sdfjaksdf");
//    groupAddPiece(g1, p1);
//    groupAddPiece(g1, p2);
//    groupAddPiece(g2, p3);
//    groupAddPiece(g2, p2);
//    auto groups = db->getAllGroupNames();
//    qDebug() << groups.size();
//    for(const auto& g: groups){
//        qDebug() << g << " ";
//    }
//    qDebug() << "\n";
//    auto pieces = db->getAllPieceNames();
//    for(const auto& p: pieces){
//        qDebug() << p << " ";
//    }
//    qDebug() << "\n";
}

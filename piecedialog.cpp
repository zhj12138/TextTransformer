#include "piecedialog.h"

pieceDialog::pieceDialog(MyDB* mdb, QWidget *parent): QDialog(parent){
    db = mdb;
    formLay = new QFormLayout();
    abbrLabel = new QLabel("缩写词");
    descLabel = new QLabel("说明");
    groupLabel = new QLabel("分组");
    activeLabel = new QLabel("启用");
    contentLabel = new QLabel("内容");
    abbrInput = new QLineEdit();
    contentInput = new QTextEdit();
    descInput = new QLineEdit();
    groupInput = new QLineEdit();
    activeBtn = new QCheckBox();
    hLay = new QHBoxLayout();
    vLay = new QVBoxLayout(this);
    saveBtn = new QPushButton("保存");
    cancleBtn = new QPushButton("取消");
    activeBtn->setChecked(true);
    connect(saveBtn, SIGNAL(clicked(bool)), this, SLOT(onSaveBtnClicked()));
    connect(cancleBtn, SIGNAL(clicked(bool)), this, SLOT(onCancleBtnClicked()));
    formLay->addRow(abbrLabel, abbrInput);
    formLay->addRow(contentLabel, contentInput);
    formLay->addRow(descLabel, descInput);
    formLay->addRow(groupLabel, groupInput);
    formLay->addRow(activeLabel, activeBtn);
    hLay->addWidget(saveBtn);
    hLay->addWidget(cancleBtn);
    vLay->addLayout(formLay);
    vLay->addLayout(hLay);
    this->setLayout(vLay);
    this->show();
}
pieceDialog::pieceDialog(const TransformPiece &piece, MyDB* mdb, QWidget *parent): pieceDialog(mdb, parent){
    orig_abbr = piece.getAbbreviation();
    abbrInput->setText(piece.getAbbreviation());
    contentInput->setPlainText(piece.getContent());
//    qDebug() << piece.getAbbreviation();
    descInput->setText(piece.getDiscription());
//    qDebug() << piece.getDiscription();
    groupInput->setText(setToString(db->getGroupNamesByPieceID(piece.getID())));
//    qDebug() << db->getGroupNamesByPieceID(piece.getID());
//    qDebug() << setToString(db->getGroupNamesByPieceID(piece.getID()));

    activeBtn->setChecked(piece.isActive());
}
void pieceDialog::onSaveBtnClicked(){
    auto abbr = abbrInput->text();
    auto desp = descInput->text();
    auto groupsString = groupInput->text();
    QSet<QString> groups;
    QString con = contentInput->toPlainText();
    stringToSet(groupsString, groups);
    if(std::find(groups.begin(), groups.end(), "所有指令") != groups.end()){
        QMessageBox::warning(this, "警告", "含有非法的组名", QMessageBox::Yes);
        return;
    }
    auto active = activeBtn->checkState();
    if(abbr != orig_abbr && db->hasPieceName(abbr)){
        QMessageBox::warning(this, "警告", "已存在相同的缩写词", QMessageBox::Yes);
    }else if(abbr == ""){
        QMessageBox::warning(this, "警告", "缩写词不能为空", QMessageBox::Yes);
    }else if(!isLegal(abbr)){
        QMessageBox::warning(this, "警告", "含有非法字符", QMessageBox::Yes);
    }else if(con == ""){
        QMessageBox::warning(this, "警告", "您未输入内容", QMessageBox::Yes);
    }else if(abbr == orig_abbr){
        auto tmpP = db->getPieceByAbbr(abbr);
        tmpP.setDiscription(desp);
        tmpP.setContent(con);
        for(const auto&group: db->getGroupNamesByPieceID(tmpP.getID())){
            TransformGroup tmpG;
            if(!db->hasGroupName(group)){
                tmpG = db->addGroup(group);
                qDebug() << "something went wrong on save btn clicked";
            }else{
                tmpG = db->getGroupByName(group);
            }
            tmpG.deletePiece(tmpP.getID());
            db->updateGroup(tmpG);
        }
        tmpP.setGroups({});//清空
        tmpP.setActive(active);
        for(const auto&group: groups){
            TransformGroup tmpG;
            if(!db->hasGroupName(group)){
                tmpG = db->addGroup(group);
            }else{
                tmpG = db->getGroupByName(group);
            }
            tmpP.addGroup(tmpG.getID());
            tmpG.addPiece(tmpP.getID());
            db->updateGroup(tmpG);
        }
        db->updatePiece(tmpP);
        emit saveBtnClicked();
        this->close();
    }else{
        if(orig_abbr != ""){//modify, need to delete original piece
            auto origP = db->getPieceByAbbr(orig_abbr);
            for(const auto& group: db->getGroupNamesByPieceID(origP.getID())){
                TransformGroup tmpG;
                if(!db->hasGroupName(group)){
                    tmpG = db->addGroup(group);
                    qDebug() << "something went wrong on delete orig abbr";
                }else{
                    tmpG = db->getGroupByName(group);
                }
                tmpG.deletePiece(origP.getID());
                db->updateGroup(tmpG);
            }
            db->deletePieceByAbbr(orig_abbr);
        }
        auto tmpP = db->addPiece(abbr, con, {}, desp, active);
        for(const auto&group: groups){
            TransformGroup tmpG;
            if(!db->hasGroupName(group)){
                tmpG = db->addGroup(group);
            }else{
                tmpG = db->getGroupByName(group);
            }
            tmpP.addGroup(tmpG.getID());
            tmpG.addPiece(tmpP.getID());
            db->updateGroup(tmpG);
        }
        db->updatePiece(tmpP);
        emit saveBtnClicked();
        this->close();
    }
}
void pieceDialog::onCancleBtnClicked(){
    this->close();
}

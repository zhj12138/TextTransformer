#ifndef PIECEDIALOG_H
#define PIECEDIALOG_H
#include <QDialog>
#include <QWidget>
#include <QFormLayout>
#include <QLineEdit>
#include <QLabel>
#include <QString>
#include <QCheckBox>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QMessageBox>
#include <QTextEdit>
#include <QSet>
#include "transformpiece.h"
#include "transformgroup.h"
#include "myfuncs.h"
#include "mydb.h"
class pieceDialog: public QDialog
{
    Q_OBJECT
public:
    pieceDialog(MyDB* mdb, QWidget *parent=nullptr);
    pieceDialog(const TransformPiece& piece, MyDB* mdb, QWidget *parent=nullptr);
signals:
    void saveBtnClicked();
private:
    QFormLayout* formLay;
    QLabel* abbrLabel;
    QLabel* descLabel;
    QLabel* groupLabel;
    QLabel* activeLabel;
    QLabel* contentLabel;
    QLineEdit* abbrInput;
    QTextEdit* contentInput;
    QLineEdit* descInput;
    QLineEdit* groupInput;
    QCheckBox* activeBtn;
    QHBoxLayout* hLay;
    QVBoxLayout* vLay;
    QPushButton* saveBtn;
    QPushButton* cancleBtn;
    QString orig_abbr = "";
    MyDB* db;
private slots:
    void onSaveBtnClicked();
    void onCancleBtnClicked();
};

#endif // PIECEDIALOG_H

#ifndef MAINWINDOW_H
#define MAINWINDOW_H


#include<QMenu>
#include <QMainWindow>
#include<QNetworkAccessManager>
#include<QNetworkReply>
#include"weatherdata.h"
#include<QList>
#include<QLabel>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();


private:
    void OnReplied(QNetworkReply*reply);
protected:
    void contextMenuEvent(QContextMenuEvent*event);
    void mousePressEvent(QMouseEvent*event);
    void mouseMoveEvent(QMouseEvent*event);
    void getWeatherInfo(QString cityCode);
    void parseJson(QByteArray &byteArray);
    void updateUI();

private:
    QMenu *mExitMenu;
    QAction *mExitAct;
    QPoint moffset;
    QNetworkAccessManager *mnetAccessManager;
    Today mToday;
    Day mDay[6];
    QList<QLabel*> mWeekList;
    QList<QLabel*> mDateList;


    QList<QLabel*> mTypeList;
    QList<QLabel*> mTypeIconList;
    QList<QLabel*> mAqiList;
    QList<QLabel*> mFlList;
    QList<QLabel*> mFxList;
    QMap<QString,QString>mTypeMap;

    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H

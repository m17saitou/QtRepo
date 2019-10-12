#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "task.hpp"
#include "jsonReceive.hpp"
#include "Monte_Carlo.hpp"
#include "Board.hpp"
#include <stdio.h>
#include <map>
#include <string>
#include <QtGui>
#include <iostream>
#include <QTimer>
#include <QString>
#include <QVector>
#include <QColor>
#include <QFuture>
#include <QtConcurrent>
#include <QtWidgets/QtWidgets>
#include <QCoreApplication>
#include <QLineEdit>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QSizePolicy>
#include <QtWidgets/QMainWindow>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QNetworkReply>

class Action;
class QLineEdit;
class Monte_Carlo;
class Board;
class jsonReceive;
class QLabel;
class QString;
class QPushButton;
class QTableWidget;
class QColor;
class QTimer;
class QNetworkReply;
class QNetworkAccessManager;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    int ourTID;
    QFuture <std::vector<Action>> ret;
    MainWindow(QWidget *parent = 0);
    ~MainWindow();
    QString postAgentAct;
    QString serverURLStr;
    std::string jsonNameStr;
    static std::vector<Action> map (const Task& t);
    static void reduce(std::vector<Action>& finalActs,const std::vector<Action>& result);
    int uploadActJson();
    void downloadBoard();
    void onGetBoardJSONFinished(QNetworkReply* reply);
private:
    Board *forDisplayBoard;
    QTimer *timerSearch;
    QLabel *ourTeamID;
    QLineEdit *ourTeamID_Num;
    QLabel *turn;
    QLabel *ourColor;
    QLabel *turn_Num;
    QLabel *ourP;
    QLabel *enemyP;
    QLabel *point;
    QLabel *tile;
    QLabel *area;
    QLabel *total;
    QLabel *ourAreaP;//自チームのエリアポイント表示部分
    QLabel *enemyAreaP;//敵チームのエリアポイント表示部分
    QLabel *ourTileP;//自チームのタイルポイント表示部分
    QLabel *enemyTileP;//敵チームのタイルポイント表示部分
    QLabel *ourPoint;//自チームの合計得点表示部分
    QLabel *enemyPoint;//敵チームの合計得点表示部分
    QLabel *MatchID;
    QLineEdit *matcheditID;//試合番号を入力する部分
    QTableWidget *agentWhereXY;//右下に座標を表示していく表
    QTableWidget *boardDisplay;//盤面を表示する部分
    QTimer *turnInterval; // 1ターン分のインターバルタイマ
    QTimer *sendInterval; // Actionの送信のインターバルタイマ
    QPushButton *getJsonFile;
    QPushButton *startSearch;
    QPushButton *autoBattle;
private slots:
    void getJson();
    void startSearching();
    void timeUP();
    void onPostFinished(QNetworkReply*);
};

#endif // MAINWINDOW_H

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
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QSizePolicy>
#include <QtWidgets/QMainWindow>

class Action;
class Monte_Carlo;
class Board;
class jsonReceive;
class QLabel;
class QString;
class QPushButton;
class QTableWidget;
class QColor;
class QTimer;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    int ourTID;
    QFuture <std::vector<Action>> ret;
    MainWindow(QWidget *parent = 0);
    ~MainWindow();
    QString postAgentAct;
    static std::vector<Action> map (const Task& t);
    static void reduce(std::vector<Action>& finalActs,const std::vector<Action>& result);

private:
    Board *forDisplayBoard;
    std::string jsonNameStr;
    QTimer *timerSearch;
    QLabel *ourTeamID;
    QLabel *ourTeamID_Num;
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
    QTableWidget *agentWhereXY;//右下に座標を表示していく表
    QTableWidget *boardDisplay;//盤面を表示する部分
    QPushButton *getJsonFile;
    QPushButton *startSearch;
    QPushButton *stopSearch;
private slots:
    void getJson();
    void startSearching();
    void timeUP();
};

#endif // MAINWINDOW_H

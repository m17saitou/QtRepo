/*
Qt version 4.8.7
Include Path : /usr/lib/x86_64-linux-gnu/qt4/bin/qmake
盤面の表示用の表の作り方は http://mf-atelier.sakura.ne.jp/mf-atelier/modules/tips/program/Qt/qt_tips.html を参考にしながらやってみる
*/
#include "mainwindow.h"
#include "jsonReceive.hpp"
#include "Board.hpp"
#include <stdio.h>
#include <map>
#include <string>
#include <QtGui>
#include <iostream>
#include <QString>
#include <QtWidgets/QtWidgets>
#include <QCoreApplication>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QSizePolicy>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    QStringList argStr = QCoreApplication::arguments();
    bool ok;
    ourTID = argStr.value(1).toInt(&ok,10);//コマンドライン引数からのチームID読み込み
    ourTeamID = new QLabel(tr("Our TeamID"));//自チームのTeamID表示Label
    ourTeamID_Num = new QLabel("0");
    ourTeamID->setBuddy(ourTeamID_Num);
    turn = new QLabel(tr("Turn"));//盤面のターン表示Label
    turn_Num = new QLabel("0");
    turn->setBuddy(turn_Num);
    ourColor = new QLabel("Our TeamColor : Pink");
    point = new QLabel("Point");
    tile = new QLabel("Tile");
    area = new QLabel("Area");
    total = new QLabel("Total");
    ourP = new QLabel(tr("Our"));
    enemyP = new QLabel(tr("Enemy"));
    ourTileP = new QLabel("0");
    enemyTileP = new QLabel("0");
    ourAreaP = new QLabel("0");
    enemyAreaP = new QLabel("0");
    ourPoint = new QLabel("0");
    enemyPoint = new QLabel("0");

    boardDisplay = new QTableWidget(20,20,this);
    boardDisplay->setFixedSize(630,630);
    boardDisplay->setHorizontalHeaderLabels( QStringList() <<"01"<<"02"<<"03"<<"04"<<"05"<<"06"<<"07"<<"08"<<"09"<<"10"<<"11"<<"12"<<"13"<<"14"<<"15"<<"16"<<"17"<<"18"<<"19"<<"20");
    boardDisplay->setVerticalHeaderLabels( QStringList() <<"01"<<"02"<<"03"<<"04"<<"05"<<"06"<<"07"<<"08"<<"09"<<"10"<<"11"<<"12"<<"13"<<"14"<<"15"<<"16"<<"17"<<"18"<<"19"<<"20");
    for(int i=0;i<20;i++){
        boardDisplay->setRowHeight(i,30);
        boardDisplay->setColumnWidth(i,30);
        for(int j=0;j<20;j++){
            boardDisplay->setCellWidget(i, j, new QLabel());
        }
    }
    boardDisplay->setSelectionMode(QAbstractItemView::NoSelection);
    boardDisplay->setEditTriggers(QAbstractItemView::NoEditTriggers);

    getJsonFile = new QPushButton(tr("GetJsonFile"));
    getJsonFile->setDefault(true);
    getJsonFile->setEnabled(true);
    startSearch = new QPushButton(tr("StartSearch"));
    startSearch->setDefault(true);
    startSearch->setEnabled(true);
    stopSearch = new QPushButton(tr("StopSearch"));
    stopSearch->setDefault(true);
    stopSearch->setEnabled(true);

    connect(getJsonFile,SIGNAL(clicked()),this, SLOT(getJson()));
    connect(startSearch,SIGNAL(clicked()),this, SLOT(startSearching()));

    QHBoxLayout *topArea = new QHBoxLayout();//ターンと自チームのTeamIDのLayout
    topArea->addWidget(ourTeamID);
    topArea->addWidget(ourTeamID_Num);
    topArea->addWidget(turn);
    topArea->addWidget(turn_Num);
    topArea->addWidget(ourColor);
    QVBoxLayout *leftSide = new QVBoxLayout();
    leftSide->addLayout(topArea);
    leftSide->addWidget(boardDisplay);

    QVBoxLayout *pointIndex = new QVBoxLayout();//ポイント表示部分のレイアウト
    pointIndex->addWidget(point);
    pointIndex->addWidget(tile);
    pointIndex->addWidget(area);
    pointIndex->addWidget(total);
    QVBoxLayout *ourPointLayout = new QVBoxLayout();
    ourPointLayout->addWidget(ourP);
    ourPointLayout->addWidget(ourTileP);
    ourPointLayout->addWidget(ourAreaP);
    ourPointLayout->addWidget(ourPoint);
    QVBoxLayout *enemyPointLayout = new QVBoxLayout();
    enemyPointLayout->addWidget(enemyP);
    enemyPointLayout->addWidget(enemyTileP);
    enemyPointLayout->addWidget(enemyAreaP);
    enemyPointLayout->addWidget(enemyPoint);
    QHBoxLayout *pointDisplay = new QHBoxLayout();
    pointDisplay->addLayout(pointIndex);
    pointDisplay->addLayout(ourPointLayout);
    pointDisplay->addLayout(enemyPointLayout);

    QVBoxLayout *buttonLayout = new QVBoxLayout();//ボタン部分のLayout
    buttonLayout->addWidget(getJsonFile);
    buttonLayout->addWidget(startSearch);
    buttonLayout->addWidget(stopSearch);

    QVBoxLayout *rightSide = new QVBoxLayout();
    rightSide->addLayout(pointDisplay);
    rightSide->addLayout(buttonLayout);
    rightSide->addStretch();

    QHBoxLayout *mainLayout = new QHBoxLayout();//メインの全てを含んだLayout
    mainLayout->addLayout(leftSide);
    mainLayout->addLayout(rightSide);
    QWidget *widget = new QWidget();
    widget->setLayout(mainLayout);
    setCentralWidget(widget);//MainWindowのCentralWidgetにmainLayoutを表示させる部分
    
}

void MainWindow::getJson(){
    QString jsonDialogName = QFileDialog::getOpenFileName(this,tr("JsonFile Read"),".","json File (*.json)");
    if(!jsonDialogName.isEmpty()){
        jsonNameStr = jsonDialogName.toUtf8().constData();
    }
    forDisplayBoard = jsonReceive::jsonRead(ourTID,jsonNameStr);
    forDisplayBoard->display();

    std::map <int , int> xyToAgentID;
    for(int i=0;i<forDisplayBoard->num_agent;i++){
        xyToAgentID.emplace((forDisplayBoard->friend_place[i].getY()) * forDisplayBoard->width + (forDisplayBoard->friend_place[i].getX()) , i+1);
        xyToAgentID.emplace((forDisplayBoard->enemy_place[i].getY()) * forDisplayBoard->width + (forDisplayBoard->enemy_place[i].getX()) , -i-1);
    }
    QString *tileP = new QString();
    for(int y=0;y<forDisplayBoard->height;y++){
        for(int x=0;x<forDisplayBoard->width;x++){
            tileP->setNum(forDisplayBoard->field_points[y][x],10);
            decltype(xyToAgentID)::iterator thereAgent = xyToAgentID.find(y*forDisplayBoard->width + x);
            ((QLabel*)(boardDisplay->cellWidget(x,y)))->setAlignment(Qt::AlignCenter);
            if(forDisplayBoard->tiled[y][x] == 1){
                ((QLabel*)(boardDisplay->cellWidget(x,y)))-> setText(*tileP);
                boardDisplay->cellWidget(x,y)->setStyleSheet("background-color: #ffc0cb");
            }//味方のマス 背景色赤色
            else if(forDisplayBoard->tiled[y][x] == -1){
                ((QLabel*)(boardDisplay->cellWidget(x,y)))-> setText(*tileP);
                boardDisplay->cellWidget(x,y)->setStyleSheet("background-color: #00eeee");
            }//敵のマス 背景色青色
            else{
                ((QLabel*)(boardDisplay->cellWidget(x,y)))-> setText(*tileP);
            }
        }
    }
    return;
}
void MainWindow::startSearching(){
    std::cout << jsonNameStr << "\n";
}

MainWindow::~MainWindow()
{

}

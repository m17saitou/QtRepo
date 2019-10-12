/*
Qt version 4.8.7
Include Path : /usr/lib/x86_64-linux-gnu/qt4/bin/qmake
*/
#include "mainwindow.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    QStringList argStr = QCoreApplication::arguments();
    serverURLStr = "http://localhost:8081";
    ourTeamID = new QLabel(tr("Our TeamID"));//自チームのTeamID表示Label
    ourTeamID_Num = new QLineEdit("0");
    turn = new QLabel(tr("Turn"));//盤面のターン表示Label
    turn_Num = new QLabel("0");
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
    MatchID = new QLabel("MatchID : ");
    matcheditID = new QLineEdit("0",nullptr);
    turnIntv = new QLabel("Turn Interval(ms)");
    turnIntv_Num = new QLineEdit("10000",nullptr);
    searchIntv = new QLabel("Search Interval(ms)");
    searchIntv_Num = new QLineEdit("4000",nullptr);

    turnInterval = new QTimer(this);
    timerSearch = new QTimer(this);
    connect(timerSearch, SIGNAL(timeout()),this, SLOT(timeUP()));
    connect(turnInterval,SIGNAL(timeout()),this, SLOT(everyTurn()));

    boardDisplay = new QTableWidget(20,20,this);
    boardDisplay->setFixedSize(626,626);
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

    agentWhereXY = new QTableWidget(16,4,this);
    agentWhereXY->setFixedSize(145,347);
    agentWhereXY->setHorizontalHeaderLabels(QStringList() << "LID" << "GID" << "x" << "y");
    for(int i=0;i<4;i++){
        agentWhereXY->setColumnWidth(i,30);
        for(int j=0;j<16;j++){
            agentWhereXY->setRowHeight(j,20);
            agentWhereXY->setCellWidget(j,i, new QLabel());
            ((QLabel*)(agentWhereXY->cellWidget(j,i)))->setAlignment(Qt::AlignCenter);
        }
    }
    agentWhereXY->setSelectionMode(QAbstractItemView::NoSelection);
    agentWhereXY->setEditTriggers(QAbstractItemView::NoEditTriggers);

    getJsonFile = new QPushButton(tr("GetJsonFile"));
    getJsonFile->setDefault(true);
    getJsonFile->setEnabled(true);
    startSearch = new QPushButton(tr("StartSearch"));
    startSearch->setDefault(true);
    startSearch->setEnabled(true);
    autoBattle = new QPushButton(tr("autoBattle"));
    autoBattle->setDefault(true);
    autoBattle->setEnabled(true);

    connect(getJsonFile,SIGNAL(clicked()),this, SLOT(getJson()));
    connect(startSearch,SIGNAL(clicked()),this, SLOT(startSearching()));
    connect(autoBattle,SIGNAL(clicked()),this, SLOT(autoBattleing()));
    connect(autoBattle,SIGNAL(clicked()),this, SLOT(downloadBoard()));

    QHBoxLayout *topArea = new QHBoxLayout();//ターンと自チームのTeamIDのLayout
    topArea->addWidget(ourTeamID);
    topArea->addWidget(ourTeamID_Num);
    topArea->addStretch();
    topArea->addWidget(turn);
    topArea->addWidget(turn_Num);
    topArea->addStretch();
    topArea->addWidget(ourColor);
    topArea->addStretch();
    topArea->addWidget(MatchID);
    topArea->addWidget(matcheditID);
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
    buttonLayout->addWidget(autoBattle);

    QVBoxLayout *rightSide = new QVBoxLayout();
    rightSide->addWidget(turnIntv);
    rightSide->addWidget(turnIntv_Num);
    rightSide->addWidget(searchIntv);
    rightSide->addWidget(searchIntv_Num);
    rightSide->addLayout(pointDisplay);
    rightSide->addStretch();
    rightSide->addLayout(buttonLayout);
    rightSide->addStretch();
    rightSide->addWidget(agentWhereXY);

    QHBoxLayout *mainLayout = new QHBoxLayout();//メインの全てを含んだLayout
    mainLayout->addLayout(leftSide);
    mainLayout->addLayout(rightSide);
    QWidget *widget = new QWidget();
    widget->setLayout(mainLayout);
    setCentralWidget(widget);//MainWindowのCentralWidgetにmainLayoutを表示させる部分
    
}

void MainWindow::getJson(){
    forDisplayBoard->agentMapG_to_L.clear();
    forDisplayBoard->agentMapL_to_G.clear();
    for(int y=0;y<20;y++){//処理前にクリア
        for(int x=0;x<20;x++){
            ((QLabel*)(boardDisplay->cellWidget(y,x)))-> setText(" ");
            boardDisplay->cellWidget(y,x)->setStyleSheet("background-color: #fff");
            if(y<16 && x<4){
                ((QLabel*)(agentWhereXY->cellWidget(y,x)))->setText(" ");
                agentWhereXY->cellWidget(y,x)->setStyleSheet("background-color: #fff");
            }
        }
    } 
    QString jsonDialogName = QFileDialog::getOpenFileName(this,tr("JsonFile Read"),".","json File (*.json)");
    if(!jsonDialogName.isEmpty()){
        jsonNameStr = jsonDialogName.toUtf8().constData();
    }
    forDisplayBoard = jsonReceive::jsonRead(ourTeamID_Num->text().toInt(nullptr,10),jsonNameStr);

    std::map <int , int> xyToAgentID;
    for(int i=0;i<forDisplayBoard->num_agent;i++){
        xyToAgentID.emplace((forDisplayBoard->friend_place[i].getY()) * forDisplayBoard->width + (forDisplayBoard->friend_place[i].getX()) , i+1);
        xyToAgentID.emplace((forDisplayBoard->enemy_place[i].getY()) * forDisplayBoard->width + (forDisplayBoard->enemy_place[i].getX()) , -i-1);
    }
    QString *tileP = new QString();
    char ourChar = 64,enemyChar = 96;
    int wherecount=0;
    for(int y=0;y<forDisplayBoard->height;y++){
        for(int x=0;x<forDisplayBoard->width;x++){
            *tileP = QString::number(forDisplayBoard->field_points[y][x],10);
            decltype(xyToAgentID)::iterator thereAgent = xyToAgentID.find(y*forDisplayBoard->width + x);
            if(forDisplayBoard->tiled[y][x] == 1){
                if(thereAgent != xyToAgentID.end()){
                    boardDisplay->cellWidget(y,x)->setStyleSheet("background-color: #ffc0cb");
                    ourChar += thereAgent->second;
                    *tileP += QString::fromUtf8(&ourChar,1);
                    int LIDnum = thereAgent->second;
                    ((QLabel*)(agentWhereXY->cellWidget(wherecount,0)))->setText(QString::fromUtf8(&ourChar,1));
                    ((QLabel*)(agentWhereXY->cellWidget(wherecount,1)))->setText(QString::number(Board::getAIdL_to_G(LIDnum)));
                    ((QLabel*)(agentWhereXY->cellWidget(wherecount,2)))->setText(QString::number(x+1));
                    ((QLabel*)(agentWhereXY->cellWidget(wherecount,3)))->setText(QString::number(y+1));
                    agentWhereXY->cellWidget(wherecount,0)->setStyleSheet("background-color: #ffc0cb");
                    wherecount++;
                }
                else boardDisplay->cellWidget(y,x)->setStyleSheet("background-color: #dd88dd");
            }//味方のマス 背景色赤色
            else if(forDisplayBoard->tiled[y][x] == -1){
                if(thereAgent != xyToAgentID.end()){
                    boardDisplay->cellWidget(y,x)->setStyleSheet("background-color: #00eeee");
                    enemyChar += -(thereAgent->second);
                    *tileP += QString::fromUtf8(&enemyChar,1);
                    int LIDnum = thereAgent->second;
                    ((QLabel*)(agentWhereXY->cellWidget(forDisplayBoard->num_agent + (-LIDnum-1),0)))->setText(QString::fromUtf8(&enemyChar,1));
                    ((QLabel*)(agentWhereXY->cellWidget(forDisplayBoard->num_agent + (-LIDnum-1),1)))->setText(QString::number(Board::getAIdL_to_G(LIDnum)));
                    ((QLabel*)(agentWhereXY->cellWidget(forDisplayBoard->num_agent + (-LIDnum-1),2)))->setText(QString::number(x+1));
                    ((QLabel*)(agentWhereXY->cellWidget(forDisplayBoard->num_agent + (-LIDnum-1),3)))->setText(QString::number(y+1));
                    agentWhereXY->cellWidget(forDisplayBoard->num_agent + (-LIDnum-1),0)->setStyleSheet("background-color: #00eeee");
                }
                else boardDisplay->cellWidget(y,x)->setStyleSheet("background-color: #00bfff");
            }//敵のマス 背景色青色
            ((QLabel*)(boardDisplay->cellWidget(y,x)))->setAlignment(Qt::AlignCenter);
            ((QLabel*)(boardDisplay->cellWidget(y,x)))-> setText(*tileP);
            ourChar = 64,enemyChar = 96;
        }
    }
    turn_Num->setNum(forDisplayBoard->turn);
    int ourAP = forDisplayBoard->calcAreaPoint(1),
        enmAP = forDisplayBoard->calcAreaPoint(-1);
    int ourTP = forDisplayBoard->tile_point(1),
        enmTP = forDisplayBoard->tile_point(-1);
    ourAreaP->setNum(ourAP);
    enemyAreaP->setNum(enmAP);
    ourTileP->setNum(ourTP);
    enemyTileP->setNum(enmTP);
    ourPoint->setNum(ourAP + ourTP);
    enemyPoint->setNum(enmAP + enmTP);
    return;
}
void MainWindow::startSearching(){
    QVector <Task> taskArr;
    for(int i=0;i<forDisplayBoard->num_agent;i++){
        Task t = Task(forDisplayBoard,60,10000,1<<i);
        taskArr.append(t);
    }
    ret = QtConcurrent::mappedReduced(taskArr,map, reduce, QtConcurrent::SequentialReduce);
    timerSearch->start(searchIntv_Num->text().toInt());
}

std::vector<Action> MainWindow::map (const Task& t){
    return Monte_Carlo::select_best_uct_select(static_cast<Board&>(*(t.getTaskBoard())),1,static_cast<int>(t.getGameTurn()),t.getUctLoop(),t.getSearchAgentID());//引数の変更に応じて更新すること
}

void MainWindow::reduce(std::vector<Action>& finalActs,const std::vector<Action> &result){
    std::copy(result.begin(), result.end(), std::back_inserter(finalActs));
    //finalActs.push_back(result.size());
}

void MainWindow::timeUP (){
    std::cout << "TimeUP\n";
    std::vector<Action> outPutArr;
    if(ret.isFinished()==true){
        if(ret.resultCount()>0){
            outPutArr =  ret.result();
            for(int i=0;i<outPutArr.size();i++){
                std::cout << outPutArr[i].to_string()<<std::endl;
            }
            std::cout << Action::createJson(outPutArr)<<std::endl;
            postAgentAct = QString::fromStdString(Action::createJson(outPutArr));
        }
        timerSearch->stop();
    }
    int sucsess = MainWindow::uploadActJson();
    
}

int MainWindow::uploadActJson(){
    std::cout << "uploadActJson" <<std::endl;
    QString matchID=MainWindow::matcheditID->text();
    QUrl url = QUrl(serverURLStr+"/matches/"+matchID+"/action");
    QNetworkAccessManager * mgr = new QNetworkAccessManager(this);
    QNetworkRequest request(url);
    QString currentToken="553e013f0028cebf9997c457f84360af3c3d8ba6b5b6ba7b4e622f6b3685050d";
    connect(mgr, SIGNAL(finished(QNetworkReply*)), this, SLOT(onPostFinished(QNetworkReply*)));//送信が終わったときにonPostFinish(QNetworkReply* reply)が呼ばれる
    request.setHeader( QNetworkRequest::ContentTypeHeader, "application/json" );
    request.setRawHeader(QByteArray("Authorization"),currentToken.toUtf8());//tokenの設定//currentTokenはQStingなのでutf8に変換して与える。
    QString jsonStr=MainWindow::postAgentAct;//エージェントのアクションのjsonの文字列(QString)
    mgr->post(request, jsonStr.toUtf8());
    return 0;
};

void MainWindow::onPostFinished(QNetworkReply* reply){
    QByteArray bytes = reply->readAll();
    QString str = QString::fromUtf8(bytes.data(), bytes.size());
    cout <<"replay="<< str.toStdString()<< endl;
}

void  MainWindow::downloadBoard(){
    std::cout << "downloadBoard" << std::endl;
    QString matchID=MainWindow::matcheditID->text();
    QUrl url = QUrl(serverURLStr+"/matches/"+matchID+"/");
    QNetworkAccessManager * mgr = new QNetworkAccessManager(this);
    QNetworkRequest request(url);
    QString currentToken="553e013f0028cebf9997c457f84360af3c3d8ba6b5b6ba7b4e622f6b3685050d";
    connect(mgr, SIGNAL(finished(QNetworkReply*)), this, SLOT(onGetBoardJSONFinished(QNetworkReply*)));//送信が終わったときにonPostFinish(QNetworkReply* reply)が呼ばれる
    //request.setHeader( QNetworkRequest::ContentTypeHeader, "application/json" );
    request.setRawHeader(QByteArray("Authorization"),currentToken.toUtf8());//tokenの設定//currentTokenはQStingなのでutf8に変換して与える。
    mgr->get(request);
    return;
}

void MainWindow::onGetBoardJSONFinished(QNetworkReply* reply){

    QString str;
    if(reply->error() == QNetworkReply::NoError)
    {
        QFile file;
        file.setFileName("download.json");
        if (!file.open(QIODevice::WriteOnly)) return;
        str = tr("ダウンロード正常終了");
        QByteArray bytes = reply->readAll();
        QString boardJSONstr = QString::fromUtf8(bytes.data(), bytes.size());
        cout <<"replay="<< boardJSONstr.toStdString()<< endl;
        file.write(bytes);
        file.close();
    }
    else
    {
        str = reply->errorString();
        str +=  tr( "    ダウンロード異常終了" );
    }
    cout << str.toStdString()<< endl;
    return;
}

void MainWindow::autoBattleing(){
    forDisplayBoard = jsonReceive::jsonRead(MainWindow::ourTeamID_Num->text().toInt(nullptr,10),"download.json");
    MainWindow::boardReload(forDisplayBoard);
    MainWindow::autoBattle->setEnabled(false);
    turnInterval->start(turnIntv_Num->text().toInt());
    MainWindow::startSearching();
}

void MainWindow::everyTurn(){
    MainWindow::turnInterval->stop();
    MainWindow::turnInterval->start();
    MainWindow::forDisplayBoard->agentMapG_to_L.clear();
    MainWindow::forDisplayBoard->agentMapL_to_G.clear();
    MainWindow::downloadBoard();
    std::cout << "jsonreceive\n";
    forDisplayBoard = jsonReceive::jsonRead(MainWindow::ourTeamID_Num->text().toInt(nullptr,10),"download.json");
    std::cout << "boardReload\n";
    MainWindow::boardReload(forDisplayBoard);
    std::cout <<"startSrae\n";
    MainWindow::startSearching();
}

void MainWindow::boardReload(Board* dspBoard){
    for(int y=0;y<20;y++){//処理前にクリア
        for(int x=0;x<20;x++){
            ((QLabel*)(MainWindow::boardDisplay->cellWidget(y,x)))-> setText(" ");
            MainWindow::boardDisplay->cellWidget(y,x)->setStyleSheet("background-color: #fff");
            if(y<16 && x<4){
                ((QLabel*)(agentWhereXY->cellWidget(y,x)))->setText(" ");
                agentWhereXY->cellWidget(y,x)->setStyleSheet("background-color: #fff");
            }
        }
    } 
    
    std::map <int , int> xyToAgentID;
    for(int i=0;i<dspBoard->num_agent;i++){
        xyToAgentID.emplace((dspBoard->friend_place[i].getY()) * dspBoard->width + (dspBoard->friend_place[i].getX()) , i+1);
        xyToAgentID.emplace((dspBoard->enemy_place[i].getY()) * dspBoard->width + (dspBoard->enemy_place[i].getX()) , -i-1);
    }
    QString *tileP = new QString();
    char ourChar = 64,enemyChar = 96;
    int wherecount=0;
    for(int y=0;y<dspBoard->height;y++){
        for(int x=0;x<dspBoard->width;x++){
            *tileP = QString::number(dspBoard->field_points[y][x],10);
            decltype(xyToAgentID)::iterator thereAgent = xyToAgentID.find(y*dspBoard->width + x);
            if(dspBoard->tiled[y][x] == 1){
                if(thereAgent != xyToAgentID.end()){
                    MainWindow::boardDisplay->cellWidget(y,x)->setStyleSheet("background-color: #ffc0cb");
                    ourChar += thereAgent->second;
                    *tileP += QString::fromUtf8(&ourChar,1);
                    int LIDnum = thereAgent->second;
                    ((QLabel*)(agentWhereXY->cellWidget(wherecount,0)))->setText(QString::fromUtf8(&ourChar,1));
                    ((QLabel*)(agentWhereXY->cellWidget(wherecount,1)))->setText(QString::number(Board::getAIdL_to_G(LIDnum)));
                    ((QLabel*)(agentWhereXY->cellWidget(wherecount,2)))->setText(QString::number(x+1));
                    ((QLabel*)(agentWhereXY->cellWidget(wherecount,3)))->setText(QString::number(y+1));
                    agentWhereXY->cellWidget(wherecount,0)->setStyleSheet("background-color: #ffc0cb");
                    wherecount++;
                }
                else MainWindow::boardDisplay->cellWidget(y,x)->setStyleSheet("background-color: #dd88dd");
            }//味方のマス 背景色赤色
            else if(dspBoard->tiled[y][x] == -1){
                if(thereAgent != xyToAgentID.end()){
                    MainWindow::boardDisplay->cellWidget(y,x)->setStyleSheet("background-color: #00eeee");
                    enemyChar += -(thereAgent->second);
                    *tileP += QString::fromUtf8(&enemyChar,1);
                    int LIDnum = thereAgent->second;
                    ((QLabel*)(agentWhereXY->cellWidget(dspBoard->num_agent + (-LIDnum-1),0)))->setText(QString::fromUtf8(&enemyChar,1));
                    ((QLabel*)(agentWhereXY->cellWidget(dspBoard->num_agent + (-LIDnum-1),1)))->setText(QString::number(Board::getAIdL_to_G(LIDnum)));
                    ((QLabel*)(agentWhereXY->cellWidget(dspBoard->num_agent + (-LIDnum-1),2)))->setText(QString::number(x+1));
                    ((QLabel*)(agentWhereXY->cellWidget(dspBoard->num_agent + (-LIDnum-1),3)))->setText(QString::number(y+1));
                    agentWhereXY->cellWidget(dspBoard->num_agent + (-LIDnum-1),0)->setStyleSheet("background-color: #00eeee");
                }
                else MainWindow::boardDisplay->cellWidget(y,x)->setStyleSheet("background-color: #00bfff");
            }//敵のマス 背景色青色
            ((QLabel*)(MainWindow::boardDisplay->cellWidget(y,x)))->setAlignment(Qt::AlignCenter);
            ((QLabel*)(MainWindow::boardDisplay->cellWidget(y,x)))-> setText(*tileP);
            ourChar = 64,enemyChar = 96;
        }
    }
    turn_Num->setNum(dspBoard->turn);
    int ourAP = dspBoard->calcAreaPoint(1),
        enmAP = dspBoard->calcAreaPoint(-1);
    int ourTP = dspBoard->tile_point(1),
        enmTP = dspBoard->tile_point(-1);
    ourAreaP->setNum(ourAP);
    enemyAreaP->setNum(enmAP);
    ourTileP->setNum(ourTP);
    enemyTileP->setNum(enmTP);
    ourPoint->setNum(ourAP + ourTP);
    enemyPoint->setNum(enmAP + enmTP);
    cnt++;
    return;
}

MainWindow::~MainWindow()
{

}

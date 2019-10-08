#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtWidgets/QMainWindow>

class Board;
class jsonReceive;
class QLabel;
class QString;
class QPushButton;
class QTableWidget;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    int ourTID;
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Board *forDisplayBoard;
    std::string jsonNameStr;
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
};

#endif // MAINWINDOW_H

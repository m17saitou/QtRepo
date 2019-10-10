#pragma once
#include <stdexcept>
#include <iostream>
#include "Agent-Action.hpp"
#include <map>
#include <set>
#include <vector>
#include <random>
#include <algorithm>
#include <cstring>
#include "Point.hpp"
using namespace std;
class jsonReceive;
class Board
{
public:
    static int **field_points;           ///符号あり各マスのフィールド得点
    static int width;                    //符号なし　高さ
    static int height;                   //符号なし　横幅
    int turn;                            //符号なし　ターン数
    int **tiled;                         //各チームのタイルの位置　自チームを１　相手チームを-1 何もないところを0,壁の部分をを5
    static int num_agent;                //1チーム分のエージェントの数
    Point *friend_place;                 //味方エージェント
    Point *enemy_place;                  //敵エージェント
    static const int our = 1;            //自チーム
    static const int enemy = -1;         //敵チーム
    static const int empty = 0;          //空
    static int endturn;                  //ゲームの終了ターン
    static map<int, int> agentMapG_to_L; //Json(Global)から与えられたAgentIDとこのプログラム内(Local)で扱うAgentIDを変換するMap
    static map<int, int> agentMapL_to_G; //local -> Global
    static int8_t direction[][2];//x,yの順番で変位
    Board();
    Board(int turn0, int **tiled0, Point friend_place0[], Point enemy_place0[]);
    Board(int turn0, char tiled0[], Point friend_place0[], Point enemy_place0[]);
    Board(int turn0, int **tiled0, Point friend_place0[], Point enemy_place0[], Action action_pending0[],vector<int> wait_agent0);
    Board *copy();
    Board *overwrite_copy();
    void overwrite(Board &board);
    ~Board();

    static void setting(int w, int h);
    inline void setTurn(int newTurn)
    {
        turn = newTurn;
    } //ターンを更新
    inline int getTurn()
    {
        return turn;
    } //現在のターン数
    inline void set_one_tile(int x, int dx, int dy)
    {
        tiled[dy][dx] = x;
    } //1マスに対してtileの値を格納
    inline int getTile(int dx, int dy)
    {
        return tiled[dy][dx];
    } //タイルの値を取り出す

    static inline void set_num_agent(int number)
    {
        num_agent = number;
    } //1チームの人数の代入
    inline int get_num_agent()
    {
        return num_agent;
    } //1チームの人数の取得
    bool movewall(int agentID, int dx, int dy){//動けるならtrue,壁ならfalse
        //dx,dyは-1~1の整数値
        Point agentXY;
        agentXY = getAgent_place(agentID); //agentIDのエージェントがどこにいるかゲッターで持ってくる
        if ((agentXY.getX() + dx < 0) || (agentXY.getY() + dy < 0) || (agentXY.getX() + dx >= width) || (agentXY.getY() + dy >= height))
            return false;
        else
            return true; //tiled[dx+returnしたやつ][dy+returnしたやつ]が0より小さい、もしくはwidth,heightより大きいならfalseをreturn,そうでないならtrueをreturnする
    }
    bool movewall(int x,int y){
        if(x < 0)return false;
        else if(y < 0)return false;
        else if(x >= width)return false;
        else if(y >= height)return false;
        else return true;
    }
    void display();
    int tile_point(int side);
    int area_point(int side);
    static void set_field_Point(char point[]);    //タイルの値を挿入する
    static int get_field_point(int dx, int dy);   //タイルの値を取り出す
    void setTile(char jsontiled[], int h ,int w); //タイルの種類を格納
    static int addAgentMap(int globalAID, int localAID); //agentIDをJsonで受け取ったものからプログラム内で使うものに変換
    static int getAIdG_to_L(int globalAID){              //GlobalAgentID(Jsonで受け取る)を引数としLocalAgentIDを返す
        return agentMapG_to_L.at(globalAID);
    }
    static int getAIdL_to_G(int localAID){               //LocalAgentID(プログラム内で使う)を引数としGlobalAgentIDを返す
        return agentMapL_to_G.at(localAID);
    }
    int action(Action *action0, int number_agent);
    static int agent_type(int id);       //敵か味方か判断する
    Point getAgent_place(int id); //idを渡したらそのエージェントの座標を返す
    int calcAreaPoint(int teamID);//引数のIDのチームのエリアポイントを計算するメソッド
    void tracking(int teamID, int firstX, int firstY, vector<multiset<uint8_t>> *startX_endX);//エリアの境界をたどるメソッド．辿り始める座標を受け取る．vectorの中身は開始地点，終了地点のx座標
    static void displayArea(vector<multiset<uint8_t>> *startX_endX);
    // static void delField_Points();

    Action *action_pending;
    vector<int> wait_agent;
    void set_action(Action* act,int idx);
    void set_action(Action* act);
    void set_action(Action &act);
    void pending_check(Action *act,int idx);
    int count_score();
    void move(Action play_action[]);
    void display_action();

    vector<int>* get_wait_agent(){
        return &wait_agent;
    }

    bool invalid_check(int id,int num){//動けるならtrue,動けないならfalseを返す
        int dxy = num%8;
        int type = num/8;
        int dx = dx_dy[dxy].getX();
        int dy = dx_dy[dxy].getY();
        Point agentXY = getAgent_place(id);
        agentXY.addDXDY(dx,dy);
        if(type==0){//move
            //cout<<"id:"<<id<<" x:"<<agentXY.getX()<<" y:"<<agentXY.getY()<<" tile:"<<getTile(agentXY.getX(),agentXY.getY())<<endl;
            if(!movewall(agentXY.getX(),agentXY.getY()))return false;
            else if(getTile(agentXY.getX(),agentXY.getY()) == -(agent_type(id)))return false;
            else return true;
        }
        else if(type==1){//remove
            if(!movewall(agentXY.getX(),agentXY.getY()))return false;
            else if(getTile(agentXY.getX(),agentXY.getY()) == 0)return false;
            else return true;
        }
        else if(type==2){//stay
            return true;
        }
        else {
            std::cout<<"idが不正です@invalid_check";
            exit(0);
        }
    }
    static Point dx_dy[8];
};
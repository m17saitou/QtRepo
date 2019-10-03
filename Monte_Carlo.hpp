#pragma once
#include "Board.hpp"
#include "Point.hpp"
#include "Agent-Action.hpp"
#include "Node.hpp"
#include "bit_calculation.hpp"
#include <vector>
#include <random>
#include <json/json.h>

class Monte_Carlo
{
  public:
    static const long MAX_CHILD; //6975757441//その盤面から取りうる全ての状況
    static const int MAX_NODE;  //最大登録ノード数

    typedef struct child
    {
      int games;           //この手を選んだ回数
      double rate;         //この手の勝率
      Action child_move[16]; //手の場所
    } CHILD;

    typedef struct node_friend
    {
      int child_num; //子局面の数
      CHILD child[100];
      int child_games_sum; //子局面の回数の合計
    } NODE_FRIEND;
    
    typedef struct node_enemy
    {
      int child_num; //子局面の数
      CHILD child[100];
      int child_games_sum; //子局面の回数の合計
    } NODE_ENEMY;

    static int all_playouts;    //playoutを行った回
    static int node_num_friend;        //現在登録しているノードの数
    static int node_num_enemy;        //現在登録しているノードの数
    static NODE_FRIEND node_friend[100]; //ノードの要素を格納
    static NODE_ENEMY node_enemy[100]; //ノードの要素を格納 

    static int playout(Board &board, int max_turn);

    static vector<Action*> select_best_uct(Board &board,int which_turn);
    //static void add_child(NODE *pNode,Action action[]);
    static int search_uct(int which_turn,Node* pN,Board &board);
};
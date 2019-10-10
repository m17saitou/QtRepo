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
    static int all_playouts;    //playoutを行った回

    static int playout(Board &board, int max_turn,int which_turn);

    static vector<Action> select_best_uct(Board &board,int which_turn,int remainingTurn);
    static int search_uct(int which_turn,Node* pN,Board &board,int remainingTurn);
    
    static vector<Action> select_best_uct_select(Board &board,int which_turn,int remainingTurn,int uct_loop,uint16_t agent_bit);
    static int search_uct_select(int which_turn,Node* pN,Board &board,int remainingTurn,uint16_t agent_bit);    
};

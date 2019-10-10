/*
*Agentの動きを扱うためのクラス．
 */
#include <sstream>
#include <vector>
#pragma once

class Action{//エージェントの行動のクラス 他のプログラムとも共用
    public:
        enum actionType {Move=0 ,Remove=1 ,Stay=2};//列挙型Actiontypeを宣言．エージェントの行動種類を表す．
    private:
        actionType agentAction;
        int8_t agent_ID;//エージェントID
        int8_t direction_X;//行動のx方向の向き -1:Left 0:Center 1:Right
        int8_t direction_Y;//行動のy方向の向き -1:Under 0:Center 1:Upper

    public:
        Action(){
            agent_ID = 0;
            agentAction = Stay;//初期化内容は停留を表すStay
            direction_X = 0;
            direction_Y = 0;
        };//コンストラクタの宣言

        void setActionType(enum actionType action){this->agentAction = action;};
        enum actionType getActionType(){return this->agentAction;};
        void setAgentID(int8_t id){this->agent_ID = id;};
        int8_t getAgentID(){return this->agent_ID;};
        void setDX(int8_t dx){this->direction_X = dx;};
        int8_t getDX(){return this->direction_X;};
        void setDY(int8_t dy){this->direction_Y = dy;};
        int8_t getDY(){return this->direction_Y;};
        static std::string createJson(std::vector<Action> act);
        std::string to_string(){//chikara:追加した
          std::stringstream ss;
          ss <<"[" << (int)getAgentID() << ":";
          if(getActionType()==actionType::Move) ss << "move:";
          else if(getActionType()==actionType::Remove) ss<< "remove:";
          else if(getActionType()==actionType::Stay) ss<< "stay:";
          ss << "(" << (int)getDX() << "," << (int)getDY() << ")]";
          return ss.str();
        }
        ~Action(){};
};

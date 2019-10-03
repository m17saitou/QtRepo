/*
*Agentの動きを扱うためのクラス．
 */
#include <sstream>
#pragma once

class Action{//エージェントの行動のクラス 他のプログラムとも共用
    public:
        enum actionType {Move=0 ,Remove=1 ,Stay=2};//列挙型Actiontypeを宣言．エージェントの行動種類を表す．
    private:
        actionType agentAction;
        int agent_ID;//エージェントID
        int direction_X;//行動のx方向の向き -1:Left 0:Center 1:Right
        int direction_Y;//行動のy方向の向き -1:Under 0:Center 1:Upper

    public:
        Action(){
            agent_ID = 0;
            agentAction = Stay;//初期化内容は停留を表すStay
            direction_X = 0;
            direction_Y = 0;
        };//コンストラクタの宣言

        void setActionType(enum actionType action){this->agentAction = action;};
        enum actionType getActionType(){return this->agentAction;};
        void setAgentID(int id){this->agent_ID = id;};
        int getAgentID(){return this->agent_ID;};
        void setDX(int dx){this->direction_X = dx;};
        int getDX(){return this->direction_X;};
        void setDY(int dy){this->direction_Y = dy;};
        int getDY(){return this->direction_Y;};
        std::string to_string(){//chikara:追加した
          std::stringstream ss;
          ss <<"[" << getAgentID() << ":";
          if(getActionType()==actionType::Move) ss << "move:";
          else if(getActionType()==actionType::Remove) ss<< "remove:";
          else if(getActionType()==actionType::Stay) ss<< "stay:";
          ss << "(" << getDX() << "," << getDY() << ")]";
          return ss.str();
        }
        ~Action(){};
};

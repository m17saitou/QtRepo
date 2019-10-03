#include "Node.hpp"
#include "Board.hpp"
#include <float.h>
#include <map>

Node* Node::create_node(int which_turn,Board &board)
{  
    uint16_t bit = UINT16_C(0);
    int cnt = 0;
    vector<int>* wait_agent = board.get_wait_agent();
    //cout<<"size:"<<wait_agent->size()<<endl;
    for(int i=0;i<wait_agent->size();i++){
        int type = wait_agent->at(i);
        //cout<<type<<endl;
        if(Board::agent_type(type)!= which_turn) continue;
        if(type > 0){
            bit = bit_calculation::high_add_bit(which_turn,bit,type-1); 
            //bit |= (UINT16_C(1)<<(type-1));
        }
        else if(type < 0){
            bit = bit_calculation::high_add_bit(which_turn,bit,-type-1);
            //bit |= (UINT16_C(0x100)<<(-type-1));
        }
        cnt++;
    }
    //cout<<"bit:"<<bit<<endl;
    Node* new_node = new Node(bit,cnt);
    //new_node->print_agent_bit();
    //new_node->set_active_agent(bit);
    cnt = 0;
    if(which_turn == 1){
        for(int i=0;i<board.num_agent;i++){
            bit = UINT16_C(0);
            if(!bit_calculation::low_return_bit(i,new_node->get_active_agent()))continue;
            for(int j=0;j<16;j++){
                if(board.invalid_check(i+1,j)){
                    //cout<<"i:"<<i<<" j:"<<j<<endl;
                    bit = bit_calculation::low_add_bit(bit,j);
                    //cout<<"bit:"<<hex<<bit<<endl;
                }
            }
            //cout<<"bit:"<<hex<<bit<<" cnt:"<<cnt<<endl;
            new_node->set_agent_bit(bit,cnt);
             cnt++;
        }  
    }
    if(which_turn == -1){
        for(int i=0;i<board.num_agent;i++){
            bit = UINT16_C(0);
            if(!bit_calculation::low_return_bit(i+8,new_node->get_active_agent()))continue;
            for(int j=0;j<16;j++){
                if(board.invalid_check(-(i+1),j)){
                    bit = bit_calculation::low_add_bit(bit,j);
                }
            }
            new_node->set_agent_bit(bit,cnt);
            cnt++;
        }
    }
    return new_node;
}
Action* Node::select_best_agent_action(int id){
    double best_rate = -DBL_MAX;
    int best_dxdy = -1;
    if(id > 0){
        for(int i = 0;i< 16;i++){
            Child* child = get_children((uint16_t)(id*17)+i);
            if(child != nullptr){
                double rate = child->getRate();
                if(best_rate < rate){
                    best_rate = rate;
                    best_dxdy = i;
                }
            }
        }
    }
    else if(id < 0){
        for(int i = 0;i< 16;i++){
            Child* child = get_children((uint16_t)(id*17)-i);
            if(child != nullptr){
                double rate = child->getRate();
                if(best_rate < rate){
                    best_rate = rate;
                    best_dxdy = i;
                }
            }
        }
    }
    Action* action = new Action();
    action->setAgentID(id);
    int action_type = best_dxdy / 17;
    if(action_type == 0)action->setActionType(Action::actionType::Move);//stayは今の所ない
    else if(action_type == 1)action->setActionType(Action::actionType::Remove);
    action->setDX(Board::dx_dy[best_dxdy/2].getX());
    action->setDY(Board::dx_dy[best_dxdy/2].getY());
    return action;
}

/*
int main(){
      cout<<"はじめ"<<endl;
      Board *board = jsonReceive::jsonRead(1,"A-1.json");
      board->display();
      Node* node = Node::create_node(1,*board);
      node->print_agent_bit();
      cout<<node->get_active_agent()<<endl;
}*/
/*
int main(){
    
    return 0;
}*/
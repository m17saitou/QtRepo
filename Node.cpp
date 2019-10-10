#include "Node.hpp"
#include "Board.hpp"
#include <float.h>
#include <map>

Node* Node::create_node(int which_turn,Board &board)//which_turn:1は味方で,-1が敵
{
    uint16_t bit = UINT16_C(0);
    int cnt = 0;
    vector<int>* wait_agent = board.get_wait_agent();
    //cout << "at create_node()wait_agent=";
    //for(vector<int>::iterator i=wait_agent->begin();i!=wait_agent->end();i++){
      //cout <<(*i) << ",";
    //}
    //cout << endl;
    //cout<<"which_turn:"<<which_turn<<endl;
    for(int i=0;i<wait_agent->size();i++){
        int type = wait_agent->at(i);
        //cout<<type<<endl;
        if(Board::agent_type(type)!= which_turn) continue;
        if(type > 0){
            //bit = bit_calculation::high_add_bit(which_turn,bit,type-1);
            bit |= (UINT16_C(1)<<(type-1));
        }
        else if(type < 0){
            //cout << i << ":"<<(-type-1+8) << endl;
            //bit = bit_calculation::high_add_bit(which_turn,bit,-type-1);
            bit |= (UINT16_C(0x100)<<(-type-1));
        }
        cnt++;
    }
    //cout<<"at crete_node()//bit:"<<bit<<endl;
    Node* new_node = new Node(bit,cnt);
    //new_node->print_agent_bit();
    //new_node->set_active_agent(bit);
    //cout << std::hex << "new_node::active_agent=0x" << new_node->get_active_agent() << std::dec << endl;
    cnt = 0;
    if(which_turn == 1){
        for(int i=0;i<board.num_agent;i++){
            bit = UINT16_C(0);
            if(!bit_calculation::low_return_bit(i,new_node->get_active_agent()))continue;
            for(int j=0;j<16;j++){
                if(board.invalid_check(i+1,j)){//agentID=i+1,action番号=j
                    //cout<<"i:"<<i<<" j:"<<j<<endl;
                    bit = bit_calculation::low_add_bit(bit,j);//動けるビットを立てる
                    //cout<<"bit:"<<hex<<bit<<dec<<endl;
                }
            }
            //cout<<"bit:"<<hex<<bit<<" cnt:"<<cnt<<endl;
            new_node->set_agent_bit(bit,cnt);//動けるエージェントのcnt番目の動けるアクションを表す値を保存する
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
Action Node::select_best_agent_action(int id){
    double best_rate = -DBL_MAX;
    int best_dxdy = -1;
    //cout << "children=";
    //for(map<int16_t,Child*>::iterator it=children.begin();it!=children.end();it++){
      //cout << "(" <<it->first<<","<< std::hex <<it->second<< ")" << std::dec;
    //}
    //cout <<endl;
    if(id > 0){
        for(int i = 0;i< 16;i++){
            //Child* child = get_children((uint16_t)(id*17)+i);//chikaraキャストの意味ないためコメントアウト
            Child* child = get_children((int16_t)(id*17+i));//chikara修正のため差し替え追加:キャストを有効に
            if(child != nullptr){
                double rate = child->getRate();
                //cout << i << ":rate=" << rate << endl;
                if(best_rate < rate){
                    best_rate = rate;
                    best_dxdy = i;
                }
            }
        }
    }
    else if(id < 0){
        for(int i = 0;i< 16;i++){
            //Child* child = get_children((uint16_t)(id*17)-i);//chikaraキャストの意味ないためコメントアウト
            Child* child = get_children((int16_t)(id*17-i));//chikara修正のため差し替え追加:キャストを有効に
            if(child != nullptr){
                double rate = child->getRate();
                if(best_rate < rate){
                    best_rate = rate;
                    best_dxdy = i;
                }
            }
        }
    }
    if(best_dxdy==-1) throw runtime_error("bestが選ばれない");
    Action action = Action();//chikara:引数がフィールドの数だけあるコンストラクタを使ったほうが良いが保留
    action.setAgentID(id);
    //cout << "id=" << id << endl;
    //cout << "best_dxdy=" <<best_dxdy << endl;
    //int action_type = best_dxdy / 17;//chikaraコメントアウト
    int action_type = best_dxdy / 8;//chikara修正のため差し替え追加
    if(action_type == 0)action.setActionType(Action::actionType::Move);//stayは今の所ない
    else if(action_type == 1)action.setActionType(Action::actionType::Remove);//chikara::stayの場合も書くべき、そのうちバグの原因になる
    //action->setDX(Board::dx_dy[best_dxdy/2].getX());//chikaraコメントアウト
    //action->setDY(Board::dx_dy[best_dxdy/2].getY());//chikaraコメントアウト
    //cout <<"("<< Board::dx_dy[best_dxdy%8].getX() <<"," << Board::dx_dy[best_dxdy%8].getY() <<")" << endl;
    action.setDX(Board::dx_dy[best_dxdy%8].getX());//chikara修正のため差し替え追加
    action.setDY(Board::dx_dy[best_dxdy%8].getY());//chikara修正のため差し替え追加
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

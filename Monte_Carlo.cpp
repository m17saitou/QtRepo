#include "Monte_Carlo.hpp"
#include "jsonReceive.hpp"
#include <stdlib.h>
#include <cstdlib>
#include <time.h>
#include <vector>
#include <float.h>
#include <chrono>
#include <iostream>
using namespace std;

const long Monte_Carlo::MAX_CHILD=1000;
const int Monte_Carlo::MAX_NODE=100;
int Monte_Carlo::all_playouts;    //playoutを行った回
int Monte_Carlo::node_num_friend;        //現在登録しているノードの数
int Monte_Carlo::node_num_enemy;        //現在登録しているノードの数

Monte_Carlo::NODE_FRIEND Monte_Carlo::node_friend[100]; //ノードの要素を格納
Monte_Carlo::NODE_ENEMY Monte_Carlo::node_enemy[100]; //ノードの要素を格納


int Monte_Carlo::playout(Board &board, int max_turn)
{ //プレイアウトをおこなう関数
      all_playouts++;
      random_device rnd;
      mt19937 mt(rnd());
      //srand(0);//chikara:一時的に試した
      //mt19937 mt(0);
      Board* pCopy_board = board.copy();
      Action play_action;
     // cout<<"actionの配列を用意";
      for (int cnt_turn = 0; cnt_turn < max_turn - board.turn; cnt_turn++)
      {
            //cout<<cnt_turn+1<<"たーん目"<<endl;
            for(int i = 0;i < pCopy_board->num_agent;i++)
            {
                  uniform_int_distribution<int> rand_action(0,(17*(pCopy_board->num_agent-i))-1);
                  //cout<<"乱数を用意\n";
                  for(int j=0;j<2;j++){
                        int num = rand_action(mt);
                        int ID;
                        int idx;
                        if(j==0){
                              ID = pCopy_board->wait_agent[num/17];
                              idx=num/17;
                        }
                        else if(j==1){
                              ID = pCopy_board->wait_agent[(num/17)+(pCopy_board->num_agent-i)-1];
                              idx=(num/17)+(pCopy_board->num_agent-i)-1;
                        }
                        //cout<<"IDを追加\n";
                        //cout<<"IDは"<<ID<<"\n";
                        play_action.setAgentID(ID);
                        while(true){
                              int dxy = (num%17)%8;
                              int dx = Board::dx_dy[dxy].getX();
                              int dy = Board::dx_dy[dxy].getY();
                              if(pCopy_board->movewall(ID,dx,dy)==true){
                                    play_action.setDX(dx);
                                    play_action.setDY(dy);
                                    //cout<<"dxは"<<dx<<endl;
                                    //cout<<"dyは"<<dy<<endl;
                                    break;
                              }
                              num = rand_action(mt);
                        }
                        if((num%17)/8==0)play_action.setActionType(Action::actionType::Move);
                        else if((num%17)/8==1)play_action.setActionType(Action::actionType::Remove);
                        else if((num%17)/8==2)play_action.setActionType(Action::actionType::Stay);
                        else {
                              //cout<<num<<" "<<num%17<<" "<<((num%17)/8)<<endl;
                              exit(0);
                        }
                        //cout<<"行動を格納\n";
                        pCopy_board->display_action();//コメントにしたらなんか動かなくなる
                        //cout <<"play_action:"<< play_action.to_string() << endl;
                        pCopy_board->pending_check(&play_action,idx);
                        //cout<<""<<endl;
                  }
            }
      }
      return pCopy_board->count_score();
}

vector<Action*> Monte_Carlo::select_best_uct(Board &board,int which_turn)
{ //UCTを繰り返す関数//rootのノードを作成し、ある回数UCTを繰り返し、終わったら一番多く選ばれた手を返す
      Node* node = Node::create_node(which_turn,board);
      int uct_loop = 100;
      Board* board_copy = board.overwrite_copy();
      for(int i = 0;i < uct_loop;i++){
            search_uct(which_turn,node,board);
            which_turn*=(-1);
            board_copy->overwrite(board);
      }
      vector<Action*> best_action;
      for(int i = 0;i < Board::num_agent;i++){
            Action* action = node->select_best_agent_action(i+1);
            best_action.push_back(action);
            action = node->select_best_agent_action(-i-1);
            best_action.push_back(action);
      }
      return best_action;
}

int Monte_Carlo::search_uct(int which_turn,Node* pN,Board &board)//which_turnは1(味方)か-1(敵)のみをとる
{ //UCBで手を選びプレイアウトを行う関数
  //全ての手の中でUCB値が一番高い手を選ぶ
  //未展開の手がある場合はそれを優先的に選ぶ
      srand(time(NULL));
      //cout<<"乱数の種を用意@search_uct"<<endl;
      int select_id = 0;
      int select_dxdy = -1;
      double max_ucb = -DBL_MAX;
      int cnt = 0;
      Child* select_c;
      Child* c;
      if(which_turn == 1){
            //cout<<"which_turnが味方"<<endl;
            for(int i=0;i<board.num_agent;i++){
                  double ucb = 0;
                  if(!bit_calculation::low_return_bit(i,pN->get_active_agent()))continue;
                  for(int j=0;j<16;j++){
                        if(pN->get_agent_bit(cnt) & (1<<j) != 0){
                              map<int16_t,Child*>::iterator i_children = pN->children.find((i+1)*17+j);
                              if(i_children == pN->children.end()){
                                    ucb = 10000 + rand();
                                    c = nullptr;
                                    //cout<<"手がない"<<endl;
                              }
                              else{
                                    c = i_children->second;
                                    const double W = 0.31;
                                    ucb = c->getRate() + W*sqrt(log(pN->get_game_sum())/c->getGames());
                                    //cout<<"手がある"<<endl;
                              }
                              if(ucb > max_ucb){
                                     max_ucb = ucb;
                                     select_id = i+1;
                                     select_dxdy = j;
                                     select_c = c;
                                     //cout<<"ucbを更新"<<endl;
                              }
                        }
                  }
            cnt++;
            }
      }
      else if(which_turn == -1){
           // cout<<"which_turnが敵"<<endl;
            for(int i=0;i<board.num_agent;i++){
                  double ucb = 0;
                  if(!bit_calculation::low_return_bit(i,pN->get_active_agent()))continue;
                  for(int j=0;j<16;j++){
                        if(pN->get_agent_bit(cnt) & (1<<j) != 0){
                              map<int16_t,Child*>::iterator i_children = pN->children.find((i-1)*17-j);
                              if(i_children == pN->children.end()){
                                    ucb = 10000 + rand();
                                    c = nullptr;
                                    //cout<<"手がない"<<endl;
                              }
                              else{
                                    Child* c = i_children->second;
                                    const double W = 0.31;
                                    ucb = c->getRate() + W*sqrt(log(pN->get_game_sum())/c->getGames());
                                    //cout<<"手がある"<<endl;
                              }
                              if(ucb > max_ucb){
                                     max_ucb = ucb;
                                     select_id = i-1;
                                     select_dxdy = j; 
                                     select_c = c;
                                     //cout<<"ucbを更新"<<endl;
                              }
                        }
                  }
            cnt++;
            }
      }
      //cout<<"actionを作り始める"<<endl;
      Action *action = new Action();
      action->setAgentID(select_id);
      int action_type = select_dxdy / 17;
      if(action_type == 0)action->setActionType(Action::actionType::Move);//stayは今の所ない
      else if(action_type == 1)action->setActionType(Action::actionType::Remove);
      action->setDX(Board::dx_dy[select_dxdy/2].getX());
      action->setDY(Board::dx_dy[select_dxdy/2].getY());
      //cout<<"action作り終わる"<<endl;
      board.set_action(action);
      //cout<<"set_actionを呼ぶ"<<endl;
      int win;
      if(c == nullptr){
            c =new Child();
            c->setAct((select_id*17)+select_dxdy);
            //cout<<"手がないから作る"<<endl;
      }
      if(c->getGames() == 0){
            win = playout(board,30);
            //cout<<"回数0回"<<endl;
      }
      else{
            if(c->getNext() == NULL) c->setNext(pN->create_node(-which_turn,board));
            win = -search_uct(-which_turn,pN,board);
            cout<<"回数1回以上"<<endl;
      }
      c->setRate((c->getRate() * c->getGames() + win)/(c->getGames() + 1));
      c->addGames();
      pN->add_game_sum();
      //cout<<"勝率とか色々更新"<<endl;
      return win;
}

/*int main(){//playout用動作確認用
      cout<<"はじめ\n";
      Board *board = jsonReceive::jsonRead(1,"A-1.json");
      int score = Monte_Carlo::playout(*board,30);
      return 0;
}*/
/*
int main(){
      cout<<"はじめ@Monte_Carlo\n";
      Board *board = jsonReceive::jsonRead(1,"F-1.json");
      cout<<"board作る"<<endl;
      Node *node = Node::create_node(1,*board);
      cout<<"node作る"<<endl;
      int win = Monte_Carlo::search_uct(1,node,*board);
      cout<<"win:"<<win<<endl; 
      return 0;
}
*/

// int main(){
//       Board *board = jsonReceive::jsonRead(1,"A-1.json");
//       auto start = std::chrono::system_clock::now();
//       vector<Action*> action = Monte_Carlo::select_best_uct(*board,1);
//       auto end = std::chrono::system_clock::now();
//       auto sub = end - start;
//       auto msec = std::chrono::duration_cast<std::chrono::milliseconds>(sub).count();
//       cout << msec << endl;
//       return 0;
// }

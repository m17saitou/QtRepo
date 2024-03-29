#include "Monte_Carlo.hpp"
#include "jsonReceive.hpp"
#include <stdlib.h>
#include <cstdlib>
#include <time.h>
#include <vector>
#include <float.h>
#include <chrono>
#include <string>
#include <iostream>
using namespace std;


int Monte_Carlo::all_playouts;    //playoutを行った回


int Monte_Carlo::playout(Board &board, int max_turn,int which_turn)//chikara修正のため差し替え追加
{ //プレイアウトをおこなう関数
      all_playouts++;
      random_device rnd;
      mt19937 mt(rnd());
      //srand(0);//chikara:一時的に試した
      //mt19937 mt(0);
      //Board* pCopy_board = board.copy();//chikaraコメントアウト
      Board* pCopy_board = &board;//chikara修正のため差し替え追加
      Action play_action;
     // cout<<"actionの配列を用意";
      //for (int cnt_turn = 0; cnt_turn < max_turn - board.turn; cnt_turn++)//chikaraコメントアウト
      for (int cnt_turn = 0; cnt_turn < max_turn ; cnt_turn++)//chikara修正のため差し替え追加
      {
            //cout<<cnt_turn+1<<"たーん目"<<endl;
            //for(int i = 0;i < pCopy_board->num_agent;i++)//chikaraコメントアウト
            //{//chikaraコメントアウト
            int i=(pCopy_board->num_agent)-(pCopy_board->get_wait_agent()->size()+1)/2;//chikara修正のため差し替え追加
                  uniform_int_distribution<int> rand_action(0,(17*(pCopy_board->num_agent-i))-1);
                  //cout<<"乱数を用意\n";
                  //for(int j=0;j<2;j++){//chikaraコメントアウト

                        int num = rand_action(mt);
                        int ID;
                        int idx;
                        int j=pCopy_board->get_wait_agent()->size()%2;//chikara追加
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
                  //}//chikaraコメントアウト
            //}//chikaraコメントアウト
      }
      if(which_turn>0) return pCopy_board->count_score();//chikara追加
      else if(which_turn<0) return -(pCopy_board->count_score());//chikara追加
      else throw runtime_error("which_turnが0");//chikara追加
      //return pCopy_board->count_score();//chikaraコメントアウト
}

vector<Action> Monte_Carlo::select_best_uct(Board &board,int which_turn,int remainingTurn)//chikara::返り値のActionの実態はnewで作っているため使用後にdeleteが必要
{ //UCTを繰り返す関数//rootのノードを作成し、ある回数UCTを繰り返し、終わったら一番多く選ばれた手を返す
      Node* node = Node::create_node(which_turn,board);
      int uct_loop = 1000;//chikara:こんなところで定数を代入しない。引数で持ってくること
      Board* board_copy = board.overwrite_copy();
      for(int i = 1;i <= uct_loop;i++){
            //cout <<"uct_loop_i=" << i<< endl;
            //node->printChildren();
            //cout <<"uct_loop_i=" << i<< endl;
            //search_uct(which_turn,node,board);//chikaraコメントアウト
            search_uct(which_turn,node,*board_copy,remainingTurn*Board::num_agent);//chikara修正のため差し替え追加
            //which_turn*=(-1);//chikaraコメントアウト
            board_copy->overwrite(board);
      }
      vector<Action> best_action;
      for(int i = 0;i < Board::num_agent;i++){
            Action action = node->select_best_agent_action(i+1);
            best_action.push_back(action);
            //action = node->select_best_agent_action(-i-1);//chikaraコメントアウト
            //best_action.push_back(action);//chikaraコメントアウト
      }
      return best_action;
}//agent全部探すやつ

int Monte_Carlo::search_uct(int which_turn,Node* pN,Board &board,int remainingTurn)
{ //UCBで手を選びプレイアウトを行う関数
  //全ての手の中でUCB値が一番高い手を選ぶ
  //未展開の手がある場合はそれを優先的に選ぶ
      //srand(time(NULL));//chikaraコメントアウト：こんなところで行わない、もっと上位で行う。
      //cout<<"乱数の種を用意@search_uct"<<endl;
      //pN->printChildren();
      int agentID = 0;
      int select_dxdy = -1;
      double max_ucb = -DBL_MAX;
      int cnt = 0;
      Child* select_c;
      Child* c;
      //cout << "remainingTurn=" << remainingTurn << endl;
      //cout << std::hex << "active_agent=0x" << pN->get_active_agent() << std::dec << endl;
      if(which_turn == 1){//chikara:できるだけif(which_turn>0){にするほうがよい
            //cout<<"which_turnが味方"<<endl;
            for(int i=0;i<board.num_agent;i++){
                  double ucb = 0;
                  //cout << std::hex << "active_agent=0x" << pN->get_active_agent() << std::dec << endl;
                  if(!bit_calculation::low_return_bit(i,pN->get_active_agent()))continue;
                  for(int j=0;j<16;j++){
                        //if(pN->get_agent_bit(cnt) & (1<<j) != 0){//chikaraコメントアウト
                        if((pN->get_agent_bit(cnt) & (1<<j)) != 0){//chikara修正のため差し替え追加:カッコ追加
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
                                     agentID = i+1;//エージェントIDになる
                                     select_dxdy = j;
                                     select_c = c;
                                     //cout<<"ucbを更新"<<endl;
                              }
                        }
                  }
                  cnt++;
            }
      }
      else if(which_turn == -1){//chikara:できるだけif(which_turn<0){にするほうがよい
           // cout<<"which_turnが敵"<<endl;
           //cout <<"min_ucb=" << min_ucb << endl;
            for(int i=0;i<board.num_agent;i++){ 
                  double ucb = 0;
                  //cout <<i<<";;;0x"<< hex << pN->get_active_agent()<<dec<<endl;
                  if(!bit_calculation::low_return_bit(i+8,pN->get_active_agent()))continue;//chikara:+8を追加
                  for(int j=0;j<16;j++){
                    //cout <<i<<";;"<<j<< endl;
                        //if(pN->get_agent_bit(cnt) & (1<<j) != 0){//chikaraコメントアウト
                        if((pN->get_agent_bit(cnt) & (1<<j)) != 0){//chikara修正のため差し替え追加:カッコ追加
                              //map<int16_t,Child*>::iterator i_children = pN->children.find((i-1)*17-j);//chikaraコメントアウト
                              map<int16_t,Child*>::iterator i_children = pN->children.find((-i-1)*17-j);//chikara修正のため差し替え追加:-追加
                              if(i_children == pN->children.end()){
                                    //ucb = 10000 + rand();//chikaraコメントアウト
                                    ucb = 10000 + (rand() & 0x7fff); //chikara修正のため差し替え追加
                                    c = nullptr;
                                    //cout<<"手がない"<<endl;
                              }
                              else{
                                    //Child* c = i_children->second;//chikaraコメントアウト
                                    c = i_children->second;//chikara修正のため差し替え追加
                                    //cout <<i<<";"<<j<<";" <<hex<<":c=" <<c << dec<< endl;
                                    const double W = 0.31;
                                    ucb = c->getRate() + W*sqrt(log(pN->get_game_sum())/c->getGames());
                                    //cout<<"手がある"<<endl;
                              }
                              //cout <<i<<";"<<j<<":ucb=" << ucb <<hex<<":c=" <<c << dec<< endl;
                              if(ucb > max_ucb){//chikaraコメントアウト
                              //if(ucb<min_ucb){//chikara修正のため差し替え追加
                                     max_ucb = ucb;//chikaraコメントアウト
                                     //min_ucb=ucb;//chikara修正のため差し替え追加
                                     //agentID = i-1;//chikaraコメントアウト
                                     agentID=-i-1;//chikara修正のため差し替え追加
                                     select_dxdy = j;
                                     select_c = c;
                                     //cout<<"ucbを更新"<<endl;
                              }
                        }
                  }
                  cnt++;
            }
      }
      if(select_dxdy ==-1) throw runtime_error("bestが選ばれない");
      //cout<<"actionを作り始める"<<endl;
      Action action = Action();//chikara:引数がフィールドの数だけあるコンストラクタを使ったほうが良いが保留
      //cout << "agentID=" << agentID << endl;
      //cout << "select_dxdy=" << select_dxdy << endl;
      //cout << "select_c=" << hex <<select_c << dec<< endl;
      //cout << "key="<< (agentID>0 ? (agentID*17)+select_dxdy:(agentID*17)-select_dxdy) << endl;
      action.setAgentID(agentID);
      //int action_type = select_dxdy / 17;//chikaraコメントアcウト
      int action_type = select_dxdy / 8;//chikara修正のため差し替え追加
      if(action_type == 0)action.setActionType(Action::actionType::Move);//stayは今の所ない
      else if(action_type == 1)action.setActionType(Action::actionType::Remove);//chikara::stayの場合も書くべき、そのうちバグの原因になる
      //action->setDX(Board::dx_dy[select_dxdy/2].getX());//chikaraコメントアウト
      //action->setDY(Board::dx_dy[select_dxdy/2].getY());//chikaraコメントアウト
      action.setDX(Board::dx_dy[select_dxdy%8].getX());//chikara修正のため差し替え追加
      action.setDY(Board::dx_dy[select_dxdy%8].getY());//chikara修正のため差し替え追加
      //cout<<"action作り終わる"<<endl;
      board.set_action(action);
      //cout<<"set_actionを呼ぶ"<<endl;
      int win;
      if(select_c == nullptr){
            select_c =new Child();//chikara:select_cに変更
            int16_t key;//chikara追加
            if(agentID>0) key=(agentID*17)+select_dxdy;//chikara追加
            else if(agentID<0)key=(agentID*17)-select_dxdy;//chikara追加
            else throw runtime_error("agentIDが0");//chikara追加
            //c->setAct((agentID*17)+select_dxdy);//chikaraコメントアウト
            select_c->setAct(key);//chikara修正のため差し替え追加//chikara:select_cに変更
            pN->children.emplace(key,select_c);//chikara追加//chikara:select_cに変更
            //cout<<"手がないから作る"<<endl;
      }
      //pN->printChildren();
      if(select_c->getGames() == 0){//chikara:select_cに変更
            //win = playout(board,30);//chikaraコメントアウト
            win = -playout(board,remainingTurn-1,-which_turn);//chikara修正のため差し替え追加
            //cout<<"回数0回"<<endl;
      }
      else{
            //if(c->getNext() == NULL) c->setNext(pN->create_node(-which_turn,board));//chikaraコメントアウト
            if(select_c->getNext() == nullptr){//chikara追加ここから//chikara:select_cに変更
              Node* nxt=pN->create_node(-which_turn,board);
              //cout << std::hex << "create_node_nxt=0x" << nxt << std::dec << endl;
              //cout << "key="<< (agentID>0 ? (agentID*17)+select_dxdy:(agentID*17)-select_dxdy) << endl;
              //cout << "after create_node:remainingTurn=" << remainingTurn << endl;
              //cout << std::hex << "active_agent=0x" << nxt->get_active_agent() << std::dec << endl;
              select_c->setNext(nxt);//chikara:select_cに変更
              //cout << std::hex << "getNext()=0x" << select_c->getNext() << std::dec << endl;
            }//chikara追加ここまで
            //win = -search_uct(-which_turn,pN,board);//chikaraコメントアウト
            //cout << "0remainingTurn=" << remainingTurn << endl;
            //cout << std::hex << "getNext()=" << select_c->getNext() << std::dec << endl;
            //cout << std::hex << "active_agent=0x" << select_c->getNext()->get_active_agent() << std::dec << endl;
            win = -search_uct(-which_turn,select_c->getNext(),board,remainingTurn-1);//chikara修正のため差し替え追加//chikara:select_cに変更
            //cout<<"回数1回以上"<<endl;
      }
      select_c->setRate((select_c->getRate() * select_c->getGames() + win)/(select_c->getGames() + 1));//chikara:select_cに変更
      select_c->addGames();//chikara:select_cに変更
      pN->add_game_sum();
      //cout<<"勝率とか色々更新"<<endl;
      return win;
}//agentぜんぶ

vector<Action> Monte_Carlo::select_best_uct_select(Board &board,int which_turn,int remainingTurn,int uct_loop,uint16_t agent_bit)//chikara::返り値のActionの実態はnewで作っているため使用後にdeleteが必要
{ //UCTを繰り返す関数//rootのノードを作成し、ある回数UCTを繰り返し、終わったら一番多く選ばれた手を返す
      //board.display();
      //std::cout << which_turn <<endl;
      //std::cout << remainingTurn << endl;
      //std::cout << uct_loop << endl;
      //std::cout << agent_bit <<endl;
      //std::cout << "vettor\n";
      //vector<int>* tmpvect = board.get_wait_agent();
      //for(int cnt=0;cnt < tmpvect->size();cnt++){
      //      std::cout << tmpvect->at(cnt) <<endl;
      //}
      Node* node = Node::create_node(which_turn,board);
      Board* board_copy = board.overwrite_copy();
      //cout<<std::hex<<node->get_agent_bit(0)<<std::dec<<endl;
      //cout<<std::hex<<node->get_agent_bit(1)<<std::dec<<endl;
      //cout<<std::hex<<node->get_agent_bit(2)<<std::dec<<endl;
      for(int i = 1;i <= uct_loop;i++){
            //cout <<"uct_loop_i=" << i<< endl;
            //node->printChildren();
            //cout <<"uct_loop_i=" << i<< endl;
            //search_uct(which_turn,node,board);//chikaraコメントアウト
            search_uct_select(which_turn,node,*board_copy,remainingTurn*Board::num_agent,agent_bit);
            MainWindow::lock.lockForRead();
            bool localStop= MainWindow::stop;
            MainWindow::lock.unlock();
            if(localStop) break;
            board_copy->overwrite(board);
            //if(i %1000 == 0)cout<<"現在"<<i<<"回探索しました"<<endl;
      }
      vector<Action> best_action;
      int cnt = 0;
      for(int i = 0;i < Board::num_agent;i++){
            if(!((1<<i) & agent_bit))continue;
            cout << i+1 <<endl;
            Action action = node->select_best_agent_action(i+1);
            best_action.push_back(action);
            cnt++;
            //action = node->select_best_agent_action(-i-1);//chikaraコメントアウト
            //best_action.push_back(action);//chikaraコメントアウト
      }
      return best_action;
}//agent指定

int Monte_Carlo::search_uct_select(int which_turn,Node* pN,Board &board,int remainingTurn,uint16_t agent_bit)
{ //UCBで手を選びプレイアウトを行う関数
  //全ての手の中でUCB値が一番高い手を選ぶ
  //未展開の手がある場合はそれを優先的に選ぶ
      //srand(time(NULL));//chikaraコメントアウト：こんなところで行わない、もっと上位で行う。
      //cout<<"乱数の種を用意@search_uct"<<endl;
      //pN->printChildren();
      //cout<<std::hex<<pN->get_agent_bit(1)<<std::dec<<"first"<<endl;
      int agentID = 0;
      int select_dxdy = -1;
      double max_ucb = -DBL_MAX;
      int cnt = 0;
      Child* select_c;
      Child* c;
      //cout << "remainingTurn=" << remainingTurn << endl;
      //cout << std::hex << "active_agent=0x" << pN->get_active_agent() << std::dec << endl;
      if(which_turn > 0){//chikara:できるだけif(which_turn>0){にするほうがよい
            //cout<<"which_turnが味方"<<endl;
            for(int i=0;i<board.num_agent;i++){
                  double ucb = 0;
                  //cout << std::hex << "active_agent=0x" << pN->get_active_agent() << std::dec << endl;
                  if(!((1<<i) & agent_bit))continue;
                  if(!bit_calculation::low_return_bit(i,pN->get_active_agent()))continue;
                  for(int j=0;j<16;j++){
                        //cout << std::hex<<pN->get_agent_bit(i)<<std::dec<<endl;
                        if((pN->get_agent_bit(i) & (1<<j)) != 0){//chikara修正のため差し替え追加:カッコ追加
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
                                     agentID = i+1;//エージェントIDになる
                                     select_dxdy = j;
                                     select_c = c;
                                     //cout<<"ucbを更新"<<endl;
                              }
                        }  
                  }
            }
      }
      else if(which_turn < 0){//chikara:できるだけif(which_turn<0){にするほうがよい
           // cout<<"which_turnが敵"<<endl;
           //cout <<"min_ucb=" << min_ucb << endl;
            for(int i=0;i<board.num_agent;i++){
                  double ucb = 0;
                  //cout <<i<<";;;0x"<< hex << pN->get_active_agent()<<dec<<endl;
                  if(!((1<<(i+8)) & agent_bit))continue;
                  if(!bit_calculation::low_return_bit(i+8,pN->get_active_agent()))continue;//chikara:+8を追加
                  for(int j=0;j<16;j++){
                    //cout <<i<<";;"<<j<< endl;
                        //if(pN->get_agent_bit(cnt) & (1<<j) != 0){//chikaraコメントアウト
                        if((pN->get_agent_bit(cnt) & (1<<j)) != 0){//chikara修正のため差し替え追加:カッコ追加
                              //map<int16_t,Child*>::iterator i_children = pN->children.find((i-1)*17-j);//chikaraコメントアウト
                              map<int16_t,Child*>::iterator i_children = pN->children.find((-i-1)*17-j);//chikara修正のため差し替え追加:-追加
                              if(i_children == pN->children.end()){
                                    //ucb = 10000 + rand();//chikaraコメントアウト
                                    ucb = 10000 + (rand() & 0x7fff); //chikara修正のため差し替え追加
                                    c = nullptr;
                                    //cout<<"手がない"<<endl;
                              }
                              else{
                                    //Child* c = i_children->second;//chikaraコメントアウト
                                    c = i_children->second;//chikara修正のため差し替え追加
                                    //cout <<i<<";"<<j<<";" <<hex<<":c=" <<c << dec<< endl;
                                    const double W = 0.31;
                                    ucb = c->getRate() + W*sqrt(log(pN->get_game_sum())/c->getGames());
                                    //cout<<"手がある"<<endl;
                              }
                              //cout <<i<<";"<<j<<":ucb=" << ucb <<hex<<":c=" <<c << dec<< endl;
                              if(ucb > max_ucb){//chikaraコメントアウト
                              //if(ucb<min_ucb){//chikara修正のため差し替え追加
                                     max_ucb = ucb;//chikaraコメントアウト
                                     //min_ucb=ucb;//chikara修正のため差し替え追加
                                     //agentID = i-1;//chikaraコメントアウト
                                     agentID=-i-1;//chikara修正のため差し替え追加
                                     select_dxdy = j;
                                     select_c = c;
                                     //cout<<"ucbを更新"<<endl;
                              }
                        }
                  }
                  cnt++;
            }
      }
      if(select_dxdy ==-1) throw runtime_error("bestが選ばれない");
      //cout<<"actionを作り始める"<<endl;
      Action action = Action();//chikara:引数がフィールドの数だけあるコンストラクタを使ったほうが良いが保留
      //cout << "agentID=" << agentID << endl;
      //cout << "select_dxdy=" << select_dxdy << endl;
      //cout << "select_c=" << hex <<select_c << dec<< endl;
      //cout << "key="<< (agentID>0 ? (agentID*17)+select_dxdy:(agentID*17)-select_dxdy) << endl;
      action.setAgentID(agentID);
      //int action_type = select_dxdy / 17;//chikaraコメントアcウト
      int action_type = select_dxdy / 8;//chikara修正のため差し替え追加
      if(action_type == 0)action.setActionType(Action::actionType::Move);//stayは今の所ない
      else if(action_type == 1)action.setActionType(Action::actionType::Remove);//chikara::stayの場合も書くべき、そのうちバグの原因になる
      //action->setDX(Board::dx_dy[select_dxdy/2].getX());//chikaraコメントアウト
      //action->setDY(Board::dx_dy[select_dxdy/2].getY());//chikaraコメントアウト
      action.setDX(Board::dx_dy[select_dxdy%8].getX());//chikara修正のため差し替え追加
      action.setDY(Board::dx_dy[select_dxdy%8].getY());//chikara修正のため差し替え追加
      //cout<<"action作り終わる"<<endl;
      board.set_action(action);
      //cout<<"set_actionを呼ぶ"<<endl;
      int win;
      if(select_c == nullptr){
            select_c =new Child();//chikara:select_cに変更
            int16_t key;//chikara追加
            if(agentID>0) key=(agentID*17)+select_dxdy;//chikara追加
            else if(agentID<0)key=(agentID*17)-select_dxdy;//chikara追加
            else throw runtime_error("agentIDが0");//chikara追加
            //c->setAct((agentID*17)+select_dxdy);//chikaraコメントアウト
            select_c->setAct(key);//chikara修正のため差し替え追加//chikara:select_cに変更
            pN->children.emplace(key,select_c);//chikara追加//chikara:select_cに変更
            //cout<<"手がないから作る"<<endl;
      }
      //pN->printChildren();
      if(select_c->getGames() == 0){//chikara:select_cに変更
            //win = playout(board,30);//chikaraコメントアウト
            win = -playout(board,remainingTurn-1,-which_turn);//chikara修正のため差し替え追加
            //cout<<"回数0回"<<endl;
      }
      else{
            //if(c->getNext() == NULL) c->setNext(pN->create_node(-which_turn,board));//chikaraコメントアウト
            if(select_c->getNext() == nullptr){//chikara追加ここから//chikara:select_cに変更
              Node* nxt=pN->create_node(-which_turn,board);
              //cout << std::hex << "create_node_nxt=0x" << nxt << std::dec << endl;
              //cout << "key="<< (agentID>0 ? (agentID*17)+select_dxdy:(agentID*17)-select_dxdy) << endl;
              //cout << "after create_node:remainingTurn=" << remainingTurn << endl;
              //cout << std::hex << "active_agent=0x" << nxt->get_active_agent() << std::dec << endl;
              select_c->setNext(nxt);//chikara:select_cに変更
              //cout << std::hex << "getNext()=0x" << select_c->getNext() << std::dec << endl;
            }//chikara追加ここまで
            //win = -search_uct(-which_turn,pN,board);//chikaraコメントアウト
            //cout << "0remainingTurn=" << remainingTurn << endl;
            //cout << std::hex << "getNext()=" << select_c->getNext() << std::dec << endl;
            //cout << std::hex << "active_agent=0x" << select_c->getNext()->get_active_agent() << std::dec << endl;
            win = -search_uct(-which_turn,select_c->getNext(),board,remainingTurn-1);//chikara修正のため差し替え追加//chikara:select_cに変更
            //cout<<"回数1回以上"<<endl;
      }
      select_c->setRate((select_c->getRate() * select_c->getGames() + win)/(select_c->getGames() + 1));//chikara:select_cに変更
      select_c->addGames();//chikara:select_cに変更
      pN->add_game_sum();
      //cout<<"勝率とか色々更新"<<endl;
      //cout<<std::hex<<pN->get_agent_bit(1)<<std::dec<<"last"<<endl;
      return win;
}//agent指定

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
// */

// int main(int argc,char *argv[]){//コマンドライン引数で json 行動するagentのbit ターン数 uctの回数 の順番で探索を行う　
//       std::string field = "A-1.json";
//       field = argv[1];
//       int bit = 0xFF;
//       bit = atoi(argv[2]);
//       int uct_loop = 10000;
//       uct_loop = atoi(argv[3]);
//       int turn = 30;
//       turn = atoi(argv[4]);
//       cout << "field:" << field <<endl;
//       Board *board = jsonReceive::jsonRead(1,field);
//       board->display();
//       auto start = std::chrono::system_clock::now();

//       vector<Action> action = Monte_Carlo::select_best_uct_select(*board,1,turn,uct_loop,bit);//chikara修正のため差し替え追加 
//       for(int i = 0; i< action.size();i++)cout << action[i].to_string() << endl;//chikara追加
//       //board->display();
//       auto end = std::chrono::system_clock::now();
//       auto sub = end - start;
//       auto msec = std::chrono::duration_cast<std::chrono::milliseconds>(sub).count();
//       cout << "かかった時間:"<< msec << "[ms]" << endl;
//       return 0;
// }
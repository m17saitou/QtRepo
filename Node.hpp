#pragma once
#include <map>
#include "Board.hpp"
#include "Child.hpp"
#include "bit_calculation.hpp"
#include "jsonReceive.hpp"

using namespace std;

class Node{
    private:
    int child_num;//mapに最大で登録してある子の数
    int game_sum;//子の選んだ回数
    uint16_t* agent_bit;//エージェントが行動できるかどうかをbitの配列で保持
    uint16_t  active_agent;//boardクラスのwait_agentをbitで保持 敵agentは必ず8bit目から始める

    public:
    map<int16_t,Child*> children;//子を登録,ID*17+方向(味方),ID*17-方向(敵)

    inline Node(){
        child_num = 0;
        game_sum = 0;
        active_agent = 0;
        agent_bit = new uint16_t[Board::num_agent];
    }
    inline Node(int c_num,int g_sum){
        child_num = c_num;
        game_sum = g_sum;
        agent_bit = 0;
    }

    inline Node(int c_num){
        child_num = c_num;
        game_sum = 0;
        agent_bit = 0;
    }

    inline Node(uint16_t bit,int cnt){
        child_num = 0;
        game_sum = 0;
        active_agent = bit;
        agent_bit = new uint16_t[cnt];
    }

    inline ~Node(){
    }

    inline void set_game_sum(int x){
        game_sum = x;
    }

    inline int get_game_sum(){
        return game_sum;
    }

    inline void add_game_sum(){
        game_sum++;
    }

    inline void set_child_num(int x){
        child_num = x;
    }

    inline int get_child_num(){
        return child_num;
    }

    inline void set_children(int16_t key,Child* value){
        children[key] = value;
    }

    inline Child* get_children(int16_t key){
        if(children.find(key) != children.end()){
            return children[key];
        }
        else {
            return nullptr;
        }
    }

    inline void set_agent_bit(uint16_t bit,int idx){
        agent_bit[idx] = bit;
    }

    inline uint16_t get_agent_bit(int idx){
        return agent_bit[idx];
    }

    inline void set_active_agent(uint16_t bit){
        active_agent = bit;
    }

    inline uint16_t get_active_agent(){
        return active_agent;
    }

    static Node* create_node(int which_turn,Board &board);
    Action* select_best_agent_action(int id);

    void print_agent_bit(){
        int cnt=0;
        for(int i=0;i<Board::num_agent;i++){
            if(!bit_calculation::low_return_bit(i,active_agent))continue;
            cout<<hex<<agent_bit[cnt]<<endl;
            cnt++;
        } 
    }
};
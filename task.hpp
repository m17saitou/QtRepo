#pragma once
#include "Agent-Action.hpp"

class Board;

class Task{
public:
    inline Task(){
        taskBoard =nullptr;
        gTurn = 0;
        uctLoop = 0;
        taskAID = 0;
    };
    inline Task(Board* taskB, char turn,int loop,uint16_t agentID){
        taskBoard = taskB;
        gTurn = turn;
        uctLoop = loop;
        taskAID = agentID;
    };
    inline Board* getTaskBoard() const{return taskBoard;};
    inline char getGameTurn() const{return gTurn;};
    inline int getUctLoop() const{return uctLoop;};
    inline uint16_t getSearchAgentID() const{return taskAID;};
private:
    Board *taskBoard;
    char gTurn;
    int uctLoop;
    uint16_t taskAID;
};
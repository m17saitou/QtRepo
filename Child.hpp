#pragma once
#ifndef __Node__
#define __Node__
#include <string>
#include <stdio.h>
#include <sstream>
//#include <Node.hpp>
using namespace std;

class Node;

class Child
{
    int16_t act;//手の番号　敵はマイナス
    int games;//手を選んだ回数
    double rate;//手を選んだ勝率
    Node* next;//次のノード
public:
    inline Child()
    {
        act=0;
        games=0;
        rate=0;
        next=nullptr;
    }
    inline Child(int a,int g,double r,Node* n)
    {
        act=a;
        games=g;
        rate=r;
        next=n;
    }
    inline Child(int a,int g,double r)
    {
        act=a;
        games=g;
        rate=r;
        next=nullptr;
    }
    inline void setAct(int action)
    {
        act=action;
    }
    inline int getAct()
    {
        return act;
    }
    inline void setGames(int many)
    {
        games=many;
    }
    inline int getGames()
    {
        return games;
    }
    inline void addGames(){
        games++;
    }
    inline void setRate(double Winning)
    {
        rate=Winning;
    }
    inline double getRate()
    {
        return rate;
    }
    inline void setNext(Node* node)
    {
        next=node;
    }
    inline Node* getNext()
    {
        return next;
    }
    inline string to_string()
    {
        std::ostringstream str;
        str << act << games << rate << next;
        return str.str();
    }
};

#endif
#pragma once
using namespace std;

class Point{
    private:
        int x;
        int y;
    public:
        Point(){x = -1; y = -1;};
        Point(int ax,int ay){x = ax;y = ay;};
        ~Point(){};
        void show(){cout<<"x:"<<x<<" y:"<<y<<'\n';}
        void setX(int a){x=a;}
        void setY(int b){y=b;}
        void setXY(int a,int b){x=a;y=b;}
        void addDX(int a){x+=a;}
        void addDY(int b){y+=b;}
        void addDXDY(int a,int b){x+=a;y+=b;}
        int getX(){return x;}
        int getY(){return y;}
};
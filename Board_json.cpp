#include <iostream>
#include <cstdlib>
#include "Agent-Action.hpp"
#include "Board.hpp"
//using namespace

#include <json/json.h>

string Board::createBoardJson(int w,int h,int t,int **point,int **tile,Action *act,int num_act)
{
    std::string jsonStr;
    Json::Value place,placese,agent,team,roop,root,field,width,height,points,startedAtUnixTime,turn,tiled,teams,teamID,agents,x,y,tilePoint,areaPoint,actions;
    Json::StreamWriterBuilder writerBuilder;
    std::ostringstream os;

    for(int i=0;i<h;i++){
        for(int j=0;j<w;j++){
            root[i][j]=point[i][j];
            roop[i][j]=tile[i][j];
        }
    }
    for(int i=0;i<2;i++){
        agent["teamID"]=i;
        if(i==0){
            for(int i=0;i<num_act;i++){
                place["agentID"]=Board::getAIdL_to_G(act[i].getAgentID()); 
                place["x"]=act[i].getDX();
                place["y"]=act[i].getDY();

                placese[i]=place;
            }
            agent["agents"]=placese;
        }
        else{
            for(int i=num_act;i<num_act*2;i++){
                place["agentID"]=Board::getAIdL_to_G(act[i].getAgentID()); 
                place["x"]=act[i].getDX();
                place["y"]=act[i].getDY();

                placese[i]=place;
            }
            agent["agents"]=placese;
        }
        //if(i==0){
        //    agent["tilePoint"]=Board::tile_point(1);
        //    agent["areaPoint"]=Board::area_point(1);
        //}
        //else{
        //    agent["tilePoint"]=Board::tile_point(-1);
        //    agent["areaPoint"]=Board::area_point(-1);
        //}

        team[i]=agent;
    }

    field["width"]=w;
    field["height"]=h;
    field["tiled"]=root;
    field["startedAtUnixTime"]=0;
    field["turn"]=t;
    field["tiled"]=roop;
    field["teams"]=team;
    field["actions"]=" ";

    std::unique_ptr<Json::StreamWriter>jsonWriter(writerBuilder.newStreamWriter());
    jsonWriter->write(root,&os);
    jsonStr=os.str();

    std::cout << "Json:\n" <<jsonStr <<std::endl;
    return jsonStr;
}
int main ()
{
//   createJson();
}
//g++ Action.cpp Board.cpp -ljsoncpp
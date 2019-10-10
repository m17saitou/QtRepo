#include <iostream>
#include <cstdlib>
#include <memory>
#include <vector>
#include "Agent-Action.hpp"
#include "Board.hpp"
//using namespace

#include <json/json.h>

std::string Action::createJson(std::vector <Action> act)
{
    std::string jsonStr;
    Json::Value root,agents, agent, agentID, type, dx, dy;
    Json::StreamWriterBuilder writerBuilder;
    std::ostringstream os;

    for(int i=0;i<act.size();i++){
        agent["agentID"]=Board::getAIdL_to_G(act[i].getAgentID());
        if(act[i].getActionType()==actionType::Move)agent["type"]="move";
        else if(act[i].getActionType()==actionType::Remove)agent["type"]="remove";
        else if(act[i].getActionType()==actionType::Stay)agent["type"]="stay";
        agent["dx"]=act[i].getDX();
        agent["dy"]=act[i].getDY();

        agents[i]=agent;
    }
    
    root["actions"]=agents;

    std::unique_ptr<Json::StreamWriter>jsonWriter(writerBuilder.newStreamWriter());
    jsonWriter->write(root,&os);
    jsonStr=os.str();

    std::cout << "Json:\n" <<jsonStr <<std::endl;
    return jsonStr;
}
// int main ()
// {
// //   createJson();
// }
// //g++ Action.cpp Board.cpp -ljsoncpp
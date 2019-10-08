#include <iostream>
#include <cstdlib>
#include <memory>
#include "Agent-Action.hpp"
#include "Board.hpp"
//using namespace

#include <json/json.h>

std::string Action::createJson(Action *act,int num_act)
{
    std::string jsonStr;
    Json::Value root,agents, agent, agentID, type, dx, dy;
    Json::StreamWriterBuilder writerBuilder;
    std::ostringstream os;

    for(int i=0;i<num_act;i++){
        agent["agentID"]=Board::getAIdL_to_G(act[i].getAgentID());
        agent["type"]=act[i].getActionType();
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
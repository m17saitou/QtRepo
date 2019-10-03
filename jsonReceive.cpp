#include "jsonReceive.hpp"
#include "Board.hpp"

//コンパイル時は $g++ jsonReceive.cpp -ljsoncpp でコンパイル
Board* jsonReceive::jsonRead(int ourTeamID,std::string jsonName){
    //json読み込み準備部分
    // std::cout << "読み込み開始\n";
    std::ifstream infile(jsonName);
    if(!infile) throw std::runtime_error("Jsonファイルが開けません");
    Json::Value root;
    Json::CharReaderBuilder rbuilder;
    std::string errs;
    // std::cout << "json読み込み\n";
    bool parsingSuccessful = Json::parseFromStream(rbuilder, infile, &root, &errs);
    if(!parsingSuccessful) throw std::runtime_error("Jsonふぁいるが解析できません");
    //json読み込み準備完了
    
    // std::cout << "rootのなかに格納はできてる\n";
    int width = root["width"].asInt();
    int height = root["height"].asInt();
    Board::setting(width, height);//Boardクラスのセッターに幅と高さを渡す
    // std::cout << "Boardに高さと幅は渡せた\n";
    char f_points[400];//各タイルの得点を入れる配列 入るのは数値 Board.field_pointsに入れる
    int count = 0;
    if(root["points"].size() == height){
        for ( int j = 0; j < root["points"].size(); j++ ){
            if(root["points"][j].size() == width){
                for ( int i = 0; i < root["points"][j].size(); i++ ){
                    f_points[count] = root["points"][j][i].asInt();
                    count++;
                }
            }
            else throw std::runtime_error("Jsonで取得した幅，高さとJson内のタイルポイントデータがあっていません");
        }
    }
    else throw std::runtime_error("Jsonで取得した幅，高さとJson内のタイルポイントデータがあっていません");//幅か高さが合っていなかったときにおかしいことを通知
    // std::cout << "Points\n";
    Board::set_field_Point(f_points);
    // std::cout << "field Pointsは入れれた\n";
    int turn = root["turn"].asInt();
    char tiledArray[400];//タイルの置いてあるところを示す配列．Boardのセッターで二次元配列に直す．
    count = 0;
    if(root["tiled"].size() == height){
        for (int j = 0; j < root["tiled"].size(); j++){ //tiledをjsonから取得
            if(root["tiled"][j].size() == width){
                for (int i = 0; i < root["tiled"][j].size(); i++){
                    if (root["tiled"][j][i].asInt() == ourTeamID)
                        tiledArray[count] = 1; //自チームのとき，1
                    else if (root["tiled"][j][i].asInt() == 0)
                        tiledArray[count] = 0; //0の時，Tiledに0を入れる
                    else
                        tiledArray[count] = -1;//自分のTeamIDでも0でもないものを敵タイルとして扱う
                    count++;
                }
            }
        }
    }

    Json::Value temporary;//一時的に1チーム分の内容を格納する．
    Point friends[8];
    Point enemy[8];
    int countAgents;//エージェント数を数える部分．
    int ourTileP, enemyTileP;//Jsonから現在の自チーム,敵チームタイルポイントを受け取って(とりあえず)入れておく
    int ourAreaP, enemyAreaP;//自チーム，敵チームエリアポイントを入れておく
    for(int team=0;team<2;team++){
        temporary = root["teams"][team]["agents"];
        if(root["teams"][team]["teamID"].asInt() == ourTeamID){
            ourTileP = root["teams"][team]["tilePoint"].asInt();
            ourAreaP = root["teams"][team]["areaPoint"].asInt();
        }
        else{
            enemyTileP = root["teams"][team]["tilePoint"].asInt();
            enemyAreaP = root["teams"][team]["areaPoint"].asInt();
        }
        countAgents = temporary.size();
        for(int i=0; i<temporary.size();i++){
            int agentid = temporary[i]["agentID"].asInt();
            int agentx = temporary[i]["x"].asInt()-1;
            int agenty = temporary[i]["y"].asInt()-1;
            if(root["teams"][team]["teamID"].asInt() == ourTeamID){//ここで味方か相手かを判断して格納する先を変更
                friends[i].setX(agentx);
                friends[i].setY(agenty);
                if(Board::addAgentMap(agentid,i+1) != 0) throw std::runtime_error("エージェントIDがおかしいです");//正常に入れられなくなったらループ終了
            }
            else{
                enemy[i].setX(agentx);
                enemy[i].setY(agenty);
                if(Board::addAgentMap(agentid,-i-1) != 0) throw std::runtime_error("エージェントIDがおかしいです2");//正常に入れられなくなったらループ終了 相手なのでAgentIDは負
            }
        }
    }
    Board::set_num_agent(countAgents);
    // std::cout << "newBoard直前\n";
    return new Board(turn,tiledArray,friends,enemy);
}
/*
int main (){
    std::string filename = "A-1.json";
    Board *jsonSet = jsonReceive::jsonRead(1,filename);
    //std::cout << "インスタンスを生成してReturnまで成功\n";
    jsonSet->display();
    Action actionarray[6];
    for(int i=1; i <= 3; i++){
        actionarray[i-1].setAgentID(i);
        actionarray[i-1].setActionType(Action::actionType::Move);
        actionarray[i-1].setDX(-1);
        actionarray[i-1].setDY(-1);
        actionarray[i+2].setAgentID(-i);
        actionarray[i+2].setActionType(Action::actionType::Move);
        actionarray[i+2].setDX(1);
        actionarray[i+2].setDY(1);
    }
    int errcheck = jsonSet->action(actionarray , 6);
    printf("%x\n",errcheck);
}
*/

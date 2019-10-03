#include "Board.hpp"
#include "bit_calculation.hpp"
#include "jsonReceive.hpp"
#include <map>
#include <bitset>
using namespace std;

//Board::friend_place=nullptr;
int Board::width = -1;
int Board::height = -1;
int **Board::field_points = nullptr;
int **tiled = nullptr;
int Board::num_agent = -1;
map<int, int> Board::agentMapG_to_L;
map<int, int> Board::agentMapL_to_G;
int8_t Board::direction [][2] = {{1,0},{1,1},{0,1},{-1,1},{-1,0},{-1,-1},{0,-1},{1,-1}};

Point Board::dx_dy[] = {{-1,-1},{0,-1},{1,-1},{-1,0},{1,0},{-1,1},{0,1},{1,1}};

Board::Board()
{
    int **tiled = new int *[width];
    for (int i = 0; i < width; i++)
    {
        tiled[i] = new int[height];
    }
    friend_place = new Point[num_agent];
    enemy_place = new Point[num_agent];
    action_pending = new Action[2*num_agent];
    wait_agent.reserve(2*num_agent);
    turn = 0;
    for (int i = 0; i < num_agent; i++)
    {
        friend_place[i].setX(-1);
        friend_place[i].setY(-1);
    }
    for (int i = 0; i < num_agent; i++)
    {
        enemy_place[i].setX(-1);
        enemy_place[i].setY(-1);
    }
    for (int i = 1; i < width + 1; i++)
    {
        for (int j = 1; j < height + 1; j++)
        {
            set_one_tile(empty, i, j);
        }
    }
    for(int i = 1; i <= num_agent;i++)wait_agent.push_back(i);
    for(int i = 1; i <= num_agent;i++)wait_agent.push_back(-i);
}
Board::Board(int turn0, int **tiled0, Point friend_place0[], Point enemy_place0[])
{
    tiled = new int *[height];
    for (int i = 0; i < height; i++)
    {
        tiled[i] = new int[width];
    }
    friend_place = new Point[num_agent];
    enemy_place = new Point[num_agent];
    action_pending = new Action[2*num_agent];
    wait_agent.reserve(2*num_agent);
    turn = turn0;
    for (int a = 0; a < num_agent; a++)
    {
        friend_place[a] = friend_place0[a];
    }
    for (int a = 0; a < num_agent; a++)
    {
        enemy_place[a] = enemy_place0[a];
    }
    for (int i = 0; i < width; i++)
    {
        for (int j = 0; j < height; j++)//j<=heightでセグフォ
        {
            set_one_tile(tiled0[j][i], i, j);//tiled0[i][j]で逆
        }
    }
    for(int i = 1; i <= num_agent;i++)wait_agent.push_back(i);
    for(int i = 1; i <= num_agent;i++)wait_agent.push_back(-i);
}
Board::Board(int turn0, char tiled0[], Point friend_place0[], Point enemy_place0[])
{
    tiled = new int *[height];
    for (int i = 0; i < height; i++)
    {
        tiled[i] = new int[width];
    }
    friend_place = new Point[num_agent];
    enemy_place = new Point[num_agent];
    action_pending = new Action[2*num_agent];
    turn = turn0;
    for (int a = 0; a < num_agent; a++)
    {
        friend_place[a] = friend_place0[a];
    }
    for (int a = 0; a < num_agent; a++)
    {
        enemy_place[a] = enemy_place0[a];
    }
    for (int j = 0; j < height; j++)
    {
        for (int i = 0; i <width; i++)
        {
            tiled[j][i] = tiled0[j * width + i];
        }
    }
    for(int i = 1; i <= num_agent;i++)wait_agent.push_back(i);
    for(int i = 1; i <= num_agent;i++)wait_agent.push_back(-i);
}
Board::Board(int turn0, int **tiled0, Point friend_place0[], Point enemy_place0[], Action action_pending0[], vector<int> wait_agent0){
    turn = turn0;
    tiled = new int *[height];
    for (int i = 0; i < height; i++)
    {
        tiled[i] = new int[width];
    }
    friend_place = new Point[num_agent];
    enemy_place = new Point[num_agent];
    action_pending = new Action[2*num_agent];
    wait_agent.reserve(2*num_agent);
    std::memcpy(friend_place,friend_place0,sizeof(Point)*num_agent);
    std::memcpy(enemy_place,enemy_place0,sizeof(Point)*num_agent);
    std::memcpy(action_pending,action_pending0,sizeof(Action)*num_agent*2);
    for(int j = 0; j < height; j++){
        std::memcpy(tiled[j],tiled0[j],sizeof(int)*width);
    }
    std::copy(wait_agent0.begin(),wait_agent0.end(),std::back_inserter(wait_agent));
}
Board *Board::copy()
{ //Boardのコピー
    return new Board(turn, tiled, friend_place, enemy_place);
}

Board *Board::overwrite_copy(){
    return new Board(turn, tiled, friend_place, enemy_place, action_pending,wait_agent);
}

void Board::overwrite(Board &board){
    std::memcpy(this->friend_place, board.friend_place, sizeof(Point)*board.num_agent);
    std::memcpy(this->enemy_place, board.enemy_place, sizeof(Point)*board.num_agent);
    std::memcpy(this->action_pending, board.action_pending, sizeof(Action)*board.num_agent*2);
    for(int j = 0; j < height; j++){
        std::memcpy(this->tiled[j], board.tiled[j], sizeof(int)*width);
    }
    std::copy(board.wait_agent.begin(), board.wait_agent.end(), std::back_inserter(this->wait_agent));
}

Board::~Board()
{ //Boardのデストラクタ
    if (tiled != NULL)
    {
        for (int i = 0; i < width; i++)
        {
            delete[] tiled[i];
        }
        delete[] tiled;
    }
    if (friend_place != NULL)
    {
        delete[] friend_place;
    }
    if (enemy_place != NULL)
    {
        delete[] enemy_place;
    }
}
void Board::setting(int w, int h)
{ //高さと幅の値の代入とfield_pointsの初期化
    width = w;
    height = h;
    field_points = new int *[height];
    for (int i = 0; i < height; i++)
    {
        field_points[i] = new int[width];
    }
}
void Board::set_field_Point(char point[]){
    for (int j = 0; j < height; j++)
    {
        for (int i = 0; i < width; i++)
        {
            // cout << i << j << "\n";
            field_points[j][i] = point[j * width + i];
        }
    }
}

void Board::display(){
    map <int , int> xyToAgentID;
    for(int i=0;i<num_agent;i++){
        xyToAgentID.emplace((friend_place[i].getY()) * width + (friend_place[i].getX()) , i+1);
        xyToAgentID.emplace((enemy_place[i].getY()) * width + (enemy_place[i].getX()) , -i-1);
    }
    for(int j = 0; j<height;j++){
        for(int i = 0;i<width;i++){
            decltype(xyToAgentID)::iterator thereAgent = xyToAgentID.find(j*width + i);
            if(tiled[j][i] == 1)printf("\x1b[31m");
            else if(tiled[j][i] == -1)printf("\x1b[33m");
            if(thereAgent != xyToAgentID.end()){
                printf("%3d %2d",field_points[j][i],thereAgent->second);
            }
            else printf("%3d   ",field_points[j][i]);
            printf("\x1b[39m");
        }
        cout << "\n\n";
    }
    //現在の盤面を出力
}
int Board::addAgentMap(int globalAID,int localAID){//返り値はint. 正常に入った時0 G->L異常で1 L->G異常で2
    pair<map<int, int>::iterator, bool> P2 = agentMapG_to_L.emplace(globalAID, localAID);//Global(Jsonから取得)からLocal(このプログラム内で使う)agentIDに変換
    pair<map<int, int>::iterator, bool> P1 = agentMapL_to_G.emplace(localAID, globalAID);//逆引き
    if(!P1.second)return 1;
    else if (!P2.second)return 2;
    else return 0;
}
int Board::getAIdG_to_L(int globalAID){
    return agentMapG_to_L.at(globalAID);//与えられたglobalAIDに対応するlocalAIDを返す
}
int Board::getAIdL_to_G(int localAID){
    return agentMapL_to_G.at(localAID);//与えられたlocalAIDに対応するGlobalAIDを返す
}
int Board::tile_point(int side)
{ //sideで味方(1)か敵(-1)かを指定できる
    int tile_points;
    tile_points = 0;
    //for (int i = 1; i <= width; i++)//chikaraコメントアウト
    for (int i = 0; i < width; i++)//chikara修正のため差し替え追加
    {
        //for (int j = 1; j <= height; j++)//chikaraコメントアウト
        for (int j = 0; j < height; j++)//chikara修正のため差し替え追加
        {
            if (tiled[i][j] == side)
                tile_points = tile_points + field_points[i][j];
        }
    }
    return tile_points;
    //タイルポイント計算メソッド
}
int Board::area_point(int side)
{
    // int area_points[];
    return 0;
    //領域ポイント計算メソッド
}

int Board::action(Action *action0,int number_agent){
    int action_bit = 0x0000;
    map<int, int> place_check; //key=座標,value=agentIDとする,この時、座標は100*x+yで表すものとする
    //cout<<"ループ1"<<endl;
    for(int i = 0;i < number_agent;i++){
        int id = action0[i].getAgentID();
        Point agent_xy = getAgent_place(id);
        //cout<< "id:"<<id<<" x:"<<agent_xy.getX()<<" y:"<<agent_xy.getY()<<" dx:"<<action0[i].getDX()<<" dy:"<<action0[i].getDY()<<endl;
        if(action0[i].getActionType() == Action::actionType::Move){
            place_check[(agent_xy.getX()*100)+agent_xy.getY()] = id + 20;
            if(movewall(id,action0[i].getDX(),action0[i].getDY()) == false){
                if(id > 0) {
                    action_bit = bit_calculation::high_add_bit(1,action_bit,id-1);
                    //cout<<id<<"が行動不可 1"<<endl;
                }
                else if(id < 0) {
                    action_bit = bit_calculation::high_add_bit(-1,action_bit,(-id)-1);
                    //cout<<id<<"が行動不可 2"<<endl;
                }
                else {//debug
                    //cout << id <<"がおかしい in first Move"<<endl;
                    exit(0);
                }
            }
        }
        else if(action0[i].getActionType() == Action::actionType::Remove){
            place_check[(agent_xy.getX()*100)+agent_xy.getY()] = id + 20;
            if(movewall(id,action0[i].getDX(),action0[i].getDY()) == false){
                if(id > 0) {
                    action_bit = bit_calculation::high_add_bit(1,action_bit,id-1);
                    //cout<<id<<"が行動不可 3"<<endl;
                }
                else if(id < 0){
                    action_bit = bit_calculation::high_add_bit(-1,action_bit,(-id)-1);
                    //cout<<id<<"が行動不可 4"<<endl;
                }
                else {//debug
                    //cout << id <<"がおかしい in first Remove"<<endl;
                    exit(0);
                }
            }
            else if(tiled[agent_xy.getY()+action0[i].getDY()][agent_xy.getX()+action0[i].getDX()]==0){
                if(id > 0){ 
                    action_bit = bit_calculation::high_add_bit(1,action_bit,id-1);
                    //cout<<id<<"が行動不可 5"<<endl;
                }
                else if(id < 0){
                    action_bit = bit_calculation::high_add_bit(-1,action_bit,(-id)-1);
                    //cout<<id<<"が行動不可 6"<<endl;
                }
                else {//debug
                    //cout << id <<"がおかしい in first Remove"<<endl;
                    exit(0);
                }
            }
        }
        else if(action0[i].getActionType() == Action::actionType::Stay){
            place_check[agent_xy.getX()*100+agent_xy.getY()] = id + 20;
            if(id == 0){
                //cout << id <<"がおかしい in first Stay"<<endl;
                    exit(0);
            }
        }
    }
    ////cout<<"ループ2"<<endl;
    for(int i = 0;i < number_agent;i++){
        bool can;
        int id = action0[i].getAgentID();
        Point agent_xy = getAgent_place(id);
        //cout<<"id:"<<id<<" x:"<<agent_xy.getX()<<" y:"<<agent_xy.getY()<<" dx:"<<action0[i].getDX()<<" dy:"<<action0[i].getDY()<<endl;
        if(id > 0) can = bit_calculation::high_return_bit(1,id-1,action_bit);
        else if(id < 0) can = bit_calculation::high_return_bit(-1,-id-1,action_bit);
        if(can == false){
            //Point agent_xy = getAgent_place(id);
            if(action0[i].getActionType() == Action::actionType::Move){
                int will_place = ((agent_xy.getX()+action0[i].getDX())*100)+(agent_xy.getY()+action0[i].getDY());
                
                if(place_check.find(will_place) != place_check.end()){
                    int overlap_id = place_check[will_place];
                    if(id > 0) {
                        action_bit = bit_calculation::high_add_bit(1,action_bit,id-1);
                        //cout<<id<<"が行動不可 7"<<endl;
                    }
                    else if(id < 0){
                        action_bit = bit_calculation::high_add_bit(-1,action_bit,(-id)-1);
                        //cout<<id<<"が行動不可 8"<<endl;
                    }
                    if(overlap_id < 10){
                         if(overlap_id > 0) {
                            action_bit = bit_calculation::high_add_bit(1,action_bit,overlap_id-1);
                            //cout<<overlap_id<<"が行動不可 9"<<endl;
                         }
                        else if(overlap_id < 0){
                            action_bit = bit_calculation::high_add_bit(-1,action_bit,(-overlap_id)-1);
                            //cout<<overlap_id<<"が行動不可 10"<<endl;
                        }
                    }
                }
                else{
                    if(getTile(agent_xy.getX()+action0[i].getDX(),agent_xy.getY()+action0[i].getDY())==-1){
                        action_bit = bit_calculation::high_add_bit(1,action_bit,id-1);
                        //cout<<id<<"が行動不可 11"<<endl;
                    }
                    else if(getTile(agent_xy.getX()+action0[i].getDX(),agent_xy.getY()+action0[i].getDY())==1){
                        action_bit = bit_calculation::high_add_bit(-1,action_bit,(-1*id)-1);
                        //cout<<id<<"が行動不可 12"<<endl;
                    }
                    else {
                        place_check[((agent_xy.getX()+action0[i].getDX())*100)+(agent_xy.getY()+action0[i].getDY())] = id;
                    }
                }
            }
            if(action0[i].getActionType() == Action::actionType::Remove){
                int will_place = ((agent_xy.getX()+action0[i].getDX())*100)+(agent_xy.getY()+action0[i].getDY());
                if(place_check.find(will_place) != place_check.end()){
                    int overlap_id = place_check[will_place];
                    if(id > 0) {
                        action_bit = bit_calculation::high_add_bit(1,action_bit,id-1);
                        //cout<<id<<"が行動不可 13"<<endl;
                    }
                    else if(id < 0) {
                        action_bit = bit_calculation::high_add_bit(-1,action_bit,(-id)-1);
                        //cout<<id<<"が行動不可 14"<<endl;
                    }
                    if(overlap_id < 10){
                         if(overlap_id > 0) {
                            action_bit = bit_calculation::high_add_bit(1,action_bit,overlap_id-1);
                            //cout<<id<<"が行動不可 15"<<endl;
                         }
                        else if(overlap_id < 0){
                            action_bit = bit_calculation::high_add_bit(-1,action_bit,(-overlap_id)-1);
                            //cout<<id<<"が行動不可 16"<<endl;
                        }
                    }
                }
                else place_check[((agent_xy.getX()+action0[i].getDX())*100)+(agent_xy.getY()+action0[i].getDY())] = id;
            }
        }
    }
    //cout<<"ループ3"<<endl;
    for(int i = 0;i < number_agent;i++){
        int id =action0[i].getAgentID();
        Point agent_xy = getAgent_place(id);
        //cout<< "id:"<<id<<" x:"<<agent_xy.getX()<<" y:"<<agent_xy.getY()<<" dx:"<<action0[i].getDX()<<" dy:"<<action0[i].getDY()<<endl;
        bool can;
        if(id > 0)
            can = bit_calculation::high_return_bit(1,id-1,action_bit);
        else if(id < 0)
            can = bit_calculation::high_return_bit(-1,(-id)-1,action_bit);
        if(can == false){
            if(action0[i].getActionType() == Action::actionType::Move){
                //Point agent_xy = getAgent_place(id);
                agent_xy.addDXDY(action0[i].getDX(),action0[i].getDY());
                if(id > 0){
                    friend_place[id-1].setXY(agent_xy.getX(),agent_xy.getY());
                }
                else if(id < 0){
                    enemy_place[(-id)-1].setXY(agent_xy.getX(),agent_xy.getY());
                }
                set_one_tile(agent_type(id),agent_xy.getX(),agent_xy.getY());
            }
            if(action0[i].getActionType() == Action::actionType::Remove){
                Point agent_xy = getAgent_place(id);
                set_one_tile(0,agent_xy.getX()+action0[i].getDX(),agent_xy.getY()+action0[i].getDY());
            }
        }
    }
    return action_bit;
}

int Board::agent_type(int id)
{
    if (0 < id)
        return 1;
    else if (0 > id)
        return -1;
    else
        throw runtime_error("agentIDが正しくありません。@agent_type");
} /////敵か味方か判断する
Point Board::getAgent_place(int id)
{
    if (agent_type(id) == 1)
        return friend_place[abs(id) - 1];
    else if (agent_type(id) == -1)
        return enemy_place[abs(id) - 1];
    else
        throw runtime_error("agentIDが正しくありません@getAgent_place");
} //idを渡したらそのエージェントの座標を返す
int Board::calcAreaPoint(int teamID){//領域の開始点を探す
    //printf("calc start\n");
    vector <multiset<uint8_t>> startX_endX(height);
    int x,y,newX,newY;
    multiset<uint8_t>::iterator xIt;
    for(y=0;y<height;y++){
        for(x=0;x<width;x++){
            //printf("%d,%d\n",x,y);
            if(getTile(x,y) == teamID){

                xIt = startX_endX[y].lower_bound((uint8_t)x);
                if(xIt!=startX_endX[y].end() && x==(*xIt)){
                    x=*(++xIt);
                }else{
                    tracking(teamID,x,y,&startX_endX);
                    xIt = startX_endX[y].lower_bound(x);
                    x=*(++xIt);
                }
            }
        }
    }
    int areaScore = 0;
    for(y=0;y<height;y++){
        for(multiset<uint8_t>::iterator i = startX_endX[y].begin();i!=startX_endX[y].end();i++){
            int x1 = (*i);
            i++;
            int x2 = (*i);
            for(int dx=x1;dx<=x2;dx++){
                if(tiled[y][dx] != teamID){
                    areaScore += (field_points[y][dx]>0) ? field_points[y][dx] : -field_points[y][dx];//絶対値を入れる用に参考演算子で調べる．
                }
            }
        }
    }
    //displayArea(&startX_endX);
    return areaScore;
}
void Board::tracking(int teamID, int firstX, int firstY, vector<multiset<uint8_t>> *startX_endX){
    int x=firstX, y=firstY, newX, newY;
    int d=5,d0,i;
    for(i=0;i<8;i++,d=((d+1)%8)){
        newX = x + direction[d][0];
        if(newX < 0 || newX >= width)continue;
        newY = y + direction[d][1];
        if(newY < 0 || newY >= height)continue;
        if(tiled[newY][newX] == teamID)break;
    }
    if(i == 8){
        startX_endX->at(y).insert(x);
        startX_endX->at(y).insert(x);
        return;
    }
    d0 = d;

    while(1){
        int lastD = d;
        y = newY,
        x = newX;
        d = (d+4)%8;
        //printf("%d %d %d \n",newX,newY,d);
        while(1){
            d=(d+1)%8;
            newX = x + direction[d][0];
            if(newX < 0|| newX >= width)continue;
            newY = y + direction[d][1];
            if(newY < 0|| newY >= height)continue;
            //printf("break前 > %d %d %d %d",newX, newY,tiled[newY][newX],teamID);
            if(tiled[newY][newX] == teamID)break;
        }
        //printf("\n");
        switch (d)
        {
        case 0:
            if(lastD >= 4)startX_endX->at(y).insert(x);
            break;
        case 1:
        case 2:
            if(lastD != 4)startX_endX->at(y).insert(x);
            break;
        case 3:
            if(lastD != 4 && lastD != 5)startX_endX->at(y).insert(x);
            break;
        case 4:
            if(lastD <= 3)startX_endX->at(y).insert(x);
            break;
        case 5:
        case 6:
            if(lastD != 0)startX_endX->at(y).insert(x);
            break;
        case 7:
            if(lastD > 1)startX_endX->at(y).insert(x);
            break;
        default:
            throw runtime_error("dの値が正しくありません\n");
            break;
        }
        if(direction[d][1] * direction[lastD][1] < 0 && (d+2)%8 != lastD) startX_endX->at(y).insert(x);
        if(x == firstX && y == firstY && d== d0)return;
    }
}
void Board::displayArea(vector <multiset<uint8_t>> *startX_endX){
    for(int y=0;y<height;y++){
        for(multiset<uint8_t>::iterator i = startX_endX->at(y).begin();i!=startX_endX->at(y).end();i++){
            int x1 = (*i);
            printf("(%d,%d)",x1,y);
        }
        printf("\n");
    }
}

void Board::set_action(Action *act,int idx){
    int ID = act->getAgentID();
    if(ID>0)
        action_pending[ID-1] = *act;
    if(ID<0)
        action_pending[num_agent+(-ID-1)] = *act;
    //cout << "wait_agent.erase at " << idx << endl;
    wait_agent.erase(wait_agent.begin()+idx);
}

void Board::set_action(Action *act){
    int ID = act->getAgentID();
    if(ID>0)
        action_pending[ID-1] = *act;
    if(ID<0)
        action_pending[num_agent+(-ID-1)] = *act;
    vector<int>::iterator itr = std::find(wait_agent.begin(),wait_agent.end(),ID);
    if(itr != wait_agent.end())wait_agent.erase(itr);
    else cout<<"ちゃんとはいってない@set_action"<<endl;
}

void Board::pending_check(Action *act,int idx){
    set_action(act,idx);
    if(wait_agent.empty()==true){
        //display();
        //display_action();        
        int bit = action(action_pending,2*num_agent);
        //move(action_pending);
        //display();
        for(int i=0;i<num_agent;i++) wait_agent.push_back(i+1);//chikara追加
        for(int i=0;i<num_agent;i++) wait_agent.push_back(-i-1);//chikara追加
    }
}

int Board::count_score(){
    //勝ちか負けか判断するメソッド,勝ちの時は+,負けの時は-になる
      int friend_tile_point = tile_point(1);
      int friend_area_point =calcAreaPoint(1); 
      int enemy_tile_point = tile_point(-1);
      int enemy_area_point = calcAreaPoint(-1);
      int friend_point = friend_tile_point + friend_area_point;
      int enemy_point = enemy_tile_point + enemy_area_point;
      if (friend_point != enemy_point)
            return friend_point - enemy_point;
      else
            return friend_tile_point - enemy_tile_point;
}

void Board::move(Action play_action[])
{
    cout<<"moveはじめ"<<endl;
    for(int i = 0;i < 2 * num_agent;i++){
            if(play_action[i].getAgentID()>0){//chikara修正のため差し替え追加
                int j=play_action[i].getAgentID()-1;//chikara追加
                cout<<"味方"<<endl;
                if(play_action[i].getActionType()==Action::actionType::Move){
                    cout<<"Move"<<endl;
                    friend_place[j].addDXDY(play_action[i].getDX(),play_action[i].getDY());
                    set_one_tile(1,friend_place[j].getX(),friend_place[j].getY());
                }
                else if(play_action[i].getActionType()==Action::actionType::Remove){
                    cout<<"Remove"<<endl;
                    set_one_tile(0,friend_place[j].getX()+play_action[i].getDX(),friend_place[j].getY()+play_action[i].getDY());
                }
                else if(play_action[i].getActionType()==Action::actionType::Stay){
                    //何もなし
                    cout<<"Stay"<<endl;
                }
            }
            if(play_action[i].getAgentID()<0){//chikara修正のため差し替え追加
                int j=-play_action[i].getAgentID()-1;//chikara追加
                cout<<"敵"<<endl;
                if(play_action[i].getActionType()==Action::actionType::Move){
                    cout<<"Move"<<endl;
                    enemy_place[j].addDXDY(play_action[i].getDX(),play_action[i].getDY());
                    set_one_tile(-1,enemy_place[j].getX(),enemy_place[j].getY());
                }
                else if(play_action[i].getActionType()==Action::actionType::Remove){
                    cout<<"Remove"<<endl;
                    set_one_tile(0,enemy_place[j].getX()+play_action[i].getDX(),enemy_place[j].getY()+play_action[i].getDY());
                }
                else if(play_action[i].getActionType()==Action::actionType::Stay){
                    //何もなし
                    cout<<"Stay"<<endl;
                }
            }
    }
}

void Board::display_action(){//chikara:表示のしかたを変えた
    cout << "action_pending[]=";
    for(int i=0;i<2*num_agent;i++){
        Action *at=&action_pending[i];
        cout << at->to_string();
    }
    cout << endl;
    cout << "wait_agent[]=";
    for(int i=0;i<wait_agent.size();i++){
      cout << wait_agent[i] << ";";
    }
    cout << endl;
}
/*
int main(){
    Board *board =jsonReceive::jsonRead(1,"A-1.json");
    board->display();
    Action action0[6];
    action0[0].setAgentID(1);
    action0[0].setActionType(Action::actionType::Move);
    action0[0].setDX(-1);
    action0[0].setDY(-1);
    action0[1].setAgentID(2);
    action0[1].setActionType(Action::actionType::Move);
    action0[1].setDX(0);
    action0[1].setDY(-1);
    action0[2].setAgentID(3);
    action0[2].setActionType(Action::actionType::Remove);
    action0[2].setDX(1);
    action0[2].setDY(1);
    action0[3].setAgentID(-1);
    action0[3].setActionType(Action::actionType::Move);
    action0[3].setDX(-1);
    action0[3].setDY(1);
    action0[4].setAgentID(-2);
    action0[4].setActionType(Action::actionType::Move);
    action0[4].setDX(-1);
    action0[4].setDY(-1);
    action0[5].setAgentID(-3);
    action0[5].setActionType(Action::actionType::Move);
    action0[5].setDX(-1);
    action0[5].setDY(-1);
    int bit = board->action(action0,6);
    cout<<endl;
    board->display();
    cout<<"\n"<<hex<<bit<<endl;
    return 0;
}
*/
/*int main(){
    Board *board =jsonReceive::jsonRead(1,"A-1.json");
    board->display();
    for(int i=1;i<=Board::num_agent;i++){
        for(int j=0;j<17;j++){
            cout<<"id:"<<i<<" num:"<<j<<" T or F:"<<board->invalid_check(i,j)<<endl;
        }
    }
    for(int i=1;i<=Board::num_agent;i++){
        for(int j=0;j<17;j++){
            cout<<"id:"<<-i<<" num:"<<j<<" T or F:"<<board->invalid_check(-i,j)<<endl;
        }
    }
}*/
/*
int main(){
    Board *board =jsonReceive::jsonRead(1,"A-1.json");
    board->display();
    board->overwrite_copy();
    board->display();
    return 0;
}*/

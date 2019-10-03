#include <climits>
#include <vector>
#include <stack>
#include "Agent-Action.hpp"

class AlphaBetaSearch{
    /*
    * 探索する深さの制限用の定数 MaxDepth
    * boardで盤面を取得
    */

    static Action ourMax (int alpha,int beta,Board &board,int MaxDepth){ //自分の手の評価
        if(Depth <= 0){//末端かどうかの判定．一つ枝を降りる度にDepthをデクリメントしていく．
            return evaluation(board);//もし、目的の深度ならそこで評価して評価値を返す
        }
        int max = -INT_MAX;

        std::vector<std::vector<Action>> searchArray =  boardExpention(int Agent.agentID,Board &board);//Actionが格納されたVectorを受け取る．
        int aIDx,i = 0,agentOnTeam = Board.num_agent;//stackInはスタックに入れるIDと順番を格納、agentOnTeamは1チームのエージェント数
        std::vector<int> agentActionIndex;//そのエージェントのアクションが何番目かをいれる．添字でエージェントを判別
        while (1){ //Stackにエージェントのアクションとそのエージェントの中での順番をしめす配列を格納する
            while(agentActionIndex.size()>agentOnTeam){//パターン完成時の処理
                 //Vecotrを取り出して別配列に入れてBoardのActionメソッドに渡す．帰ってきたBoardを引数にして再帰呼び出し
                agentActionIndex.push_back(0);
            }
            //agentActionIndexの保持する値から、各エージェントのActionを作って、配列に収めて、Boardクラスのactionメソッドを呼んで子局面の盤面状態を作る部分を作ってください。
            
            while(agentActionIndex.size()>0){
                aIDx = agentActionIndex.pop_back();
                if(aIDx<searchArray[agentActionIndex.size()-1].size())break;
            }
            if(agentActionIndex.size()==0)break;
            aIDx++;
            agentActionIndex.push_back(aIDx);
        }
        
    }

    
    static int intOurMax (int alpha,int beta,Board& board,int MaxDepth){ //自分の手の評価
        if(board.turn == MaxDepth){//深さ制限
            return evaluation(board);//もし、目的の深度ならそこで評価して評価値を返す
        }
        
        int max = alpha;
    }
    static int enemyMin (int alpha,int beta,Board& board, int MaxDepth){//相手の手の評価
        if(board.turn == MaxDepth){//深さ制限
            return evaluation(board);
        }
        int min = beta;
    }
    static int evaluation(Board& board){//Board型の引数
        // 評価関数
    }
};
#include<iostream>
#include<queue>
#include<set>
#include<stack>
#include<map>
#include<climits>
#include<string.h>
#include "parse.h"
using namespace std;

const int UP = 1;
const int DOWN = 2;
const int RIGHT = 3;
const int LEFT = 4;
static int solutions = 0;
static int puzzles = 0;
struct Entry{
    int depth;
    string str;
    Entry(int newDepth, string newStr){
        depth = newDepth;
        str = newStr;
    }

};

struct AsEntry: public Entry{
    int score;
    AsEntry(int newDepth, string newStr, int newScore):
        Entry(newDepth, newStr){
        score = newScore;
    }
};

struct Stats{
    int depth, goals;
    unsigned maxSize;
    Stats(int newDepth, int newGoals, unsigned newMaxSize){
        depth = newDepth;
        goals = newGoals;
        maxSize = newMaxSize;
    }
};


class asCompare
{
    public:
          bool operator() (const AsEntry& lhs, const AsEntry&rhs) const{
                    return (lhs.score>rhs.score);
          }
};

map<int, vector<Stats> > statMap;


int getConfig(){
   if(puzzles < 3){
       return 1;
   }else if(puzzles < 7){
       return 2;
   }else if (puzzles < 15){
       return 3;
   }else if (puzzles < 31){
       return 4;
   }else if (puzzles < 51){
       return 5;
   }else if (puzzles < 91){
       return 6;
   }else if (puzzles < 157){
       return 7;
   }else if (puzzles < 285){
       return 8;
   }else if (puzzles < 449){
       return 9;
   }else{
       return 10;
   }
}

int getH2Value(string puzzle){
    int sum = 0;
    string correct = "012345678";
    for(int i = 0; i< 9; i++){
        for(int j =0; j<9; j++){
            if(correct[i] != puzzle[i]){
                int offX = abs( i%3 - j%3);
                int offY = abs( i/3 - j/3);
                int manhantan = offX + offY;
                sum+= manhantan;
            }
        }
    }
    return sum;
}

int getHValue(string puzzle){
    string correct = "012345678";
    int misplaced = 0;
    for(int i = 0; i<9; i++){
        if(correct[i] != puzzle[i]){
            misplaced++;
        }
    }
    //printf("%s\n%s\n%d\n", correct.c_str(), puzzle.c_str(), misplaced);
    return misplaced;
}

int getH3Value(string puzzle){
    return getHValue(puzzle) * 5;
}


void asExpand(AsEntry &current, map<string, int> &g_score, map<string, int> &f_score,
        int &states, priority_queue<AsEntry, vector<AsEntry>, asCompare> &toExplore, 
        set<string> &explored,
        unsigned &maxSize,
        int (hCalc)(string)){
    for(int move = 1; move < 5; move++){
        string next = nextState (current.str, move);
        if(strcmp(next.c_str(), "") != 0){
            states++;
            int t_g = g_score[current.str] + 1;
            int t_f = t_g + hCalc(next);
            //if neighbor in closedset and tentative_f_score >= f_score[neighbor]
            if(explored.count(next) != 0 && t_f > f_score[next]){
                continue;
            }
            g_score[next] = t_g;
            f_score[next] = t_f;
            toExplore.push(AsEntry(current.depth+1, next, t_f));
            maxSize = toExplore.size() > maxSize ? 
                toExplore.size() : maxSize;
        }
    }
}

void asSearch(string puzzle, int (hCalc)(string)){
    priority_queue<AsEntry, vector<AsEntry>, asCompare> toExplore;
    set<string> explored;
    map<string, int> g_score;
    map<string, int> f_score;
    g_score[puzzle] = 0;
    f_score[puzzle] = 0 + hCalc(puzzle);
    int states = 0;
    unsigned maxSize = 1;
    int config = getConfig();

    toExplore.push(AsEntry(0, puzzle, hCalc(puzzle)));
    while(toExplore.empty() == false){
        AsEntry current = toExplore.top();
        if(isGoalState(current.str)){
            cout<<"found solution"<<solutions<<endl;
            solutions++;
            statMap[config].push_back(Stats(current.depth+1, states, maxSize));
            return;
        }

        toExplore.pop();
        explored.insert(current.str);
		asExpand(current, g_score, f_score,
			states, toExplore, explored,maxSize, hCalc);
    }
}

void ids(string puzzle) {
    stack<Entry> *toExplore = new stack<Entry>();
    set<string> *explored = new set<string>();
    int maxDepth = 0;
    unsigned maxSize = 0;
    int config = getConfig();
    int states = 0;
    while(1){ 
        delete toExplore;
        delete explored;
        maxDepth++;
        toExplore = new stack<Entry>();
        explored = new set<string>();
        toExplore->push(Entry(1,puzzle));

        while(toExplore->empty() == false){
            Entry current = toExplore->top();
            toExplore->pop();
            if(current.depth >= maxDepth){
                continue;
            }
            for(int move = 1; move < 5; move++){
                string next = nextState (current.str, move);
                if(strcmp(next.c_str(), "") != 0){
                    if(explored->count(next) == 0){
                        states++;
                        if(isGoalState(next)){
                            solutions++;
                            statMap[config].push_back(Stats(current.depth+1, 
                                        states, maxSize));
                            return;
                        }
                        explored->insert(next);
                        toExplore->push(Entry(current.depth+1, next));
                        maxSize = toExplore->size() > maxSize ? 
                            toExplore->size() : maxSize;
                    }
                }

            }
        }
    }
}


bool checkNextState(Entry &entry, string &nextState, 
        stack<Entry> &toExplore, set<string> &explored,
        int &config, int &states, unsigned &maxSize){
    if(strcmp(nextState.c_str(), "") != 0){
        states++;
        if(isGoalState(nextState)){
            cout<<"found solution"<<solutions<<endl;
            solutions++;
            statMap[config].push_back(Stats(entry.depth+1, states, maxSize));
            return true;
        }
        if(explored.count(nextState) == 0){
            explored.insert(nextState);
            toExplore.push(Entry(entry.depth+1, nextState));
            maxSize = toExplore.size() > maxSize ? 
                toExplore.size() : maxSize;
        }
    }
    return false;
}

void dfs(string &puzzle) {
    stack<Entry> toExplore;
    set<string> explored;
    
    toExplore.push(Entry(0,puzzle));
    int states = 0;
    unsigned maxSize = 1;
    int config = getConfig();
    while(toExplore.empty() == false){
        Entry current = toExplore.top();
        toExplore.pop();
        for(int move = 1; move < 5; move++){
            string next = nextState (current.str, move);
            if(checkNextState(current, next, toExplore, 
                        explored, config, states, maxSize) == true){
                return;
            }
        }
    }

}
void bfs(string &puzzle) {
    queue<Entry> toExplore;
    
    toExplore.push(Entry(1,puzzle));
    int states = 0;
    unsigned maxSize = 1;
    int config = getConfig();
    while(toExplore.empty() == false){
        Entry current = toExplore.front();
        toExplore.pop();
        for(int move = 1; move < 5; move++){
            string next = nextState (current.str, move);
            if(strcmp(next.c_str(), "") != 0){
                states++;
                if(isGoalState(next)){
                    statMap[config].push_back(Stats(current.depth+1, states, maxSize));
                    solutions++;
                    return;
                }
                toExplore.push(Entry(current.depth+1, next));
                maxSize = toExplore.size() > maxSize ? 
                    toExplore.size() : maxSize;
            }

        }
    }

}



string hillClimbAux(string &puzzle){
    string prev = "";
    string best = puzzle;
    while(strcmp(prev.c_str(), best.c_str())!=0){
        int bestH = getH2Value(best);
        prev = best;
        for(int move = 1; move < 5; move++){
            string next = nextState (best, move);
            if(strcmp(next.c_str(), "") != 0){
                int h = getH2Value(next);
                if(h < bestH){
                    bestH = h;
                    best = next;
                }
            }
        }
    }
    return best;
}

void hillClimb(string &puzzle){
    string best = hillClimbAux(puzzle);
    if(isGoalState(best)){
        solutions++;
    }
}

string generateSecondBest(string best){
    return best;
}

void hillClimb2(string &puzzle, int maxSecond){
    string best = puzzle;
    while(maxSecond-- > 0){
        best = hillClimbAux(best);
        if(isGoalState(best)){
            solutions++;
            return;
        }else{
            best = generateSecondBest(best);
        }
    }
}

double averageVector(vector<Stats> &stats, char stat){
    double sum =0 ;
    double n = (int) stats.size();
    for(int i =0; i < n; i++){
        switch(stat){
            case 'd': 
                sum += stats[i].depth;
                break;
            case 'g':
                sum += stats[i].goals;
                break;
            case 'm':
                sum += (double) stats[i].maxSize;
                break;
        }
    }
    return sum/n;
}

void printGroup(int group){
    double averageDepth = 0;
    double averageGoalsTested = 0;
    double averageMaxFrontier = 0;

    averageDepth = averageVector(statMap[group], 'd');
    averageGoalsTested = averageVector(statMap[group], 'g');
    averageMaxFrontier = averageVector(statMap[group], 'm');

    printf("%d, %f, %f, %f\n", group, averageDepth, averageGoalsTested, averageMaxFrontier);


}

void printOutput(){
    for(int i = 1; i <11; i++){
        printGroup(i);
    }

    double percent = (double)solutions/(double) puzzles;
    cout<<"Percent complete "<<percent<<endl;
}

int main(int argc, const char* argv[]) {
    if(argc != 2){
        cerr<<"arg 1 must be search type\n";
        return -1;
    }
    Puzzles puz("puzzles.txt");
    string next_puzzle = puz.nextPuzzle();
    char s = argv[1][0];
    while(next_puzzle.size()) {
        cout<<"on puzzle "<<puzzles<<endl;
        switch(s){
            case 'b':
                bfs(next_puzzle);
                break;
            case 'd':
                dfs(next_puzzle);
                break;
            case 'a':
                asSearch(next_puzzle, getHValue);
                break;
            case 'x':
                asSearch(next_puzzle, getH2Value);
                break;
            case 'z':
                asSearch(next_puzzle, getH3Value);
                break;
            case 'i':
                ids(next_puzzle);
                break;
            case 'h':
                hillClimb(next_puzzle);
                break;
            default:
                cerr<<"invalid search type"<<endl;
                return -1;

        }
        puzzles++;
        next_puzzle = puz.nextPuzzle();
    }
    printOutput();
    return 0;
}

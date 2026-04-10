#include <iostream>
#include <fstream>
#include <ctime>
#include <cctype>
using namespace std;

string user;

void clearScreen(){
    cout << "\033[2J\033[H";
}
int getTimeLeft(time_t start, int limit){
    return limit - (int)(time(0) - start);
}
void saveResult(string player, string game, string result){
    ofstream f("scores.txt", ios::app);
    if(f){
        f << player << "," << game << "," << result << endl;
    }
}

void showHistory(){
    ifstream f("scores.txt");

    if(!f){
        cout<<"\nNo records found!\n";
        return;
    }

    string line;
    cout<<"\n===== LEADERBOARD =====";
    cout<<"\t\tUser: "<<user<<"\n";

    cout<<"Game\tResult\n";
    cout<<"----------------------\n";

    while(getline(f,line)){
        string player, game, result;

        int p1 = line.find(',');
        int p2 = line.find(',', p1+1);

        if(p1 != -1 && p2 != -1){
            game = line.substr(p1+1, p2-p1-1);
            result = line.substr(p2+1);

            cout<<game<<"\t"<<result<<endl;
        }
    }
}

void clearHistory(){
    ofstream f("scores.txt", ios::trunc);
    cout<<"\nLeaderboard Cleared!\n";
    cout<<"All records removed successfully.\n";
}

class Game{
public:
    virtual void startGame() = 0;
};

/* ================= TIC TAC TOE ================= */
class TicTacToe : public Game{
    char b[3][3];
    char p;

public:
    TicTacToe(){
        char c='1';
        for(int i=0;i<3;i++)
            for(int j=0;j<3;j++)
                b[i][j]=c++;
        p='X';
    }

    void display(){
        cout<<"\n";
        for(int i=0;i<3;i++){
            cout<<" ";
            for(int j=0;j<3;j++){
                cout<<b[i][j];
                if(j<2) cout<<" | ";
            }
            cout<<"\n";
            if(i<2) cout<<"---+---+---\n";
        }
    }

    bool win(){
        for(int i=0;i<3;i++){
            if(b[i][0]==p&&b[i][1]==p&&b[i][2]==p) return true;
            if(b[0][i]==p&&b[1][i]==p&&b[2][i]==p) return true;
        }
        if(b[0][0]==p&&b[1][1]==p&&b[2][2]==p) return true;
        if(b[0][2]==p&&b[1][1]==p&&b[2][0]==p) return true;
        return false;
    }

    bool draw(){
        for(int i=0;i<3;i++)
            for(int j=0;j<3;j++)
                if(b[i][j]!='X' && b[i][j]!='O')
                    return false;
        return true;
    }

    void startGame(){
        time_t start=time(0);

        while(true){
            int t=getTimeLeft(start,60);
            if(t<=0){
                cout<<"\nTimeout!\n";
                saveResult(user,"TTT","Timeout");
                break;
            }

            clearScreen();
            cout<<"===== TIC TAC TOE =====\n";
            cout<<"Time Left: "<<t<<"\n";
            display();

            int pos;
            cout<<"\nPlayer "<<p<<" (1-9): ";
            cin>>pos;

            if(pos<1||pos>9) continue;

            int r=(pos-1)/3;
            int c=(pos-1)%3;

            if(b[r][c]=='X'||b[r][c]=='O') continue;

            b[r][c]=p;

            if(win()){
                display();
                cout<<"\nPlayer "<<p<<" Wins!\n";
                saveResult(user,"TTT","Win");
                break;
            }

            if(draw()){
                display();
                cout<<"\nDraw!\n";
                saveResult(user,"TTT","Draw");
                break;
            }

            p = (p=='X') ? 'O' : 'X';
        }

        cout<<"\nThanks for playing Tic Tac Toe!\n";
    }
};

/* ================= CONNECT 4 ================= */
class Connect4 : public Game{
    char b[6][7];
    char p;

public:
    Connect4(){
        for(int i=0;i<6;i++)
            for(int j=0;j<7;j++)
                b[i][j]='.';
        p='X';
    }

    void display(int t){
        clearScreen();
        cout<<"===== CONNECT 4 =====\n";
        cout<<"Time Left: "<<t<<"\n\n";

        for(int i=0;i<6;i++){
            cout<<"| ";
            for(int j=0;j<7;j++){
                cout<<b[i][j]<<" ";
            }
            cout<<"|\n";
        }
        cout<<"---------------------\n";
        cout<<"  0 1 2 3 4 5 6\n";
    }

    bool drop(int c){
        for(int i=5;i>=0;i--){
            if(b[i][c]=='.'){
                b[i][c]=p;
                return true;
            }
        }
        return false;
    }

    bool win(){
        for(int i=0;i<6;i++){
            for(int j=0;j<7;j++){
                if(b[i][j]==p){

                    if(j+3<7 && b[i][j+1]==p && b[i][j+2]==p && b[i][j+3]==p) return true;
                    if(i+3<6 && b[i+1][j]==p && b[i+2][j]==p && b[i+3][j]==p) return true;
                    if(i+3<6 && j+3<7 && b[i+1][j+1]==p && b[i+2][j+2]==p && b[i+3][j+3]==p) return true;
                    if(i+3<6 && j-3>=0 && b[i+1][j-1]==p && b[i+2][j-2]==p && b[i+3][j-3]==p) return true;
                }
            }
        }
        return false;
    }

    void startGame(){
        time_t start=time(0);

        while(true){
            int t=getTimeLeft(start,60);
            if(t<=0){
                cout<<"\nTimeout!\n";
                saveResult(user,"C4","Timeout");
                break;
            }

            display(t);

            int c;
            cout<<"\nPlayer "<<p<<" (0-6): ";
            cin>>c;

            if(c<0||c>6||!drop(c)) continue;

            if(win()){
                display(t);
                cout<<"\nPlayer "<<p<<" Wins!\n";
                saveResult(user,"C4","Win");
                break;
            }

            p = (p=='X') ? 'O' : 'X';
        }

        cout<<"\nThanks for playing Connect 4!\n";
    }
};

/* ================= SNAKE ================= */
class SnakeGame : public Game{
    int x,y,fx,fy,score;

public:
    SnakeGame(){
        x=2; y=2;
        score=0;
        generateFood();
    }

    void generateFood(){
        do{
            fx=rand()%5;
            fy=rand()%5;
        }while(fx==x && fy==y);
    }

    void display(int t){
        clearScreen();
        cout<<"===== SNAKE GAME =====\n";
        cout<<"Time: "<<t<<" Score: "<<score<<"\n\n";
        cout<<"Use W A S D keys\n\n";

        for(int i=0;i<5;i++){
            for(int j=0;j<5;j++){
                if(i==x&&j==y) cout<<" S ";
                else if(i==fx&&j==fy) cout<<" F ";
                else cout<<" . ";
            }
            cout<<"\n";
        }
    }

    void startGame(){
        cout<<"\nSnake Instructions:\n";
        cout<<"Move using W(UP) A(LEFT) S(DOWN) D(RIGHT)\nEat food to score\nAvoid walls\n\n";

        time_t start=time(0);

        while(true){
            int t=getTimeLeft(start,60);
            if(t<=0) break;

            display(t);

            char m;
            cin>>m;
            m=tolower(m);

            if(m=='w') x--;
            else if(m=='s') x++;
            else if(m=='a') y--;
            else if(m=='d') y++;

            if(x<0||y<0||x>=5||y>=5) break;

            if(x==fx && y==fy){
                score++;
                generateFood();
            }
        }

        cout<<"\nFinal Score: "<<score<<"\n";
        saveResult(user,"Snake","Score "+to_string(score));

        cout<<"\nThanks for playing Snake Game!\n";
    }
};

/* ================= MAIN ================= */
int main(){
    srand(time(0));

    cout<<"===== GAME HUB =====\n";
    cout<<"Enter Username: ";
    cin>>user;

    while(true){
        cout<<"\n1.TicTacToe\n2.Connect4\n3.Snake\n4.Leaderboard\n5.Clear\n6.Exit\n";
        cout<<"Choice: ";

        int ch;
        cin>>ch;

        Game *g = NULL;

        if(ch==1) g = new TicTacToe();
        else if(ch==2) g = new Connect4();
        else if(ch==3) g = new SnakeGame();
        else if(ch==4){ showHistory(); continue; }
        else if(ch==5){ clearHistory(); continue; }
        else if(ch==6){
            cout<<"\nThanks! Visit again.\n";
            break;
        }
        else continue;

        g->startGame();
        delete g;
    }

    return 0;
}

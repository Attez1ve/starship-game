#include <iostream>
#include <Windows.h>
#include <conio.h>

using namespace std;

class Game{
    // m - y     |* x
    // n - x     |y
private:
    HANDLE hConsole;
    static const int m = 19, n = 42;
    const int id_white_color = 15*17, id_default_color = 15, id_red_color = 4 * 17, id_gray_color = 8*17;
    int move_framerate, shoot_framerate, meteorite_framerate;
    int max_meteorites_count, current_meteorites_count{}, current_meteorite{}, meteorite_chance;
    int key = '0';
    COORD cPos{}, shipPos{};
    int board[m][n]{};
    long max_score;
public:
    long score = 0;
    bool is_playing;

    Game(int move, int shoot, int meteorite, int max_meteorites, int chance, long m_score, HANDLE handle){
        move_framerate = move;
        shoot_framerate = shoot;
        meteorite_framerate = meteorite;
        max_meteorites_count = max_meteorites;
        meteorite_chance = chance;
        max_score = m_score;
        hConsole = handle;

        is_playing = true;
        }

    void start(){
        //generate borders
        for (int y = 0; y < m; ++y) {
            for (int x = 0; x < n; ++x) {
                if (y == 0 || y == m - 1 || x <= 1 || x >= n - 2){
                    board[y][x] = 1;
                }
                else{
                    board[y][x] = 0;
                }
            }
        }
        shipPos.X = 3;
        shipPos.Y = (m-1)/2;
        board[shipPos.Y][shipPos.X] = 2; // start ship position
        for (int y = 0; y < m; ++y) {
            for (int x = 0; x < n; ++x) {
                if (board[y][x] == 1){
                    SetConsoleTextAttribute(hConsole, id_white_color);
                    cout << "#";
                    SetConsoleTextAttribute(hConsole, id_default_color);
                } else {
                    cout << board[y][x];
                }
            }
            cout << endl;
        }
    }

    void draw(){
        cPos.X = 2;
        cPos.Y = 1;
        for (int y = 1; y < m-1; ++y) {
            SetConsoleCursorPosition(hConsole, cPos);
            for (int x = 2; x < n-2; ++x) {
                if (board[y][x] == 2){
                    cout << ">";
                } else if (board[y][x] == 3) {
                    cout << "-";
                } else if (board[y][x] == 4) {
                    cout << "@";
                } else {
                    cout << " ";
                }
            }
            cPos.Y++;
        }
        update_score();
    }

    void next_frame(int frame){
        if (_kbhit()){
            key = _getch();
        }
        if (frame % shoot_framerate == 0){
            update_shoots();
        }
        if (frame % meteorite_framerate == 0){
            update_meteorites();
        }
        if (frame % move_framerate == 0 && is_playing){
            update_move();
        }
    }

private:
    void update_score(){
        cPos.X = n/2-4;
        cPos.Y = m+3;
        SetConsoleCursorPosition(hConsole, cPos);
        cout << "Score = " << score*100 << endl;
        cPos.Y++;
        cPos.X -= 2;
        SetConsoleCursorPosition(hConsole, cPos);
        cout << "Max Score = " << max(max_score, score) * 100 << endl;
    }

    void update_shoots(){
        for (int y = 1; y < m-1; ++y) {
            for (int x = n-2; x > 1; --x) {
                if (board[y][x] == 3){
                    board[y][x] = 0;
                    if (board[y][x+1] == 4){
                        board[y][x+1] = 0;
                        score++;
                    } else {
                        board[y][x+1] = 3;
                    }
                }
            }
        }
    }

    void update_meteorites(){
        for (int y = 1; y < m-1; ++y) {
            for (int x = 3; x < n-1; ++x) {
                if (board[y][x] == 4){
                    if (x == 3){
                        game_over(y, x);
                        return;
                    }
                    board[y][x] = 0;
                    if (board[y][x-1] == 3){
                        board[y][x-1] = 0;
                        score++;
                    } else {
                        board[y][x-1] = 4;
                    }
                }
            }
        }
        gen_meteorites();
    }

    void gen_meteorites(){
        current_meteorites_count = rand()%(max_meteorites_count+1);
        for (int i = 0; i < current_meteorites_count; ++i) {
            if (rand()%100 < meteorite_chance){
                do {
                    current_meteorite = rand()%(m-1)+1;
                } while(board[current_meteorite][n-3] == 4);
                board[current_meteorite][n-3] = 4;
            }
        }
    }

    void update_move(){
        switch (key) {
            case 'w':
                if (shipPos.Y > 1){
                    if (board[shipPos.Y-1][shipPos.X] == 4){
                        game_over(shipPos.Y-1, shipPos.X);
                    } else {
                        board[shipPos.Y][shipPos.X] = 0;
                        shipPos.Y--;
                        board[shipPos.Y][shipPos.X] = 2;
                    }
                }
                break;
            case 's':
                if (shipPos.Y < m-2){
                    if (board[shipPos.Y+1][shipPos.X] == 4){
                        game_over(shipPos.Y+1, shipPos.X);
                    } else {
                        board[shipPos.Y][shipPos.X] = 0;
                        shipPos.Y++;
                        board[shipPos.Y][shipPos.X] = 2;
                    }
                }
                break;
            case 'u':
                board[shipPos.Y][shipPos.X+1] = 3;
                break;
        }

        key = '0';
    }

    void game_over(int m_y, int m_x){
        is_playing = false;

        cPos.X = 0;
        cPos.Y = 0;
        SetConsoleCursorPosition(hConsole, cPos);
        for (int y = 0; y < m; ++y) {
            for (int x = 0; x < n; ++x) {
                if (y == 0 || y == m - 1 || x <= 1 || x >= n - 2){
                    SetConsoleTextAttribute(hConsole, id_gray_color);
                    cout << "#";
                    SetConsoleTextAttribute(hConsole, id_default_color);
                }
                else{
                    cout << " ";
                }
            }
            cout << endl;
        }
        cPos.X = m_x-1;
        cPos.Y = m_y;
        SetConsoleCursorPosition(hConsole, cPos);
        SetConsoleTextAttribute(hConsole, id_red_color);
        cout << "@@";
        SetConsoleTextAttribute(hConsole, id_default_color);
        update_score();
    }
};

int main() {
    srand(time(NULL));

    int framerate_milliseconds = 50;
    long max_score = 0;

    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO structCursorInfo;
    GetConsoleCursorInfo(hConsole,&structCursorInfo);
    structCursorInfo.bVisible = FALSE;
    SetConsoleCursorInfo( hConsole, &structCursorInfo);

    string choose;
    cout << "For exit out the game write 'exit': ";
    cin >> choose;
    while (choose != "exit"){
        system("cls");
        Game game = Game(4, 2, 7, 3, 30, max_score, hConsole);
        game.start();
        int frame = 1;
        while (true){
            Sleep(framerate_milliseconds);
            game.next_frame(frame);
            if (!game.is_playing)
                break;
            game.draw();
            frame++;
            frame %= 2000;
        }
        if (game.score > max_score)
            max_score = game.score;
        cout << endl << "For exit out the game write 'exit': ";
        cin >> choose;
    }

    cout << endl;

    system("pause");
    return 0;
}

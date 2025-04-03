#include <iostream>
#include <conio.h>
#include <windows.h>
#include <cstdlib>
#include <ctime>
#include <string>
using namespace std;
int WIDTH = 40;
int HEIGHT = 20;
enum Direction { STOP = 0, UP, DOWN, LEFT, RIGHT };
enum Difficulty { EASY, MEDIUM, HARD };
int highScoreEasy = 0;
int highScoreMedium = 0;
int highScoreHard = 0;

struct Node {
    int x, y;
    Node* next;
    Node(int _x, int _y) : x(_x), y(_y), next(nullptr) {}
};
class Snake {
private:
    int x, y;
    Node* tail;
    int tailLength;
    Direction dir;
public:
    Snake() {
        x = WIDTH / 2;
        y = HEIGHT / 2;
        tail = nullptr;
        tailLength = 0;
        dir = STOP;
    }
    ~Snake() {
        while(tail) {
            Node* temp = tail;
            tail = tail->next;
            delete temp;
        }
    }
    void reset() {
        x = WIDTH / 2;
        y = HEIGHT / 2;
        while(tail) {
            Node* temp = tail;
            tail = tail->next;
            delete temp;
        }
        tail = nullptr;
        tailLength = 0;
        dir = STOP;
    }
    void Directionchange(Direction newDir) {
        if ((dir == UP && newDir == DOWN) || (dir == DOWN && newDir == UP) || (dir == LEFT && newDir == RIGHT) || (dir == RIGHT && newDir == LEFT))
            return;
        dir = newDir;
    }
    void movecase() {
        int oldX = x, oldY = y;
        switch(dir) {
            case UP:    y--; break;
            case DOWN:  y++; break;
            case LEFT:  x--; break;
            case RIGHT: x++; break;
            default: break;
        }
        Node* newNode = new Node(oldX, oldY);
        newNode->next = tail;
        tail = newNode;
        tailLength++;
    }
    void LastTailSegmentremove() {
        //if (!tail) return;
        if (!tail->next) {
            delete tail;
            tail = nullptr;
            tailLength = 0;
            return;
        }
        Node* current = tail;
        while(current->next->next)
            current = current->next;
        delete current->next;
        current->next = nullptr;
        tailLength--;
    }
    bool Selfcollides() {
        for (Node* current = tail; current; current = current->next) {
            if (current->x == x && current->y == y)
                return true;
        }
        return false;
    }
    
    bool collidesWall() {
        return (x < 0 || x >= WIDTH || y < 0 || y >= HEIGHT);
    }
    bool Occupies(int posX, int posY) {
        if (x == posX && y == posY)
            return true;
        Node* current = tail;
        while(current) {
            if(current->x == posX && current->y == posY)
                return true;
            current = current->next;
        }
        return false;
    }
    int getX() { return x; }
    int getY() { return y; }
    int getTailLength() { return tailLength; }
    int getTailX(int index) {
        Node* current = tail;
        while(index && current) {
            current = current->next;
            index--;
        }
        if(current)
            return current->x;
        return -1;
    }
    int getTailY(int index) {
        Node* current = tail;
        while(index && current) {
            current = current->next;
            index--;
        }
        if(current)
            return current->y;
        return -1;
    }
};

class Food {
private:
    int x, y;
public:
    Food() { x = 0; y = 0; }
    void spawn(Snake &snake) {
        do {
            x = rand() % WIDTH;
            y = rand() % HEIGHT;
        } while(snake.Occupies(x, y));
    }
    int getX() { return x; }
    int getY() { return y; }
};
class GameLogic {
private:
    Snake snake;
    Food food;
    int score;
    bool gameOver;
    Difficulty diff;
    HANDLE console;
    void setCursorPosition(int posX, int posY) {
        COORD coord;
        coord.X = posX;
        coord.Y = posY;
        SetConsoleCursorPosition(console, coord);
    }
    void updateHighScore() {
        if (diff == EASY && score > highScoreEasy)
            highScoreEasy = score;
        else if (diff == MEDIUM && score > highScoreMedium)
            highScoreMedium = score;
        else if (diff == HARD && score > highScoreHard)
            highScoreHard = score;
    }
public:
    GameLogic(Difficulty d) : diff(d) {
        console = GetStdHandle(STD_OUTPUT_HANDLE);
        score = 0;
        gameOver = false;
        snake.reset();
        food.spawn(snake);
    }
    void draw() {
        string output;
        for (int i = 0; i < WIDTH + 2; i++)
            output += "||";
        output += "\n";
        for (int i = 0; i < HEIGHT; i++) {
            for (int j = 0; j < WIDTH; j++) {
                if (j == 0)
                    output += "||";
                if (i == snake.getY() && j == snake.getX())
                    output += (gameOver ? "X" : "@");
                else if (i == food.getY() && j == food.getX())
                    output += "F";
                else {
                    bool printed = false;
                    if(snake.Occupies(j,i)){
                        output += gameOver ? "x" : "*";
                        printed = true;
                    }
                    if (!printed)
                        output += " ";
                }
                if (j == WIDTH - 1)
                    output += "||";
            }
            output += "\n";
        }
        for (int i = 0; i < WIDTH + 2; i++)
            output += "||";
        output += "\n";
        output += "  Score: " + to_string(score) + "    |    High Score: ";
        if (diff == EASY)
            output += to_string(highScoreEasy);
        else if (diff == MEDIUM)
            output += to_string(highScoreMedium);
        else
            output += to_string(highScoreHard);
        output += "\n";
        setCursorPosition(0, 0);
        cout << output;
    }
    void inputletter() {
        if (_kbhit()) {
            char ch = _getch();
            switch (ch) {
                case 'w': 
                case 'W':
                    snake.Directionchange(UP); break;
                case 's':
                case 'S':
                    snake.Directionchange(DOWN); break;
                case 'a':
                case 'A': 
                    snake.Directionchange(LEFT); break;
                case 'd':
                case 'D':
                    snake.Directionchange(RIGHT); break;
                case 'x': 
                case 'X':
                    gameOver = true; break;
            }
        }
    }
    void update() {
        snake.movecase();
        if (snake.getX() == food.getX() && snake.getY() == food.getY()) {
            score += 10;
            food.spawn(snake);
        } else {
            snake.LastTailSegmentremove();
        }
        if (snake.collidesWall() || snake.Selfcollides()) {
            gameOver = true;
            return;
        }
        updateHighScore();
    }
    bool isGameOver() {
        return gameOver;
    }
    void delay(int ms) {
        Sleep(ms);
    }
    int getScore() {
        return score;
    }
};
int main() {
    srand(time(0));
    string userName;
    system("cls");
    cout << "Welcome to the Snake Game!\n";
    cout << "Please enter your name: ";
    getline(cin, userName);
    if (userName.empty()) {
        userName = "Player";
    }
    bool exitGame = false;
    while (!exitGame) {
        system("cls");
        cout << "==============================================\n";
        cout << "         WELCOME " << userName << " TO SNAKE GAME!\n";
        cout << "==============================================\n";
        cout << "Select Difficulty Level:\n";
        cout << "  a) Easy    \n";
        cout << "  b) Medium  \n";
        cout << "  c) Hard    \n";
        cout << "  x) Exit\n";
        cout << "==============================================\n";
        cout << "Enter your choice: ";
        char diffInput;
        cin >> diffInput;
        Difficulty currentDiff;
        int delayTime = 200;
        if (diffInput == 'a' || diffInput == 'A') {
            currentDiff = EASY;
            delayTime = 200;
        } else if (diffInput == 'b' || diffInput == 'B') {
            currentDiff = MEDIUM;
            delayTime = 120;
        } else if (diffInput == 'c' || diffInput == 'C') {
            currentDiff = HARD;
            delayTime = 80;
        } else if (diffInput == 'x' || diffInput == 'X') {
            break;
        } else {
            cout << "Invalid choice. Please try again.\n";
            Sleep(800);
            continue;
        }
        bool backToMenu = false;
        while (!backToMenu && !exitGame) {
            GameLogic game(currentDiff);
            system("cls");
            cout << "\nHello " << userName << ", get ready to play!\n";
            cout << "Press any key to start the game...";
            _getch();
            while (!game.isGameOver()) {
                game.draw();
                game.inputletter();
                game.update();
                game.delay(delayTime);
            }
            game.draw();
            cout << "===========================================================\n";
            cout<<  "                      player status                         \n";
            cout << "              GAME OVER, " << userName << "!\n";
            cout << "              Final Score: " << game.getScore() << "\n";
            cout << "=======================================================\n";
            cout << "Press 'p' to play again, 'm' for Main Menu, or 'x' to Exit: ";
            char postChoice;
            cin >> postChoice;
            if (postChoice == 'p' || postChoice == 'P') {
                continue;
            } else if (postChoice == 'x' || postChoice == 'X') {
                exitGame = true;
                break;
            } else {
                backToMenu = true;
            }
        }
    }
}
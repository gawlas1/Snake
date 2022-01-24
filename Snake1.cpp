#include <iostream>
#include <windows.h> //sleeping and random number generator
#include <vector>
#include <conio.h> //keyboard input
#include <string>
#include <ctime> //calculate game time

using namespace std;

char checkKey(char key); //check if wsad or arrows keys pressed, remember last pressed
void startGame(); //creates apples and starts ticks
void drawMap(); //create map, add snake and apples to it
void moveSnake(char key); //move snake depending on last key pressed
void tick(); //ticks of game
void addApple(); //add apple to game
int checkApples(int x, int y); //check if apples is in given coords and return index of apple if found (else return -1)
void endGame(); //ticks of game are no more, print win/lose stats
void showMenu(); //shoe menu which options to choose
int checkNumber(string value); //return int from string (if letters in string return -1)
void writeOnCoords(int x, int y, char letter); //change printed text on givven coords (letters)
void writeOnCoords(int x, int y, int num); //change printed text on givven coords (numbers)

int mapWidth = 7; //game map width
int mapHeight = 5; //game map height
int moveInterval = 800; //time between snake moves in ms
int applesNumber = 4; //max number of apples in the game
int whichApple = -1; //which apple was eaten (helps with removing apple from apples vector)
int applesEaten = 0; //how many apples eaten
int ticks = 0; //how many ticks in current game
int timeStart = 0; //time when game started
char key = '0'; //last key pressed, will change into w s a or d
bool gameOver = false; //is game over, if true tick funciton will stop
bool collision = false; //did snake head collided with it's tail
bool gameStarted = false; //with first move change to true
vector<vector<char>> map; //2D vector with map info
vector<vector<int>> snakeBody{ {mapWidth / 2,mapHeight / 2} }; //2D vector with snake segments coords
vector<vector<int>> apples; //2D vector with apples coords
vector<int> snakeBottom{ snakeBody[0][0], snakeBody[0][1] }; //vector with last segment of snake (helps with moving snake)



int main()
{
    setlocale(LC_CTYPE, "Polish"); //polish letters
    showMenu();
    return 0;
}

char checkKey(char key) {
    char newKey = key;
    if (_kbhit())
    {
        //get last wsad or arrow key pressed, can't change direction into oposite if tail exists
        while (_kbhit() == 1) {
            switch (_getch()) {
            case 'w':
            case 72:
                if (key != 's' || applesEaten == 0) { 
                    newKey = 'w'; if (!gameStarted) { gameStarted = 1; timeStart = time(NULL); } 
                }; break;
            case 'a':
            case 75:
                if (key != 'd' || applesEaten == 0) {
                    newKey = 'a'; if (!gameStarted) { gameStarted = 1; timeStart = time(NULL); }
                }; break;
            case 's':
            case 80:
                if (key != 'w' || applesEaten == 0) {
                    newKey = 's'; if (!gameStarted) { gameStarted = 1; timeStart = time(NULL); }
                }; break;
            case 'd':
            case 77:
                if (key != 'a' || applesEaten == 0) {
                    newKey = 'd'; if (!gameStarted) { gameStarted = 1; timeStart = time(NULL); }
                }; break;
            }
        }
    }
    return newKey;
}

void startGame() {
    //generate map vector depending on map size
    for (int i = 0; i < mapHeight; i++) {
        map.push_back({});

        for (int j = 0; j < mapWidth; j++) {
            map[i].push_back('_');
        }
    }
    map[snakeBody[0][1]][snakeBody[0][0]] = 'O';

    //create apples on random free places
    srand(time(NULL));
    int x, y;
    for (int i = 0; i < applesNumber; i++) {
        x = rand() % mapWidth;
        y = rand() % mapHeight;
        while (map[y][x] != '_') {
            x = rand() % mapWidth;
            y = rand() % mapHeight;
        }
        apples.push_back({ y,x });
        map[y][x] = 'a';
    }



    //print map
    for (int j = 0; j < mapWidth; j++) {
        cout << map[0][j] << " ";
    }
    //print apples eaten counter
    cout << "        " << applesEaten << "/" << mapWidth * mapHeight - 1 << endl;
    for (int i = 1; i < mapHeight; i++) {
        for (int j = 0; j < mapWidth; j++) {
            cout << map[i][j] << " ";
        }
        cout << endl;
    }
    tick();
}

void drawMap() {

    //create empty map
    for (int i = 0; i < mapHeight; i++) {
        for (int j = 0; j < mapWidth; j++) {
            map[i][j] = '_';
        }
    }

    //draw and add to map snake 
    map[snakeBody[0][1]][snakeBody[0][0]] = 'O';
    writeOnCoords(snakeBody[0][0], snakeBody[0][1], 'O');

    for (int i = 1; i < snakeBody.size(); i++) {
        map[snakeBody[i][0]][snakeBody[i][1]] = 'o';
    }
    if (applesEaten != 0) {
        writeOnCoords(snakeBody[1][1], snakeBody[1][0], 'o');
    }

    //add apples to map 
    for (int i = 0; i < applesNumber; i++) {
        map[apples[i][0]][apples[i][1]] = 'a';
    }
}

void moveSnake(char key) {
    //disapear last segment
    if (applesEaten > 0) {
        writeOnCoords(snakeBody[snakeBody.size() - 1][1], snakeBody[snakeBody.size() - 1][0], '_');
    }

    //every tail segment is next segment
    for (int i = snakeBody.size() - 1; i > 0; i--) {
        if (i == 1) {
            snakeBody[i][0] = snakeBody[i - 1][1];
            snakeBody[i][1] = snakeBody[i - 1][0];
        }
        else {
            snakeBody[i][0] = snakeBody[i - 1][0];
            snakeBody[i][1] = snakeBody[i - 1][1];
        }
    }

    //if no tail hide place where head was
    if (applesEaten == 0) {
        writeOnCoords(snakeBody[0][0], snakeBody[0][1], '_');
    }

    //checking last key pressed and moving head according to that
    switch (key) {
    case 'w':
        snakeBody[0][1]--;
        if (snakeBody[0][1] < 0) {
            snakeBody[0][1] = mapHeight - 1;
        }
        break;
    case 's':
        snakeBody[0][1]++;
        if (snakeBody[0][1] >= mapHeight) {
            snakeBody[0][1] = 0;
        }
        break;
    case 'a':
        snakeBody[0][0]--;
        if (snakeBody[0][0] < 0) {
            snakeBody[0][0] = mapWidth - 1;
        }
        break;
    case 'd':
        snakeBody[0][0]++;
        if (snakeBody[0][0] >= mapWidth) {
            snakeBody[0][0] = 0;
        }
        break;
    }

}

void tick() {
    while (gameOver == false) {
        ticks++;
        key = checkKey(key);
        snakeBottom[0] = snakeBody[applesEaten][0];
        snakeBottom[1] = snakeBody[applesEaten][1];

        //check head collision with tail
        if (map[snakeBody[0][1]][snakeBody[0][0]] == 'o') {
            gameOver = true;
            break;
        }
        moveSnake(key);

        //check if apple eaten and if enough place add next apple
        whichApple = checkApples(snakeBody[0][0], snakeBody[0][1]);
        if (whichApple != -1) {
            applesEaten++;
            writeOnCoords(mapWidth + 4, 0, applesEaten);
            addApple();
            if (applesEaten == 1) {
                snakeBody.push_back({ snakeBottom[1], snakeBottom[0] });
                writeOnCoords(snakeBottom[0], snakeBottom[1], 'o');
            }
            else {
                snakeBody.push_back({ snakeBottom[0], snakeBottom[1] });
                writeOnCoords(snakeBottom[1], snakeBottom[0], 'o');
            }
        }

        drawMap();

        //wait before next tick
        Sleep(moveInterval);
    }

    //if collision end game
    endGame();
}

int checkApples(int y, int x) {
    for (int i = 0; i < applesNumber; i++) {
        if (x == apples[i][0] && y == apples[i][1]) {
            return i;
        }
    }
    return -1;
}

void addApple() {
    //count free spaces
    int freeSpaces = 0;
    for (int i = 0; i < mapHeight; i++) {
        for (int j = 0; j < mapWidth; j++) {
            if (map[i][j] == '_') {
                freeSpaces++;
            }
        }
    }
    //choose random free space and append apple there
    if (freeSpaces > 0) {
        int a = rand() % freeSpaces;
        int whichFreeSpace = -1;
        for (int i = 0; i < mapHeight; i++) {
            for (int j = 0; j < mapWidth; j++) {
                if (map[i][j] == '_') {
                    whichFreeSpace++;
                    if (a == whichFreeSpace) {
                        apples[whichApple][0] = i;
                        apples[whichApple][1] = j;
                        writeOnCoords(j, i, 'a');
                    }
                }
            }
        }
    }
    else {
        //if no more place for apples, less apples in game
        vector<vector<int>>::iterator it = apples.begin();
        apples.erase(it + whichApple);
        applesNumber--;
        if (applesNumber == 0) {
            //if 0 apples in game end
            gameOver = true;
        }
    }
}

void endGame() {
    system("cls");
    //if 0 apples left won, else lose
    if (applesEaten == mapWidth * mapHeight - 1) {
        cout << "Wygrałes :D" << endl;
    }
    else {
        cout << "Przegrałes :c" << endl;
    }

    int seconds = time(NULL) - timeStart; //lenght of game in seconds

    //diffrent grammar for diffrent amount of eaten apples
    if (applesEaten == 1) {
        cout << "Zjadłes " << applesEaten << " jabłko" << endl;
    }
    else if (applesEaten == 2 || applesEaten == 3 || applesEaten == 4) {
        cout << "Zjadłes " << applesEaten << " jabłka" << endl;
    }
    else {
        cout << "Zjadłes " << applesEaten << " jabłek" << endl;
    }

    //diffrent grammar for diffrent amount of seconds
    if (seconds == 1) {
        cout << "Rozgrywaka trwała " << seconds << " sekundę (" << ticks << " ticków)" << endl << endl;
    }
    else if (seconds == 2 || seconds == 3 || seconds == 4) {
        cout << "Rozgrywaka trwała " << seconds << " sekundy (" << ticks << " ticków)" << endl << endl;
    }
    else {
        cout << "Rozgrywaka trwała " << seconds << " sekund (" << ticks << " ticków)" << endl << endl;
    }

    //change game values to base values
    mapWidth = 7;
    mapHeight = 5;
    moveInterval = 800;
    applesNumber = 4;
    whichApple = -1;
    applesEaten = 0;
    ticks = 0;
    key = '0';
    timeStart = 0;
    gameOver = false;
    gameStarted = 0;
    map.clear();
    snakeBody.clear();
    snakeBody = { {mapWidth / 2,mapHeight / 2} };
    apples.clear();
    snakeBottom.clear();
    snakeBottom = { snakeBody[0][0], snakeBody[0][1] };

    Sleep(500);
    cout << "Wpisz cokolwiek, aby wrocic do menu glownego" << endl;
    system("pause > nul");
    showMenu();
}

void showMenu() {
    system("cls");
    cout << "  SSSS                       kk" << endl;
    cout << " SS   SS                     kk" << endl;
    cout << " SS   SS                     kk" << endl;
    cout << " SS        nnnnn     aaaa    kk  kk    eeee" << endl;
    cout << "  SSS      nn  nn   aa  aa   kk kk    ee  ee" << endl;
    cout << "    SSS    nn  nn     aaaa   kkkk     eeeeee" << endl;
    cout << "      SS   nn  nn    aa aa   kkk      ee" << endl;
    cout << " SS   SS   nn  nn   aa  aa   kkkk     ee" << endl;
    cout << " SS   SS   nn  nn   aa  aa   kk kk    ee  ee" << endl;
    cout << "  SSSSS    nn  nn    aaaaa   kk  kk    eeee" << endl << endl << endl;    cout << "Witaj w grze Snake!" << endl;
    cout << "Wybierz co chcesz zrobić wpisując daną cyfrę:" << endl;
    cout << "1) Graj" << endl;
    cout << "2) Tryb niestandardowy" << endl;
    cout << "3) Instrukcja" << endl;
    cout << "4) O autorze" << endl;
    cout << "5) Wyjście" << endl;


    //helpletter string is transferd to letter int, they are used to saftly cin
    string helpa;
    int a;
    string helpb;
    int b;
    string helpc;
    int c;
    cin >> helpa;
    a = checkNumber(helpa);
    while (a < 1 || a > 5) {
        system("cls");
        cout << "  SSSS                       kk" << endl;
        cout << " SS   SS                     kk" << endl;
        cout << " SS   SS                     kk" << endl;
        cout << " SS        nnnnn     aaaa    kk  kk    eeee" << endl;
        cout << "  SSS      nn  nn   aa  aa   kk kk    ee  ee" << endl;
        cout << "    SSS    nn  nn     aaaa   kkkk     eeeeee" << endl;
        cout << "      SS   nn  nn    aa aa   kkk      ee" << endl;
        cout << " SS   SS   nn  nn   aa  aa   kkkk     ee" << endl;
        cout << " SS   SS   nn  nn   aa  aa   kk kk    ee  ee" << endl;
        cout << "  SSSSS    nn  nn    aaaaa   kk  kk    eeee" << endl << endl << endl;
        cout << "Wybrano niepoprawnie, wpisz cyfre od 1 do 5" << endl;
        cout << "1) Graj" << endl;
        cout << "2) Tryb niestandardowy" << endl;
        cout << "3) Instrukcja" << endl;
        cout << "4) O autorze" << endl;
        cout << "5) Wyjście" << endl;
        cin >> helpa;
        a = checkNumber(helpa);
    }
    system("cls");
    cout << "  SSSS                       kk" << endl;
    cout << " SS   SS                     kk" << endl;
    cout << " SS   SS                     kk" << endl;
    cout << " SS        nnnnn     aaaa    kk  kk    eeee" << endl;
    cout << "  SSS      nn  nn   aa  aa   kk kk    ee  ee" << endl;
    cout << "    SSS    nn  nn     aaaa   kkkk     eeeeee" << endl;
    cout << "      SS   nn  nn    aa aa   kkk      ee" << endl;
    cout << " SS   SS   nn  nn   aa  aa   kkkk     ee" << endl;
    cout << " SS   SS   nn  nn   aa  aa   kk kk    ee  ee" << endl;
    cout << "  SSSSS    nn  nn    aaaaa   kk  kk    eeee" << endl << endl << endl;
    switch (a) {
    case 1:
        //game difficulty
        cout << "Wybierz poziom trudności: " << endl;
        cout << "1) Łatwy" << endl;
        cout << "2) Średni" << endl;
        cout << "3) Trudny" << endl;
        cout << "4) Bardzo trudny" << endl;
        cin >> helpb;
        b = checkNumber(helpb);
        while (b < 1 || b > 4) {
            system("cls");
            cout << "  SSSS                       kk" << endl;
            cout << " SS   SS                     kk" << endl;
            cout << " SS   SS                     kk" << endl;
            cout << " SS        nnnnn     aaaa    kk  kk    eeee" << endl;
            cout << "  SSS      nn  nn   aa  aa   kk kk    ee  ee" << endl;
            cout << "    SSS    nn  nn     aaaa   kkkk     eeeeee" << endl;
            cout << "      SS   nn  nn    aa aa   kkk      ee" << endl;
            cout << " SS   SS   nn  nn   aa  aa   kkkk     ee" << endl;
            cout << " SS   SS   nn  nn   aa  aa   kk kk    ee  ee" << endl;
            cout << "  SSSSS    nn  nn    aaaaa   kk  kk    eeee" << endl << endl << endl;
            cout << "Wybrano niepoprawnie, wpisz cyfre od 1 do 4" << endl;
            cout << "1) Łatwy" << endl;
            cout << "2) Średni" << endl;
            cout << "3) Trudny" << endl;
            cout << "4) Bardzo trudny" << endl;
            cin >> helpb;
            b = checkNumber(helpb);
        }
        system("cls");
        switch (b) {
        case 1:
            //easy
            mapWidth = 5;
            mapHeight = 4;
            moveInterval = 1200;
            applesNumber = 3;
            snakeBody.clear();
            snakeBody = { {mapWidth / 2,mapHeight / 2} };
            snakeBottom.clear();
            snakeBottom = { snakeBody[0][0], snakeBody[0][1] };
            startGame();
            break;
        case 2:
            //normal
            mapWidth = 7;
            mapHeight = 5;
            moveInterval = 500;
            applesNumber = 4;
            snakeBody.clear();
            snakeBody = { {mapWidth / 2,mapHeight / 2} };
            snakeBottom.clear();
            snakeBottom = { snakeBody[0][0], snakeBody[0][1] };
            startGame();
            break;
        case 3:
            //hard
            mapWidth = 9;
            mapHeight = 7;
            moveInterval = 300;
            applesNumber = 6;
            snakeBody.clear();
            snakeBody = { {mapWidth / 2,mapHeight / 2} };
            snakeBottom.clear();
            snakeBottom = { snakeBody[0][0], snakeBody[0][1] };
            startGame();
            break;
        case 4:
            //very hard
            mapWidth = 13;
            mapHeight = 10;
            moveInterval = 125;
            applesNumber = 15;
            snakeBody.clear();
            snakeBody = { {mapWidth / 2,mapHeight / 2} };
            snakeBottom.clear();
            snakeBottom = { snakeBody[0][0], snakeBody[0][1] };
            startGame();
            break;
        }
        break;
    case 2:
        //custom mode
        // 
        //cin map width
        cout << "Podaj szerokość planszy (od 2 do 30): " << endl;
        cin >> helpc;
        c = checkNumber(helpc);
        while (c < 2 || c > 30) {
            system("cls");
            cout << "  SSSS                       kk" << endl;
            cout << " SS   SS                     kk" << endl;
            cout << " SS   SS                     kk" << endl;
            cout << " SS        nnnnn     aaaa    kk  kk    eeee" << endl;
            cout << "  SSS      nn  nn   aa  aa   kk kk    ee  ee" << endl;
            cout << "    SSS    nn  nn     aaaa   kkkk     eeeeee" << endl;
            cout << "      SS   nn  nn    aa aa   kkk      ee" << endl;
            cout << " SS   SS   nn  nn   aa  aa   kkkk     ee" << endl;
            cout << " SS   SS   nn  nn   aa  aa   kk kk    ee  ee" << endl;
            cout << "  SSSSS    nn  nn    aaaaa   kk  kk    eeee" << endl << endl << endl;
            cout << "Podano niepoprawny rozmiar" << endl;
            cout << "Podaj szerokość planszy (od 2 do 30): " << endl;
            cin >> helpc;
            c = checkNumber(helpc);
        }
        mapWidth = c;
        system("cls");
        cout << "  SSSS                       kk" << endl;
        cout << " SS   SS                     kk" << endl;
        cout << " SS   SS                     kk" << endl;
        cout << " SS        nnnnn     aaaa    kk  kk    eeee" << endl;
        cout << "  SSS      nn  nn   aa  aa   kk kk    ee  ee" << endl;
        cout << "    SSS    nn  nn     aaaa   kkkk     eeeeee" << endl;
        cout << "      SS   nn  nn    aa aa   kkk      ee" << endl;
        cout << " SS   SS   nn  nn   aa  aa   kkkk     ee" << endl;
        cout << " SS   SS   nn  nn   aa  aa   kk kk    ee  ee" << endl;
        cout << "  SSSSS    nn  nn    aaaaa   kk  kk    eeee" << endl << endl << endl;
        //cin map height
        cout << "Podaj wysokość planszy (od 2 do 30): " << endl;
        cin >> helpc;
        c = checkNumber(helpc);
        while (c < 2 || c > 30) {
            system("cls");
            cout << "  SSSS                       kk" << endl;
            cout << " SS   SS                     kk" << endl;
            cout << " SS   SS                     kk" << endl;
            cout << " SS        nnnnn     aaaa    kk  kk    eeee" << endl;
            cout << "  SSS      nn  nn   aa  aa   kk kk    ee  ee" << endl;
            cout << "    SSS    nn  nn     aaaa   kkkk     eeeeee" << endl;
            cout << "      SS   nn  nn    aa aa   kkk      ee" << endl;
            cout << " SS   SS   nn  nn   aa  aa   kkkk     ee" << endl;
            cout << " SS   SS   nn  nn   aa  aa   kk kk    ee  ee" << endl;
            cout << "  SSSSS    nn  nn    aaaaa   kk  kk    eeee" << endl << endl << endl;
            cout << "Podano niepoprawny rozmiar" << endl;
            cout << "Podaj wysokość planszy (od 2 do 30): " << endl;
            cin >> helpc;
            c = checkNumber(helpc);
        }
        mapHeight = c;
        system("cls");
        cout << "  SSSS                       kk" << endl;
        cout << " SS   SS                     kk" << endl;
        cout << " SS   SS                     kk" << endl;
        cout << " SS        nnnnn     aaaa    kk  kk    eeee" << endl;
        cout << "  SSS      nn  nn   aa  aa   kk kk    ee  ee" << endl;
        cout << "    SSS    nn  nn     aaaa   kkkk     eeeeee" << endl;
        cout << "      SS   nn  nn    aa aa   kkk      ee" << endl;
        cout << " SS   SS   nn  nn   aa  aa   kkkk     ee" << endl;
        cout << " SS   SS   nn  nn   aa  aa   kk kk    ee  ee" << endl;
        cout << "  SSSSS    nn  nn    aaaaa   kk  kk    eeee" << endl << endl << endl;
        //cin move interval
        cout << "Podaj częstotliwość ruchu węża w milisekundach (od 10 do 1500): " << endl;
        cin >> helpc;
        c = checkNumber(helpc);
        while (c < 10 || c > 1500) {
            system("cls");
            cout << "  SSSS                       kk" << endl;
            cout << " SS   SS                     kk" << endl;
            cout << " SS   SS                     kk" << endl;
            cout << " SS        nnnnn     aaaa    kk  kk    eeee" << endl;
            cout << "  SSS      nn  nn   aa  aa   kk kk    ee  ee" << endl;
            cout << "    SSS    nn  nn     aaaa   kkkk     eeeeee" << endl;
            cout << "      SS   nn  nn    aa aa   kkk      ee" << endl;
            cout << " SS   SS   nn  nn   aa  aa   kkkk     ee" << endl;
            cout << " SS   SS   nn  nn   aa  aa   kk kk    ee  ee" << endl;
            cout << "  SSSSS    nn  nn    aaaaa   kk  kk    eeee" << endl << endl << endl;
            cout << "Podano niepoprawną czestotliwość" << endl;
            cout << "Podaj częstotliwość ruchu węża w milisekundach (od 10 do 1500): " << endl;
            cin >> helpc;
            c = checkNumber(helpc);
        }
        moveInterval = c;
        system("cls");
        cout << "  SSSS                       kk" << endl;
        cout << " SS   SS                     kk" << endl;
        cout << " SS   SS                     kk" << endl;
        cout << " SS        nnnnn     aaaa    kk  kk    eeee" << endl;
        cout << "  SSS      nn  nn   aa  aa   kk kk    ee  ee" << endl;
        cout << "    SSS    nn  nn     aaaa   kkkk     eeeeee" << endl;
        cout << "      SS   nn  nn    aa aa   kkk      ee" << endl;
        cout << " SS   SS   nn  nn   aa  aa   kkkk     ee" << endl;
        cout << " SS   SS   nn  nn   aa  aa   kk kk    ee  ee" << endl;
        cout << "  SSSSS    nn  nn    aaaaa   kk  kk    eeee" << endl << endl << endl;
        //cin apple amount
        cout << "Podaj ilość jabłek na planszy(od 1 do " << (mapWidth * mapHeight) - 1 << "): " << endl;
        cin >> helpc;
        c = checkNumber(helpc);
        while (c < 1 || c > mapWidth * mapHeight - 1) {
            system("cls");
            cout << "  SSSS                       kk" << endl;
            cout << " SS   SS                     kk" << endl;
            cout << " SS   SS                     kk" << endl;
            cout << " SS        nnnnn     aaaa    kk  kk    eeee" << endl;
            cout << "  SSS      nn  nn   aa  aa   kk kk    ee  ee" << endl;
            cout << "    SSS    nn  nn     aaaa   kkkk     eeeeee" << endl;
            cout << "      SS   nn  nn    aa aa   kkk      ee" << endl;
            cout << " SS   SS   nn  nn   aa  aa   kkkk     ee" << endl;
            cout << " SS   SS   nn  nn   aa  aa   kk kk    ee  ee" << endl;
            cout << "  SSSSS    nn  nn    aaaaa   kk  kk    eeee" << endl << endl << endl;
            cout << "Podano niepoprawną czestotliwość" << endl;
            cout << "Podaj ilość jabłek na planszy(od 1 do " << (mapWidth * mapHeight) - 1 << "): " << endl;
            cin >> helpc;
            c = checkNumber(helpc);
        }
        applesNumber = c;

        system("cls");
        //snake starting coords depending on map sizze
        snakeBody.clear();
        snakeBody = { {mapWidth / 2,mapHeight / 2} };
        snakeBottom.clear();
        snakeBottom = { snakeBody[0][0], snakeBody[0][1] };
        startGame();
        break;
    case 3:
        //tutorial
        cout << "Celem Snake'a jest zjedzenie jak największej ilości jabłek." << endl;
        cout << "Aby zjeść jabłko należy tak się poruszać wężem, aby jego głowa znalazła się na jabłku." << endl;
        cout << "Gdy wąż zjada jabłko jego ogon rośnie." << endl;
        cout << "Jeśli wąż głową uderzy w ogon, przegrywasz." << endl << endl;
        cout << "Sterowanie wężem:" << endl;
        cout << "GÓRA - W   DÓŁ - S   LEWO - A   PRAWO - D" << endl << endl;
        cout << "LEGENDA:" << endl;
        cout << "O - głowa węża   o - ogon węża   a-jabłko" << endl << endl;
        cout << "Wpisz cokolwiek, aby wrocic do menu glownego" << endl;
        system("pause > nul");
        showMenu();
        break;

    case 4:
        //creator info
        cout << "Gre stworzyl Filip Gawlas" << endl << endl;
        cout << "Wpisz cokolwiek, aby wrócić do menu głównego" << endl;
        system("pause > nul");
        showMenu();
        break;
    case 5:
        //quit game
        exit(0);
    }
}

int checkNumber(string value) {
    int sum = 0;
    int number = 0;
    int j = 0;
    for (int i = value.length() - 1; i >= 0; i--) {
        //check in char is number
        if (value[i] < 0 + '0' || value[i]>9 + '0') {
            return -1;
        }
        //get int from number char
        number = value[i] - '0';
        //every number will be 10 times bigget than last
        for (int k = 0; k < j; k++) {
            number *= 10;
        }
        sum += number;
        j++;
    }
    return sum;
}


void writeOnCoords(int x, int y, char letter) {

    //set cursor
    COORD c;
    c.X = x * 2;
    c.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), c);

    //write on cursor
    cout << letter;

    //hide cursor
    ::HANDLE hConsoleOut = ::GetStdHandle(STD_OUTPUT_HANDLE);
    ::CONSOLE_CURSOR_INFO hCCI;
    ::GetConsoleCursorInfo(hConsoleOut, &hCCI);
    hCCI.bVisible = FALSE;
    ::SetConsoleCursorInfo(hConsoleOut, &hCCI);
}

void writeOnCoords(int x, int y, int num) {
    //set cursor
    COORD c;
    c.X = x * 2;
    c.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), c);

    //write on cursor
    cout << num << "/" << mapWidth * mapHeight - 1;

    //hide cursor
    ::HANDLE hConsoleOut = ::GetStdHandle(STD_OUTPUT_HANDLE);
    ::CONSOLE_CURSOR_INFO hCCI;
    ::GetConsoleCursorInfo(hConsoleOut, &hCCI);
    hCCI.bVisible = FALSE;
    ::SetConsoleCursorInfo(hConsoleOut, &hCCI);
}
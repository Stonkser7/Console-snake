#include <iostream>
#include <iomanip>
#include <conio.h>
#include <windows.h>
#include <string>
#include <vector>
using namespace std;
enum GameState { GS_PLAY, GS_PAUSE, GS_EXIT, GS_OVER };
enum Key { K_UP, K_DOWN, K_RIGHT, K_LEFT, K_ESC, K_ENTER, K_SPACE, K_NOTHING };
enum SnakeDir { SD_RIGHT, SD_LEFT, SD_UP, SD_DOWN };

typedef void (*MenuAction)();
struct Point {
	int x;
	int y;
};
struct Snake {
	Point body[1000];
	Point truncated_body[1000];
	SnakeDir dir;
	int start_lenght;
	int length;
	int truncated_body_lenght;
};
struct Field {
	int x;
	int y;
	int width;
	int height;
};
typedef GameState State;
typedef int Score;
typedef Point Food;
typedef Point Booster;
struct Game {
	Field field;
	Snake snake;
	State state;
	Score score;
	Food food;
	Booster booster;
	int gameSpeed;
};
struct MenuItem {
	string text;
	MenuAction action;
};
struct Menu {
	string caption;
	vector <MenuItem> items;
	int activeItem;

};
void setTextColor(int textColor) {
	HANDLE hConsoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hConsoleHandle, textColor);
}
void showCursor(bool visible) {
	CONSOLE_CURSOR_INFO curs = { 0 };
	curs.dwSize = sizeof(curs);
	curs.bVisible = visible;
	::SetConsoleCursorInfo(::GetStdHandle(STD_OUTPUT_HANDLE), &curs);
}
void gotoXY(int x, int y) {
	COORD coord;
	coord.X = x;
	coord.Y = y;
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}
void initField(Field& f) {
	f.x = 0;
	f.y = 0;
	f.width = 78;
	f.height = 22;
}
void showField(Field& f) {
	setTextColor(8);
	for (int i = 0; i < f.width + 2; i++) {
		gotoXY(f.x + i, f.y);
		cout << "#";
	}
	for (int i = 1; i < f.height + 1; i++) {	//i = 1 т.к. левую и правую границу нужно нарисовать ниже потолка
		gotoXY(f.x, f.y + i);
		cout << "#" << setw(f.width + 1) << "#";
	}
	for (int i = 0; i < f.width + 2; i++) {
		gotoXY(f.x + i, f.y + f.height + 1);	// + 1 т.к. пол нужно нарисовать ниже левой и правой границ
		cout << "#";
	}
}
void showScore(Game& g) {
	setTextColor(15);
	gotoXY(g.field.x, g.field.y + g.field.height + 2);
	string score = "Score : ";
	cout << score << g.score << "     ";
}
void initSnake(Game& g) {
	g.snake.start_lenght = 4;
	g.snake.truncated_body_lenght = 0;
	g.snake.length = g.snake.start_lenght;
	g.snake.dir = SD_LEFT;
	for (int i = 0; i < g.snake.length; i++) {
		g.snake.body[i].x = (g.field.width / 2 - 1) + i;
		g.snake.body[i].y = (g.field.height / 2 + 1);
	}
}
bool isCoordRight(Game& g, int x, int y) {
	for (int i = 0; i < g.snake.length; i++) {
		if (x == g.snake.body[i].x && y == g.snake.body[i].y) {
			return false;
		}
	}
	return true;
}
void genCoord(Field& f, int& x, int& y) {
	x = rand() % f.width + 1;
	y = rand() % f.height + 1;
}
void genFood(Game& g) {
	do {
		genCoord(g.field, g.food.x, g.food.y);
	} while (!isCoordRight(g, g.food.x, g.food.y));
}
void genBooster(Game& g) {
	do {
		genCoord(g.field, g.booster.x, g.booster.y);
	} while (!isCoordRight(g, g.booster.x, g.booster.y));
}
void showFood(Game& g) {
	setTextColor(2);
	gotoXY(g.food.x + g.field.x, g.food.y + g.field.y);
	cout << "@";
}
void showBooster(Game& g) {
	setTextColor(13);
	gotoXY(g.booster.x + g.field.x, g.booster.y + g.field.y);
	cout << "B";
}
void hideBooster(Game& g) {
	gotoXY(g.booster.x + g.field.x, g.booster.y + g.field.y);
	cout << " ";
}
bool isSnakeAteFood(Game& g) {
	if (g.snake.body[0].x == g.food.x && g.snake.body[0].y == g.food.y) {
		return true;
	}
	return false;
}
bool isSnakeAteBooster(Game& g) {
	if (g.snake.body[0].x == g.booster.x && g.snake.body[0].y == g.booster.y) {
		return true;
	}
	return false;
}
bool isSnakeBoosted(Game& g) {
	if (g.gameSpeed == 50 || g.gameSpeed == 100) {
		return true;
	}
	return false;
}
void showSnake(Game& g) {
	setTextColor(14);
	for (int i = 0; i < g.snake.length; i++) {
		gotoXY(g.snake.body[i].x + g.field.x, g.snake.body[i].y + g.field.y);
		cout << "@";
	}
}
void hideSnake(Game& g) {
	gotoXY(g.snake.body[g.snake.length].x + g.field.x, g.snake.body[g.snake.length].y + g.field.y);
	cout << " ";
}
void moveSnake(Snake& s) {
	for (int i = s.length; i > 0; i--) {
		s.body[i].x = s.body[i - 1].x;
		s.body[i].y = s.body[i - 1].y;
	}
	switch (s.dir) {
	case SD_LEFT:
		s.body[0].x = s.body[0].x - 1;
		break;
	case SD_RIGHT:
		s.body[0].x = s.body[0].x + 1;
		break;
	case SD_UP:
		s.body[0].y = s.body[0].y - 1;
		break;
	case SD_DOWN:
		s.body[0].y = s.body[0].y + 1;
		break;
	}
}
void incSnake(Snake& s) {
	s.length++;
	s.body[s.length + 1].x = s.body[s.length].x;
	s.body[s.length + 1].y = s.body[s.length].y;
}
void truncSnake(Game& g) {
	for (int i = 1; i < g.snake.length; i++) {
		if (g.snake.body[0].x == g.snake.body[i].x && g.snake.body[0].y == g.snake.body[i].y) {
			for (int j = i + 1; j < g.snake.length; j++) {			//j = i + 1 because truncated tail[0] was shown on snake tail when snake gone trought itself
				g.snake.truncated_body[g.snake.truncated_body_lenght].x = g.snake.body[j].x;
				g.snake.truncated_body[g.snake.truncated_body_lenght].y = g.snake.body[j].y;
				gotoXY(g.snake.truncated_body[g.snake.truncated_body_lenght].x + g.field.x, g.snake.truncated_body[g.snake.truncated_body_lenght].y + g.field.y);
				setTextColor(4);
				cout << "@";
				g.snake.truncated_body_lenght++;
			}
			g.snake.length = i;
			if (g.snake.length < g.snake.start_lenght) {
				g.score = 0;
			}
			else {
				g.score = g.snake.length - g.snake.start_lenght;
			}
			return;
		}
	}
}
void hideTruncatedBody(Game& g) {
	for (int i = 0; i < g.snake.truncated_body_lenght; i++) {
		gotoXY(g.snake.truncated_body[i].x + g.field.x, g.snake.truncated_body[i].y + g.field.y);
		cout << " ";
	}
	g.snake.truncated_body_lenght = 0;
}
void turnSnakeUp(Game& g) {
	if (g.snake.dir != SD_DOWN && g.snake.dir != SD_UP) {
		if (isSnakeBoosted(g)) {
			g.gameSpeed = 100;
		}
		else {
			g.gameSpeed *= 2;
		}
		g.snake.dir = SD_UP;
	}
}
void turnSnakeDown(Game& g) {
	if (g.snake.dir != SD_UP && g.snake.dir != SD_DOWN) {
		if (isSnakeBoosted(g)) {
			g.gameSpeed = 100;
		}
		else {
			g.gameSpeed *= 2;
		}
		g.snake.dir = SD_DOWN;
	}
}
void turnSnakeLeft(Game& g) {
	if (g.snake.dir != SD_RIGHT && g.snake.dir != SD_LEFT) {
		if (isSnakeBoosted(g)) {
			g.gameSpeed = 50;
		}
		else {
			g.gameSpeed /= 2;
		}
		g.snake.dir = SD_LEFT;
	}
}
void turnSnakeRight(Game& g) {
	if (g.snake.dir != SD_LEFT && g.snake.dir != SD_RIGHT) {
		if (isSnakeBoosted(g)) {
			g.gameSpeed = 50;
		}
		else {
			g.gameSpeed /= 2;
		}
		g.snake.dir = SD_RIGHT;
	}
}
bool isSnakeTouchField(Game& g) {
	if (g.snake.body[0].x <= 0 || g.snake.body[0].x > g.field.width || g.snake.body[0].y <= 0 || g.snake.body[0].y > g.field.height) {
		return true;
	}
	return false;
}
bool isSnakeTouchItself(Game& g) {
	for (int i = 1; i < g.snake.length; i++) {
		if (g.snake.body[0].x == g.snake.body[i].x && g.snake.body[0].y == g.snake.body[i].y) {
			return true;
		}
	}
	return false;
}
Key takeKey() {
	if (_kbhit()) {
		unsigned char k;
		k = _getch();
		//FlushConsoleInputBuffer(GetStdHandle(STD_INPUT_HANDLE));
		switch (k) {
		case 72:
			return K_UP;
		case 80:
			return K_DOWN;
		case 75:
			return K_LEFT;
		case 77:
			return K_RIGHT;
		case 27:
			return K_ESC;
		case 13:
			return K_ENTER;
		case 32:
			return K_SPACE;
		}
	}
	return K_NOTHING;
}
void showPause(Field& f) {
	system("cls");
	setTextColor(15);
	for (int i = 0; i < f.width + 2; i++) {
		gotoXY(i + f.x, f.y);
		cout << "#";
	}
	for (int i = 1; i < f.height + 1; i++) { //i = 1 т.к. левую и правую границу нужно нарисовать ниже потолка
		gotoXY(f.x, i + f.y);
		cout << "#" << setw(f.width + 1) << "#";
	}
	for (int i = 0; i < f.width + 2; i++) {
		gotoXY(i + f.x, f.y + f.height + 1); // + 1 т.к. пол нужно нарисовать ниже левой и правой границ
		cout << "#";
	}
	string pause_text = "GAME PAUSED!";
	string pressESCtoEXIT_text = "PRESS ESC TO EXIT";
	string pressSPACEtoCONTINUE = "PRESS SPACE TO CONTINUE";
	gotoXY(f.width / 2 - 4 + f.x, f.height / 2 + f.y);
	cout << pause_text;
	gotoXY(f.width / 2 - 7 + f.x, f.height / 2 + 1 + f.y);
	cout << pressESCtoEXIT_text;
	gotoXY(f.width / 2 - 10 + f.x, f.height / 2 + 2 + f.y);
	cout << pressSPACEtoCONTINUE;
}
void unpauseGame(Game& g) {
	g.state = GS_PLAY;
	system("cls");
	showField(g.field);
	showSnake(g);
	showFood(g);
	showBooster(g);
	showScore(g);
}
void pauseGame(Game& g) {
	showPause(g.field);
	Key key;
	while (g.state == GS_PAUSE) {
		key = takeKey();
		switch (key) {
		case K_SPACE:
			unpauseGame(g);
			break;
		case K_ESC:
			g.state = GS_EXIT;
			break;
		}
	}
}
bool isGameOver(Game& g) {
	return isSnakeTouchField(g);  //|| isSnakeTouchItself(g)
}
void showGameOver(Field& f) {
	system("cls");
	setTextColor(4);
	for (int i = 0; i < f.width + 2; i++) {
		gotoXY(i + f.x, f.y);
		cout << "#";
	}
	for (int i = 1; i < f.height + 1; i++) { //i = 1 т.к. левую и правую границу нужно нарисовать ниже потолка
		gotoXY(f.x, i + f.y);
		cout << "#" << setw(f.width + 1) << "#";
	}
	for (int i = 0; i < f.width + 2; i++) {
		gotoXY(i + f.x, f.y + f.height + 1); // + 1 т.к. пол нужно нарисовать ниже левой и правой границ
		cout << "#";
	}
	string gameOver_text = "GAME OVER!";
	string tryAgain_text = "PRESS ENTER TO TRY AGAIN";
	string pressESCtoEXIT_text = "PRESS ESC TO EXIT";
	gotoXY(f.width / 2 - 3 + f.x, f.height / 2 + f.y);
	cout << gameOver_text;
	gotoXY(f.width / 2 - 10 + f.x, f.height / 2 + 1 + f.y);
	cout << tryAgain_text;
	gotoXY(f.width / 2 - 7 + f.x, f.height / 2 + 2 + f.y);
	cout << pressESCtoEXIT_text;
}
void initGame(Game& g) {
	g.state = GS_PLAY;
	g.score = 0;
	g.gameSpeed = 170;	    //game delay
	initField(g.field);
	initSnake(g);
	genFood(g);
	genBooster(g);
}
void playSnake(Game& g) {
	int trunc_delay = 0;
	int delayToSpawnMods = 0;
	int maxDelayToSpawnMods = 130;
	int modsEffectDelay = 0;
	int maxModsEffectDelay = 90;
	while (g.state == GS_PLAY) {
		if (delayToSpawnMods >= maxDelayToSpawnMods) {
			hideBooster(g);
			genBooster(g);
			showBooster(g);
			delayToSpawnMods = 0;
		}

		if (modsEffectDelay >= maxModsEffectDelay) {
			if (g.snake.dir == SD_RIGHT || g.snake.dir == SD_LEFT) {
				g.gameSpeed = 170;
			}																	//set normal gameSpeed
			else if (g.snake.dir == SD_UP || g.snake.dir == SD_DOWN) {
				g.gameSpeed = 340;
			}
		}

		if (trunc_delay >= 2) {
			hideTruncatedBody(g);
			trunc_delay = 0;
		}


		switch (takeKey()) {				//Logic
		case K_ESC:
			g.state = GS_EXIT;
			break;
		case K_SPACE:
			g.state = GS_PAUSE;
			pauseGame(g);
			break;
		}

		switch (takeKey()) {				//Control the snake
		case K_UP:
			turnSnakeUp(g);
			break;
		case K_DOWN:
			turnSnakeDown(g);
			break;
		case K_LEFT:
			turnSnakeLeft(g);
			break;
		case K_RIGHT:
			turnSnakeRight(g);
			break;
		}

		moveSnake(g.snake);

		if (isSnakeAteFood(g)) {
			g.score++;
			incSnake(g.snake);
			showScore(g);
			genFood(g);
			showFood(g);
		}

		if (isSnakeAteBooster(g)) {
			if (g.snake.dir == SD_LEFT || g.snake.dir == SD_RIGHT) {
				g.gameSpeed = 50;
			}
			else {
				g.gameSpeed = 100;
			}
			delayToSpawnMods = 0;
			modsEffectDelay = 0;
		}

		if (g.state == GS_PLAY) {
			hideSnake(g);
			showSnake(g);
		}

		if (isSnakeTouchItself(g)) {
			truncSnake(g);
			showScore(g);
		}
		if (g.snake.truncated_body_lenght > 0) {
			trunc_delay++;
		}

		if (isGameOver(g)) {
			showGameOver(g.field);
			Key k;
			do {
				k = takeKey();
			} while (k != K_ENTER && k != K_ESC);
			switch (k) {
			case K_ENTER:
				system("cls");
				initGame(g);
				showField(g.field);
				showFood(g);
				showBooster(g);
				showScore(g);
				showSnake(g);
				break;
			case K_ESC:
				g.state = GS_OVER;
				break;
			}
		}


		//gotoXY(g.field.x, g.field.y + g.field.height + 3);
		//cout << g.gameSpeed << "   ";
		//cout << endl << g.snake.body[0].x << " " << g.snake.body[0].y << " ";
		//cout << endl << "delayToSpawnMods:" << delayToSpawnMods << "   " << "modsEffectDelay:" << modsEffectDelay << "   ";
		//cout << endl << "Food:" <<  g.food.x << " " << g.food.y << "  " << "Booster:" << g.booster.x << " " << g.booster.y << "   ";

		delayToSpawnMods++;
		modsEffectDelay++;
		Sleep(g.gameSpeed);
	}
}
void runGame() {
	system("cls");

	Game snakeGame;
	showCursor(FALSE);
	initGame(snakeGame);
	showField(snakeGame.field);
	showFood(snakeGame);
	showBooster(snakeGame);
	showScore(snakeGame);

	playSnake(snakeGame);
}
void initMenu(Menu& m, string caption) {
	m.activeItem = -1;
	m.caption = caption;
}
void addMenuItem(Menu& m, string text, MenuAction action) {
	MenuItem menuItem;
	menuItem.text = text;
	menuItem.action = action;
	m.items.push_back(menuItem);
}
void nextMenuItem(Menu& m) {
	if (m.activeItem >= m.items.size()) {
		m.activeItem = 0;
	}
	else {
		m.activeItem++;
	}
}
void prevMenuItem(Menu& m) {
	if (m.activeItem <= 0) {
		m.activeItem = m.items.size();
	}
	else {
		m.activeItem--;
	}
}
void showMenu(Menu& m) {
	gotoXY(50, 3);
	setTextColor(8);
	cout << m.caption;
	for (int i = 0; i < m.items.size(); i++) {
		if (i == m.activeItem) {
			setTextColor(7);
		}
		else {
			setTextColor(8);
		}
		gotoXY(50, 5 + i);
		cout << m.items[i].text;
	}
	if (m.activeItem == m.items.size()) {
		setTextColor(7);
	}
	else {
		setTextColor(8);
	}
	gotoXY(50, 5 + m.items.size());
	cout << "EXIT";
}
void executeMenuAction(Menu& m) {
	if (m.activeItem != -1) {
		m.items[m.activeItem].action();
	}
}
void runMenu(Menu& m) {
	showCursor(FALSE);
	initMenu(m, "MAIN MENU");
	addMenuItem(m, "PLAY", runGame);
	bool isExit = false;
	showMenu(m);
	while (!isExit) {
		switch (takeKey()) {
		case K_UP:
			prevMenuItem(m);
			showMenu(m);
			break;
		case K_DOWN:
			nextMenuItem(m);
			showMenu(m);
			break;
		case K_ENTER:
			if (m.activeItem == m.items.size()) {
				isExit = true;
				break;
			}
			else {
				executeMenuAction(m);
				if (m.items[m.activeItem].action == runGame) {
					isExit = true;
				}
				break;
			}
		}
	}
}
int main() {
	Menu menu;
	runMenu(menu);
}
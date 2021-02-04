#include <iostream>
#include <Windows.h>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <chrono>
#include <thread>
#include <fstream>
#include <string>
#include <algorithm>
#include <math.h>

HANDLE handle;
const char SQUARE = (char)219;
int points = 0, level = 0;

void SET_CURSOR(short coordX, short coordY) {
	SetConsoleCursorPosition(handle, COORD() = { coordX, coordY });
}

void SetColor(int color) {
	SetConsoleTextAttribute(handle, color);
}

struct Cell { int color = 0; bool exists = false, movable = false; };
Cell grid[25][20];

struct Position { int x, y; };

char** generateShape(int type, int& formSIZE) {
	std::ifstream reader("shapes/" + std::to_string(type) + ".txt");
	int i = -1;
	reader >> formSIZE;
	char** form = new char* [formSIZE / 2];
	for (std::string line; std::getline(reader, line);) {
		if (i >= 0) {
			form[i] = new char[formSIZE];
			for (int l = 0; l < formSIZE; ++l) {
				form[i][l] = (line[l] == '0' ? SQUARE : ' ');
			}
		}
		++i;
	}
	return form;
}

class Shape {
	void Update() {
		for (int i = 0; i < formSIZE / 2; ++i)
			for (int j = 0; j < formSIZE; ++j) {
				if (form[i][j] != ' ') {
					grid[position[i][j].y][position[i][j].x].color = color;
					grid[position[i][j].y][position[i][j].x].exists = false;
					grid[position[i][j].y][position[i][j].x].movable = true;
				}
				else {
					grid[position[i][j].y][position[i][j].x].color = color;
					grid[position[i][j].y][position[i][j].x].exists = false;
					grid[position[i][j].y][position[i][j].x].movable = false;
				}
			}
	}

	bool Touch() {
		for (int i = 0; i < 24; ++i)
			for (int j = 0; j < 20; ++j)
				if (grid[i][j].movable && !grid[i + 1][j].movable && grid[i + 1][j].exists)
					return true;
		return false;
	}

	bool canMove(int x, int y) {
		for (int i = 0; i < formSIZE / 2; ++i)
			for (int j = 0; j < formSIZE; ++j) {
				if (form[i][j] != ' ' && grid[position[i][j].y + y][position[i][j].x + x].exists)
					return false;
				if (form[i][j] != ' ' && (position[i][j].x + x < 0 || position[i][j].x + x > 19))
					return false;
			}
		return true;
	}

	void clear() {
		for (int i = 0; i < formSIZE / 2; ++i)
			for (int j = 0; j < formSIZE; ++j) {
				if (grid[position[i][j].y][position[i][j].x].movable) {
					grid[position[i][j].y][position[i][j].x].exists = false;
					grid[position[i][j].y][position[i][j].x].movable = false;
				}
			}
	}
public:
	bool rotatableZ, rotatableX, moving;
	int color, formSIZE, type;
	char** form;
	Position** position; // current shape's position

	Shape(int type, int color) {
		this->color = color;
		this->type = type;

		std::ifstream reader("shapes/" + std::to_string(type) + ".txt");
		int i = -1;
		moving = true;
		reader >> formSIZE;
		form = new char* [formSIZE / 2];
		position = new Position * [formSIZE / 2];
		for (std::string line; std::getline(reader, line);) {
			if (i >= 0) {
				form[i] = new char[formSIZE];
				position[i] = new Position[formSIZE];
				for (int l = 0; l < formSIZE; ++l) {
					form[i][l] = (line[l] == '0' ? SQUARE : ' ');
					position[i][l] = Position() = { l, i };
				}
			}
			++i;
		}

		Update();
	}

	bool CanRotate(int deg) {
		Rotate(deg, false);
		for (int i = 0; i < formSIZE / 2; ++i)
			for (int j = 0; j < formSIZE; ++j) {
				if ((position[i][j].x < 0 && form[i][j] != ' ') || (position[i][formSIZE - 1 - j].x >= 20 && form[i][formSIZE - 1 - j] != ' ')) { //  && (form[i][formSIZE - 1 - j] != ' ' || form[i][j] != ' ')) {
					Rotate(-deg, false);
					return false;
				}
				if ((position[i][formSIZE - 1 - j].y > 24 && form[i][formSIZE - 1 - j] != ' ')) { //  && (form[i][formSIZE - 1 - j] != ' ' || form[i][j] != ' ')) {
					Rotate(-deg, false);
					return false;
				}
				if (grid[position[i][j].y][position[i][j].x].exists && !grid[position[i][j].y][position[i][j].x].movable) {
					Rotate(-deg, false);
					return false;
				}
			}

		Rotate(-deg, false);
		return true;
	}

	void Hold() { clear(); }

	void Move(int x, int y) {
		if (Touch() && moving)
			moving = false;
		if (moving) {
			clear();

			if (!canMove(x, y))
				x = 0;

			for (int i = 0; i < formSIZE / 2; ++i)
				for (int j = 0; j < formSIZE; ++j) {
					position[i][j].y += y;
					position[i][j].x += x;

					if (form[i][j] != ' ') {
						grid[position[i][j].y][position[i][j].x].exists = grid[position[i][j].y][position[i][j].x].movable = true;
						grid[position[i][j].y][position[i][j].x].color = color;
					}
					if (form[i][j] != ' ' && position[i][j].y >= 24) { //  - (limitDown - (formSIZE - 1 - stIndex - i))) {
						moving = false;
					}
				}
		}

		if (!moving) {
			for (int i = 24; i >= 0; --i)
				for (int j = 0; j < 20; ++j)
					if (grid[i][j].movable) {
						grid[i][j].movable = false;
						grid[i][j].exists = true;
					}
			return;
		}
	}

	void Rotate(int deg, bool update) {
		char** copy = new char* [formSIZE / 2];
		for (int i = 0; i < formSIZE / 2; ++i) {
			copy[i] = new char[formSIZE / 2];
			for (int j = 0; j < formSIZE; ++j)
				copy[i][j] = form[i][j];
		}

		for (int a = 0; a < 1; ++a) {
			for (int i = 0; i <= formSIZE - 2; i += 2)
				for (int j = 0; j <= formSIZE - 2; j += 2) {
					if (deg > 0) {
						form[j / 2][formSIZE - 1 - i] = copy[i / 2][j];
						form[j / 2][formSIZE - 2 - i] = copy[i / 2][j + 1];
					}
					else {
						form[formSIZE / 2 - 1 - j / 2][i] = copy[i / 2][j];
						form[formSIZE / 2 - 1 - j / 2][i + 1] = copy[i / 2][j + 1];
					}
				}

			for (int i = 0; i < formSIZE / 2; ++i)
				memcpy(copy[i], form[i], formSIZE);
		}

		delete[] copy;
		if (update)
			Update();
	}
};

void displayBoard(int x, int y) {
	for (int i = 0; i < 26; ++i) {
		x = 1;
		SET_CURSOR(x, ++y);
		for (int j = 0; j < 22; ++j) {
			SET_CURSOR(++x, y);
			if ((j == 0 || j == 21) && i < 25)
				printf("%c", 179);
			else if (i == 25 && (j == 0 || j == 21))
				printf("%c", j == 0 ? (char)192 : (char)217);
			else
				if (i >= 25)
					printf("%c%c", 196, 196);
				else if (j > 0 && i < 25 && grid[i][j - 1].exists) {
					SetColor(grid[i][j - 1].color);
					printf("%c", SQUARE);
					SetColor(15);
				}
				else
					printf(" ");
		}
	}
}

void checker() {
	std::vector<int> rows;

	for (int i = 0; i < 25; ++i) {
		int removable = 0;
		for (int j = 0; j < 20; ++j)
			removable += grid[i][j].exists && !grid[i][j].movable;
		if (removable == 20)
			rows.push_back(i);
	}

	for (int i = 0; i < rows.size(); ++i)
		for (int j = 0; j < 20; ++j)
			grid[rows[i]][j].color = 15;
	if (rows.size() > 0) {
		displayBoard(1, 0);
		std::this_thread::sleep_for(std::chrono::milliseconds(200));
	}

	for (int i = 0; i < rows.size(); ++i)
		for (int j = 0; j < 20; ++j)
			grid[rows[i]][j].exists = false;

	for (int row : rows) {
		for (int r = row + 1; r >= 0; --r) {
			for (int c = 0; c < 20; ++c) {
				if (r - 1 >= 0 && !grid[r][c].exists && grid[r - 1][c].exists) {
					grid[r][c].color = grid[r - 1][c].color;
					grid[r][c].exists = true;
					grid[r - 1][c].exists = false;
				}
			}
		}
	}

	points += 10 * rows.size() * (level + 1);
}

bool fail() {
	for (int i = 0; i < 20; ++i)
		if (grid[1][i].exists && !grid[1][i].movable)
			return true;
	return false;
}

class GameHandler {
private:
	struct Record {
		std::string date;
		int score = 0;
	};
	void display(std::string filename, int posX, int posY, int& previousDims, bool& displaying) {
		if (filename.empty()) {
			displaying = false;
			for (int i = 0; i < previousDims; ++i) {
				for (int j = 0; j < 40; ++j) {
					SET_CURSOR(posX + j, posY + i);
					printf(" ");
				}
			}
			SET_CURSOR(75, 23);
			printf("        ");
			return;
		}
		displaying = true;
		std::ifstream reader("resources/" + filename);
		int counter = 0;
		for (std::string line; std::getline(reader, line);) {
			SET_CURSOR(posX, posY + counter);
			for (int i = 0, lines = 1; i < line.size(); ++i, ++lines) {
				if (lines % 40 == 0) {
					SET_CURSOR(posX, posY + ++counter);
				}
				printf("%c", line[i]);
			}
			++counter;
		}
		previousDims = counter;
		SET_CURSOR(75, 23);
		SetColor(10);
		printf("%c BACK %c", 16, 17);
		SetColor(15);
	}
	void saveif() {
		std::ifstream reader("resources/hs.txt");
		std::vector<Record> records;
		int sc;
		std::string dt;
		int b = -1;
		while (!reader.eof()) {
			reader >> dt >> sc;
			if (reader.eof())
				break;
			records.push_back(Record());
			records[records.size() - 1].date = dt;
			records[records.size() - 1].score = sc;
		}
		std::sort(records.begin(), records.end(), [](Record& r1, Record r2) {return r1.score < r2.score; });
		if (records.size() < 5) {
			auto t = std::time(0);
#pragma warning(suppress : 4996)
			std::tm* now = localtime(&t);
			records.push_back(Record()); // = { std::to_string(now->tm_year + 1900) + '/' + std::to_string(now->tm_mon + 1) + '/' + std::to_string(now->tm_mday), points });

			records[records.size() - 1].date = std::to_string(now->tm_year + 1900) + '/' + std::to_string(now->tm_mon + 1) + '/' + std::to_string(now->tm_mday);
			records[records.size() - 1].score = points;
		}
		else if (records[0].score < points) {
			auto t = std::time(0);
#pragma warning(suppress : 4996)
			std::tm* now = localtime(&t);
			records[0] = Record();
			records[0].date = std::to_string(now->tm_year + 1900) + '/' + std::to_string(now->tm_mon + 1) + '/' + std::to_string(now->tm_mday);
			records[0].score = points;
		}
		reader.close();
		std::sort(records.begin(), records.end(), [](Record& r1, Record r2) {return r1.score < r2.score; });
		std::ofstream writer("resources/hs.txt", std::ios::out);
		for (int i = records.size() - 1 > 5 ? 5 : records.size() - 1; i >= 0; --i)
			writer << records[i].date << ' ' << records[i].score << '\n';
	}
	void handleIndexes(int index, int& previousDim, bool& displaying) {
		switch (index) {
		case 0:
			system("CLS");
			Play();
			break;
		case 1:
			display("controls.txt", 50, 10, previousDim, displaying);
			break;
		case 2:
			display("hs.txt", 50, 10, previousDim, displaying);
			break;
			break;
		case 3:
			display("about.txt", 50, 10, previousDim, displaying);
			break;
		case 10:
			display("", 50, 10, previousDim, displaying);
			break;
		default:
			PostMessage(GetConsoleWindow(), WM_CLOSE, 0, 0);
			break;
		}
	}
public:
	void DisplayMenu() {
		std::string menuText[5]{ "PLAY", "CONTROLS", "HIGHSCORES", "ABOUT", "EXIT" };
		bool displaying = 0;
		int previousDim = 0;

		SetColor(15);

		for (short a = 0, chosen = -1, x = 1, y = 0, textIndex = 0;; a += a == 0) {
			bool update = a == 0;
			if (GetAsyncKeyState(VK_UP) & 0x01 && !displaying) {
				chosen = chosen == 0 ? 4 : chosen - 1;
				update = true;
			}
			if (GetAsyncKeyState(VK_DOWN) & 0x01 && !displaying) {
				chosen = chosen == 4 ? 0 : chosen + 1;
				update = true;
			}

			if (GetAsyncKeyState(VK_RETURN) & 0x01) {
				handleIndexes(displaying && chosen != 4 && chosen != -1 ? 10 : chosen, previousDim, displaying);
				update = true;
			}

			if (update) {
				for (int i = 0; i < 25; ++i) {
					x = 1;
					SET_CURSOR(x, ++y);
					if (i == 0 || i == 24)
						printf("%c", i == 0 ? 201 : 200);
					for (int j = 0; j < 40; ++j) {
						if ((i == 0 && j == 39) || (i == 24 && j == 39) || (i == 0 || i == 24) || (j == 0 || j == 39))
							printf("%c", i == 0 && j == 39 ? 187 : (i == 24 && j == 39 ? 188 : (i == 0 || i == 24 ? 205 : 186)));
						else {
							if (x == 40 / 2 - (menuText[textIndex].size() + (textIndex == chosen && !displaying ? 4 : 2)) / 2 && y == 10 + textIndex) {
								SetColor(textIndex == chosen && !displaying ? 10 : 15);
								printf("%c %s %c", textIndex == chosen && !displaying ? 16 : 0, menuText[textIndex].c_str(), textIndex == chosen && !displaying ? 17 : 0);
								x += menuText[textIndex].size() + (textIndex == chosen && !displaying ? 4 : 2);
								j += menuText[textIndex].size() + (textIndex == chosen && !displaying ? 4 : 2);
								++textIndex;
								SetColor(15);
							}
							printf(" ");
						}
						SET_CURSOR(++x, y);
					}
				}

			}
			x = 1;
			y = textIndex = 0;
		}
	}
	void PauseGame() {
		std::string options[2]{ "RESUME", "GO BACK TO MENU" };

		for (int counter = 0, index = 0;; counter += counter == 0) {
			if (GetAsyncKeyState(VK_ESCAPE) & 0x01)
				break;
			if (GetAsyncKeyState(VK_RETURN) & 0x01) {
				if (index == 1) {
					system("CLS");
					DisplayMenu();
				}
				return;
			}
			if (counter == 0 || (GetAsyncKeyState(VK_UP) & 0x01 || GetAsyncKeyState(VK_DOWN) & 0x01)) {
				index = index == 0 ? 1 : 0;
				for (int i = 0; i < 2; ++i) {
					SET_CURSOR(10 - (options[i].size() / 2), 12 + i);
					SetColor(index == i ? 10 : 15);
					printf("%c %s %c", 175, options[i].c_str(), 174);
				}
			}
		}
	}
	void Play() {
		if (GetAsyncKeyState(VK_SPACE) & 0x01);

		points = 0;
		for (int i = 0; i < 25; ++i)
			for (int j = 0; j < 20; ++j) {
				grid[i][j] = Cell(); // = { rand() % 4, false, false };
				grid[i][j].color = rand() % 4;
				grid[i][j].exists = false;
				grid[i][j].movable = false;
			}

		int x = 2, y = 0, counter = 0, nextShape = 0, nextColor = 0, heldShape = -1, heldColor = -1;

		Shape shapy = Shape(rand() % 6, rand() % 7 + 8);
		nextShape = rand() % 6;
		nextColor = rand() % 7 + 8;

		auto previousTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();
		auto currentTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();

		bool pause = false, getHeld = true, gameOver = false;
		float time = 100, cooldownCounter = 300;

		for (int ct = 0;; ++ct) {
			level = floor(points / 100);
			time = (level + 1) * 100;

			if (GetAsyncKeyState(VK_ESCAPE) & 0x01)
				PauseGame();

			gameOver = fail();
			if (!gameOver)
				checker();

			if (!shapy.moving && !gameOver) {
				shapy = Shape(nextShape, nextColor);
				nextShape = rand() % 6;
				nextColor = rand() % 7 + 8;
				ct = 0;
				getHeld = true;
			}
			if (ct == 0 && !gameOver) {
				SET_CURSOR(30, 7);
				printf("NEXT SHAPE:");
				int siz = 0;
				char** shpy = generateShape(nextShape, siz);
				for (int i = 0; i < 5; ++i)
					for (int j = 0; j < 10; ++j) {
						SET_CURSOR(31 + j, 9 + i);
						if (i < siz / 2 && j < siz && shpy[i][j] != ' ') {
							SetColor(nextColor);
							printf("%c", SQUARE);
							SetColor(15);
						}
						else
							printf(" ");
					}
			}

			SET_CURSOR(30, 14);
			printf("POINTS: %i", points);
			SET_CURSOR(30, 16);
			printf("LEVEL: %i", level);
			SET_CURSOR(30, 18);
			printf("HOLD:");
			if (ct == 0) {
				if (heldShape != -1) {
					int siz = 0;
					char** shpy2 = generateShape(heldShape, siz);
					for (int i = 0; i < 5; ++i)
						for (int j = 0; j < 10; ++j) {
							SET_CURSOR(31 + j, 20 + i);
							if (i < siz / 2 && j < siz && shpy2[i][j] != ' ') {
								SetColor(heldColor);
								printf("%c", SQUARE);
								SetColor(15);
							}
							else
								printf(" ");
						}
				}
				else
					for (int i = 0; i < 5; ++i)
						for (int j = 0; j < 10; ++j) {
							SET_CURSOR(31 + j, 20 + i);
							printf(" ");
						}
			}

			SET_CURSOR(0, 26);

			displayBoard(x, y);

			if (!gameOver) {
				int moveX = 0, moveY = 1;
				if (GetAsyncKeyState(VK_SHIFT) & 0x01) {
					if (heldShape == -1) {
						shapy.Hold();
						heldShape = shapy.type;
						heldColor = shapy.color;
						shapy = Shape(nextShape, nextColor);
						nextShape = rand() % 6;
						nextColor = rand() % 7 + 8;
						ct = -1;
						getHeld = false;
					}
					if (heldShape != -1 && getHeld) {
						shapy.Hold();
						shapy = Shape(heldShape, heldColor);
						heldShape = -1;
						ct = -1;
					}
				}
				if (GetAsyncKeyState(VK_LEFT) & 0x01)
					moveX -= 2;
				else if (GetAsyncKeyState(VK_RIGHT) & 0x01)
					moveX += 2;
				else if (GetAsyncKeyState('Z') & 0x01 && shapy.CanRotate(-1))
					shapy.Rotate(-1, true);
				else if (GetAsyncKeyState('X') & 0x01 && shapy.CanRotate(1))
					shapy.Rotate(1, true);
				y = 0;

				int countr = -1;
				while ((GetAsyncKeyState(VK_DOWN) & 0x8000) && shapy.moving && ++countr < 2) {
					shapy.Move(0, 1);
				}
				if (GetAsyncKeyState(VK_SPACE) & 0x01)
					while (shapy.moving) {
						shapy.Move(0, 1);
					}

				currentTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();
				float dt = (currentTime - previousTime) * 0.001;

				moveY = 1;
				cooldownCounter -= time * dt;
				if (cooldownCounter <= 0) {
					previousTime = currentTime;
					cooldownCounter = 200;
				}
				else moveY = 0;
				shapy.Move(moveX, moveY);
			}

			if (gameOver)
				GameOver();
		}

	}
	void GameOver() {
		saveif();
		std::string options[2]{ "PLAY AGAIN", "GO BACK TO MENU" };
		SET_CURSOR(12 - 5, 10);
		printf("GAME OVER!");

		for (int counter = 0, index = 0;; counter += counter == 0) {
			if (GetAsyncKeyState(VK_RETURN) & 0x01) {
				system("CLS");
				if (index == 1)
					DisplayMenu();
				else
					Play();
				return;
			}
			if (counter == 0 || GetAsyncKeyState(VK_UP) & 0x01 || GetAsyncKeyState(VK_DOWN) & 0x01) {
				index = index == 0 ? 1 : 0;
				for (int i = 0; i < 2; ++i) {
					SET_CURSOR(10 - (options[i].size() / 2), 12 + i);
					SetColor(index == i ? 10 : 15);
					printf("%c %s %c", 175, options[i].c_str(), 174);
				}
			}
		}
	}
};

int main()
{
	handle = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTitle(TEXT("Tetris Terminal C++"));
	srand(time(NULL));
	GameHandler gh;

	gh.DisplayMenu();

	return 0;
}

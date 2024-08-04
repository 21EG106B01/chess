#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <map>
using namespace sf;

std::vector<Sprite> pieces, totkilled;

std::string names[12] = { "White King", "White Queen", "White Bishop", "White Knight", "White Rook", "White Pawn",
	"Black King", "Black Queen", "Black Bishop", "Black Knight", "Black Rook", "Black Pawn" };

std::map<int, std::string> piecename;

bool isMove = false, allowedmove = true, turn = false, moved = false;

int selected = -1, killed = -1;
int mousex, mousey, lastx, lasty, cp = 0;
int board[8][8] = {
{5, 4, 3, 2, 1, 3, 4, 5},
{6, 6, 6, 6, 6, 6, 6, 6},
{0, 0, 0, 0, 0, 0, 0, 0},
{0, 0, 0, 0, 0, 0, 0, 0},
{0, 0, 0, 0, 0, 0, 0, 0},
{0, 0, 0, 0, 0, 0, 0, 0},
{12,12,12,12,12,12,12,12},
{11,10, 9, 8, 7, 9,10,11}
};
std::vector<std::vector<int>> piecepos, lastpos;
std::vector<int> oldpos = { -1, -1, -1 }, newpos = { -1, -1, -1 };

sf::Font font;
Text debugtext("Debug", font, 15);
Text debug2("debug2", font, 15);
Texture backtex;
Sprite backSprite;
Texture piecetex;


void nomove(int x);
void updateboard();
bool checkmove();


int main() {

	backtex.loadFromFile("images/Chess-back.png");
	piecetex.loadFromFile("images/pieces-s.png");
	backSprite.setTexture(backtex);
	font.loadFromFile("arial.ttf");
	for (int i = 1; i <= 12; i++) {
		piecename[i] = names[i - 1];
	}
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
			if (board[i][j] == 0)
				continue;
			Sprite piece(piecetex, sf::IntRect(((board[i][j] - 1) * 60), 0, 50, 50));
			piece.setPosition((j * 50), (i * 50));
			pieces.push_back(piece);
			piecepos.push_back({ j, i, board[i][j] });
		}
	}
	
	RenderWindow window(VideoMode(600, 430), "Chess");
	
	debugtext.setPosition(10, 410);
	debugtext.setFillColor(sf::Color::White);
	debug2.setPosition(300, 410);
	debug2.setFillColor(sf::Color::White);



	while (window.isOpen()) {

		Event e;
		while (window.pollEvent(e)) {
			if (e.type == Event::Closed)
				window.close();
		}

		mousex = Mouse::getPosition(window).x / 50;
		mousey = Mouse::getPosition(window).y / 50;
		debugtext.setString(std::to_string(mousex + 1) + ", " + std::to_string(mousey + 1));

//MOUSE BUTTON PRESSED
		for (int i = 0; i < pieces.size(); i++)
			if (pieces[i].getGlobalBounds().contains(Mouse::getPosition(window).x, Mouse::getPosition(window).y)) {
				//debug2.setString(std::to_string(i));
				if (e.type == Event::MouseButtonPressed) {
					selected = i;
					isMove = true;
					lastx = mousex;
					lasty = mousey;
				}
			}

//MOUSE BUTTON RELEASED
		if (selected >= 0 && ((turn == false && (piecepos[selected][2] >= 0 && piecepos[selected][2] <= 6)) || (turn == true && (piecepos[selected][2] >= 7 && piecepos[selected][2] <= 12)))) {
			if (selected >= 0 and isMove) {
				pieces[selected].setPosition(Mouse::getPosition(window).x - 24, Mouse::getPosition(window).y - 24);
				if (e.type == Event::MouseButtonReleased) {
					isMove = false;
					pieces[selected].setPosition(mousex * 50, mousey * 50);
					lastpos.push_back({ lastx, lasty, piecepos[selected][2] });
					piecepos[selected][0] = mousex;
					piecepos[selected][1] = mousey;
					if (mousex > 7 || mousey > 7 || mousex < 0 || mousey < 0) {
						moved = false;
						nomove(selected);
						continue;
					}
					else moved = true;
				}
			}

			if (selected >= 0 and !isMove) {
				oldpos[0] = lastx;
				oldpos[1] = lasty;
				oldpos[2] = piecepos[selected][2];
				newpos[0] = piecepos[selected][0];
				newpos[1] = piecepos[selected][1];
				newpos[2] = piecepos[selected][2];
				allowedmove = checkmove();

				for (int i = 0; i < piecepos.size(); i++) {
					if (selected == i)
						continue;
					cp = board[newpos[1]][newpos[0]];
					if ((piecepos[i][0] == piecepos[selected][0]) && (piecepos[i][1] == piecepos[selected][1]) && allowedmove) {
						if ((piecepos[i][2] < 7 and piecepos[selected][2] < 7) or (piecepos[i][2] >= 7 and piecepos[selected][2] >= 7)) {
							moved = false;
							nomove(selected);
							continue;
						}
						else
							moved = true;
						//std::cout << piecepos[i][0]+1 << ", " << piecepos[i][1]+1 << " - " << piecename[piecepos[i][2]] << "\n";
						totkilled.push_back(pieces[i]);
						pieces.erase(pieces.begin() + i);
						piecepos.erase(piecepos.begin() + i);
						break;
					}
				}

				if (!allowedmove)
					nomove(selected);
				updateboard();
				if (moved && allowedmove) {
					turn = !turn;
					moved = false;
					debug2.setString(std::to_string(turn));
				}
				selected = -1;
			}
			cp = 0;
		}

		window.clear();
		window.draw(backSprite);
		for (int i = 0; i < pieces.size(); i++)
			window.draw(pieces[i]);
		window.draw(debugtext);
		window.draw(debug2);
		window.display();
	}

	return 0;
}

void updateboard() {
	if (allowedmove) {
		board[oldpos[1]][oldpos[0]] = 0;
		board[newpos[1]][newpos[0]] = newpos[2];
	}
	std::system("cls");
	std::cout << "Board Positions:\n";
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++)
			std::cout << board[i][j] << "\t";
		std::cout << "\n";
	}
	std::cout << "\n";

}

bool checkmove() {
	int lx = oldpos[0], ly = oldpos[1], lp = oldpos[2], nx = newpos[0], ny = newpos[1], np = newpos[2], ci = 0;

	//KINGS
	if (np == 1 || np == 7) {
		if ((lx == nx && (ly == ny + 1 || ly == ny - 1)) || (ly == ny && (lx == nx - 1 || lx == nx + 1)))
			return true;
		if (((lx == nx + 1) || (lx == nx - 1)) && ((ly == ny + 1) || (ly == ny - 1)))
			return true;
	}
	//QUEENS
	if (np == 2 || np == 8) {
		//Diagonal Move
		if ((((lx - nx) == (ly - ny)) && (lx != nx))) {
			if (nx > lx)
				for (int i = 1; i < nx - lx; i++) {
					if (board[ly + i][lx + i])
						return false;
				}
			if (nx < lx)
				for (int i = 1; i < lx - nx; i++) {
					if (board[ny + i][nx + i])
						return false;
				}
			return true;
		}
		if ((((lx + ly) == (nx + ny)) && (lx != nx))) {
			if (nx > lx)
				for (int i = 1; i < nx-lx; i++) {
					if (board[ly - i][lx + i])
						return false;
				}
			if (nx < lx)
				for (int i = 1; i < lx-nx; i++) {
					if (board[ny - i][nx + i])
						return false;
				}
			return true;
		}
		//Straight Move
		if ((lx == nx && ly != ny)) {
			if (ny > ly)
				for (int i = 1; i < ny - ly; i++) {
					if (board[ly + i][lx])
						return false;
				}
			if (ny < ly)
				for (int i = 1; i < ly - ny; i++) {
					if (board[ny + i][nx])
						return false;
				}
			return true;
		}
		if ((ly == ny && lx != nx)) {
			if (nx > lx)
				for (int i = 1; i < nx - lx; i++) {
					if (board[lx + i][ly])
						return false;
				}
			if (nx < lx)
				for (int i = 1; i < lx - nx; i++) {
					if (board[nx + i][ny])
						return false;
				}
			return true;
		}
	}
	//BISHOPS
	if (np == 3 || np == 9) {
		if ((((lx - nx) == (ly - ny)) && (lx != nx))) {
			if (nx > lx)
				for (int i = 1; i < nx - lx; i++) {
					if (board[ly + i][lx + i])
						return false;
				}
			if (nx < lx)
				for (int i = 1; i < lx - nx; i++) {
					if (board[ny + i][nx + i])
						return false;
				}
			return true;
		}
		if ((((lx + ly) == (nx + ny)) && (lx != nx))) {
			if (nx > lx)
				for (int i = 1; i < nx - lx; i++) {
					if (board[ly - i][lx + i])
						return false;
				}
			if (nx < lx)
				for (int i = 1; i < lx - nx; i++) {
					if (board[ny - i][nx + i])
						return false;
				}
			return true;
		}
	}
	//KNIGHTS
	if (np == 4 || np == 10) {
		if (((nx == lx + 2 || nx == lx - 2) && (ny == ly + 1 || ny == ly - 1)) || ((ny == ly + 2 || ny == ly - 2) && (nx == lx + 1 || nx == lx - 1)))
		return true;
	}
	//ROOKS
	if (np == 5 || np == 11) {
		if ((lx == nx && ly != ny)) {
			if (ny > ly)
				for (int i = 1; i < ny - ly; i++) {
					if (board[ly + i][lx])
						return false;
				}
			if (ny < ly)
				for (int i = 1; i < ly - ny; i++) {
					if (board[ny + i][nx])
						return false;
				}
			return true;
		}
		if ((ly == ny && lx != nx)) {
			if (nx > lx)
				for (int i = 1; i < nx - lx; i++) {
					if (board[lx + i][ly])
						return false;
				}
			if (nx < lx)
				for (int i = 1; i < lx - nx; i++) {
					if (board[nx + i][ny])
						return false;
				}
			return true;
		}
	}
	//PAWNS
	if (np == 6 || np == 12) {
		if (board[ny][nx] == 0) {
			if (np == 12 && ny == ly - 1 && nx == lx)
				return true;
			if (np == 6 && ny == ly + 1 && nx == lx)
				return true;
		}
		if (np == 12 && ny == ly - 1 && (nx == lx + 1 || nx == lx - 1) && board[ny][nx])
			return true;
		if (np == 6 && ny == ly + 1 && (nx == lx + 1 || nx == lx - 1) && board[ny][nx])
			return true;
	}
	ci == 0;
	return false;
}

void nomove(int x) {
	pieces[x].setPosition(lastpos[lastpos.size() - 1][0] * 50, lastpos[lastpos.size() - 1][1] * 50);
	piecepos[x][0] = lastx;
	piecepos[x][1] = lasty;	
	board[oldpos[1]][oldpos[0]] = newpos[2];
	board[newpos[1]][newpos[0]] = cp;
}

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#pragma warning(disable : 4996)	

struct board_S {
	unsigned short* table;
	int X = 0;
	int Y = 0;
	int size = 0;
	int sequence = 0;
	unsigned short active_player = 1;
	int free_space = 0;
	board_S(int Y, int X, unsigned short active_player, int sequence) : X(X), Y(Y), active_player(active_player), sequence(sequence){
		table = (unsigned short*)calloc(X*Y,X * Y * sizeof(*table));
		size = X * Y;
	}
	~board_S() {
		free(table);
	}


	void print() {
		for (int j = 0; j < Y; j++) {
			for (int i = 0; i < X; i++) {
				printf("%i ",(*this)(j,i));
			}
			printf("\n");
		}
	}
	unsigned short getOponent() {
		if (active_player == 1) {
			return 2;
		}
		else {
			return 1;
		}
	}

	unsigned short& operator()(int m) {
		return table[m];
	}
	unsigned short operator()(int m) const{
		return table[m];
	}
	unsigned short& operator()(int y, int x) {
		return table[y * X + x];
	}
	unsigned short operator()(int y, int x) const{
		return table[y * X + x];
	}
};

struct ret {
	int b;
	int a;
	bool control;

	ret add(const ret& r, int sequence, bool player) const {
		ret temp = {0,0,false};
		bool t1 = r.control || control;

		temp.b = b + r.b + 1;
		temp.a = a + r.a + t1;

		if ((temp.b + r.a + r.control >= sequence) != (temp.b + a + control >= sequence)) {
			temp.control = true;
		}
		if (player && temp.b + t1 == sequence) {
			temp.control = t1;
		}
		return temp;
	}
};

int checkSequence(board_S& board, int y, int x, int dy, int dx, unsigned short active, int number = 0) {
	if (x+dx >=0 && x+dx < board.X && y + dy >= 0 && y + dy < board.Y) {
		if (board(y+dy,x+dx) == active) {
			return checkSequence(board, y+dy, x+dx, dy, dx, active, number + 1);
		}
	}
	return number;
}

ret checkSequencev2(board_S& board, int y, int x, int dy, int dx, unsigned short active ,int a = 0, int b = 0, bool skip = false) {
	if (x + dx >= 0 && x + dx < board.X && y + dy >= 0 && y + dy < board.Y) {
		bool test = board(y + dy, x + dx) == active;
		if (test || (!skip && board(y+dy,x + dx) == 0)) {
			if (!test) skip = true;
			if (!skip) {
				b++;
			}
			else if(test){
				a++;
			}
			return checkSequencev2(board, y + dy, x + dx, dy, dx, active, a, b, skip);
		}
	}

	return { b, a, skip};
}

unsigned short checkPoint(board_S& board, int y, int x) {
	int res;
	unsigned short num = board(y, x);
	if (num == 0)return 0;
	res = checkSequence(board, y, x, -1, -1, num) + checkSequence(board, y, x, 1, 1, num) + 1;
	if (res >= board.sequence) return num;
	res = checkSequence(board, y, x, 1, -1, num) + checkSequence(board, y, x, -1, 1, num) + 1;
	if (res >= board.sequence) return num;
	res = checkSequence(board, y, x, -1, 0, num) + checkSequence(board, y, x, 1, 0, num) + 1;
	if (res >= board.sequence) return num;
	res = checkSequence(board, y, x, 0, -1, num) + checkSequence(board, y, x, 0, 1, num) + 1;
	if (res >= board.sequence) return num;
	return 0;
}

ret checkPointv2(board_S& board, int y, int x, unsigned short active = 0) {
	ret res;
	unsigned short num = board(y, x);
	int test = 0;
	if (num == 0)return {0,0,false};

	res = checkSequencev2(board, y, x, -1, -1, num).add(checkSequencev2(board, y, x, 1, 1, num),board.sequence, active == num);
	if (res.control) {
		test++;
	} else {
		if (res.a + res.b >= board.sequence) {
			return { num, 0, false };
		}
	}
	res = checkSequencev2(board, y, x, 1, -1, num).add(checkSequencev2(board, y, x, -1, 1, num), board.sequence, active == num);
	if (res.control) {
		test++;
	}else {
		if (res.a + res.b >= board.sequence) {
			return { num, 0, false };
		}
	}
	res = checkSequencev2(board, y, x, -1, 0, num).add(checkSequencev2(board, y, x, 1, 0, num), board.sequence, active == num);
	if (res.control) {
		test++;
	} else {
		if (res.a + res.b >= board.sequence) {
			return { num, 0, false };
		}
	}

	res = checkSequencev2(board, y, x, 0, -1, num).add(checkSequencev2(board, y, x, 0, 1, num), board.sequence, active == num);
	if (res.control) {
		test++;
	} else {
		if (res.a + res.b >= board.sequence) {
			return { num, 0, false };
		}
	}

	if (test >= 1) {
		if (test >= 2 || num == active) {
			return { num, 0, false };
		}
		return { 0, 0, true };
	}
	else {
		return { 0, 0, false };
	}
}


void best(unsigned short a, unsigned short& best, unsigned short player) {
	if (a == player) {
		best = player;
	}
	else if (a == 0 && best != player) {
		best = 0;
	}
	else if(best != 0 && best != player) {
		best = a;
	}
}

ret GEN_ALL_POS_MOV_CUT_IF_GAME_OVER(board_S& board) {
	bool control = false;

	for (int j = 0; j < board.Y; j++) {
		for (int i = 0; i < board.X; i++) {
			if (board(j, i) == 0) {
				board(j, i) = board.active_player;
				control = checkPoint(board, j, i);
				board(j, i) = 0;
				if(control)return { j,i,true };
			}
		}
	}
	return { 0, 0, false };
}

unsigned short SOLVE_GAME_STATE(board_S& board, int y, int x) {
	board(y, x) = board.active_player;
	board.free_space--;
	ret temp = checkPointv2(board, y, x);
	if (temp.control) {
		ret temp2 = GEN_ALL_POS_MOV_CUT_IF_GAME_OVER(board);
		board.active_player = board.getOponent();

		unsigned short t = SOLVE_GAME_STATE(board, temp2.b, temp2.a);
		board(y, x) = 0;
		board.free_space++;
		board.active_player = board.getOponent();
		return t;
	}

	board.active_player = board.getOponent();

	if (temp.b == 0 && board.free_space == 0) {
		board(y, x) = 0;
		board.free_space++;
		board.active_player = board.getOponent();
		return 0;
	}	
	else if (temp.b != 0) {
		board(y, x) = 0;
		board.free_space++;
		board.active_player = board.getOponent();
		return (unsigned short)temp.b;
	}
	else {
		unsigned short bes = board.getOponent();
		for (int j = 0; j < board.Y && bes != board.active_player; j++) {
			for (int i = 0; i < board.X && bes != board.active_player; i++) {
				if (board(j, i) == 0) {
					best(SOLVE_GAME_STATE(board, j, i),bes,board.active_player);
				}
			}
		}
		board.active_player = board.getOponent();
		board(y, x) = 0;
		board.free_space++;
		return bes;
	}
}

unsigned short allBoardCheckWin(board_S& board) {
	unsigned short test = 0;
	for (int j = 0; j < board.Y && !test; j++) {
		for (int i = 0; i < board.X && !test; i++) {
			test = checkPoint(board, j, i);
		}
	}
	return test;
}

unsigned short allBoardCheckWinv2(board_S& board, unsigned short active) {
	unsigned short bes = 0;
	unsigned short temp= 0;
	unsigned short oponent = 0;

	for (int j = 0; j < board.Y && bes != active; j++) {
		for (int i = 0; i < board.X && bes != active; i++) {
			if (board(j,i) == 0) {
				board(j, i) = board.active_player;
				temp = checkPoint(board, j, i);
				if (temp != 0)bes = temp;
				board(j, i) = board.getOponent();
				temp = checkPoint(board, j, i);
				if (temp != 0)oponent++;
				board(j, i) = 0;
			}
		}
	}
	if (bes == 0 && oponent >= 2) bes = board.getOponent();
	return bes;
}

int main() {
	char buffer[256];
	char buffer2[256];
	setvbuf(stdout, buffer, _IOFBF, sizeof(buffer));
	setvbuf(stdin, buffer2, _IOFBF, sizeof(buffer2));

	char input[64], t;
	int x, y, xx, sequence;
	unsigned short player;
	while (true) {
		x = 0;
		y = 0;
		sequence = 0;
		player = 0;
		xx = 0;
		scanf("%100s %d %d %d %hu\n", &input, &y, &x, &sequence, &player);
		if (feof(stdin) != 0)break;
		board_S board(y, x, player,sequence);
		while (xx < x*y && !feof(stdin)) {
			t = (char)getchar();
			switch (t) {
			case '\n':
			case EOF:
			case ' ':
				xx++;
				break;
			default:
				board(xx) = t - '0';
				if (t == '0')board.free_space++;
				break;
			}
		}

		if (strcmp(input, "GEN_ALL_POS_MOV") == 0) {
			if (allBoardCheckWin(board)) {
				printf("0\n");
				continue;
			}
			printf("%i\n", board.free_space);
			for (int i = 0; i < board.size; i++) {
				if (board(i) == 0) {
					board(i) = board.active_player;
					board.print();
					board(i) = 0;
				}
			}

		}
		else if (strcmp(input, "GEN_ALL_POS_MOV_CUT_IF_GAME_OVER") == 0) {
			if (allBoardCheckWin(board)) {
				printf("0\n");
				continue;
			}	
			ret temp = GEN_ALL_POS_MOV_CUT_IF_GAME_OVER(board);

			if (temp.control) {
				board(temp.b, temp.a) = player;
				printf("1\n");
				board.print();
			}
			else {
				printf("%i\n", board.free_space);
				for (int i = 0; i < board.size; i++) {
					if (board(i) == 0) {
						board(i) = board.active_player;
						board.print();
						board(i) = 0;
					}
				}
			}
		}
		else if (strcmp(input, "SOLVE_GAME_STATE") == 0) {
			ret temp = { 0,0,false };
			unsigned short bes = board.getOponent();
			unsigned short res = allBoardCheckWin(board);
			if (!res)
				res = allBoardCheckWinv2(board, board.active_player);

				
			if (res == 0 && board.free_space != 0) {
				board.active_player = board.getOponent();
				temp = GEN_ALL_POS_MOV_CUT_IF_GAME_OVER(board);
				board.active_player = board.getOponent();
			}	

			if (temp.control) {
				bes = SOLVE_GAME_STATE(board, temp.b, temp.a);
			}
			else {
				if (res == 0 && board.free_space != 0) {
					for (int j = 0; j < board.Y && bes != player; j++) {
						for (int i = 0; i < board.X && bes != player; i++) {
							if (board(j, i) == 0)
								best(SOLVE_GAME_STATE(board, j, i), bes, board.active_player);
						}
					}
				}
				else {
					bes = res;
				}
			}				
			switch (bes) {
			case 0:
				printf("BOTH_PLAYERS_TIE\n");
				break;
			case 1:
				printf("FIRST_PLAYER_WINS\n");
				break;
			case 2:
				printf("SECOND_PLAYER_WINS\n");
				break;
			}
		}
	}
	fflush(stdout);
	return 0;
}
#include <curses.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>

typedef struct {
	int x;
	int y;
} vec2;

typedef struct {
	vec2 pos;
	int	value;
	int	revealed;
} cell;
vec2 randompos(vec2 board)
{
	vec2 pos;
	pos.x = (rand() % board.x) * 2;
	pos.y = (rand() % board.y);
	return (pos);
}
int	isbomb(int x, int y, vec2 *bombs)
{
	int	i = 0;
	while (bombs[i].x != -1)
	{
		if (x == bombs[i].x && y == bombs[i].y)
			return (1);
		i++;
	}
	return (0);
}
int	getboardindex(int x, int y, cell *board, vec2 boardsize)
{
	int	i = 0;
	int maxindex = (boardsize.x * 2) * boardsize.y;
	while (i < maxindex)
	{
		if (x == board[i].pos.x && y == board[i].pos.y)
			return (i);
		i++;
	}
	return (-1);
}
//check near bombs
int checkbombs(int x, int y, vec2 *bombs)
{
	int numbomb = 0;
	  vec2 direction[8] = {
        {-2, -1}, {0, -1}, {+2, -1},
        {-2,  0},         {+2,  0},
        {-2, +1}, {0, +1}, {+2, +1}
	};
	int i = 0;
	while (i < 8)
	{
		if (isbomb(x + direction[i].x, y + direction[i].y, bombs))
			numbomb++;
		i++;
	}
	return (numbomb);
}
cell *fillboard(vec2 boardsize, vec2 *bombs)
{
	int i = 0;
	int x;
	int maxsize = (boardsize.x * 2) * boardsize.y;
	cell *board = (cell *)malloc(sizeof(cell) * (maxsize + 1));
	if (!board)
		return (NULL);
	while (i < maxsize)
	{
		int y = 0;
		while(y < boardsize.y)
		{
			x = 0;
			while(x < (boardsize.x * 2))
			{
				board[i].pos.x = x;
				board[i].pos.y = y;
				if (isbomb(x, y, bombs))
					board[i].value = -1;
				else
					board[i].value = checkbombs(x, y, bombs);
				board[i].revealed = 0;
				i++;
				x++;
			}
			y++;
		}
	}
	return (board);
}

vec2 *generatebombs(int numbombs, vec2 board)
{
	int i = 0;
	vec2 pos;
	vec2 *bombs = (vec2 *)malloc((numbombs + 1) * sizeof(vec2));
	if (!bombs)
		return (NULL);
	bombs[i].x = -1; //initialize the array;
	while (i < numbombs)
	{
		pos = randompos(board);
		while (isbomb(pos.x, pos.y, bombs)) //Ensures it doenst generate a bomb on a previously generated one
			pos = randompos(board);
		bombs[i] = pos;
		printf("Bomb generated! x: %d, y: %d\n", bombs[i].x /2, bombs[i].y);
		i++;
		bombs[i].x = -1;
		bombs[i].y = -1;
	}
	return (bombs);
}

void drawboard(cell *board, vec2 boardsize, vec2 playerpos)
{
	int	i = 0;
	int	maxsize = (boardsize.x * 2) * boardsize.y;
	char printchar = '#';

	while(i < maxsize)
	{
		if (board[i].pos.x == playerpos.x && board[i].pos.y == playerpos.y )
			printchar = '@';
		else if (board[i].revealed == 1)
			printchar = board[i].value + '0';
		else if (board[i].revealed == 2)
			printchar = '$';
		else
			printchar = '#';

		if (board[i].pos.x % 2 == 0)
			mvaddch(board[i].pos.y, (board[i].pos.x), printchar);
		else
			mvaddch(board[i].pos.y, (board[i].pos.x), ' ');
		i++;
	}
	refresh();
}

int	main(int ac, char **av)
{
	if (ac != 4)
	{
		printf("Board Needs a X, and Y and a number of bombs! \nexample: ./game 10 10 5\n");
		return (0);
	}
	//randomize the game!
	srand(time(NULL));
	int	numbombs = atoi(av[3]);
	vec2 boardsize = {atoi(av[1]), atoi(av[2])};
	vec2 playerpos = {0, 0};

	if (numbombs >= (boardsize.x * boardsize.y))
	{
		printf("Cannot generate too many bombs, please keep beetween x and y boundaries!!!\n");
		return (0);
	}
	//generate number of bombs!
	vec2 *bombs = generatebombs(numbombs, boardsize);
	if (!bombs)
	{
		return (0);
	}
	cell *board = fillboard(boardsize, bombs);

	int	screen_width = boardsize.x * 2;
	int	screen_height = boardsize.y;
	WINDOW *win = initscr();

	keypad(win, true);
	nodelay(win, true);
	curs_set(0);
	noecho();
	clear();
	int i = 0;
	int mapfound;
	while (true)
	{
		int pressed = wgetch(win);
		if ((pressed == 'a' || pressed == 'A') && playerpos.x > 0)
			playerpos.x += -2;
		if ((pressed == 'd' || pressed == 'D' ) && playerpos.x < (boardsize.x * 2) - 2)
			playerpos.x += 2;
		if ((pressed == 's' || pressed == 'S' ) && playerpos.y < boardsize.y - 1)
			playerpos.y += 1;
		if ((pressed == 'w' || pressed == 'W') && playerpos.y > 0)
			playerpos.y += -1;
		if (pressed == '\e')
			break;
		if ((pressed == 'f') || pressed == 'F')
		{
			i = getboardindex(playerpos.x, playerpos.y, board, boardsize);
			if (board[i].revealed == 0)
			{
				board[i].revealed = 2;
			}
			else if (board[i].revealed == 2)
			{
				board[i].revealed = 0;
			}
		}
		if (pressed == 32 || pressed == KEY_ENTER)
		{
			i = getboardindex(playerpos.x, playerpos.y, board, boardsize);
			//printf("x: %d, y: %d\n", board[i].pos.x, board[i].pos.y);
			if (board[i].revealed != 2)
				if (board[i].value == -1)
				{
					printf("KABOOOOM, you lost hahaha!!!");
					break;
				}
			if (board[i].revealed == 0)
			{
				board[i].revealed = 1;
				mapfound++;
				if (mapfound == (boardsize.x * boardsize.y) - numbombs)
				{
					printf("Whaaaat?, you won? Congratulations!!!");
					break;
				}

			}
		}
		drawboard(board, boardsize, playerpos);
		refresh();
		usleep(1000); //to not overload the cpu
	}
	free(bombs);
	free(board);
	endwin();
	return (0);

}

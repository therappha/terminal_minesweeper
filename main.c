#include <curses.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>

typedef struct {
	int x;
	int y;
} vec2;

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
int	isrevealed(int x, int y, vec2 *revealed)
{
	int	i = 0;
	while (revealed[i].x != -1)
	{
		if (x == revealed[i].x && y == revealed[i].y)
			return (1);
		i++;
	}
	return (0);
}
vec2 *drawbombs(int numbombs, vec2 board)
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
		while (isbomb(pos.x, pos.y, bombs))
			pos = randompos(board);
		bombs[i] = pos;
		printf("Bomb generated! x: %d, y: %d\n", bombs[i].x /2, bombs[i].y);
		i++;
		bombs[i].x = -1;
		bombs[i].y = -1;
	}
	return (bombs);
}

void drawboard(vec2 board, vec2 playerpos, vec2 *revealed,vec2 *bombs)
{
	int x;
	int y;
	y = 0;
	char boardchar = '#';
	char reveal = '$';
	char playerchar = '@';
	char printchar = '#';
	//for some reason x and y are inverted
	while (y < board.y)
	{
		x = 0;
		while (x < board.x * 2)
		{
			if(x == playerpos.x && y == playerpos.y)
				printchar = playerchar;
			else if (isrevealed(x, y, revealed))
				printchar = reveal;
			else
				printchar = boardchar;
			mvaddch(y, x++, printchar);
			mvaddch(y, x++, ' ');
		}
		y++;
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
	srand(time(NULL));
	int	numbombs = atoi(av[3]);
	vec2 board = {atoi(av[1]), atoi(av[2])};
	vec2 playerpos = {0, 0};

	if (numbombs >= (board.x * board.y))
	{
		printf("Cannot generate too many bombs, please keep beetween x and y boundaries!!!\n");
		return (0);
	}
	vec2 *revealed = (vec2 *)malloc(sizeof(vec2) * ((board.x * 2) * board.y) + 1 );
	if (!revealed)
		return (0);
	revealed[0].x = -1;
	//generate number of bombs!
	vec2 *bombs = drawbombs(numbombs, board);
	int	screen_width = board.x * 2;
	int	screen_height = board.y;
	WINDOW *win = initscr();

	keypad(win, true);
	nodelay(win, true);
	curs_set(0);
	noecho();
	clear();
	int bombsi = 0;
	while (true)
	{
		int pressed = wgetch(win);
		if ((pressed == 'a' || pressed == 'A') && playerpos.x > 0)
			playerpos.x += -2;
		if ((pressed == 'd' || pressed == 'D' ) && playerpos.x < (board.x * 2) - 2)
			playerpos.x += 2;
		if ((pressed == 's' || pressed == 'S' ) && playerpos.y < board.y - 1)
			playerpos.y += 1;
		if ((pressed == 'w' || pressed == 'W') && playerpos.y > 0)
			playerpos.y += -1;
		if (pressed == 32)
		{
			if (isbomb(playerpos.x, playerpos.y, bombs))
			{
				printf("KABOOOOM, you lost hahaha!!!");
				endwin();
				return (0);
			}
			else if (!isrevealed(playerpos.x, playerpos.y, revealed))
			{
				revealed[bombsi] = playerpos;
				bombsi++;
				revealed[bombsi].x = -1;
				if (bombsi == (board.x * board.y - numbombs))
				{
					printf("Whaaaaaaat? You won??? Congratulations!");
					endwin();
					return (0);
				}
			}
		}
		drawboard(board, playerpos, revealed, bombs);
		refresh();
	}
	return (0);
}

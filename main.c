#include <stdbool.h>
#include <stdio.h>
#include <curses.h>
#include <unistd.h>
#include <stdlib.h>

typedef enum {
	up, right, down, left
} direction;

typedef struct snake {
	int x, y;
	direction dir;
	struct snake *next, *prev;
} snake_t;

typedef struct food {
	int x, y;
} food_t;

int row, col;
static void spawn_food(snake_t * snake, food_t * food)
{
	int x, y;
	x = rand() % col;
	y = rand() % row;
	bool flag = false;

	snake_t *temp;
	temp = snake;

	while (temp) {
		if (x == temp->x && y == temp->y) {
			flag = true;
			break;
		}
		temp = temp->next;
	}

	if (flag == true) {
		spawn_food(snake, food);
	} else {
		food->x = x;
		food->y = y;;
	}
}

static void add_part(snake_t * snake)
{
	snake_t *temp, *prev;
	temp = snake;

	while (temp->next) {
		temp = temp->next;
	}

	temp->next = malloc(sizeof(snake_t));
	prev = temp;
	temp = temp->next;
	temp->x = prev->x - 1;
	temp->y = prev->y;
	temp->dir = prev->dir;
	temp->next = NULL;
	temp->prev = prev;
}

static void init(void)
{
	initscr();
	raw();
	noecho();
	timeout(0);
	keypad(stdscr, TRUE);
	getmaxyx(stdscr, row, col);
	curs_set(0);
}

static void init_snake(snake_t * snake)
{
	snake->dir = right;
	snake->x = col / 2 + 1;
	snake->y = row / 2;
	snake->prev = NULL;

	snake->next = malloc(sizeof(snake_t));
	snake->next->x = col / 2;
	snake->next->y = row / 2;
	snake->next->dir = right;
	snake->next->next = NULL;
	snake->next->prev = snake;
}

static void print_snake(snake_t * snake)
{
	clear();
	snake_t *temp;
	temp = snake;
	char head;
	switch (snake->dir) {
	case up:
		head = '^';
		break;
	case right:
		head = '>';
		break;
	case down:
		head = 'v';
		break;
	case left:
		head = '<';
		break;
	}
	mvaddch(temp->y, temp->x, head);
	temp = temp->next;
	char body;
	while (temp) {
		if (temp->dir == down || temp->dir == up) {
			body = '|';
		} else {
			body = '-';
		}
		mvaddch(temp->y, temp->x, body);
		temp = temp->next;
	}

}

static void move_snake(snake_t * snake, food_t * food)
{
	snake_t *temp;
	temp = snake;
	while (temp->next) {
		temp = temp->next;
	}

	while (temp->prev) {
		temp->x = temp->prev->x;
		temp->y = temp->prev->y;
		temp->dir = temp->prev->dir;
		temp = temp->prev;
	}

	int ch = getch();
	switch (ch) {
	case KEY_UP:
		if (snake->dir != down) {
			snake->dir = up;
		}
		break;
	case KEY_RIGHT:
		if (snake->dir != left) {
			snake->dir = right;
		}
		break;
	case KEY_DOWN:
		if (snake->dir != up) {
			snake->dir = down;
		}
		break;
	case KEY_LEFT:
		if (snake->dir != right) {
			snake->dir = left;
		}
		break;
	default:
		break;
	}

	switch (snake->dir) {
	case up:
		if (snake->y == 0) {
			snake->y = row;
		} else {
			--snake->y;
		}
		break;
	case right:
		if (snake->x == col) {
			snake->x = 0;
		} else {
			++snake->x;
		}
		break;
	case down:
		if (snake->y == row) {
			snake->y = 0;
		} else {
			++snake->y;
		}
		break;
	case left:
		if (snake->x == 0) {
			snake->x = col;
		} else {
			--snake->x;
		}
		break;
	}

	if (snake->x == food->x && snake->y == food->y) {
		add_part(snake);
		spawn_food(snake, food);
	}

}

static void print_food(food_t * food)
{
	mvaddch(food->y, food->x, '*');
}

int main(void)
{
	init();

	snake_t *snake;
	snake = malloc(sizeof(snake_t));
	food_t *food;
	food = malloc(sizeof(food_t));
	init_snake(snake);
	add_part(snake);
	add_part(snake);
	spawn_food(snake, food);

	while (1) {
		print_snake(snake);
		print_food(food);
		move_snake(snake, food);
		if (getch() == 'q')
			break;
		sleep(1);

	}

	endwin();

	return 0;
}

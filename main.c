#include <stdbool.h>
#include <stdio.h>
#include <curses.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#define SNAKE_LEN 3
#define HEAD_UP '^'
#define HEAD_RIGHT '>'
#define HEAD_DOWN 'v'
#define HEAD_LEFT '<'
#define BODY_V '|'
#define BODY_H '-'
#define FOOD_C '#'
#define SPEED 200
#define GAME_OVER "Game Over!"
#define GAME_OVER_MSG "Press Q to quit or R to restart"

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

unsigned int row, col;
unsigned int score = 0;

static void game_over(void)
{
	char score_msg[32];
	sprintf(score_msg, "Your Score: %d", score);
	erase();
	attron(COLOR_PAIR(2));
	mvprintw(row/2 - 1, (col - strlen(GAME_OVER))/2, GAME_OVER);
	mvprintw(row/2, (col - strlen(score_msg))/2, score_msg);
	mvprintw(row/2 + 1, (col - strlen(GAME_OVER_MSG))/2, GAME_OVER_MSG);
	attroff(COLOR_PAIR(2));
	timeout(-1);

	int ch;
	while(1) {
		ch = getch();
		if (ch == 'q') {
			return;
		}
	}


}
/*
 * Spawns the food at random co ordinates
 */
static void spawn_food(snake_t * snake, food_t * food)
{
	int x, y;
	x = rand() % col;
	y = rand() % row;
	bool flag = false;

	snake_t *temp;
	temp = snake;

	// Check if location is on the snake
	while (temp) {
		if (x == temp->x && y == temp->y) {
			flag = true;
			break;
		}
		temp = temp->next;
	}

	// Try again
	if (flag == true) {
		spawn_food(snake, food);
	} else {
		food->x = x;
		food->y = y;;
	}
}

// Increase the snake length
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
	switch (prev->dir) {
	case up:
		temp->x = prev->x;
		temp->y = prev->y - 1;
		break;
	case right:
		temp->x = prev->x - 1;
		temp->y = prev->y;
		break;
	case down:
		temp->x = prev->x;
		temp->y = prev->y + 1;
		break;
	case left:
		temp->x = prev->x + 1;
		temp->y = prev->y;
		break;
	}

	temp->dir = prev->dir;
	temp->next = NULL;
	temp->prev = prev;
}

/*
 * Initialise ncurses
 */
static void init(void)
{
	initscr();
	start_color();
	init_pair(1, COLOR_RED, COLOR_BLACK);
	init_pair(2, COLOR_GREEN, COLOR_BLACK);
	init_pair(3, COLOR_YELLOW, COLOR_BLACK);
	raw();
	noecho();
	timeout(SPEED);
	keypad(stdscr, TRUE);
	getmaxyx(stdscr, row, col);
	curs_set(0);
}

/*
 * Initialise snake
 */
static void init_snake(snake_t * snake)
{
	snake->dir = right;
	snake->x = col / 2 + 1;
	snake->y = row / 2;
	snake->prev = NULL;
	snake->next = NULL;

	for (int i = 0; i < SNAKE_LEN; ++i) {
		add_part(snake);
	}
}

/*
 * Print snake
 */
static void print_snake(snake_t * snake)
{
	erase();
	snake_t *temp;
	temp = snake;

	// Drawn head
	char head;
	switch (snake->dir) {
	case up:
		head = HEAD_UP;
		break;
	case right:
		head = HEAD_RIGHT;
		break;
	case down:
		head = HEAD_DOWN;
		break;
	case left:
		head = HEAD_LEFT;
		break;
	}
	attron(COLOR_PAIR(1));
	mvaddch(temp->y, temp->x, head);
	attroff(COLOR_PAIR(1));

	// Draw body
	temp = temp->next;
	char body;

	attron(COLOR_PAIR(2));
	while (temp) {
		if (temp->dir == down || temp->dir == up) {
			body = BODY_V;
		} else {
			body = BODY_H;
		}
		mvaddch(temp->y, temp->x, body);
		temp = temp->next;
	}
	attroff(COLOR_PAIR(2));

	mvprintw(row - 1, 0, "Score: %d", score);
}

/*
 * Move snake in direction of head, checking if it collides with food
 */
static bool move_snake(snake_t * snake, food_t * food)
{
	snake_t *temp;
	temp = snake;
	bool alive = true;
	static int speed = SPEED;

	// Find last part
	while (temp->next) {
		temp = temp->next;
	}

	// Set part location to previous part location
	while (temp->prev) {
		temp->x = temp->prev->x;
		temp->y = temp->prev->y;
		temp->dir = temp->prev->dir;
		temp = temp->prev;
	}

	// Move snake head in original or specified direction
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
	case 'q':
		alive = false;
		break;
	default:
		break;
	}

	if (alive == false)
		return alive;

	// Let snake wrap around
	switch (snake->dir) {
	case up:
		if (snake->y == 0) {
			snake->y = row - 1;
		} else {
			--snake->y;
		}
		break;
	case right:
		if (snake->x == col - 1) {
			snake->x = 0;
		} else {
			++snake->x;
		}
		break;
	case down:
		if (snake->y == row - 1) {
			snake->y = 0;
		} else {
			++snake->y;
		}
		break;
	case left:
		if (snake->x == 0) {
			snake->x = col - 1;
		} else {
			--snake->x;
		}
		break;
	}

	// If snake collides with food, respawn food and increase snake length
	if (snake->x == food->x && snake->y == food->y) {
		add_part(snake);
		speed = speed >= 50 ? speed * 0.9 : 50;
		score += 10;
		timeout(speed);
		spawn_food(snake, food);
	}

	temp = snake->next;

	while (temp) {
		if (temp->x == snake->x && temp->y == snake->y) {
			alive = false;
			break;
		} else {
			temp = temp->next;
		}
	}

	return alive;

}

/*
 * Prints the food
 */
static void print_food(food_t * food)
{
	attron(COLOR_PAIR(3));
	mvaddch(food->y, food->x, FOOD_C);
	attroff(COLOR_PAIR(3));
}

int main(void)
{
	// Initialise ncurses
	init();
	// Initialise snake
	snake_t *snake;
	snake = malloc(sizeof(snake_t));
	food_t *food;
	food = malloc(sizeof(food_t));
	init_snake(snake);
	spawn_food(snake, food);

	// Game loop
	while (1) {
		print_snake(snake);
		print_food(food);
		if (move_snake(snake, food) == false) {
			game_over();
			break;
		}

	}

	// Cleanup before exit
	free(food);
	snake_t *temp = snake;
	snake_t *prev = NULL;

	while (temp) {
		prev = temp;
		temp = temp->next;
		free(prev);
	}

	clear();
	endwin();
	return 0;
}

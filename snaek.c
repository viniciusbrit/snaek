// SNAEK by viniciusbrit
// Version: 0.10 [16/01/2023]

// Compile using: gcc -o snaek snaek.c -lncurses

/*
Things to do:
-Implement grace turns before an imminent death
-Add timer to the top title bar
*/

#include <curses.h>
#include <stdlib.h>
#include <time.h>

#define MAX_X 80
#define MAX_Y 18
#define INITIAL_LENGTH 5

// direction constants
#define UP 1
#define DOWN 2
#define LEFT 3
#define RIGHT 4

// for starting the game
int gamestatus = 0;

struct Snake
{
    int x[MAX_X * MAX_Y];
    int y[MAX_X * MAX_Y];
    int length;
    int direction;
    int previousdir;
};
struct Snake snake;

struct Dot
{
    int x;
    int y;
};
#define MAX_DOTS 15
struct Dot dots[MAX_DOTS];

int numDots = 0;
int score = 0;

void spawnDot()
{
    // generate a random position for the dot
    int x = rand() % (MAX_X - 2) + 1; // generate a number between 1 and MAX_X - 2
    int y = rand() % (MAX_Y - 2) + 6; // generate a number between 6 (wall at 5) and MAX_Y - 2

    // add the dot to the array
    dots[numDots].x = x;
    dots[numDots].y = y;
    numDots++;
}

void moveSnake()
{
    int newX = snake.x[0];
    int newY = snake.y[0];
    int oldX, oldY;

    // update the position of the snake based on its direction
    switch (snake.direction)
    {
    case UP:
        newY--;
        break;
    case DOWN:
        newY++;
        break;
    case LEFT:
        newX--;
        break;
    case RIGHT:
        newX++;
        break;
    }
    // check if the snake is colliding with itself
    for (int i = 1; i < snake.length; i++)
    {
        if (snake.x[0] == snake.x[i] && snake.y[0] == snake.y[i])
        {
            // the snake has collided with itself, so exit the game
            endwin();
            printf("Game Over! [Total: %d]\n", score);
            exit(0);
        }
    }

    // check if the snake is colliding with the border walls
    if (snake.x[0] == 0 || snake.x[0] == MAX_X - 1 || snake.y[0] == 5 || snake.y[0] == MAX_Y + 5)
    {
        // the snake has collided with a border wall, so exit the game
        endwin();
        printf("Game Over! [Total: %d]\n", score);
        exit(0);
    }

    // update the positions of all the segments of the snake
    for (int i = snake.length - 1; i > 0; i--)
    {
        snake.x[i] = snake.x[i - 1];
        snake.y[i] = snake.y[i - 1];
    }
    snake.x[0] = newX;
    snake.y[0] = newY;
}

int main()
{
    initscr();
    noecho();
    cbreak();
    start_color();

    // color pairs
    init_pair(1, COLOR_RED, COLOR_BLACK);
    init_pair(2, COLOR_GREEN, COLOR_BLACK);
    init_pair(3, COLOR_YELLOW, COLOR_BLACK);
    init_pair(4, COLOR_WHITE, COLOR_BLACK);
    init_pair(5, COLOR_BLACK, COLOR_WHITE);

    clear();
    attron(COLOR_PAIR(4));
    move(6, 20);
    printw("####### ###    ##  #####  ###### ##  ###");
    move(7, 20);
    printw("###     ####   ## ##   ## ##     ## ###");
    move(8, 20);
    printw("####### ## ##  ## ####### #####  ######");
    move(9, 20);
    printw("    ### ##  ##### ##   ## ###    ### ###");
    move(10, 20);
    printw("####### ##   #### ##   ## ###### ###  ##"); 
    attroff(COLOR_PAIR(4));
    attron(COLOR_PAIR(5));
    move(12, 32);
    printw("PRESS E TO START");
    move(13, 32);
    printw("PRESS Q TO QUIT");
    attroff(COLOR_PAIR(6));

    // game start/quit menu
    int startkey = getch();

    switch (startkey)
    {
        case 'q':
            endwin();
            exit(0);
        case 'e':
            gamestatus = 1;
    }

    // initialize the snake
    snake.x[0] = MAX_X / 2;
    snake.y[0] = MAX_Y / 2;
    snake.length = INITIAL_LENGTH;
    snake.direction = RIGHT;

    for (int i = 1; i < INITIAL_LENGTH; i++)
    {
        snake.x[i] = snake.x[i - 1] - 1;
        snake.y[i] = snake.y[0];
    }

    // seed the random number generator
    srand(time(NULL));

    // spawn the initial dots
    for (int i = 0; i < MAX_DOTS; i++)
    {
        spawnDot();
    }

    timeout(100);

    while (gamestatus)
    {
        int c = getch();
        // update the direction of the snake based on the user input
        switch (c)
        {
        case 'w':
            snake.previousdir = snake.direction;
            if (snake.previousdir != DOWN)
            {
            snake.direction = UP;
            timeout(175);
            }
            break;
        case 's':
            snake.previousdir = snake.direction;
            if (snake.previousdir != UP)
            {
            snake.direction = DOWN;
            timeout(175);
            }
            break;
        case 'a':
            snake.previousdir = snake.direction;
            if (snake.previousdir != RIGHT)
            {
            snake.direction = LEFT;
            timeout(100);
            }
            break;
        case 'd':
            snake.previousdir = snake.direction;
            if (snake.previousdir != LEFT)
            {
            snake.direction = RIGHT;
            timeout(100);
            }
            break;
        }
        moveSnake();

        // check if the head of the snake is colliding with any dots
        for (int i = 0; i < numDots; i++)
        {
            if (snake.x[0] == dots[i].x && snake.y[0] == dots[i].y)
            {
                // remove the dot from the array
                for (int j = i; j < numDots - 1; j++)
                {
                    dots[j] = dots[j + 1];
                }
                numDots--;

                // increase the size and score of the snake
                snake.length++;
                score++;
                // add code to increase the score here

                // spawn a new dot"
                spawnDot();
            }
        }

        // SCREEN DRAWING ROUTINE
        // clear the screen
        clear();

        // draw the snake
        attron(COLOR_PAIR(1));
        mvaddch(snake.y[0], snake.x[0], ACS_BLOCK);
        attroff(COLOR_PAIR(1));
        for (int i = 1; i < snake.length; i++)
        {
            attron(COLOR_PAIR(2));
            mvaddch(snake.y[i], snake.x[i], ACS_BLOCK);
            attroff(COLOR_PAIR(2));
        }

        // draw the dots
        for (int i = 0; i < numDots; i++)
        {
            attron(COLOR_PAIR(3));
            mvaddch(dots[i].y, dots[i].x, ACS_DIAMOND);
            attroff(COLOR_PAIR(3));
        }

        // draw the title bar
        attron(COLOR_PAIR(3));
        move(0, 20);
        printw("####### ###    ##  #####  ###### ##  ###");
        move(1, 20);
        printw("###     ####   ## ##   ## ##     ## ###");
        move(2, 20);
        printw("####### ## ##  ## ####### #####  ######");
        move(3, 20);
        printw("    ### ##  ##### ##   ## ###    ### ###");
        move(4, 20);
        printw("####### ##   #### ##   ## ###### ###  ##"); 
        move(1,3);
        printw("[SCORE: %d]", score);
        attroff(COLOR_PAIR(3));

        // draw the border
        // drawing the border last eliminates terminal cursor from showing up elsewhere
        for (int i = 0; i < MAX_X; i++)
        {
            mvaddch(5, i, ACS_CKBOARD); // starting border height is 5 because of the title
            mvaddch(MAX_Y + 5, i, ACS_CKBOARD); //
        }
        for (int i = 5; i < (MAX_Y + 5); i++) // starting border height is 5 because of the title
        {
            mvaddch(i, 0, ACS_CKBOARD);
            mvaddch(i, MAX_X - 1, ACS_CKBOARD);
        }

        // refresh the screen
        refresh();
    }
}

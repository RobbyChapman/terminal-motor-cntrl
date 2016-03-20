/**
 * Created by Robby Chapman on 3/19/16.
 */

#include <ncurses.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    short left;
    short right;
} MotorSpeed;

typedef struct {
    short row;
    short col;
} CURSOR_COORDS;

#pragma mark - Constants //259
static const bool DEBUG = true;
static const int QUIT_KEY = 113;
static const int INTERACTIVE_KEY = 105;
static const int MOTOR_MAX = 3200;
static const int MOTOR_IDLE = MOTOR_MAX / 2;
static const char *HEADER_TITLE = (char *) "Motor Controller";
static const char *INTERACTIVE_MESSAGE = (char *) "Press 'i' for interactive mode, 'q' to quit";

#pragma mark - Instance
bool shouldLoop = true;
CURSOR_COORDS coords;
WINDOW *window;
short speedLeft = MOTOR_IDLE, speedRight = MOTOR_IDLE;

#pragma mark - Prototypes

void turnLeft();

void turnRight();

void increaseSpeed();

void decreaseSpeed();

void validateTerminal();

void initScreen();

void setColors();

void pollArrowInput();

void destroyWindow();

MotorSpeed calculateMotorSpeed(bool &);

bool checkInteractiveMode();

short run();

#pragma mark - Main

int main(int argc, char *argv[]) {

    return run();
}

short run() {

    initScreen();
    if (checkInteractiveMode()) pollArrowInput();
    destroyWindow();

    return 0;
}

bool checkInteractiveMode() {

    while (1) {
        char userInput = (char) getch();;
        switch (userInput) {
            case (char) INTERACTIVE_KEY:
                return true;
            case (char) QUIT_KEY:
                return false;
            default:
                break;
        }
    }
}

void destroyWindow() {

    refresh();
    endwin();
}

void pollArrowInput() {

    keypad(stdscr, TRUE);

    int x, y, width, height;
    getmaxyx(window, y, x);
    width = 40;
    height = 10;
    setColors();
    WINDOW *temp = newwin(height, width, y/2 + 2, (x/2) - width/2 + 5);

    box(temp, '#', '*');
    setColors();
    refresh();
    wrefresh(temp);

    while (shouldLoop) {
        MotorSpeed speed = calculateMotorSpeed(shouldLoop);

        refresh();

        int marginLeft = 3;
        int marginTop = 3;

        attron(COLOR_PAIR(1));
        mvwprintw(temp, marginTop, marginLeft,  "Speed Left: %i", speed.left);
        mvwprintw(temp, marginTop + 2, marginLeft,  "Speed Right: %i", speed.right);

        refresh();
        attroff(COLOR_PAIR(1));

        setColors();
        wrefresh(temp);
        refresh();
        setColors();
        refresh();
        wrefresh(temp);
    }
}

void initScreen() {

    initscr();
    cbreak();
    validateTerminal();

    getmaxyx(stdscr, coords.row, coords.col);

    int x, y, margin, headerHeight, headerWidth;
    margin = 5;

    getmaxyx(stdscr, y, x);

    window = newwin(y - (margin * 2), x - (margin * 2), margin, margin);

    getmaxyx(window, y, x);

    headerHeight = 5;
    headerWidth = 50;

    WINDOW *titleWin = newwin(headerHeight, headerWidth, margin, (margin + x / 2) - headerWidth / 2);

    box(window, '#', '*');

    wborder(titleWin, '#', '#', '*', '-', '*', '*', '+', '+');

    getmaxyx(titleWin, y, x);

    mvwprintw(titleWin, y / 2, x / 2 - ((int) strlen(HEADER_TITLE) / 2), "%s", HEADER_TITLE);

    getmaxyx(window, y, x);
    move((y / 2) + 7, x / 2 + 4);
    mvwprintw(window, y / 2, x / 2 - ((int) strlen(INTERACTIVE_MESSAGE) / 2), "%s", INTERACTIVE_MESSAGE);

    wrefresh(stdscr);
    wrefresh(window);
    wrefresh(titleWin);
    refresh();

    setColors();
}

void setColors() {

    start_color();
    init_pair(1, COLOR_GREEN, COLOR_BLACK);
    attron(COLOR_PAIR(1));
}

void validateTerminal() {

    if (!has_colors()) {
        endwin();
        refresh();
        printf("%s\n", "Your terminal does not support colors!");
        exit(1);
    }
}

MotorSpeed calculateMotorSpeed(bool &proceed) {

    int directionInput = getch();
    switch (directionInput) {
        case KEY_DOWN:
            decreaseSpeed();
            break;
        case KEY_UP:
            increaseSpeed();
            break;
        case KEY_RIGHT:
            turnRight();
            break;
        case KEY_LEFT:
            turnLeft();
            break;
        case QUIT_KEY:
            proceed = false;
            break;
        default:
            break;
    }

    return (MotorSpeed) {speedLeft, speedRight};
}

void increaseSpeed() {

    ++speedLeft;
    ++speedRight;
}

void decreaseSpeed() {

    --speedLeft;
    --speedRight;
}

void turnLeft() {

    --speedLeft;
    ++speedRight;
}

void turnRight() {

    ++speedLeft;
    --speedRight;
}
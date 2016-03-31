/**
 * Created by Robby Chapman on 3/19/16.
 */

#include "TerminalMotorController.h"
#include <ncurses.h>
#include <string.h>

typedef struct {
    short left;
    short right;
} MotorSpeed;

typedef struct {
    short row;
    short col;
} CURSOR_COORDINATES;

#pragma mark - Constants
static const uint8_t QUIT_KEY = 113;
static const uint8_t INTERACTIVE_KEY = 105;
static const uint16_t MOTOR_MAX = 3200;
static const int16_t MOTOR_MIN = -MOTOR_MAX;
static const uint8_t NORMALIZED_MAX = 10;
static const int8_t NORMALIZED_MIN = -NORMALIZED_MAX;
static const int16_t NORMALIZED_DELTA = (const __int16_t) ((NORMALIZED_MIN * -1) + MOTOR_MAX) / (NORMALIZED_MIN * -1) + NORMALIZED_MAX;
static const int16_t MOTOR_IDLE = MOTOR_MAX / 2;
static const char * HEADER_TITLE = (char *) "Motor Controller";
static const char * INTERACTIVE_MESSAGE = (char *) "Press 'i' for interactive mode, 'q' to quit";

#pragma mark - Instance
bool shouldLoop = true;
CURSOR_COORDINATES coordinates;
WINDOW *window;
WINDOW *messageWindow;
int16_t speedLeft = (int16_t) MOTOR_IDLE, speedRight = (__uint16_t) MOTOR_IDLE;

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
MotorSpeed calculateMotorSpeed(bool);
bool checkInteractiveMode();
int8_t normalizeMotorSpeed(int16_t);

void initMotorCtrl() {

    initScreen();
    if (checkInteractiveMode()) pollArrowInput();
    destroyWindow();
}

int8_t normalizeMotorSpeed(int16_t x) {

    /* a + (x-A)*(b-a)/(B-A) */
    return (int8_t) (NORMALIZED_MIN + (x - MOTOR_MIN) * (NORMALIZED_MAX - NORMALIZED_MIN) / (MOTOR_MAX - MOTOR_MIN));
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

//TODO: This needs a major refactor. I was in a rush
void pollArrowInput() {

    keypad(stdscr, TRUE);
    wclear(messageWindow);

    int x, y, width, height;
    getmaxyx(window, y, x);
    wrefresh(messageWindow);
    wrefresh(window);

    width = 40;
    height = 10;
    setColors();
    refresh();
    WINDOW *temp = newwin(height, width, y / 2 + 2, (x / 2) - width / 2 + 5);

    box(temp, '#', '*');
    setColors();
    refresh();
    wrefresh(temp);

    while (shouldLoop) {
        MotorSpeed speed = calculateMotorSpeed(shouldLoop);
        //printf("Speed is %i", normalizeMotorSpeed(speed.left)); <-- Debug
        setColors();
        refresh();

        int marginLeft = 3;
        int marginTop = 3;

        char *speedLeftStr = (char *) "Speed Left: %i";

        attron(COLOR_PAIR(1));
        mvwprintw(temp, marginTop, marginLeft + 8, speedLeftStr, speed.left);
        mvwprintw(temp, marginTop + 2, marginLeft + 8, "Speed Right: %i", speed.right);

        refresh();
        attroff(COLOR_PAIR(1));

        setColors();
        wrefresh(temp);
        refresh();
        setColors();
        refresh();
        wrefresh(temp);
    }
    setColors();
}

//TODO: This needs a major refactor. I was in a rush
void initScreen() {

    initscr();
    cbreak();
    noecho();
    nodelay(window, true);
    validateTerminal();

    getmaxyx(stdscr, coordinates.row, coordinates.col);

    int x, y, margin, headerHeight, headerWidth;
    margin = 5;

    getmaxyx(stdscr, y, x);

    window = newwin(y - (margin * 2), x - (margin * 2), margin, margin);

    getmaxyx(window, y, x);

    headerHeight = 5;
    headerWidth = 50;

    WINDOW *titleWin = newwin(headerHeight, headerWidth, margin, (margin + x / 2) - headerWidth / 2);

    box(window, '#', '*');

    wborder(titleWin, '#', '#', '*', '*', '*', '*', ACS_LLCORNER, ACS_LRCORNER);

    getmaxyx(titleWin, y, x);

    mvwprintw(titleWin, y / 2, x / 2 - ((int) strlen(HEADER_TITLE) / 2), "%s", HEADER_TITLE);

    getmaxyx(window, y, x);
    move((y / 2) + 7, x / 2 + 4);

    int w = (int) strlen(INTERACTIVE_MESSAGE);
    int messagePadding = 6;

    messageWindow = newwin(5, w + messagePadding, (y / 2) + 4, (x / 2) - (w / 2) + 1);
    box(messageWindow, '#', '*');

    getmaxyx(messageWindow, y, x);
    mvwprintw(messageWindow, y / 2, (x / 2) - (w / 2), "%s", INTERACTIVE_MESSAGE);

    refresh();

    wrefresh(stdscr);
    wrefresh(window);
    wrefresh(titleWin);
    wrefresh(messageWindow);
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

MotorSpeed calculateMotorSpeed(bool proceed) {

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

bool isGreaterThanMin(short speed) {

    return speed >= MOTOR_MIN;
}

bool isLessThanMax(short speed) {

    return speed <= MOTOR_MAX;
}

void increaseSpeed() {

    if (isLessThanMax(speedLeft)) speedLeft += NORMALIZED_DELTA;
    if (isLessThanMax(speedRight)) speedRight += NORMALIZED_DELTA;
}

void decreaseSpeed() {

    if (isGreaterThanMin(speedLeft)) speedLeft -= NORMALIZED_DELTA;
    if (isGreaterThanMin(speedRight)) speedRight -= NORMALIZED_DELTA;
}

void turnLeft() {

    if (isGreaterThanMin(speedLeft)) speedLeft -= NORMALIZED_DELTA;
    if (isLessThanMax(speedRight)) speedRight += NORMALIZED_DELTA;
}

void turnRight() {

    if (isLessThanMax(speedLeft)) speedLeft += NORMALIZED_DELTA;
    if (isGreaterThanMin(speedRight)) speedRight -= NORMALIZED_DELTA;
}

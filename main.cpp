/**
 * Created by Robby Chapman on 3/19/16.
 */

#include <ncurses.h>
#include <stdlib.h>

typedef struct {
    short left;
    short right;
} MotorSpeed;

/* Constants */
static const bool DEBUG = true;
static const int QUIT_KEY = 113;
static const int MOTOR_MAX = 3200;
static const int MOTOR_IDLE = MOTOR_MAX / 2;

/* Instance */
bool shouldLoop = true;
short speedLeft = MOTOR_IDLE, speedRight = MOTOR_IDLE;

/* Prototypes */
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
short run();

int main(int argc, char *argv[]) {

    return run();
}

short run() {

    initScreen();
    setColors();
    pollArrowInput();
    destroyWindow();

    return 0;
}

void destroyWindow() {

    refresh();
    endwin();
}

void pollArrowInput() {

    while (shouldLoop) {
        MotorSpeed speed = calculateMotorSpeed(shouldLoop);
#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCSimplifyInspection"
        if (DEBUG) printf("Motor Speed is %i:::%i", speed.left, speed.right);
#pragma clang diagnostic pop
        refresh();
    }
}

void initScreen() {

    initscr();
    validateTerminal();
    keypad(stdscr, TRUE);
}

void setColors() {

    start_color();
    init_pair(1, COLOR_RED, COLOR_CYAN);
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
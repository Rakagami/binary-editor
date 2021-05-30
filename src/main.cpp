#include <curses.h>
#include <stdlib.h>
#include <stdint.h>
#include <vector>
#include <time.h>

// Unix spcific
#include <unistd.h>

using namespace std;

// Size of a word in bit
#define WORDSIZE 8
// How many columns are displayed per line (one column is one word)
#define WORDCOLS 4

#define MAXBUF 30

int inputhandler(WINDOW * win);
int samplebuf(vector<uint8_t> buf, int i);
void displaybinwin(WINDOW * binwin, vector<uint8_t> buf);
void displayhexwin(WINDOW * hexwin, vector<uint8_t> buf);
// We want 3 Windows: +++++++++++++++++++++ + input             + +                   +
// +                   +
// +++++++++++++++++++++
// + hex + binary      +
// +     +             +
// +     +             +
// +++++++++++++++++++++
int main(int argc, char * argv[]){
    /* Initilaizations */
    vector<uint8_t> buf;
    size_t cnt = 0; // Count of written bits
    buf.clear();
    buf.resize(MAXBUF);
    buf[0] = 0x95;
    buf[1] = 0xF2;
    buf[2] = 0x6F;
    

    /* NCURSES START */
    initscr();
    cbreak();
    noecho();

    int yBeg, xBeg, yMax, xMax;
    getmaxyx(stdscr, yMax, xMax);

    WINDOW *inputwin = newwin(yMax/2, xMax, 0, 0);
    WINDOW *hexwin = newwin(yMax/2, xMax/3, yMax/2, 0);
    WINDOW *binwin = newwin(yMax/2, (xMax * 2)/3, yMax/2, xMax/3);
    refresh();

    box(inputwin, 0, 0);
    box(hexwin, 0, 0);
    box(binwin, 0, 0);
    wrefresh(inputwin);
    wrefresh(hexwin);
    wrefresh(binwin);

    keypad(stdscr, true);

    while(1)
    {
        inputhandler(inputwin);
        displaybinwin(binwin, buf);
        displayhexwin(hexwin, buf);
        refresh();
    }

    getch();

    endwin();
    /* NCURSES END */
}

int inputhandler(WINDOW * win) {
    int c = getch();
    wmove(win, 1, 1);
    switch(c) {
        case KEY_UP:
            wprintw(win, "You pressed up!");
            break;
        case KEY_DOWN:
            wprintw(win, "You pressed down!");
            break;
        case KEY_LEFT:
            wprintw(win, "You pressed left!");
            break;
        case KEY_RIGHT:
            wprintw(win, "You pressed right!");
            break;
        case 'o':
            wprintw(win, "You pressed o (O)!");
            break;
        case 'i':
            wprintw(win, "You pressed i (1)!");
            break;
        default:
            wprintw(win, "PRESS A BUTTON");
    }
    wrefresh(win);

    return c;
}

int samplebuf(vector<uint8_t> buf, int i) {
    int idx = i/8;
    int mask = 1 << (7 - (i % 8));
    return (buf[idx] & mask) == 0 ? 0 : 1; 
}

void displaybinwin(WINDOW * binwin, vector<uint8_t> buf) {
    int startx = 1, starty = 1;
    int offset = 0;
    wmove(binwin, starty, startx);
    //Iterating through each bit
    for(int i=0; i<(buf.size() * 8); i++) {
        if(i%(WORDCOLS * WORDSIZE) == 0 && i != 0)
            wmove(binwin, starty + (++offset), startx);
        else if(i%WORDSIZE == 0 && i != 0)
            wprintw(binwin, " ");
        wprintw(binwin, "%d", samplebuf(buf, i));
    }
    wrefresh(binwin);
}

void displayhexwin(WINDOW * hexwin, vector<uint8_t> buf) {
    int startx = 1, starty = 1;
    int offset = 0;
    wmove(hexwin, starty, startx);
    //iterating through
    int colsize = (WORDSIZE * WORDCOLS) / 8;
    for(int i=0; i<buf.size(); i++) {
        if(i%colsize == 0)
            wprintw(hexwin, "0x ");
        wprintw(hexwin, "%02x ", buf[i]);
        if(i%colsize == colsize-1)
            wmove(hexwin, starty + (++offset), startx);
    }
    wrefresh(hexwin);
}

#include <curses.h>
#include <stdlib.h>
#include <stdint.h>
#include <vector>
#include <time.h>

// Unix spcific #include <unistd.h> 
using namespace std;

// Size of a word in bit
#define WORDSIZE 8
// How many columns are displayed per line (one column is one word)
#define WORDCOLS 4
#define MAXBUF 30

#define BIGENDIAN true

int inputhandler(struct glbctx &ctx, WINDOW * win);
int samplebuf(vector<uint8_t> buf, int i);
void displaybinwin(WINDOW * binwin, vector<uint8_t> buf);
void displayhexwin(WINDOW * hexwin, vector<uint8_t> buf);

struct glbctx {
    vector<uint8_t> buf;
    uint8_t inputbuf;
    size_t bitptr;
    size_t byteptr;
};

// We want 3 Windows: 
// +++++++++++++++++++++
// + input             + 
// +                   +
// +                   +
// +++++++++++++++++++++
// + hex + binary      +
// +     +             +
// +     +             +
// +++++++++++++++++++++
int main(int argc, char * argv[]){
    /* Initilaizations */
    struct glbctx ctx;
    ctx.inputbuf = 0x0;
    ctx.bitptr = 0; 
    ctx.byteptr = 0;

    ctx.buf.clear();
    ctx.buf.resize(MAXBUF);

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
        inputhandler(ctx, inputwin);
        displaybinwin(binwin, ctx.buf);
        displayhexwin(hexwin, ctx.buf);
        refresh();
    }

    getch();

    endwin();
    /* NCURSES END */
}

int inputhandler(struct glbctx &ctx, WINDOW * win) {
    int c = getch();
    wmove(win, 1, 1);
    uint8_t byte, mask;
    switch(c) {
        //case KEY_UP:
        //    wprintw(win, "You pressed up!");
        //    break;
        //case KEY_DOWN:
        //    wprintw(win, "You pressed down!");
        //    break;
        //case KEY_LEFT:
        //    wprintw(win, "You pressed left!");
        //    break;
        //case KEY_RIGHT:
        //    wprintw(win, "You pressed right!");
        //    break;
        case 'o':
            wprintw(win, "You pressed o (O)!");
            byte = ctx.buf[ctx.byteptr];
            mask;
            if(BIGENDIAN) {
                mask = 0x80 >> ctx.bitptr++;
                mask = ~mask;
            } else {
                mask = 0x1 << ctx.bitptr++;
                mask = ~mask;
            }
            ctx.buf[ctx.byteptr] = byte & mask;
            ctx.byteptr += ctx.bitptr >> 3;
            ctx.bitptr = ctx.bitptr % 8;
            break;
        case 'i':
            wprintw(win, "You pressed i (1)!");
            byte = ctx.buf[ctx.byteptr];
            mask;
            if(BIGENDIAN) {
                mask = 0x80 >> ctx.bitptr++;
            } else {
                mask = 0x1 << ctx.bitptr++;
            }
            ctx.buf[ctx.byteptr] = byte | mask;
            ctx.byteptr += ctx.bitptr >> 3;
            ctx.bitptr = ctx.bitptr % 8;
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

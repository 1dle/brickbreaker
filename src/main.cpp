#include <iostream>
#include <ncurses.h>

int main(){

    initscr();

    printw("henlo");
    
    box(stdscr, '*', '*');
    refresh();
    //getch();

    endwin();
    return 0;
}

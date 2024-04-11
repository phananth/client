/*
* FILE : chat-ui.c
* PROJECT : SENG2030 - Assignment #4
* PROGRAMMER : Anthony Phan
* FIRST VERSION : April 3, 2024
* DESCRIPTION :
* The functions in this file are used for setting up ncurses for the client.
*/

#include "../inc/chat-ui.h"

WINDOW *outgoing_win, *incoming_win;

// FUNCTION     : initNcurses
// DESCRIPTION  : used to initialize ncurses to use within the app
// PARAMETERS   : nothing
// RETURNS      : nothing
void initNcurses() {
    initscr();
    cbreak();
    noecho();

    // allow color
    start_color();

    // make color pair
    init_pair(1, COLOR_BLACK, COLOR_WHITE);

    // set the whole screen to the color pair
    wbkgd(stdscr, COLOR_PAIR(1));
}

// FUNCTION     : createWindows
// DESCRIPTION  : used to create windows in the chat app
// PARAMETERS   : nothing
// RETURNS      : nothing
void createWindows() {

    // get the window dimensions
    int max_y, max_x;
    getmaxyx(stdscr, max_y, max_x);

    // center the headings 
    int centerMessages = (max_x - strlen("Messages")) / 2;
    int centerOutgoing = (max_x - strlen("Outgoing Message")) / 2;

    // make incoming and outgoing windows
    incoming_win = newwin(15, max_x, 3, 0);
    outgoing_win = newwin(3, max_x, max_y - 3, 0);

    // give the windows a box
    box(incoming_win, 0, 0);
    box(outgoing_win, 0, 0);

    // display the headings
    mvprintw(1, centerMessages, "Messages");
    mvprintw(max_y - 5, centerOutgoing, "Outgoing Message");

    // refresh the window
    refresh();
    wrefresh(incoming_win);
    wrefresh(outgoing_win);

    // display > in the outgoing window 
    mvwprintw(outgoing_win, 1, 1, ">");
    wrefresh(outgoing_win);

}

// FUNCTION     : cleanupNcurses
// DESCRIPTION  : clean up ncurse by ending the windows and deleting incoming and outgoing windows
// PARAMETERS   : nothing
// RETURNS      : nothing
void cleanupNcurses() {
    endwin();
    delwin(outgoing_win);
    delwin(incoming_win);
}
#ifndef _MY_GUI_H_
#define _MY_GUI_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <curses.h>

#define MAX_NUM_WIN  5 //MAX NUMBER of WINDOWS, win[1] for dialague win[0] for user list
WINDOW* win[ MAX_NUM_WIN ] = {0}; 


void gui_active_input_win(){ // win[2] is the input window
      touchwin( win[2] );
      wrefresh( win[2] );
}

int gui_init(){ // initiate screen and color pair
    initscr();
    refresh();
    start_color();
    init_pair(1, COLOR_WHITE, COLOR_CYAN);
    init_pair(2, COLOR_YELLOW, COLOR_BLUE);
    init_pair(3, COLOR_BLACK, COLOR_RED);
}

int gui_init_wins(){

    // Location fo Windows
    win[0] = newwin(20,10,0,0);
    win[1] = newwin(20,50,0,10);
    win[2] = newwin(1,60,20,0);

    // Color of Windows
    wbkgd( win[0], COLOR_PAIR(1));
    wbkgd( win[1], COLOR_PAIR(2)|A_BOLD);
    wbkgd( win[2], COLOR_PAIR(3)|A_BOLD);

    scrollok( win[0], TRUE);
    scrollok( win[1], TRUE);

    keypad( win[0], TRUE);
    keypad( win[1], TRUE);
    keypad( win[2], TRUE);
    keypad( stdscr, TRUE);
}

void gui_refresh_wins(){      //refresh windows
    for(int i = 0; i < MAX_NUM_WIN; i++){
        if(win[i]){
          touchwin( win[i]);
          wrefresh( win[i]);
        }
    }
}

void gui_user_win(dchat * p_chat){ //win[0] is the user list
  wclear(win[0]);
  wprintw(win[0], "==users==\n");
  for (auto iter = p_chat->all_members_list.begin(); iter != p_chat->all_members_list.end(); iter++){
    wprintw(win[0],"%s\n", &(iter->first[0]) );
    wprintw(win[0],"%s\n", &(iter->second[0]) );
  }  
  wrefresh(win[0]);
}

void gui_promote_input_win(char* str){
  gui_active_input_win();
  wclear( win[2]);
  wprintw( win[2], "%s", str);
  wrefresh( win[2]);
}

void gui_get_string(char* str){
  gui_active_input_win();
  wgetnstr( win[2], str, 4096);
}

void gui_print_string(char* str){
  wprintw( win[1], "%s\n", str);
  wrefresh( win[1] );
}

void gui_end_win(){
  endwin();
}


#endif // _MY_GUI_H_


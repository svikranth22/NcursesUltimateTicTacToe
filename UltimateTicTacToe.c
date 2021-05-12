#include <stdio.h>
#include <ncurses.h>
#include <stdlib.h>
#include <string.h>

#define DIM 3
#define SQ_HEIGHT 13
#define SQ_WIDTH 25

typedef struct Player {
    char name[10];
    char mark;
    struct Player* next;
} Player;

Player *link_players(char p1name[], char p1mark, char p2name[], char p2mark) {
   Player *p1 = (Player*) malloc(sizeof(Player));
   Player *p2 = (Player*) malloc(sizeof(Player));
   strcpy(p1->name, p1name);
   p1->mark = p1mark;
   p1->next = p2;
   strcpy(p2->name, p2name);
   p2->mark = p2mark;
   p2->next = p1;  
   return p1;
}

typedef struct Board {
    int xval[DIM];
    int yval[DIM];
    char spot[DIM][DIM];
    char state;
    int moves;
    WINDOW *win;
} Board;

void create_board(Board *local_board, int starty, int startx) {
    local_board->state = 'U';
    local_board->moves = 0;
    int x = (SQ_WIDTH-1)/(2*DIM);
    int y = (SQ_HEIGHT)/(2*DIM);
    local_board->win = newwin(SQ_HEIGHT, SQ_WIDTH, starty, startx);
    wmove(local_board->win, ((SQ_HEIGHT-1)/DIM), 0);
    whline(local_board->win, 0, SQ_WIDTH);
    wmove(local_board->win, ((SQ_HEIGHT-1)/DIM)*2, 0);
    whline(local_board->win, 0, SQ_WIDTH);
    wmove(local_board->win, 0, ((SQ_WIDTH-1)/DIM));
    wvline(local_board->win, 0, SQ_HEIGHT);
    wmove(local_board->win, 0, ((SQ_WIDTH-1)/DIM)*2);
    wvline(local_board->win, 0, SQ_HEIGHT);
    for(int i = 0; i < DIM; i++) {
        local_board->xval[i] = x + (x*i*2);
        local_board->yval[i] = y + (y*i*2);
        for(int j = 0; j < DIM; j++) {
            local_board->spot[i][j] = 'U';
        }
    }
}

WINDOW *indicate;
Player *player = NULL;
Board subboard[DIM][DIM];
int boardY, boardX, spotY, spotX;
int boardsFilled = 1;
bool GAMEOVER;

char space_state(bool metaBoard, int i, int j);
void update_subboard(Board *local_board);
void update_metaboard(Board *local_board, char state);
void update_board(bool metaBoard, int i, int j);
void create_windows(void);
void play_game(bool metaBoard);
void set_firstSpace(bool metaBoard);
void player_move(bool boardSelect, int i, int j);
void check_game(bool metaBoard);

int main(void) {
    initscr();
    raw();
    noecho();
    keypad(stdscr, TRUE);
    refresh();
    player = link_players("Player1",'X', "PLayer2", 'O');
    create_windows();
    play_game(TRUE);
    getch();
    endwin();
}
void create_windows(void) {
    int starty, startx;
    starty = 0;
    for(boardY = 0; boardY < DIM; boardY++) {
        starty = boardY * (SQ_HEIGHT);
        for(boardX = 0; boardX < DIM; boardX++) {
            startx = boardX * (SQ_WIDTH);
            create_board(&subboard[boardY][boardX], starty, startx);
            box(subboard[boardY][boardX].win,0,0);
            wrefresh(subboard[boardY][boardX].win);
        }
    }
    indicate = newwin(4, 50,SQ_HEIGHT*3, 0);
}
void play_game(bool metaBoard) {
    while(GAMEOVER == FALSE) {
        if(metaBoard == FALSE) {
            player_move(metaBoard, spotY, spotX);
            wprintw(subboard[boardY][boardX].win, "%c", player->mark);
            wrefresh(subboard[boardY][boardX].win);
            check_game(FALSE);
            player = player->next;
            if(subboard[spotY][spotX].state != 'U') {
                play_game(TRUE);
            }else {
                boardY = spotY;
                boardX = spotX;
                play_game(FALSE);
            }
        }else {
            set_firstSpace(metaBoard);
            player_move(metaBoard, boardY, boardX);
            play_game(FALSE);
        }
    }
}
void update_subboard(Board *local_board) {
    local_board->spot[spotY][spotX] = player->mark;
    local_board->moves++;
}
void update_metaboard(Board *local_board, char state) {
    local_board->state = state;
    boardsFilled++;
}
void update_board(bool metaBoard, int i, int j) {
    if(metaBoard) {
        boardY = i;
        boardX = j;
    }else {
        spotY = i;
        spotX = j;
    }
    if(metaBoard) {
        box(subboard[i][j].win, 1, 1);
        wrefresh(subboard[i][j].win);
    }else {
        wmove(subboard[boardY][boardX].win, subboard[boardY][boardX].yval[i], subboard[boardY][boardX].xval[j]);
        wrefresh(subboard[boardY][boardX].win);
    }
}
char space_state(bool metaBoard, int i, int j) {
    if(metaBoard == TRUE) {
        return subboard[i][j].state;
    }else {
        return subboard[boardY][boardX].spot[i][j];
    }
}
void set_firstSpace(bool metaBoard) {
    int i = 0;
    int j = 0;
    metaBoard ? curs_set(0) : curs_set(1);
    while(space_state(metaBoard, i , j) != 'U') {
        j++;
        while(j == 2 && space_state(metaBoard, i , j) != 'U') {
            j = 0;
            i++;
        }
    }
    update_board(metaBoard, i, j);
}
void player_move(bool boardSelect, int i, int j) {
    werase(indicate);
    wprintw(indicate, "%s's (%c) turn", player->name, player->mark);
    wrefresh(indicate);
    set_firstSpace(boardSelect);
    int ch;
    char state;
    while((ch = getch()) != 10) {
        if(boardSelect == TRUE) {
            box(subboard[i][j].win, 0, 0);
            wrefresh(subboard[i][j].win);
        }
        switch(ch) {
            case KEY_RIGHT:
                j++;
                if(j > 2){
                    j = 0;
                    i++;
                }
                if(i > 2) i = 0;
                while((state = space_state(boardSelect, i, j)) != 'U'){
                    j++;
                    while(j == 2 && space_state(boardSelect, i , j) != 'U') {
                        j = 0;
                        i++;
                    }
                }
                break;
            case KEY_LEFT:
                j--;
                if(j < 0) {
                    j = 2;
                    i--;
                }
                if(i < 0) i = 2;
                while((state = space_state(boardSelect, i, j)) != 'U'){
                    j--;
                    while(j == 0 && space_state(boardSelect, i , j) != 'U') {
                        j = 2;
                        i--;
                    }
                }
                break;
            case KEY_UP:
                do{
                    i--;
                    if(i < 0) i = 2;
                }while(space_state(boardSelect, i, j) != 'U');
                break;
            case KEY_DOWN:
                do{
                    i++;
                    if(i > 2) i = 0;
                }while(space_state(boardSelect, i, j) != 'U');
                break;
            }
        update_board(boardSelect, i, j);     
    }

    box(subboard[boardY][boardX].win, 0, 0);
    wrefresh(subboard[boardY][boardX].win);
}
void check_game(bool metaBoard) {
    bool boardDone = FALSE;
    int c, num, i, j;
    if(metaBoard) {
        num = boardsFilled;
    }else {
        num = subboard[boardY][boardX].moves;
        update_subboard(&subboard[boardY][boardX]);
    }
    for(c = 0; (c <= 3) && (boardDone == FALSE); c++) {
        switch(c) {
            case 1:
                for(i = 0; i < DIM; i++) {
                    for(j = 0; space_state(metaBoard, i, j) == player->mark && j < DIM; j++) {
                        if(j == 2) boardDone = TRUE;
                    }
                    for(j = 0; space_state(metaBoard, j, i) == player->mark && j < DIM; j++) {
                        if(j == 2) boardDone = TRUE;
                    }
                }
                break;
            case 2:
                for(i = 0; i < DIM && space_state(metaBoard, i, i) == player->mark; i++) {
                    if(i == 2) boardDone = TRUE;
                }
                break;
            case 3:
                for(i = 0, j = 2; space_state(metaBoard, i, j) == player->mark && (i < DIM && j >= 0); i++, j--) {
                    if(i == 2 && j == 0) boardDone = TRUE;
                }
                break;
        }
    }
    if(metaBoard == FALSE) {
        if(boardDone == TRUE) {
            box(subboard[boardY][boardX].win, player->mark, player->mark);
            wrefresh(subboard[boardY][boardX].win);
            update_metaboard(&subboard[boardY][boardX], player->mark);
            check_game(TRUE);
        }else if(boardDone != TRUE && num == 9) {
            box(subboard[boardY][boardX].win, '#', '#');
            wrefresh(subboard[boardY][boardX].win);
            update_metaboard(&subboard[boardY][boardX], 'T');
        }
    }else {
        if(boardDone) {
            werase(indicate);
            wprintw(indicate, "%s (%c) won\npress any key to quit", player->name, player->mark);
            wrefresh(indicate);
            GAMEOVER = TRUE;
        }else if(boardDone != TRUE && num == 9) {
            werase(indicate);
            wprintw(indicate, "The Game is a Tie\npress any key to quit", player->name, player->mark);
            wrefresh(indicate);
            GAMEOVER = TRUE;
        }
    }
}

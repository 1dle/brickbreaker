#include <ncurses.h>
#include <iostream>
#include <unistd.h> //for usleep, sleep
#include <vector> //store bricks
#include <algorithm> //for erase from vector

#include <stdio.h>
#include <stdlib.h>


int W_WIDTH, W_HEIGHT, G_WIDTH, G_HEIGHT;
int anim_speed = 1000; //1sec

enum COLLISSION_SIDE{
    TOP, BOTTOM, LEFT, RIGHT
};


int points = 0;

struct Brick{
    public:
    int x, y, width = 4, height = 1, color_id = 3; //color = 3 || 4

    Brick(int x, int y, int color_id): x(x), y(y), color_id(color_id){

    }

};
std::vector<Brick> bricks;

struct Paddle{
public:
    int x, y, width;

    
    Paddle(int _x = 0, int _y = 0, int _width = 8):
        x(_x), y(_y), width(_width)
    {}
    
};
Paddle paddle;


WINDOW* gamew;


bool gameover = false;
bool victory = false;
struct Ball{
    public:
    int x, y, speed = 1, xdir = 1, ydir = -1;

    Ball(int x = 0, int y = 0): x(x), y(y){}

    void move(){
        if( x == 0 || x == G_WIDTH-1 ){
            xdir *= -1;
        }
        if( y == 0){
            ydir *= -1;
        }
        if(y == G_HEIGHT-1){
            gameover = true;
            return;
        }
        
        //detect collision with paddle
        if ( x >= paddle.x && x <= paddle.x + paddle.width-1 && //vertical coll
             y == paddle.y-1 //on the top of the paddle
           ){
            ydir *= -1;
        }

        x+=speed*xdir;
        y+=speed*ydir;
    }
};

Ball ball;

void draw(){
    //clear prev. screen state
    werase(gamew);


    mvwhline(gamew, paddle.y, paddle.x,  '=', paddle.width);

    //draw bricks;
    for(int i = 0; i < bricks.size(); i++){
        //every second is different color 
        wattron( gamew, COLOR_PAIR( bricks.at(i).color_id ) );
        mvwhline(gamew, bricks.at(i).y, bricks.at(i).x,  ACS_CKBOARD, bricks.at(i).width);
        wattroff( gamew, COLOR_PAIR( bricks.at(i).color_id ) ); 
    }

    //draw the ball;
    wattr_on(gamew, COLOR_PAIR(2), nullptr);
    mvwaddch(gamew, ball.y, ball.x, ACS_DIAMOND);
    wattr_off(gamew, COLOR_PAIR(2), nullptr);

    //print points
    mvwprintw(gamew, 0,0, "points: ");
    mvwprintw(gamew, 0, 8, std::to_string(points).c_str());
    
    wrefresh(gamew);

}

bool brick_col(const Brick &br){
    return (ball.x >= br.x && ball.x <= br.x + br.width && ball.y >= br.y && ball.y <= br.y + br.height);
}

COLLISSION_SIDE coll_side(const Brick &br){
    if( ball.x == br.x )
        return COLLISSION_SIDE::LEFT;
    if( ball.x == br.x + br.width )
        return COLLISSION_SIDE::RIGHT;
    if( ball.y == br.y )
        return COLLISSION_SIDE::TOP;
    if( ball.y == br.y + br.height )
        return COLLISSION_SIDE::BOTTOM;
    return COLLISSION_SIDE::BOTTOM; //for safety
}


void brick_collisions(){
    for(int i = 0; i<bricks.size(); i++){
        //check every brick
        Brick br = bricks.at(i);
        if(brick_col(br)){
            COLLISSION_SIDE br_coll = coll_side(br);

            if(br_coll == COLLISSION_SIDE::TOP || br_coll == COLLISSION_SIDE::BOTTOM )
                ball.ydir *= -1;
            else
                ball.xdir *= -1;

            //and remove the brick from the arr
            bricks.erase( bricks.begin()+i );
            points++;

            if(points %5 == 0 && anim_speed >= 400){
                anim_speed -= 200;
            }

            if(bricks.size() == 0){
                gameover = true;
                victory = true;
                return;
            }
            
        }

    }
}


//this function handle the inputs (arrow keys (L,R) for move the paddle)
void input(){
    int frame = 0;

    int c;
    while( (c = wgetch(gamew)) != 27){
        

        if(!gameover){
            frame++;

            switch (c)
            {
            case KEY_LEFT:
                if(paddle.x > 0){
                    paddle.x -= 1;
                }
                break;
            case KEY_RIGHT:
                if(paddle.x+paddle.width < G_WIDTH){
                    paddle.x += 1;
                }
                break;
            default:
                break;
            }

            //move the ball too

            if(frame >= anim_speed){ //1sec

                brick_collisions();

                ball.move();
                frame = 0;
            }

            usleep(100);
            draw();
        }else{
            attr_on(COLOR_PAIR(2), nullptr);
            if(!victory){
                //print gameover
                mvprintw( W_HEIGHT / 2, W_WIDTH /2 - 5, "GAME OVER!" );
                mvprintw( W_HEIGHT / 2+1, W_WIDTH /2 - 6, "QUIT : [ESC]" );

            }else{
                mvprintw( W_HEIGHT / 2, W_WIDTH /2 - 4, "YOU WON!" );
                mvprintw( W_HEIGHT / 2+1, W_WIDTH /2 - 6, "QUIT : [ESC]" );
            }
            attr_off(COLOR_PAIR(2), nullptr);
            refresh();
            
        }
    }
       
}


void init_colors(){
    start_color();	
    //        id, foreground, background
	init_pair(1, COLOR_WHITE, COLOR_BLACK); //default
    init_pair(2, COLOR_RED, COLOR_BLACK); //red
    init_pair(3, COLOR_GREEN, COLOR_BLACK );
    init_pair(4, COLOR_BLUE, COLOR_BLACK );
}


void init_bricks(){
    //num of bricks depends on the size of the screen, (not the best solution for full screen play :P, bigger res bigger challange)
    int num_x = (G_WIDTH-2) / 5;
    int num_y = 3;

    int px = 2;
    int py = -1;
    for (int i = 0; i < num_y * num_x; i++)
    {
        if(i%num_x == 0){
            px = 2;
            py += 2;
        }else{
            px+=5;
        }

        bricks.push_back(Brick(px, py, 3+(i%2))); //alternating brick colors

    }
    


}


int main(){
    initscr();
    init_colors();
    getmaxyx(stdscr, W_HEIGHT, W_WIDTH);

    G_WIDTH = W_WIDTH-2;
    G_HEIGHT = W_HEIGHT-2;

    gamew = newwin(G_HEIGHT, G_WIDTH, 1, 1);

    noecho();
    raw();
    nodelay(gamew, true);
    keypad(gamew, true);
    curs_set(0);

    paddle = Paddle(G_WIDTH/2-4, G_HEIGHT-2, 8);
    ball = Ball(G_WIDTH / 2, G_HEIGHT/2);
    init_bricks();

    box(stdscr, 0, 0);
    wrefresh(stdscr);
    input();

    nodelay(gamew, false);
    delwin(gamew);
    endwin();
    return 0;
}

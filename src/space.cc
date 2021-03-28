#include <ncurses.h>
#include <stdlib.h>
#include <vector>
#include <math.h>
#include <thread>
#include <chrono>

void drawPixel(int x, int y, char c)
{
#ifdef DEBUG
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    refresh();
#endif
    mvprintw(y,x,"%c",c);
}

void drawLine(float x, float y, float x1, float y1, char c){
    auto x2 = x * 1.0f;
    auto y2 = y * 1.0f;

    auto l = std::max(std::abs(x1 - x), std::abs(y1 - y));

    auto incX = (x1 - x) / l;
    auto incY = (y1 - y) / l;

    while(! ((std::abs(y2 - y1)<1.f) and (std::abs(x2- x1)<1.f)) ){
            drawPixel(int(x2), int(y2), c);
            x2 = x2 + incX;
            y2 = y2 + incY;
    }
}

struct Point{
    float x, y;
    char shape;
    int color;
    Point():x(0),y(0),shape(0),color(0){}
    Point(float x, float y):x(x),y(y),shape('#'),color(rand()%7){}
    Point operator+=(const Point& p){
        x+=p.x;
        y+=p.y;
        return *this;
    }

};

Point rotatePointByAngleAroundPoint(const Point& p, const Point& origin, float angle){
    return { std::cos(angle)*(p.x - origin.x) - std::sin(angle)*(p.y-origin.y) + origin.x , std::sin(angle)*(p.x-origin.x) + std::cos(angle)*(p.y - origin.y) + origin.y};
}

void drawLine(Point p1, Point p2, char c){
    drawLine(p1.x, p1.y, p2.x, p2.y, c);
}

class Stone{
    std::vector<Point> points;
    Point center;
    public:

    Stone(std::vector<Point> points):points(points) {
        Point temp(0.f,0.f);
        for(auto& point: points){
            temp+=point;
        }
        center = Point(temp.x/float(points.size()) , temp.y/float(points.size()));
        }

    void move(const Point& p){
        for(auto& point: points){
            point+=p;
        }
        center+=p;
    }
    void rotateByAngle(float angle){
        for(auto& point: points){
            point = rotatePointByAngleAroundPoint(point, center, angle);
        }

    }
    void draw(){
        for(auto i=0; i<points.size(); ++i){
            drawLine(points[i], points[(i+1)%points.size()], points[i].shape);
            #ifdef DEBUG
            mvprintw(points[i].y+1, points[i].x+1, "%i", i+1);
            #endif
        }
    }
    void clear(){
        for(auto i=0; i<points.size(); ++i){
            drawLine(points[i], points[(i+1)%points.size()], ' ');
            #ifdef DEBUG
            mvprintw(points[i].y+1, points[i].x+1, "%c", ' ');
            #endif
        }
    }

};


int main(){
    auto window = initscr();

    int width, height;
    getmaxyx(window, height, width);
    timeout(1000 / 30); // ~30 fps
    curs_set(0);
    noecho();
    Stone stone({{3.f,3.f}, {15.f,6.f}, {18.f,19.f} , {5.f,25.f} });

    int key;
    do{
        //clear();
        box(window, '|', '-');
        // logic here:i
        stone.move({0.2f,0.2f});
        stone.rotateByAngle(M_PI/90.f); // ~ 2"
        // drawing here
        stone.draw();
        move(height,width);
        //  update screen
        refresh();
        // wait for timeout or key
        key = getch();

        // clear by things:
        stone.clear();

    } while(key!='q');


    endwin();
    return 0;
}

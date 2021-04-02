#include <ncurses.h>
#include <cstdlib>
#include <vector>
#include <cmath>
#include <thread>
#include <chrono>

void drawPixel(int x, int y, char c)
{
#ifdef DRAWDEBUG
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
    Point():x(0.f),y(0.f),shape(0),color(0){}
    Point(float x, float y):x(x),y(y),shape('#'),color(rand()%7){}
    Point operator*=(const float a){
        x*=a;
        y*=a;
        return *this;
    }
    Point operator*(const float a){
        Point temp(*this);
        temp*=a;
        return temp;
    }
    Point operator+=(const Point& p){
        x+=p.x;
        y+=p.y;
        return *this;
    }
    Point operator+(const Point& p){
        Point temp(*this);
        temp+=p;
        return temp;
    }

};


float length(const Point& p1, const Point& p2){
    return std::sqrt((p1.x-p2.x)*(p1.x-p2.x) + (p1.y-p2.y)*(p1.y-p2.y));
}

Point rotatePointByAngleAroundPoint(const Point& p, const Point& origin, float angle){
    return { std::cos(angle)*(p.x - origin.x) - std::sin(angle)*(p.y-origin.y) +
    origin.x , std::sin(angle)*(p.x-origin.x) + std::cos(angle)*(p.y - origin.y) + origin.y};
}

void drawLine(Point p1, Point p2, char c){
    drawLine(p1.x, p1.y, p2.x, p2.y, c);
}


class Map{
    private:
    int width{};
    int height{};
    WINDOW* window{};
    public:
    Map(WINDOW* window):window(window){
        getmaxyx(window, height, width);
    }
    int getWidth() const{
        return width;
    }
    int getHeight() const{
        return height;
    }
    WINDOW* getWindow() const{
        return window;
    }
};


class Stone{
    std::vector<Point> points{};
    Point center{};
    Point direction{};
    float spin{};
    public:

    Stone(std::vector<Point> points):points(points) {
        Point temp(0.f,0.f);
        for(auto& point: points){
            temp+=point;
        }
        center = Point(temp.x/float(points.size()) , temp.y/float(points.size()));
        }
    void setDirection(const Point& newDirection){
        direction = newDirection;
    }
    Point getDirection() const {
        return direction;
    }
    void setSpin(const float newSpin){
        spin = newSpin;
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
    void draw() const{
        for(auto i=0; i<points.size(); ++i){
            drawLine(points[i], points[(i+1)%points.size()], points[i].shape);

            #ifdef DEBUG
            mvprintw(points[i].y+1, points[i].x+1, "%i", i+1);
            #endif
        }
    }
    void clear() const {
        for(auto i=0; i<points.size(); ++i){
            drawLine(points[i], points[(i+1)%points.size()], ' ');
            #ifdef DRAWDEBUG
            mvprintw(points[i].y+1, points[i].x+1, "%c", ' ');
            #endif
        }
    }
    void update(Map map){
        mvprintw(0,0, "x:%f y:%f", center.x, center.y);
        move(direction);
        if(center.x<0.f){
            Point temp{};
            temp.x= map.getWidth()-1 + center.x;
            move(temp);
        }
        if(center.x>map.getWidth()){
            Point temp{};
            temp.x=-map.getWidth();
            move(temp);
        }
        if(center.y<0.f){
            Point temp{};
            temp.y= map.getHeight()-1 + center.y;
            move(temp);
        }
        if(center.y>map.getHeight()){
            Point temp{};
            temp.y= -map.getHeight() ;
            move(temp);
        }
        rotateByAngle(spin);
    }
};



class Ship: public Stone{
    private:
    Point speed{0.f, 0.2f};
    float spinSpeed = M_PI/45.f;
    public:
    Ship(const Stone& shipShape):Stone(shipShape){}
    void handleKey(const int key){
        if(key =='d'){
            rotateByAngle(spinSpeed);
            speed = rotatePointByAngleAroundPoint(speed, {0.f,0.f}, spinSpeed);
        }
        if(key =='a'){
            rotateByAngle(-spinSpeed);
            speed = rotatePointByAngleAroundPoint(speed, {0.f,0.f}, -spinSpeed);
        }
        if(key=='w'){
            auto dir = getDirection();
            if(length(dir,{0.f,0.f})<1.f){
                dir += speed;
            }
            else{
                dir*=0.95f;
            }
            setDirection(dir);
        }

    }
};

class Clock{


};
int main(){
    Map map(initscr());

    timeout(1000 / 30); // ~30 fps
    curs_set(0);
    noecho();
    Stone stone({{3.f,3.f}, {15.f,6.f}, {18.f,19.f} , {5.f,25.f} });
    Ship ship(Stone({{0.f,4.f},{4.f,-3.f},{0.f,-1.f},{-4.f,-3.f}}));
    stone.setSpin(M_PI/90.f);
    stone.setDirection({0.5f,0.5f});

    std::vector<Stone> stones = {stone};

    ship.move({map.getWidth()/2.f, map.getHeight()/2.f});
    //auto start = std::chrono::high_resolution_clock::now();

    int key;
    do{
        erase();
        box(map.getWindow(), '|', '-');
        // logic here:

        ship.handleKey(key);

        for(auto& stone:stones){
            stone.update(map);
        }
        ship.update(map);

        // drawing here
        for(auto& stone:stones){
            stone.draw();
        }
        ship.draw();

        move(map.getHeight(),map.getWidth());
        //  update screen
        refresh();
        // wait for timeout or key
        key = getch();

        // clear by things:
        //stone.clear();

    } while(key!='q');

    endwin();
    return 0;
}

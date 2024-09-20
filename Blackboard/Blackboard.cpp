using namespace std;
#include <iostream>
#include <vector>
#include <set>
#include <string>
#include <utility>
#define WIDTH 80
#define HEIGHT 25
#define PI 3.14159265358979323846

class Figure
{
protected:
    const string type;
    string id;
    vector<pair<int, int>> coordinates;

    string findID()
    {
        string id = "";
        for (int i = coordinates.size() - 1; i >= 0; i--)
        {
            id += to_string(coordinates[i].first + coordinates[i].second);
        }
        id += type;
        return id;
    }

    void PutStar(int x, int y, vector<vector<char>>& grid)const {
        if (x >= 0 && x < WIDTH && y >= 0 && y < HEIGHT) {
            grid[y][x] = '*';
        }
    }

    void drawLine(vector<vector<char>>& grid, const pair<int, int>& xy1, const pair<int, int>& xy2) const
    {
        int x1 = xy1.first;
        int y1 = xy1.second;
        int x2 = xy2.first;
        int y2 = xy2.second;

        int dx = abs(x2 - x1), sx = x1 < x2 ? 1 : -1;
        int dy = -abs(y2 - y1), sy = y1 < y2 ? 1 : -1;
        int err = dx + dy, e2;

        while (true) {
            PutStar(x1, y1, grid);  
            if (x1 == x2 && y1 == y2) break;
            e2 = 2 * err;
            if (e2 >= dy) { err += dy; x1 += sx; } 
            if (e2 <= dx) { err += dx; y1 += sy; } 
        }
    }

public:

    Figure(const vector<pair<int,int>>& figure_coordinates, const string& figure_type) : coordinates(figure_coordinates), type(figure_type)
    {

    }

    virtual void draw(vector<vector<char>>& grid) const = 0; 

    bool operator<(const Figure& other) const
    {
        return id < other.id;
    }

};

class Triangle : public Figure
{
    void draw(vector<vector<char>>& grid) const override
    {
        
        drawLine(grid, coordinates[0], coordinates[1]);
        drawLine(grid, coordinates[1], coordinates[2]);
        drawLine(grid, coordinates[2], coordinates[0]);
    }
public:
    Triangle(const vector<pair<int,int>>& figure_coordinates): Figure(figure_coordinates, "triangle")
    {
        id = findID();
    }
    
};

class Rectangular : public Figure
{
    int width;
    int height;

    void draw(vector<vector<char>>& grid) const override
    {
        pair<int, int> xy1(coordinates[0]);
        pair<int, int> xy2(coordinates[0].first, coordinates[0].second + height);
        pair<int, int> xy3(coordinates[0].first+width, coordinates[0].second);
        pair<int, int> xy4(coordinates[0].first + width, coordinates[0].second + height);
        drawLine(grid, xy1, xy2);
        drawLine(grid, xy2, xy4);
        drawLine(grid, xy4, xy3);
        drawLine(grid, xy1, xy3);
    }

public:
    Rectangular(const pair<int,int>& top_left_point, int w, int h) : Figure(vector<pair<int, int>>{top_left_point}, "rectangular"), width(w), height(h)
    {
        id = findID() + to_string(width) + to_string(height);
    }



};

class Circle : public Figure
{
    int radius;
    
    void plotCirclePoints(int xc, int yc, int x, int y, vector<vector<char>>& grid) const
    {
        PutStar(xc + x, yc + y, grid); 
        PutStar(xc - x, yc + y, grid); 
        PutStar(xc + x, yc - y, grid); 
        PutStar(xc - x, yc - y, grid); 
        PutStar(xc + y, yc + x, grid); 
        PutStar(xc - y, yc + x, grid); 
        PutStar(xc + y, yc - x, grid); 
        PutStar(xc - y, yc - x, grid); 
    }
    void draw(vector<vector<char>>& grid) const override
    {
        int xc = coordinates[0].first; 
        int yc = coordinates[0].second; 
        int x = 0;
        int y = radius;
        int d = 3 - 2 * radius;
        plotCirclePoints(xc, yc, x, y, grid);
        while (y >= x)
        {
            x++;
            if (d > 0)
            {
                y--;
                d = d + 4 * (x - y) + 10;
            }
            else
            {
                d = d + 4 * x + 6;
            }
            plotCirclePoints(xc, yc, x, y, grid);
        }
    }

public:

    Circle(const pair<int, int>& center, int r) : Figure(vector<pair<int, int>>{center}, "circle"), radius(r) 
    {
        id = findID() + to_string(radius);
    }

};

class PerfectTriangle : public Figure
{
    int height;
    int base;

    void draw(vector<vector<char>>& grid) const override
    {      
        pair<int, int> xy1 = coordinates[0];
        pair<int, int> xy2(xy1.first - base / 2, xy1.second + height);
        pair<int, int> xy3(xy1.first + base / 2, xy1.second + height);
        drawLine(grid, xy1, xy2);
        drawLine(grid, xy1, xy3);
        drawLine(grid, xy2, xy3);

    }

public:
    PerfectTriangle(const pair<int, int>& vertex, int h, int b) : Figure(vector<pair<int, int>>{vertex}, "perfect triangle"), base(b), height(h)
    {
        id = findID() + to_string(height) + to_string(base);
    }
};
class Board
{
    
public:
    vector<vector<char>> grid;

    Board() : grid(HEIGHT, vector<char>(WIDTH, ' ')){}

    void print()
    {
        for (auto& row : grid)
        {
            for (char c : row)
            {
                cout << c;
            }
            cout << endl;
        }
    }

    

   
};



int main()
{
    Board board;
    set<Figure*> figures;

    //figures.insert(new Triangle(vector<pair<int, int>>{{1, 1}, { 1, 20 }, { 30, 20 }}));
    figures.insert(new Circle({ 15,15 }, 4));
    figures.insert(new PerfectTriangle({ 3,2 }, 4, 9));

    for (const auto& figure : figures)
    {
        figure->draw(board.grid);
    }

    board.print();

    for (auto& figure : figures) {
        delete figure;
    }

    return 0;

}
 
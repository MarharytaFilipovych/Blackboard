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

    string findID(vector<pair<int, int>>& coordinates)
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

    void drawLine(vector<vector<char>>& grid, const pair<int,int>& xy1, const pair<int, int>& xy2)const
    {
        const int deltaX = xy2.first - xy1.first;
        const int deltaY = xy2.second - xy1.second;
        
        int steps = abs(deltaX) > abs(deltaY) ? abs(deltaX) : abs(deltaY);
        float Xinc = deltaX / (float)(steps);
        float Yinc = deltaY / (float)(steps);
        float x = xy1.first;
        float y = xy1.second;

        for (int i = 0; i < steps; i++)
        {
            int gridX = (int)(round(x));
            int gridY = (int)(round(y));

            PutStar(gridX, gridY, grid);
            x += Xinc;
            y += Yinc;
        } 

        int endX = (int)(round(xy2.first));
        int endY = (int)(round(xy2.second));
        PutStar(endX, endY, grid);

       
    }

public:

    Figure(const vector<pair<int,int>>& figure_coordinates, const string& figure_type) : coordinates(figure_coordinates), id(findID(coordinates)), type(figure_type)
    {

    }

    virtual void draw(vector<vector<char>>& grid) const = 0; 

    bool operator<(const Figure& other)
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
    Triangle(const vector<pair<int,int>>& figure_coordinates): Figure(figure_coordinates, "triangle") {}

    
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
        id = findID(coordinates) + to_string(width) + to_string(height);
    }



};

class Circle : public Figure
{
    int radius;


    

public:

    Circle(const pair<int, int>& center, int r) : Figure(vector<pair<int, int>>{center}, "circle"), radius(r) 
    {
        id = findID(coordinates) + to_string(radius);
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

    figures.insert(new Triangle(vector<pair<int, int>>{{1, 1}, { 1, 20 }, { 30, 20 }}));
    figures.insert(new Rectangular({ 1,1 }, 90, 15));

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
 
using namespace std;
#include <iostream>
#include <vector>
#include <unordered_map>
#include <string>
#include <utility>
#include <memory>
#include <stack>
#define WIDTH 80
#define HEIGHT 25
#define PI 3.14159265358979323846


 class Type
{
    static const string convertTypeToNumber(const string& type) 
    {
        string number = "";
        for (char letter : type)
        {
            number += to_string(static_cast<int>(letter)); 
        }
        return number;
    }
public:
    static const unordered_map<string, string> types;

};

 const unordered_map<string, string> Type::types = {
     {"circle", Type::convertTypeToNumber("circle")},
     {"triangle", Type::convertTypeToNumber("triangle")},
     {"rectangle", Type::convertTypeToNumber("rectangle")}
 };
class Figure
{
protected:
    const string type;
    string id;
    vector<pair<int, int>> coordinates;


    const string findID() {
        string id = ""; 
        for (int i = coordinates.size() - 1; i >= 0; i--) {
            id += to_string(coordinates[i].first + coordinates[i].second);
        }

        id += Type::types.at(type);
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

    Figure(const vector<pair<int,int>>& figure_coordinates, const string& figure_type) : coordinates(figure_coordinates), type(figure_type){}
    
    const string getID() const
    {
        return id;
    }

    virtual void printInfo() const = 0;
    
    virtual void draw(vector<vector<char>>& grid) const = 0; 

    bool operator==(const Figure& other) const
    {
        return id == other.id;
    }

};

struct HashFunctionForFigure
{
    size_t operator()(const shared_ptr<Figure> figure)const
    {
        return (hash<string>()(figure->getID()));
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
    void printInfo() const override
    {
        cout << id << " " << type << ": (" << coordinates[0].first << "," << coordinates[0].second << "), (" << coordinates[1].first << "," << coordinates[1].second << "), (" << coordinates[2].first << "," << coordinates[2].second << ")" << endl;
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
    void printInfo() const override
    {
        cout << id << " " << type << ": (" << coordinates[0].first << "," << coordinates[0].second << "), width - " << width << ", height - " << height << endl;
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
        int aspect_ratio_correction = 2; 
        PutStar(xc + x * aspect_ratio_correction, yc + y, grid);
        PutStar(xc - x * aspect_ratio_correction, yc + y, grid);
        PutStar(xc + x * aspect_ratio_correction, yc - y, grid);
        PutStar(xc - x * aspect_ratio_correction, yc - y, grid);
        PutStar(xc + y * aspect_ratio_correction, yc + x, grid);
        PutStar(xc - y * aspect_ratio_correction, yc + x, grid);
        PutStar(xc + y * aspect_ratio_correction, yc - x, grid);
        PutStar(xc - y * aspect_ratio_correction, yc - x, grid);
    }

    void draw(vector<vector<char>>& grid) const override
    {
        int xc = coordinates[0].first; 
        int yc = coordinates[0].second; 
        int x = 0;
        int y = radius;
        int d = 1 - 2 * radius;
        int error = 0;
        while (y >= x)
        {
            plotCirclePoints(xc, yc, x, y, grid);
            error = 2 * (d + y) - 1;
            if (d > 0 && error>0)
            {
                y--;
                d = d - (2 * y  + 1);
            }
            else if (d < 0 && error <= 0)
            {          
                x++;
                d = d + 2 * x  + 1;
            }
            else
            {
                x++;
                y--;
                d = d + 2 * (x - y);
            }
            
        }
    }
    void printInfo() const override
    {
        cout << id << " " << type << ": (" << coordinates[0].first << "," << coordinates[0].second << "), radius - " << radius << endl;
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

    void printInfo() const override
    {
        cout << id << " " << type << ": (" << coordinates[0].first << "," << coordinates[0].second << "), base length - " << base << ", height - " << height << endl;
    }

public:
    PerfectTriangle(const pair<int, int>& vertex, int h, int b) : Figure(vector<pair<int, int>>{vertex}, "perfect triangle"), base(b), height(h)
    {
        id = findID() + to_string(height) + to_string(base);
    }
};
struct Board
{
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

class Commands
{
    Board board;
    stack< shared_ptr<Figure>> time_figures;
    unordered_map<string, shared_ptr<Figure>> figures;

    void viewAvailableShapes() const
    {
        cout << "The following shapes can be drawn on the screen (this is a list with relevant parameters:\n"
            << "* circle: X Y (center coordinates) RADIUS\n"
            << "* trinagle: X1 Y1 X2 Y2 X3 Y3 (3 coordinates)\n"
            << "* perfect triangle: X Y (vertex coordinates) HEIGHT BASE_LENGTH\n"
            << "* rectangular: X Y (top left coordinates) WIDTH HEIGHT\n";
    }

    void addFigure(const shared_ptr<Figure>& figure) 
    {
        figures.emplace(figure->getID(), figure);
        time_figures.push(figure);
    }

    void undo(const shared_ptr<Figure>& figure)
    {
        figures.erase(figure->getID());
        time_figures.pop();
    }
    
    void clear()
    {
        figures.clear();
        while (!time_figures.empty())
        {
            time_figures.pop();
        }
    }

    void draw()
    {
        for (const auto& figure : figures)
        {
            figure.second->draw(board.grid);
        }
        board.print();

    }
    void list()
    {
        for (const auto& figure : figures)
        {
            figure.second->printInfo();
        }
    }
};
class UserInput
{



};


int main()
{

    //figures.insert(new Triangle(vector<pair<int, int>>{{1, 1}, { 1, 20 }, { 30, 20 }}));
    //auto circle = make_shared<Circle>(make_pair(30, 10), 8);
    //figures.emplace(circle->getID(),circle);
    //figures.insert(new PerfectTriangle({ 3,2 }, 4, 9));

   


    
    return 0;

}
 
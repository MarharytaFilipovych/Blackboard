#include <iostream>
#include <vector>
#include <unordered_map>
#include <string>
#include <utility>
#include <memory>
#include <stack>
#include <algorithm>
#include <unordered_set>
#include <sstream>
#include <fstream>
//#include <windows.h>
#include <cmath>
#define NOGDI
#define WIDTH 80
#define HEIGHT 40
#undef max
#undef min
using namespace std;

struct Board
{
    vector<vector<char>> grid;

    Board() : grid(HEIGHT, vector<char>(WIDTH, ' ')) {}

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
    void clear()
    {
        for (auto& row : grid)
        {
            fill(row.begin(), row.end(), ' '); 
        }
    }
    void putStar(int x, int y) {
        if (x >= 0 && x < WIDTH && y >= 0 && y < HEIGHT) {
            grid[y][x] = '*';
        }
    }

};

 class Type
{

    static const string convertTypeToNumber(const string& type) 
    {
        int number = 0;
        for (char letter : type)
        {
            number += static_cast<int>(letter); 
        }
        return to_string(number);
    }
public:
    static const unordered_map<string, string> types;

};

 const unordered_map<string, string> Type::types = 
 {
     {"circle", Type::convertTypeToNumber("circle")},
     {"triangle", Type::convertTypeToNumber("triangle")},
     {"rectangle", Type::convertTypeToNumber("rectangle")},
     {"perfect triangle", Type::convertTypeToNumber("perfect triangle")},
     {"line", Type::Type::convertTypeToNumber("line")},
     {"square", Type::Type::convertTypeToNumber("square")} 
 };
 
static Board board;

class Figure
{

protected:
    string type;
    string id;
    vector<pair<int, int>> coordinates;
    string color = "none";

    const double calculateArea(const pair<int, int>& p1, const pair<int, int>& p2, const pair<int, int>& p3)const
    {
        return abs(static_cast<double>(p1.first * (p2.second - p3.second) + p2.first * (p3.second - p1.second) + p3.first * (p1.second - p2.second)) / 2.0);
    }

    const string findID() {
        string id = ""; 
        for (int i = coordinates.size() - 1; i >= 0; i--) {
            id += to_string(coordinates[i].first);
            id += to_string(coordinates[i].second);
        }

        id += Type::types.at(type);
        return id; 
    }

    
    void drawLine( const pair<int, int>& xy1, const pair<int, int>& xy2) const
    {
        int x1 = xy1.first;
        int y1 = xy1.second;
        int x2 = xy2.first;
        int y2 = xy2.second;

        int dx = abs(x2 - x1), sx = x1 < x2 ? 1 : -1;
        int dy = -abs(y2 - y1), sy = y1 < y2 ? 1 : -1;
        int err = dx + dy, e2;

        while (true) {
            if (x1 == x2 && y1 == y2) break;
            board.putStar(x1, y1);  
            e2 = 2 * err;
            if (e2 >= dy) { err += dy; x1 += sx; } 
            if (e2 <= dx) { err += dx; y1 += sy; } 
        }
    }



    virtual ~Figure() = default;


public:

    Figure(const vector<pair<int,int>>& figure_coordinates, const string& figure_type) : coordinates(figure_coordinates), type(figure_type){}
    
    const string getID() const
    {
        return id;

    }

    virtual bool contains(const pair<int,int>& point) const = 0;


    void setType(const string& new_type) {
        type = new_type;
    }
    void setColor(const string& new_color)
    {
        color = new_color;
    }

    virtual void printInfo() const = 0;
    
    virtual void draw() const = 0; 

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
    friend class PerfectTriangle;

    void draw() const override
    {
        drawLine( coordinates[0], coordinates[1]);
        drawLine(coordinates[1], coordinates[2]);
        drawLine(coordinates[2], coordinates[0]);
    }

    void printInfo() const override
    {
        cout << id << " " << type << ": (" << coordinates[0].first << "," << coordinates[0].second << "), (" << coordinates[1].first << "," << coordinates[1].second << "), (" << coordinates[2].first << "," << coordinates[2].second << ")" << endl;
    }

    

    bool contains(const pair<int,int>& point)const override
    {
        double area = calculateArea(coordinates[0], coordinates[1], coordinates[2]);
        double area1 = calculateArea(point, coordinates[1], coordinates[2]);
        double area2 = calculateArea(coordinates[0], point, coordinates[2]);
        double area3 = calculateArea(coordinates[0], coordinates[1], point);
        if (color != "none")
        {
            return (area1 + area2 + area3 == area) || (area1 == 0 || area2 == 0 || area3 == 0);
        }
        else
        {
            return  area1 == 0 || area2 == 0 || area3 == 0;
              
        }
       
    }


public:

    Triangle(const pair<int, int>& p1, const pair<int, int>& p2, const pair<int, int>& p3)
        : Figure(vector<pair<int, int>>{p1, p2, p3}, "triangle") 
    {
        id = findID();
    }
    
};
class PerfectTriangle : public Figure
{
    int height;

    void resolveCoordinates()
    {
        int x = coordinates[0].first;
        int y = coordinates[0].second;
        int leftBaseX = x - height + 1;
        int rightBaseX = x + height - 1;
        int baseY = y + height - 1;

        coordinates.push_back({ leftBaseX, baseY });
        coordinates.push_back({ rightBaseX, baseY });

    }

    void draw() const override {
        int x = coordinates[0].first;
        int y = coordinates[0].second;

        for (int i = 0; i < height; ++i) {
            int leftMost = x - i;
            int rightMost = x + i;
            int posY = y + i;
            board.putStar(leftMost, posY);
            board.putStar(rightMost, posY);
        }
        for (int j = 0; j < 2 * height - 1; ++j) {
            int baseX = x - height + 1 + j;
            int baseY = y + height - 1;
            board.putStar(baseX, baseY);

        }
    }

    void printInfo() const override
    {
        cout << id << " " << type << ": (" << coordinates[0].first << "," << coordinates[0].second << ")," << ", height - " << height << endl;
    }

    bool contains(const pair<int, int>& point) const override
    {

        Triangle triangle(coordinates[0], coordinates[1], coordinates[2]);
        return triangle.contains(point);
    }

public:

    PerfectTriangle(const pair<int, int>& vertex, int h) : Figure(vector<pair<int, int>>{vertex}, "perfect triangle"), height(h)
    {
        id = findID() + to_string(height);
        resolveCoordinates();
    }
};


class Rectangle : public Figure
{
    int width;
    int height;

protected:

     void draw() const override
    {
        pair<int, int> xy1(coordinates[0]);
        pair<int, int> xy2(coordinates[0].first, coordinates[0].second + height);
        pair<int, int> xy3(coordinates[0].first+width, coordinates[0].second);
        pair<int, int> xy4(coordinates[0].first + width, coordinates[0].second + height);
        drawLine(xy1, xy2);
        drawLine(xy2, xy4);
        drawLine(xy4, xy3);
        drawLine(xy3, xy1); 
    }
     void printInfo() const override
    {
        cout << id << " " << type << ": (" << coordinates[0].first << "," << coordinates[0].second << "), width - " << width << ", height - " << height << endl;
    }

     bool contains(const pair<int, int>& point)const override
     {
         int x = point.first;
         int y = point.second;
         if (color != "none") {
             return (x >= coordinates[0].first && x <= coordinates[0].first + width &&
                 y >= coordinates[0].second && y <= coordinates[0].second + height);
         }
         else {
             bool onLeftEdge = (x == coordinates[0].first && y >= coordinates[0].second && y <= coordinates[0].second + height);
             bool onRightEdge = (x == coordinates[0].first + width && y >= coordinates[0].second && y <= coordinates[0].second + height);
             bool onTopEdge = (y == coordinates[0].second && x >= coordinates[0].first && x <= coordinates[0].first + width);
             bool onBottomEdge = (y == coordinates[0].second + height && x >= coordinates[0].first && x <= coordinates[0].first + width);
             return onLeftEdge || onRightEdge || onTopEdge || onBottomEdge;
         }
        
    }

public:
    Rectangle(const pair<int,int>& top_left_point, int w, int h) : Figure(vector<pair<int, int>>{top_left_point}, "rectangle"), width(w), height(h)
    {
        id = findID() + to_string(width) + to_string(height);
    }



};
class Square : public Rectangle
{
    void draw() const override
    {
        Rectangle::draw();
    }
    void printInfo()const override
    {
        Rectangle::printInfo();
    }
    bool contains(const pair<int, int>& point)const override
    {
        return Rectangle::contains(point);
    }
public:
    Square(const pair<int, int>& top_left_point, int side_length) : Rectangle(top_left_point, side_length, side_length)
    {
        setType("square");
        id = findID() + to_string(side_length);
    }
};

class Circle : public Figure
{
    int radius;
    
    void plotCirclePoints(int xc, int yc, int x, int y) const
    {
        board.putStar(xc + x * 2, yc + y);
        board.putStar(xc - x * 2, yc + y);
        board.putStar(xc + x * 2, yc - y);
        board.putStar(xc - x * 2, yc - y);
        board.putStar(xc + y * 2, yc + x);
        board.putStar(xc - y * 2, yc + x);
        board.putStar(xc + y * 2, yc - x);
        board.putStar(xc - y * 2, yc - x);
    }

    void draw() const override
    {
        int xc = coordinates[0].first;
        int yc = coordinates[0].second;
        int x = 0;
        int y = radius;
        int d = 1 - radius;  
        plotCirclePoints(xc, yc, x, y);
        while (y > x)
        {
            x++;
            if (d < 0)
            {
                d += 2 * x + 1;  
            }
            else
            {
                y--;
                d += 2 * (x - y) + 1;  
            }
            plotCirclePoints(xc, yc, x, y);
        }
    }

    void printInfo() const override
    {
        cout << id << " " << type << ": (" << coordinates[0].first << "," << coordinates[0].second << "), radius - " << radius << endl;
    }

    bool contains(const pair<int, int>& point)const override
    {
        int x = point.first;
        int y = point.second;
        if (color != "none")
        {
            return pow(x - coordinates[0].first, 2) + pow(y - coordinates[0].second, 2) <= pow(radius, 2);
        }
        else
        {
            return pow(x - coordinates[0].first, 2) + pow(y - coordinates[0].second, 2) == pow(radius, 2);
        }
    }

public:

    Circle(const pair<int, int>& center, int r) : Figure(vector<pair<int, int>>{center}, "circle"), radius(r) 
    {
        id = findID() + to_string(radius);
    }

};


class Line : public Figure
{
    void draw() const override
    {
        pair<int, int> xy1 = coordinates[0];
        pair<int, int> xy2 = coordinates[1];
        drawLine(xy1, xy2);
    }
    void printInfo() const override
    {
        cout << id << " " << type << ": (" << coordinates[0].first << "," << coordinates[0].second << "), (" << coordinates[1].first << "," << coordinates[1].second << ")" << endl;
    }

    bool contains(const pair<int, int>& point)const override
    {     
        bool onSegment = min(coordinates[0].first, coordinates[1].first) <= point.first && max(coordinates[0].first, coordinates[1].first) >= point.first
            && min(coordinates[0].second, coordinates[1].second) <= point.second && max(coordinates[0].second, coordinates[1].second) >= point.second;

        double area = calculateArea(point, coordinates[0], coordinates[1]);
        return onSegment && area == 0;

    }
public:

    Line(const pair<int, int>& p1, const pair<int, int>& p2) : Figure(vector<pair<int, int>> {p1, p2}, "line")
    {
        id = findID();
    }
    };
class Commands
{
    vector<string> time_figures;
    unordered_map<string, shared_ptr<Figure>> figures;
    shared_ptr<Figure> selected_figure;

    void drawTheBoard()
    {
        board.clear();
        for (auto& figure : figures)
        {
            figure.second->draw();
        }
    }
    bool checkIfFigureExists(const string& id)const
    {
        if (figures.find(id) == figures.end())
        {
            cout << "This figure doesn't exist!" << endl;
            return false;
        }
        return true;
    }
    


public:

    void viewAvailableShapes() const
    {
        cout << "The following shapes can be drawn on the screen (this is a list with relevant parameters:\n"
            << "* circle: X Y (center coordinates) RADIUS\n"
            << "* trinagle: X1 Y1 X2 Y2 X3 Y3 (3 coordinates)\n"
            << "* perfect triangle: X Y (vertex coordinates) HEIGHT\n"
            << "* rectangle: X Y (top left coordinates) WIDTH HEIGHT\n"
            << "* line: X1 Y1 X2 Y2 (2 coordinates)\n"
            << "* square: X Y (top left coordinates) SIDE_LENGTH\n";
    }

    void addFigure(const shared_ptr<Figure>& figure, const string& color) 
    {
        if (figures.find(figure->getID()) != figures.end())
        {
            cout << "This figure has been already added earlier! Try to use some imagination!" << endl;
            return;
        }
        string id = figure->getID();
        figure->setColor(color);
        figures.emplace(id, figure);
        time_figures.push_back(id);
        figure->draw();
        cout << "The figure was added, don't worry!:)" << endl;

    }

    void undo()
    {
        string id = time_figures.back();
        time_figures.pop_back();
        figures.erase(id);
        drawTheBoard();
    }
    
    void clear()
    {
        figures.clear();
        time_figures.clear();
        board.clear();
    }

    void draw()
    {
        system("cls");       
        board.print(); 
    }
    
    void list()
    {
        for (const auto& figure : figures)
        {
            figure.second->printInfo();
        }
    }

    void save(const string& file_path)
    {
        ofstream file(file_path);

        if (!file.is_open()) {
            cout << "The file can't be opened. Believe me, I tried((((!" << endl;
            return;
        }

        for (const auto& row : board.grid)
        {
            for (char c : row) {
                file << c;
            }
            file << '\n';  
        }
        file.close();
        cout << "The board was saved successfully!" << endl;
    }

    void load(const string& file_path)
    {
        ifstream file(file_path);
        if (!file.is_open())
        {
            cout << "I have 2 ideas: either your file doesn't exist here or something is not right!" << endl;
            return;
        }
        board.clear();
        string line;
        int line_count = 0;
        while (getline(file, line))
        {
            if (line.size() > WIDTH && line_count > HEIGHT)
            {
                cout << "Incorrect board format. It is too big!" << endl;
                return;
            }
            for (int i = 0; i < WIDTH; i++)
            {
                if (line[i] != ' ' && line[i] != '*')
                {
                    cout << "Incorrect symbols on the board!" << endl;
                    return;
                }
                board.grid[line_count][i] = line[i];
            }    
            line_count++;
        }
        file.close();
        cout << "The board was loaded successfully!" << endl;
    }

    void remove(const string& id)
    {
        if (checkIfFigureExists(id))
        {
            figures.erase(id);
            time_figures.erase(find(time_figures.begin(), time_figures.end(), id));
            drawTheBoard();
            cout << "The figure was removed successully!" << endl;
        }    
    }

    void selectByID(const string& id)
    {
        if (checkIfFigureExists(id))
        {
            shared_ptr<Figure> figure = figures.find(id)->second;
            selected_figure = figure;
            figure->printInfo();
        }
    }

    void selectByCoordinates(const pair<int, int>& point)
    {
        selected_figure = nullptr;
        for (int i = time_figures.size() - 1; i >= 0; i--)
        {
            string id = time_figures[i];
            shared_ptr<Figure> figure = figures.find(id)->second;
            if (figure->contains(point))
            {
                selected_figure = figure;
                figure->printInfo();
                return;
            }
        }
        cout << "No figures at this point!" << endl;
    }
    void paint(const string& color)
    {
        selected_figure->setColor(color);
        cout << "The selected figure was painted with a " << color << "color" << endl;

    }
};
class UserInput
{
    Commands action;
    string userInput;
    bool exit_flag = false;
    string previous_command = "";
    
    bool checkForParameters(const istringstream& my_stream)const
    {
         if (my_stream.eof())
            {
                cout << "This command does need parameters!" << endl;
                return false;
            }
        return true;
    }
    bool isLetters(const string& data) const
    {
        return all_of(data.begin(), data.end(), ::isalpha);
    }
    bool isDigits(const string& data) const
    {
        return all_of(data.begin(), data.end(), ::isdigit);
    }
    
    void getShape(string& shape, istringstream& my_stream)
    {
        my_stream >> shape;
        if (shape == "perfect")
        {
            string add_word;
            my_stream >> add_word;
            shape = shape + " " + add_word;           
        }        
    }

    bool checkForParametersEnd(istringstream& my_stream)const
    {    
         return my_stream.eof();      
    }

    bool checkFigureWithThreeParam(istringstream& my_stream, int& x, int& y, int& radius)const
    {
        if (my_stream >> x >> y >> radius)
        {
            return checkForParametersEnd(my_stream);

        }      
        return false;
    }

    bool checkTriangle(istringstream& my_stream, int& x1, int& y1, int& x2, int& y2, int& x3, int& y3)const
    {
        if (my_stream >> x1 >> y1 >> x2 >> y2 >> x3 >> y3) 
        {
            return checkForParametersEnd(my_stream);
        }
        return false;  
    }

    bool checkFigureWithFourParam(istringstream& my_stream, int& x, int& y, int& sth1, int& sth2)
    {
        if (my_stream >> x >> y >> sth1 >> sth2)
        {
            return checkForParametersEnd(my_stream);
        }
        return false;
    }

    bool isWithinBounds(int x, int y) {
        return x >= 0 && y >= 0 && x < WIDTH && y < HEIGHT;
    }

    void processCircle(istringstream& my_stream, const string& color) 
    {
        int x, y, radius;
        if (!checkFigureWithThreeParam(my_stream, x, y, radius) || !isWithinBounds(x, y) || radius < 0 || radius > WIDTH || radius > HEIGHT)
        {
            cout << "Incorrect parameters! Check 'shapes' command!" << endl;
            return;
        }
        action.addFigure(make_shared<Circle>(make_pair(x, y), radius), color);
    }

    void processTriangle(istringstream& my_stream, const string& color)
    {
        int x1, y1, x2, y2, x3, y3;
        if (!checkTriangle(my_stream, x1, y1, x2, y2, x3, y3) || !isWithinBounds(x1, y1) || !isWithinBounds(x2, y2) || !isWithinBounds(x3, y3)) {
            cout << "Incorrect parameters! Check 'shapes' command!" << endl;
            return;
        }
        action.addFigure(make_shared<Triangle>(make_pair(x1, y1), make_pair(x2, y2), make_pair(x3, y3)), color);
    }

    void processRectangle(istringstream& my_stream, const string& color)
    {
        int x, y, width, height;
        if (!checkFigureWithFourParam(my_stream, x, y, width, height) || !isWithinBounds(x, y) || !isWithinBounds(width, height))
        {
            cout << "Incorrect parameters! Check 'shapes' command!" << endl;
            return;
        }
        action.addFigure(make_shared<Rectangle>(make_pair(x, y), width, height), color);
    }

    void processSquare(istringstream& my_stream, const string& color)
    {
        int x, y, side_length;
        if (!checkFigureWithThreeParam(my_stream, x, y, side_length) || !isWithinBounds(x, y) || side_length < 0 || side_length > WIDTH || side_length > HEIGHT)
        {
            cout << "Incorrect parameters! Check 'shapes' command!" << endl;
            return;
        }
        action.addFigure(make_shared<Square>(make_pair(x, y), side_length), color);
    }

    void processPerfectTriangle(istringstream& my_stream, const string& color)
    {
        int x, y, height;
        if (!checkFigureWithThreeParam(my_stream, x, y, height) || !isWithinBounds(x, y) || height < 0 || height > HEIGHT)
        {
            cout << "Incorrect parameters! Check 'shapes' command!" << endl;
            return;
        }
        action.addFigure(make_shared<PerfectTriangle>(make_pair(x, y), height), color);
    }

    void processLine(istringstream& my_stream, const string& color)
    {
        int x1, y1, x2, y2;
        if (!checkFigureWithFourParam(my_stream, x1, y1, x2, y2) || !isWithinBounds(x1, y1) || !isWithinBounds(x2, y2)) {

            cout << "Incorrect parameters! Check 'shapes' command!" << endl;
            return;
        }
        action.addFigure(make_shared<Line>(make_pair(x1, y1), make_pair(x2, y2)), color);
    }
    bool getColorIfAny(istringstream& my_stream, string& color) const
    {
        string fill_word;
        my_stream >> fill_word;

        if (fill_word == "fill")
        {
            my_stream >> color; 
            return validateColor(color); 
        }

        for (int i = fill_word.length() - 1; i >= 0; --i) {
            my_stream.putback(fill_word[i]);
        }


        color = "none"; 

        return true; 
    }


    bool validateColor(const string& color) const
    {
        if (!isLetters(color))
        {
            cout << "This color is not available!" << endl;
            return false;
        }
        return true;
    }

    void processShape(istringstream& my_stream, const string& shape) 
    {
        string color;
        if (!getColorIfAny(my_stream, color))
        {
            return;
        }
        
        
        if (shape == "circle") 
        {
            processCircle(my_stream, color);
        }
        else if (shape == "triangle") 
        {
            processTriangle(my_stream, color);
        }
        else if (shape == "rectangle") 
        {
            processRectangle(my_stream, color);
        }
        else if (shape == "perfect triangle") 
        {
            processPerfectTriangle(my_stream, color);
        }
        else if (shape == "line")
        {
            processLine(my_stream, color);
        }
        else if (shape == "square")
        {
            processSquare(my_stream, color);
        }
        else {
            cout << "This shape is not available right now!" << endl;
        }
    }
    void checkReturnValue(istringstream& my_stream, string& value) const
    {
        if (!checkForParameters(my_stream)) {
            return;
        }
        my_stream >> value;
    }

    void add(istringstream& my_stream) 
    {
        if (!checkForParameters(my_stream)) {
            return;
        }
        string shape;
        getShape(shape, my_stream);
        processShape(my_stream, shape);
    }

    void remove(istringstream& my_stream)
    {
        string id;
        checkReturnValue(my_stream, id);
        action.remove(id);
        if (!checkForParametersEnd(my_stream))
        {
            cout << "This command already has enough parameters!" << endl;
            return;
        }
    }

    void paint(istringstream& my_stream)
    {
        string color;
        checkReturnValue(my_stream, color);
        if (!validateColor(color))
        {
            return;
        }
        action.paint(color);
        if (!checkForParametersEnd(my_stream))
        {
            cout << "This command already has enough parameters!" << endl;
            return;
        }
    }
    void handleCoordinates(istringstream& my_stream, string& value)
    {
        int y;
        my_stream >> y;
        int x = stoi(value);
        if (!isWithinBounds(x, y))
        {
            cout << "Incorrect coordinates!" << endl;
            return;
        }
        if (!checkForParametersEnd(my_stream))
        {
            cout << "This command already has enough parameters!" << endl;
            return;
        }
        action.selectByCoordinates({ x,y });
    }

    void select(istringstream& my_stream)
    {
        string value;
        checkReturnValue(my_stream, value);
        if (!isDigits(value))
        {
            cout << "Incorrect input!" << endl;
            return;
        }
        if (!checkForParametersEnd(my_stream))
        {
            handleCoordinates(my_stream, value);
        }   
        else
        {
            action.selectByID(value);
        }
    }

    void loadOrSave(const string& command, istringstream& my_stream) 
    {
        if (!checkForParameters(my_stream)) 
        {
            return;
        }
        string file_path;
        my_stream >> file_path;

        if (!checkForParametersEnd(my_stream)) 
        {
            cout << "This command already has enough parameters!" << endl;
            return;
        }
        if (command == "load") 
        {
            action.load(file_path);
        }
        else 
        {
            action.save(file_path);
        }
    }

    void clearScreenAndPrompt() 
    {
        cout << "Press Enter to continue..." << endl;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        system("cls");
    }

    void help()
    {
        cout << "There are 10 commands developed explicitly for you:\n"
            << "* draw: view your blackboard with the figures added.\n"
            << "* add FIGURE PARAMETERS: add a new figure to the blackboard (check the 'shapes' command to use this command correctly).\n"
            << "* shapes: this is list of all available shapes and their parameters, specially designed to assist you in using the 'add' command.\n"
            << "* clear: clear all content from the board.\n"
            << "* undo: remove the last added figure.\n"
            << "* list: view the list of all added figures along with relevant information about them.\n"
            << "* load FILE_PATH: load a blackboard from your file. Be cautious, as the board must meet the following requirements: symbols can be either ' ' or '*', a maximum width of the board 80 and a height of 40.\n"
            << "* save FILE_PATH: save your blackboard to the specified file.\n"
            << "* help:get assistance with using the commands.\n"
            << "* exit: finish your work here.\n"
            << "* remove ID: remove the figure by specifying its id.\n";
    }
    void exit()
    {
        cout << "Thanks for using my program:) Have a good rest of your life:)" << endl;
        exit_flag = true;
    }
    void takeUserInput()
    {
        if (previous_command == "draw")
        {
            clearScreenAndPrompt();
        }
        string command;
        cout << "Enter a command with proper parameteers:" << endl;
        getline(cin, userInput);
        if (userInput.empty())
        {
            return;
        }
        istringstream my_stream(userInput);
        my_stream >> command;
        if (command != "save" && command != "load" && command != "add" && command != "remove" && command != "select" && command != "paint")
        {
            if (!checkForParametersEnd(my_stream))
            {
                cout << "Incorrect input!" << endl;
                return;
            }
        }
        if (command == "draw")
        {
            action.draw();
        }
        else if (command == "list")
        {
            action.list();
        }
        else if (command == "clear")
        {
            action.clear();
        }
        else if (command == "shapes")
        {
            action.viewAvailableShapes();
        }
        else if (command == "add")
        {
            add(my_stream);
        }
        else if (command == "undo")
        {
            action.undo();
        }
        else if (command == "load" || command == "save")
        {           
            loadOrSave(command, my_stream);
        }    
        else if (command == "help")
        {
            help();
        }
        else if (command == "exit")
        {
            exit();
        }
        else if (command == "remove")
        {
            remove(my_stream);
        }
        else if (command == "paint")
        {
            paint(my_stream);
        }
        else if (command == "select")
        {
            select(my_stream);
        }
        else
        {
            cout << "This command is not implemented!" << endl;
        }

        previous_command = command;
        userInput.clear();
    }

 public:

    UserInput()
    {
        cout << "Welcome to the Best blackboard game! Start playing, do not waste any second of your precious  endless life:))))" << endl;
        while (!exit_flag)
        {
            takeUserInput();
        }
    }
};

int main()
{
    UserInput userInput;
    return 0;
}
 
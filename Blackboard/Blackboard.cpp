﻿#include <iostream>
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
#include <cmath>
#define NOGDI
#define WIDTH 80
#define HEIGHT 40
#include <windows.h>
#undef max
#undef min
#define DEFAULT_COLOR_CODE 7
using namespace std;

struct Board
{
    vector<vector<char>> grid;
    unordered_map<string, int> colors = { {"blue", 1},{"green", 2},{"cyan", 3},{"red", 4},{"purple", 5},{"yellow", 6},{"none", 7},
     {"grey", 8},{"bright-blue", 9},{"bright-green", 10},{"bright-cyan", 11},{"bright-red", 12},{"pink", 13},{"bright-yellow", 14}, {"white", 15} };

    unordered_set<char> possible_board_symbols;

    int getColor(const string& color)const
    {
        return colors.find(color)->second;
    }

    Board() : grid(HEIGHT, vector<char>(WIDTH, ' ')) { generatePossibleCharsOfColorCodes(); }

    void print() const
    {
        for (auto& row : grid)
        {
            for (char c : row)
            {
                if (c != ' ')
                {
                    handleColor(c);
                }
                else
                {
                    cout << c;
                }
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

    void putStar(const int x, const int y, const int color_code) {

        if (x >= 0 && x < WIDTH && y >= 0 && y < HEIGHT)
        {
            grid[y][x] = static_cast<char>(color_code);
        }
    }

private:

    void color_symbol(int code)const
    {
        HANDLE console_color = GetStdHandle(STD_OUTPUT_HANDLE);
        SetConsoleTextAttribute(console_color, code);

    }

    void handleColor(char c)const
    {
        int color_code = static_cast<int>(c);
        c = '*';
        color_symbol(color_code);
        cout << c;
        color_symbol(DEFAULT_COLOR_CODE);

    }

    void generatePossibleCharsOfColorCodes()
    {
        for (const auto& sth : colors)
        {
            possible_board_symbols.insert(static_cast<char>(sth.second));
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
    bool frame;
    string type;
    string id;
    vector<pair<int, int>> coordinates;
    int color_code = DEFAULT_COLOR_CODE;
    string color;

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

    void drawLine(const pair<int, int>& xy1, const pair<int, int>& xy2) const
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
            board.putStar(x1, y1, color_code);
            e2 = 2 * err;
            if (e2 >= dy) { err += dy; x1 += sx; }
            if (e2 <= dx) { err += dx; y1 += sy; }
        }
    }
    void setType(const string& new_type)
    {
        type = new_type;
    }
    virtual ~Figure() = default;

public:

    Figure(const vector<pair<int, int>>& figure_coordinates, const string& figure_type, bool structure) : coordinates(figure_coordinates), type(figure_type), frame(structure) {}

    const string getID() const
    {
        return id;
    }

    const string getType()const
    {
        return type;
    }

    virtual void regenerateID() = 0;

    virtual bool contains(const pair<int, int>& point) const = 0;


    void setColor(const string& new_color)
    {
        color = new_color;
        color_code = board.getColor(color);
    }
    void changeStructure(const string& structure)
    {
        structure == "frame" ? frame = true : frame = false;
    }
    const string getColor()const
    {
        return color;
    }

    virtual void printInfo() const = 0;

    virtual void draw() const = 0;

    virtual void move(const pair<int, int> move_point) = 0;

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
    pair<int, int> center;
    pair<int, int> xy1;
    pair<int, int> xy2;
    pair<int, int> xy3;

    void calculateCenter()
    {
        int x = (coordinates[0].first + coordinates[1].first + coordinates[2].first) / 3;
        int y = (coordinates[0].second + coordinates[1].second + coordinates[2].second) / 3;
        center = make_pair(x, y);
    }

    bool contains(const pair<int, int>& point)const override
    {
        double area = calculateArea(coordinates[0], coordinates[1], coordinates[2]);
        double area1 = calculateArea(point, coordinates[1], coordinates[2]);
        double area2 = calculateArea(coordinates[0], point, coordinates[2]);
        double area3 = calculateArea(coordinates[0], coordinates[1], point);
        if (color_code != 0)
        {
            return (area1 + area2 + area3 == area) || (area1 == 0 || area2 == 0 || area3 == 0);
        }
        else
        {
            return  area1 == 0 || area2 == 0 || area3 == 0;
        }
    }

    void drawFilled() const
    {
        int maxX = max(xy1.first, max(xy2.first, xy3.first));
        int maxY = max(xy1.second, max(xy2.second, xy3.second));
        int minX = min(xy1.first, min(xy2.first, xy3.first));
        int minY = min(xy1.second, min(xy2.second, xy3.second));

        for (int i = minX; i <= maxX; i++)
        {
            for (int j = minY; j <= maxY; j++)
            {
                if (contains({ i,j }))
                {
                    board.putStar(i, j, color_code);
                }
            }
        }
    }

    void drawFrame()const
    {
        drawLine(coordinates[0], coordinates[1]);
        drawLine(coordinates[1], coordinates[2]);
        drawLine(coordinates[2], coordinates[0]);
    }

    void draw() const override
    {
        frame ? drawFrame() : drawFilled();
    }

    void printInfo() const override
    {
        cout << id << " " << type << ": (" << coordinates[0].first << "," << coordinates[0].second << "), (" << coordinates[1].first << "," << coordinates[1].second << "), (" << coordinates[2].first << "," << coordinates[2].second << "), color: " << color << endl;
    }

    void move(const pair<int, int> move_point) override
    {
        center.first = move_point.first;
        center.second = move_point.second;
    }

    void regenerateID() override
    {
        id = findID();
    }

public:

    Triangle(const pair<int, int>& p1, const pair<int, int>& p2, const pair<int, int>& p3, bool structure)
        : Figure(vector<pair<int, int>>{p1, p2, p3}, "triangle", structure)
    {
        id = findID();
        calculateCenter();
        xy1 = coordinates[0];
        xy2 = coordinates[1];
        xy3 = coordinates[2];
    }

};
class PerfectTriangle : public Figure
{
    int height;
    pair<int, int> center;
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

        for (int i = 0; i < height; i++)
        {
            int leftMost = x - i;
            int rightMost = x + i;
            int posY = y + i;
            board.putStar(leftMost, posY, color_code);
            board.putStar(rightMost, posY, color_code);
            if (!frame)
            {
                for (int j = leftMost; j <= rightMost; j++)
                {
                    board.putStar(j, posY, color_code);
                }
            }
        }
        for (int j = 0; j < 2 * height - 1; j++)
        {
            int baseX = x - height + 1 + j;
            int baseY = y + height - 1;
            board.putStar(baseX, baseY, color_code);
        }
    }

    void printInfo() const override
    {
        cout << id << " " << type << ": (" << coordinates[0].first << "," << coordinates[0].second << ")," << ", height - " << height << ", color: " << color << endl;
    }

    bool contains(const pair<int, int>& point) const override
    {
        Triangle triangle(coordinates[0], coordinates[1], coordinates[2], frame);
        return triangle.contains(point);
    }

    void move(const pair<int, int> move_point) override
    {
        Triangle triangle(coordinates[0], coordinates[1], coordinates[2], frame);

        triangle.move(move_point);
    }

    void regenerateID() override
    {
        id = findID() + to_string(height);;
    }

public:

    PerfectTriangle(const pair<int, int>& vertex, int h, bool structure) : Figure(vector<pair<int, int>>{vertex}, "perfect triangle", structure), height(h)
    {
        id = findID() + to_string(height);
        resolveCoordinates();
    }

    void edit(const int new_height)
    {
        height = new_height;
    }

};


class Rectangle : public Figure
{
protected:

    int width;
    int height;

private:
    pair<int, int> center;

    void findCenter()
    {
        center.first = coordinates[0].first + (width / 2);
        center.second = coordinates[0].second + (height / 2);
    }

    void drawFrame()const
    {
        pair<int, int> xy1(coordinates[0]);
        pair<int, int> xy2(coordinates[0].first, coordinates[0].second + height);
        pair<int, int> xy3(coordinates[0].first + width, coordinates[0].second);
        pair<int, int> xy4(coordinates[0].first + width, coordinates[0].second + height);

        drawLine(xy1, xy2);
        drawLine(xy2, xy4);
        drawLine(xy4, xy3);
        drawLine(xy3, xy1);
    }

    void drawFilled()const
    {
        for (int i = coordinates[0].first; i < width + coordinates[0].first; i++)
        {
            for (int j = coordinates[0].second; j < height + coordinates[0].second; j++)
            {
                board.putStar(i, j, color_code);
            }
        }
    }
    void regenerateID() override
    {
        id = findID() + to_string(width) + to_string(height);
    }

    bool checkIfInside(const int x, const int y, const pair<int, int>& point)const
    {
        return (x >= coordinates[0].first && x <= coordinates[0].first + width &&
            y >= coordinates[0].second && y <= coordinates[0].second + height);
    }

    bool checkIfOnEdges(const int x, const int y, const pair<int, int>& point)const
    {
        bool onLeftEdge = (x == coordinates[0].first && y >= coordinates[0].second && y <= coordinates[0].second + height);
        bool onRightEdge = (x == coordinates[0].first + width && y >= coordinates[0].second && y <= coordinates[0].second + height);
        bool onTopEdge = (y == coordinates[0].second && x >= coordinates[0].first && x <= coordinates[0].first + width);
        bool onBottomEdge = (y == coordinates[0].second + height && x >= coordinates[0].first && x <= coordinates[0].first + width);
        return onLeftEdge || onRightEdge || onTopEdge || onBottomEdge;
    }

protected:

    void draw() const override
    {
        frame ? drawFrame() : drawFilled();
    }

    void printInfo() const override
    {
        cout << id << " " << type << ": (" << coordinates[0].first << "," << coordinates[0].second << "), width - " << width << ", height - " << height << ", color: " << color << endl;
    }

    bool contains(const pair<int, int>& point)const override
    {
        int x = point.first;
        int y = point.second;
        if (color_code != 0)
        {
            return checkIfInside(x, y, point);
        }
        else
        {
            return checkIfOnEdges(x, y, point);
        }
    }

    void move(const pair<int, int> move_point) override
    {
        findCenter();
        center.first = move_point.first;
        center.second = move_point.second;
        coordinates[0].first = center.first - (width / 2);
        coordinates[0].second = center.second - (height / 2);
    }

public:

    Rectangle(const pair<int, int>& top_left_point, int w, int h, bool structure) : Figure(vector<pair<int, int>>{top_left_point}, "rectangle", structure), width(w), height(h)
    {
        id = findID() + to_string(width) + to_string(height);
    }

    void edit(const int new_width, const int new_height)
    {
        width = new_width;
        height = new_height;
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

    void move(const pair<int, int> move_point) override
    {
        Rectangle::move(move_point);
    }

    void regenerateID() override
    {
        id = findID() + to_string(width);
    }

public:

    Square(const pair<int, int>& top_left_point, int side_length, bool structure) : Rectangle(top_left_point, side_length, side_length, structure)
    {
        setType("square");
        id = findID() + to_string(side_length);
    }

    void edit(const int new_side_length)
    {
        height, width = new_side_length;
    }
};

class Circle : public Figure
{
    int radius;

    void fillCirclePoints(int xc, int yc, int x, int y) const
    {
        for (int i = xc - x * 2; i <= xc + x * 2; i++)
        {
            board.putStar(i, yc + y, color_code);
            board.putStar(i, yc - y, color_code);
        }

        for (int i = xc - y * 2; i <= xc + y * 2; i++)
        {
            board.putStar(i, yc + x, color_code);
            board.putStar(i, yc - x, color_code);
        }
    }

    void plotCirclePoints(int xc, int yc, int x, int y) const
    {
        board.putStar(xc + x * 2, yc + y, color_code);
        board.putStar(xc - x * 2, yc + y, color_code);
        board.putStar(xc + x * 2, yc - y, color_code);
        board.putStar(xc - x * 2, yc - y, color_code);
        board.putStar(xc + y * 2, yc + x, color_code);
        board.putStar(xc - y * 2, yc + x, color_code);
        board.putStar(xc + y * 2, yc - x, color_code);
        board.putStar(xc - y * 2, yc - x, color_code);
    }

    void draw(int xc, int yc, int x, int y) const
    {
        !frame ? fillCirclePoints(xc, yc, x, y) : plotCirclePoints(xc, yc, x, y);
    }

    void draw() const override
    {
        int xc = coordinates[0].first;
        int yc = coordinates[0].second;
        int x = 0;
        int y = radius;
        int d = 1 - radius;
        draw(xc, yc, x, y);
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
            draw(xc, yc, x, y);
        }
    }

    void printInfo() const override
    {
        cout << id << " " << type << ": (" << coordinates[0].first << "," << coordinates[0].second << "), radius - " << radius << ", color: " << color << endl;
    }

    bool contains(const pair<int, int>& point)const override
    {
        int x = point.first;
        int y = point.second;
        if (color_code != 0)
        {
            return pow(x - coordinates[0].first, 2) + pow(y - coordinates[0].second, 2) <= pow(radius, 2);
        }
        else
        {
            return pow(x - coordinates[0].first, 2) + pow(y - coordinates[0].second, 2) == pow(radius, 2);
        }
    }

    void move(const pair<int, int> move_point) override
    {
        coordinates[0].first = move_point.first;
        coordinates[0].second = move_point.second;
    }

    void regenerateID() override
    {
        id = findID() + to_string(radius);
    }

public:

    Circle(const pair<int, int>& center, int r, bool structure) : Figure(vector<pair<int, int>>{center}, "circle", structure), radius(r)
    {
        id = findID() + to_string(radius);
    }

    void edit(const int new_radius)
    {
        radius = new_radius;
    }
};

class Line : public Figure
{

    pair<int, int> center;

    void calculateCenter()
    {
        int x = (coordinates[0].first + coordinates[1].first) / 2;
        int y = (coordinates[0].second + coordinates[1].second) / 2;
        center = make_pair(x, y);
    }

    void draw() const override
    {
        pair<int, int> xy1 = coordinates[0];
        pair<int, int> xy2 = coordinates[1];
        drawLine(xy1, xy2);
    }

    void printInfo() const override
    {
        cout << id << " " << type << ": (" << coordinates[0].first << "," << coordinates[0].second << "), (" << coordinates[1].first << "," << coordinates[1].second << ")" << ", color: " << color << endl;
    }

    bool contains(const pair<int, int>& point)const override
    {
        bool onSegment = min(coordinates[0].first, coordinates[1].first) <= point.first && max(coordinates[0].first, coordinates[1].first) >= point.first
            && min(coordinates[0].second, coordinates[1].second) <= point.second && max(coordinates[0].second, coordinates[1].second) >= point.second;

        double area = calculateArea(point, coordinates[0], coordinates[1]);
        return onSegment && area == 0;
    }

    void move(const pair<int, int> move_point) override
    {
        center.first = move_point.first;
        center.second = move_point.second;
    }

    void regenerateID() override
    {
        id = findID();
    }

public:

    Line(const pair<int, int>& p1, const pair<int, int>& p2, bool structure) : Figure(vector<pair<int, int>> {p1, p2}, "line", structure)
    {
        id = findID();
    }

};

class Commands
{
    vector<string> time_figures;
    unordered_map<string, shared_ptr<Figure>> figures;

    void drawTheBoard()
    {
        board.clear();
        for (int i = 0; i < time_figures.size(); i++)
        {
            figures.find(time_figures[i])->second->draw();
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

    void changeID()
    {
        string old_id = selected_figure->getID();
        figures.erase(old_id);
        selected_figure->regenerateID();
        string new_id = selected_figure->getID();
        figures.emplace(new_id, selected_figure);
        auto it = find(time_figures.begin(), time_figures.end(), old_id);
        size_t index = distance(time_figures.begin(), it);
        time_figures[index] = new_id;
    }

    void changeIDAndMakeForeground()
    {
        string old_id = selected_figure->getID();
        time_figures.erase(find(time_figures.begin(), time_figures.end(), old_id));
        figures.erase(old_id);
        selected_figure->regenerateID();
        string new_id = selected_figure->getID();
        figures.emplace(new_id, selected_figure);
        time_figures.push_back(new_id);
    }

public:
    shared_ptr<Figure> selected_figure;

    void help()const
    {
        cout << "There are 16 commands developed explicitly for you:\n"
            << "* draw: view your blackboard with the figures added.\n"
            << "* add FIGURE fill|frame COLOR PARAMETERS: add a new figure to the blackboard (check the 'shapes' command to use this command correctly). You have a noption of specifying a coolor of the figure. If you don't, the color will be defaulted to 'none'.\n"
            << "* shapes: this is list of all available shapes and their parameters, specially designed to assist you in using the 'add' command.\n"
            << "* clear: clear all content from the board.\n"
            << "* undo: remove the last added figure.\n"
            << "* list: view the list of all added figures along with relevant information about them.\n"
            << "* load FILE_PATH: load a blackboard from your file. Be cautious, as the board must meet the following requirements: symbols can be either ' ' or '*', a maximum width of the board 80 and a height of 40.\n"
            << "* save FILE_PATH: save your blackboard to the specified file.\n"
            << "* help: get assistance with using the commands.\n"
            << "* exit: finish your work here.\n"
            << "* remove ID: remove the figure by specifying its id.\n"
            << "* move X Y: move previously selected figure to the specified point. Note: all points are moved regarding to their centers.\n"
            << "* paint COLOR FRAME(FILL): paint previously selected figure with a specified color.\n"
            << "* select ID | select X Y: select a particular figure from the board. You can do that with a help of the figure's id. Additionally, you can specify a point, that belongs to that figure. Note: in case of specifying a point, the forground figure will be selected!\n"
            << "* edit VALUE (VALUE2 (if rectangle)): change the previously selected figure.\n"
            << "* colors: look at the list of available colors." << endl;
        cout << "Edit instruction: You may change a radius of a circle, a side length of a square, width and height of a rectangle or\n" <<
            "a height of a perfect triangle. You are not permitted to  change coordinates of a simple triangle or a line.\n" <<
            "If you wish to change them, delete previous ones and create new ones!" << endl;
    }

    void exit(bool& exit_flag)
    {
        cout << "Thanks for using my program:) Have a good rest of your life:)" << endl;
        exit_flag = true;
    }

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
        cout << "Your figure was added :)))))))" << endl;
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
        cout << "The board was cleared, suit yourself!" << endl;
    }

    void draw()const
    {
        system("cls");
        board.print();
    }

    void list()const
    {
        for (const auto& figure : figures)
        {
            figure.second->printInfo();
        }
    }

    void seeColors()const
    {
        cout << "You can use these colors: blue, green, cyan, red, purple,\n" <<
            "yellow, grey, bright-blue, bright-green, bright-cyan, bright-red, pink, bright-yellow, white." << endl;
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
                if (board.possible_board_symbols.find(line[i]) == board.possible_board_symbols.end())
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

    void remove(const string& id, bool& removed_successfully)
    {
        if (checkIfFigureExists(id))
        {
            figures.erase(id);
            time_figures.erase(find(time_figures.begin(), time_figures.end(), id));
            drawTheBoard();
            removed_successfully = true;
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
        if (board.grid[point.second][point.first] == ' ')
        {
            cout << "No figures at this point yet!" << endl;
            return;
        }
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
    }

    void paint(const string& color, const string& stucture)
    {
        selected_figure->setColor(color);
        selected_figure->changeStructure(stucture);
        drawTheBoard();
        cout << "The selected figure became " << color << " and " << stucture <<"!" << endl;
    }

    void move(const pair<int, int>& pointToMove)
    {
        selected_figure->move(pointToMove);
        changeIDAndMakeForeground();
        drawTheBoard();
        cout << "The selected figure was moved!" << endl;
    }

    void edit(const vector<int>& parameters)
    {
        if (selected_figure->getType() == "circle")
        {
            shared_ptr<Circle> circle = dynamic_pointer_cast<Circle>(selected_figure);
            circle->edit(parameters[0]);
        }
        else if (selected_figure->getType() == "square")
        {
            shared_ptr<Square> square = dynamic_pointer_cast<Square>(selected_figure);
            square->edit(parameters[0]);
        }
        else if (selected_figure->getType() == "rectangle")
        {
            shared_ptr<Rectangle> rectangle = dynamic_pointer_cast<Rectangle>(selected_figure);
            rectangle->edit(parameters[0], parameters[1]);
        }
        else if (selected_figure->getType() == "perfect triangle")
        {
            shared_ptr<PerfectTriangle> perfect_triangle = dynamic_pointer_cast<PerfectTriangle>(selected_figure);
            perfect_triangle->edit(parameters[0]);
        }
        changeID();
        drawTheBoard();
        cout << "The selected figure was edited!" << endl;
    }
};

class UserInput
{
    Commands action;
    string userInput;
    bool exit_flag = false;
    string previous_command = "";
    unordered_set<string> commandsWithoutParam = { "help", "exit", "list", "shapes", "clear", "draw", "undo", "colors" };

    bool checkForParameters(const istringstream& my_stream)const
    {
        if (my_stream.eof())
        {
            cout << "This command does need parameters!" << endl;
            return false;
        }
        return true;
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

    bool checkOneParam(istringstream& my_stream, int& value) const
    {
        if (my_stream >> value && value >= 0 && value < WIDTH)
        {
            return checkForParametersEnd(my_stream);

        }
        return false;
    }

    bool checkTwoParam(istringstream& my_stream, int& x, int& y)
    {
        if (my_stream >> x >> y && isWithinBounds(x, y))
        {
            return checkForParametersEnd(my_stream);
        }
        return false;
    }

    bool isWithinBounds(int x, int y)
    {
        return x >= 0 && y >= 0 && x < WIDTH && y < HEIGHT;
    }

    void processCircle(istringstream& my_stream, const string& color, bool frame)
    {
        int x, y, radius;
        if (!checkFigureWithThreeParam(my_stream, x, y, radius) || !isWithinBounds(x, y) || radius < 0 || radius > WIDTH || radius > HEIGHT)
        {
            cout << "Incorrect parameters! Check 'shapes' command!" << endl;
            return;
        }
        action.addFigure(make_shared<Circle>(make_pair(x, y), radius, frame), color);
    }

    void processTriangle(istringstream& my_stream, const string& color, bool frame)
    {
        int x1, y1, x2, y2, x3, y3;
        if (!checkTriangle(my_stream, x1, y1, x2, y2, x3, y3) || !isWithinBounds(x1, y1) || !isWithinBounds(x2, y2) || !isWithinBounds(x3, y3)) {
            cout << "Incorrect parameters! Check 'shapes' command!" << endl;
            return;
        }
        action.addFigure(make_shared<Triangle>(make_pair(x1, y1), make_pair(x2, y2), make_pair(x3, y3), frame), color);
    }

    void processRectangle(istringstream& my_stream, const string& color, bool frame)
    {
        int x, y, width, height;
        if (!checkFigureWithFourParam(my_stream, x, y, width, height) || !isWithinBounds(x, y) || !isWithinBounds(width, height))
        {
            cout << "Incorrect parameters! Check 'shapes' command!" << endl;
            return;
        }
        action.addFigure(make_shared<Rectangle>(make_pair(x, y), width, height, frame), color);
    }

    void processSquare(istringstream& my_stream, const string& color, bool frame)
    {
        int x, y, side_length;
        if (!checkFigureWithThreeParam(my_stream, x, y, side_length) || !isWithinBounds(x, y) || side_length < 0 || side_length > WIDTH || side_length > HEIGHT)
        {
            cout << "Incorrect parameters! Check 'shapes' command!" << endl;
            return;
        }
        action.addFigure(make_shared<Square>(make_pair(x, y), side_length, frame), color);
    }

    void processPerfectTriangle(istringstream& my_stream, const string& color, bool frame)
    {
        int x, y, height;
        if (!checkFigureWithThreeParam(my_stream, x, y, height) || !isWithinBounds(x, y) || height < 0 || height > HEIGHT)
        {
            cout << "Incorrect parameters! Check 'shapes' command!" << endl;
            return;
        }
        action.addFigure(make_shared<PerfectTriangle>(make_pair(x, y), height, frame), color);
    }

    void processLine(istringstream& my_stream, const string& color, bool frame)
    {
        int x1, y1, x2, y2;
        if (!checkFigureWithFourParam(my_stream, x1, y1, x2, y2) || !isWithinBounds(x1, y1) || !isWithinBounds(x2, y2)) {

            cout << "Incorrect parameters! Check 'shapes' command!" << endl;
            return;
        }
        action.addFigure(make_shared<Line>(make_pair(x1, y1), make_pair(x2, y2), frame), color);
    }

    bool getColorIAndStructure(istringstream& my_stream, string& color, bool& frame) const
    {
        string fword;
        my_stream >> fword;
        if (fword != "fill" && fword != "frame")
        {
            cout << "Specify structure of the figure: FILL or FRAME!" << endl;
            return false;
        }
        my_stream >> color;
        fword == "fill" ? frame = false : frame = true;
        return validateColor(color);
    }

    bool validateColor(const string& color) const
    {
        return board.colors.find(color) != board.colors.end();
    }

    void processShape(istringstream& my_stream, const string& shape)
    {
        bool frame = false;
        string color;
        if (!getColorIAndStructure(my_stream, color, frame))
        {
            cout << "This color is not available!" << endl;
            return;
        }
        if (shape == "circle")
        {
            processCircle(my_stream, color, frame);
        }
        else if (shape == "triangle")
        {
            processTriangle(my_stream, color, frame);
        }
        else if (shape == "rectangle")
        {
            processRectangle(my_stream, color, frame);
        }
        else if (shape == "perfect triangle")
        {
            processPerfectTriangle(my_stream, color, frame);
        }
        else if (shape == "line")
        {
            processLine(my_stream, color, frame);
        }
        else if (shape == "square")
        {
            processSquare(my_stream, color, frame);
        }
        else {
            cout << "This shape is not available right now!" << endl;
            return;
        }
    }

    void checkReturnValue(istringstream& my_stream, string& value) const
    {
        if (!checkForParameters(my_stream))
        {
            return;
        }
        my_stream >> value;
    }

    void add(istringstream& my_stream)
    {
        if (!checkForParameters(my_stream))
        {
            return;
        }
        string shape;
        getShape(shape, my_stream);
        processShape(my_stream, shape);
    }

    void remove(istringstream& my_stream)
    {
        bool removed_successfully = false;
        string id;
        checkReturnValue(my_stream, id);
        action.remove(id, removed_successfully);
        if (!checkForParametersEnd(my_stream))
        {
            cout << "This command already has enough parameters!" << endl;
            return;
        }
        if (removed_successfully) { cout << "The figure was removed successully!" << endl; }
    }

    void move(istringstream& my_stream)
    {
        int x, y;
        if (!checkTwoParam(my_stream, x, y))
        {
            cout << "Incorrect input!" << endl;
            return;
        }
        action.move({ x,y });
    }

    void paint(istringstream& my_stream)
    {
        string color;
        checkReturnValue(my_stream, color);
        if (!validateColor(color))
        {
            return;
        }
        string structure;
        checkReturnValue(my_stream, structure);
        if (structure != "fill" && structure != "frame")
        {
            cout << "Specify structure of the figure: FILL or FRAME!" << endl;
            return;
        }
        action.paint(color, structure);
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
        !checkForParametersEnd(my_stream) ? handleCoordinates(my_stream, value) : action.selectByID(value);
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
        command == "load" ? action.load(file_path) : action.save(file_path);
    }

    bool processFiguresWithOneParamForEdit(istringstream& my_stream, vector<int>& parameters)
    {
        int value;
        if (!checkOneParam(my_stream, value))
        {
            cout << "Incorrect parameters for edition!" << endl;
            return false;
        }
        parameters.push_back(value);
        return true;
    }

    bool processFiguresWithTwoParamsForEdit(istringstream& my_stream, vector<int>& parameters)
    {
        int width, height;
        if (!checkTwoParam(my_stream, width, height))
        {
            cout << "Incorrect parameters for edition!" << endl;
            return false;
        }
        parameters.push_back(width);
        parameters.push_back(height);
        return true;
    }

    void edit(istringstream& my_stream)
    {
        bool removed = false;
        if (!checkForParameters(my_stream))
        {
            return;
        }
        vector<int> parameters;
        string type = action.selected_figure->getType();
        if (type == "square" || type == "circle" || type == "perfect triangle")
        {
            if (!processFiguresWithOneParamForEdit(my_stream, parameters)) { return; };
        }
        else if (type == "rectangle")
        {
            if (!processFiguresWithTwoParamsForEdit(my_stream, parameters)) { return; }
        }
        else
        {
            cout << "You are not allowed to modify this fogure!" << endl;
            return;
        }
        if (!checkForParametersEnd(my_stream))
        {
            cout << "This command already has enough parameters!" << endl;
            return;
        }
        action.edit(parameters);
    }

    void clearScreenAndPrompt()
    {
        cout << "Press Enter to continue..." << endl;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        system("cls");
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
        if (commandsWithoutParam.find(command) != commandsWithoutParam.end())
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
            action.help();
        }
        else if (command == "exit")
        {
            action.exit(exit_flag);
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
        else if (command == "move")
        {
            move(my_stream);
        }
        else if (command == "edit")
        {
            edit(my_stream);
        }
        else if (command == "colors")
        {
            action.seeColors();
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
        cout << "Welcome to the Best blackboard game! Start playing, do not waste any second of your precious endless life:))))" << endl;
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

using namespace std;
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
#define WIDTH 80
#define HEIGHT 35

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
    
};

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
     {"rectangle", Type::convertTypeToNumber("rectangle")},
     {"perfect triangle", Type::convertTypeToNumber("perfect triangle")}
 };
static Board board;

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

    void PutStar(int x, int y)const {
        if (x >= 0 && x < WIDTH && y >= 0 && y < HEIGHT) {
            board.grid[y][x] = '*';
        }
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
            PutStar(x1, y1);  
            if (x1 == x2 && y1 == y2) break;
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
public:
    Triangle(const pair<int, int>& p1, const pair<int, int>& p2, const pair<int, int>& p3)
        : Figure(vector<pair<int, int>>{p1, p2, p3}, "triangle") 
    {
        id = findID();
    }
    
};

class Rectangle : public Figure
{
    int width;
    int height;

    void draw() const override
    {
        pair<int, int> xy1(coordinates[0]);
        pair<int, int> xy2(coordinates[0].first, coordinates[0].second + height);
        pair<int, int> xy3(coordinates[0].first+width, coordinates[0].second);
        pair<int, int> xy4(coordinates[0].first + width, coordinates[0].second + height);
        drawLine(xy1, xy2);
        drawLine(xy2, xy4);
        drawLine(xy4, xy3);
        drawLine(xy1, xy3);
    }
    void printInfo() const override
    {
        cout << id << " " << type << ": (" << coordinates[0].first << "," << coordinates[0].second << "), width - " << width << ", height - " << height << endl;
    }

public:
    Rectangle(const pair<int,int>& top_left_point, int w, int h) : Figure(vector<pair<int, int>>{top_left_point}, "rectangle"), width(w), height(h)
    {
        id = findID() + to_string(width) + to_string(height);
    }



};

class Circle : public Figure
{
    int radius;
    
    void plotCirclePoints(int xc, int yc, int x, int y) const
    {
        int aspect_ratio_correction = 2; 
        PutStar(xc + x * aspect_ratio_correction, yc + y);
        PutStar(xc - x * aspect_ratio_correction, yc + y);
        PutStar(xc + x * aspect_ratio_correction, yc - y);
        PutStar(xc - x * aspect_ratio_correction, yc - y);
        PutStar(xc + y * aspect_ratio_correction, yc + x);
        PutStar(xc - y * aspect_ratio_correction, yc + x);
        PutStar(xc + y * aspect_ratio_correction, yc - x);
        PutStar(xc - y * aspect_ratio_correction, yc - x);
    }

    void draw() const override
    {
        int xc = coordinates[0].first; 
        int yc = coordinates[0].second; 
        int x = 0;
        int y = radius;
        int d = 1 - 2 * radius;
        int error = 0;
        while (y >= x)
        {
            plotCirclePoints(xc, yc, x, y);
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

    void draw() const override
    {      
        pair<int, int> xy1 = coordinates[0];
        pair<int, int> xy2(xy1.first - base / 2, xy1.second + height);
        pair<int, int> xy3(xy1.first + base / 2, xy1.second + height);
        drawLine(xy1, xy2);
        drawLine(xy1, xy3);
        drawLine(xy2, xy3);

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

class Commands
{
    stack< shared_ptr<Figure>> time_figures;
    unordered_map<string, shared_ptr<Figure>> figures;
public:
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
        figure->draw();

    }

    void undo()
    {
        shared_ptr<Figure>& figure = time_figures.top();
        time_figures.pop();
        figures.erase(figure->getID());
        for (auto& figure : figures)
        {
            figure.second->draw();
        }
        
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
        cout << "The board was saved successfully!" << endl;
        file.close();
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
                cout << "Invalid board format. It is too big!" << endl;
                return;
            }
            for (int i = 0; i < WIDTH; i++)
            {
                if (line[i] != ' ' && line[i] != '*')
                {
                    cout << "Invalid symbols on the board!" << endl;
                    return;
                }
                board.grid[line_count][i] = line[i];
            }    
            line_count++;
        }
        file.close();
        cout << "The board was loaded successfully!" << endl;
    }
};
class UserInput
{
    Commands action;
    string userInput;
    bool isDigit(const string& data) const
    {
        return all_of(data.begin(), data.end(), ::isdigit);
    }
    
    bool checkForParameters(const istringstream& my_stream)const
    {
         if (my_stream.eof())
            {
                cout << "This command does need parameters!" << endl;
                return false;
            }
        return true;

    }
    bool checkForParametersEnd(const istringstream& my_stream)const
    {
        if (!my_stream.eof())
        {
            cout << "This command already has enough parameters!" << endl;
            return false;
        }
        return true;

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
    bool checkCircle(istringstream& my_stream, int& x, int& y, int& radius)const
    {
        if (my_stream >> x >> y >> radius)
        {
            string extra;
            if (!(my_stream >> extra))
            {
                return true;
            }
        }
        cout << "Incorrect parameters for a circle! You need to specify a center (x,y) and a radius!" << endl;
        return false;
    }
    bool checkTriangle(istringstream& my_stream, int& x1, int& y1, int& x2, int& y2, int& x3, int& y3)const
    {
        if (my_stream >> x1 >> y1 >> x2 >> y2 >> x3 >> y3) {
            string extra;
            if (!(my_stream >> extra)) {
                return true;  
            }
        }
        cout << "Incorrect parameters for a triangle! You are expected to specify 6 coordinates (x1, y1, x2, y2, x3, y3)." << endl;

        return false;  
    }
    bool checkRectangle(istringstream& my_stream, int& x, int& y, int& width, int& height)const
    {
        if (my_stream >> x >> y >> width >> height) {
           string extra;
            if (!(my_stream >> extra)) {
                return true;  
            }
        }
        cout << "Incorrect parameters for a rectangle! You are expected to specify 4 integers (x, y, width, height)." << endl;
        return false;
    }
    bool checkPerfectTriangle(istringstream& my_stream, int& x, int& y, int& height, int& base_length)
    {
        if (my_stream >> x >> y >> height >> base_length) {
            string extra;
            if (!(my_stream >> extra)) {
                return true;
            }
        }
        cout << "Incorrect parameters for a perfect triangle! You are expected to specify 4 integers (x, y, height, base length)." << endl;
        return false;
    }
    void proccessShape(istringstream& my_stream, const string& shape)
    {
        if (shape == "circle")
        {
            int x, y, radius;
            if (!checkCircle(my_stream, x, y, radius))
            {
                return;
            } 
            action.addFigure(make_shared<Circle>(make_pair(x, y), radius));
        }
        else if (shape == "triangle")
        {
            int x1, y1, x2, y2, x3, y3;
            if (!checkTriangle(my_stream, x1, y1, x2, y2, x3, y3)) {
                return;
            }
            action.addFigure(make_shared<Triangle>(make_pair(x1, y1), make_pair(x2, y2), make_pair(x3, y3)));

        }
        else if (shape == "rectangle")
        {
            int x, y, width, height;
            if (!checkRectangle(my_stream, x, y, width, height)) {
                return;
            }
            action.addFigure(make_shared<Rectangle>(make_pair(x, y), width, height));

        }
        else if (shape == "perfect triangle")
        {
            int x, y, height, base_length;
            if (!checkPerfectTriangle(my_stream, x, y, height, base_length)) {
                return;
            }
            action.addFigure(make_shared<PerfectTriangle>(make_pair(x, y), height, base_length));

        }
        else
        {
            cout << "This shape is not availbale right now!" << endl;
            return;
        }
        cout << "The figure was added, don't worry!:)" << endl;

    }
    bool exit_flag = false;
    string previous_command="";
   
    void takeUserInput()
    {
        if (previous_command == "draw")
        {
            cout << "Press Enter to continue..." << endl;
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); 
            system("cls");  
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
        if (command != "save" && command != "load" && command != "add")
        {
            if (!checkForParametersEnd(my_stream))
            {
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
            if (!checkForParameters(my_stream))
            {
                return;
            }
            string shape;
            getShape(shape, my_stream);            
            proccessShape(my_stream, shape);
        }
        else if (command == "undo")
        {
            action.undo();
        }
        else if (command == "load")
        {
           
            if (!checkForParameters( my_stream))
            {
                return;
            }
            string file_path;
            my_stream >> file_path;
            
            if (!checkForParametersEnd(my_stream))
            {
                return;
            }
            action.load(file_path);
        }
        
        else if (command == "save" )
        {
            if (!checkForParameters(my_stream))
            {
                return;
            }
            string file_path;
            my_stream >> file_path;
            if (!checkForParametersEnd(my_stream))
            {
                return;
            }
            action.save(file_path);

            
            
        }
        else if (command == "help")
        {

        }
        else if (command == "exit")
        {
            cout << "Thanks for using my program:) Have a good rest of your life:)" << endl;
            exit_flag = true;
            
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
        cout << "Welcome to the Best balckboard game! Start playing, do not waste any second of your precious  endless life:))))" << endl;
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
 
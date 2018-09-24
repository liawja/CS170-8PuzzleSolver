#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <map>
#include <queue>
#include <utility>
#include <vector>

using namespace std;

// Dimensions of the puzzle, 3x3 for 8 puzzle
const int puz_size = 3;

// Goal for an 8 puzzle
string goalString = "123456780";

// Structure to store info about each node to be explored
struct puzzleNode
{
    vector< vector<string> > puzzle;
    unsigned gn; // Cost to get to node
    unsigned hn; // Cost from heuristic
    pair<int, int> blankTile;
    
    puzzleNode()
    {
        gn = 0;
        hn = 0;
        blankTile = make_pair(0, 0);
    }
    
    puzzleNode(vector< vector<string> > p)
    {
        gn = 0;
        hn = 0;
        puzzle = p;
        // Find coordinates of the blank
        for(unsigned i = 0; i < p.size(); ++i)
        {
            for(unsigned j = 0; j < p.at(i).size(); ++j)
            {
                if(p.at(i).at(j) == "0")
                {
                    blankTile = make_pair(i, j);
                    break;
                }
            }
        }
    }
    
    // Prints current state of the puzzle
    void printPuzzleState()
    {
        for(unsigned i = 0; i < puzzle.size(); ++i)
        {
            for(unsigned j = 0; j < puzzle.at(i).size(); ++j)
            {
                cout << puzzle.at(i).at(j) << " ";
            }
            cout << endl;
        }
    }
    
    // Converts vector to a single string
    // String with map checks for previously explored states
    // String is compared with a goal string to check for a goal state
    string puzzleString()
    {
        string pString;
        for(unsigned i = 0; i < puzzle.size(); ++i)
        {
            for(unsigned j = 0; j < puzzle.at(i).size(); ++j)
            {
                pString += puzzle.at(i).at(j);
            }
        }
        return pString;
    }
    
    bool isGoal()
    {
        return (this->puzzleString() == goalString);
    }
    
    // Sets the gn and hn cost based on given heuristic h
    void setCosts(int h)
    {
        gn++;
        if(h == 1)
        {
            // Uniform cost search: just returns 0 for hn
            hn = 0;
        }
        else if(h == 2)
        {
            // Misplaced Tile Heuristic: Check how many tiles are out of place
            string s = this->puzzleString();
            unsigned sum = 0;
            for(unsigned i = 0; i < s.size(); ++i)
            {
                if(s.at(i) != goalString.at(i))
                {
                    sum++;
                }
            }
            hn = sum;
            
        }
        else if(h == 3)
        {
            // Manhattan Distance Heuristic:
            // Iterate through and check which numbers are misplaced
            // Calculate Manhattan Distance for those out of place
            // current_num tracks which number we're currently looking at
            int current_num = 1;
            int sum = 0;
            for(int i = 0; i < puzzle.size(); ++i)
            {
                for(int j = 0; j < puzzle.at(i).size(); ++j)
                {
                    // Need to ignore the last slot which would be 9 but it's 0 in this case
                    if(current_num == puz_size * puz_size)
                    {
                        break;
                    }
                    // Need to separately check for 0 since it won't work with the equations below
                    if(stoi(puzzle.at(i).at(j)) != current_num && stoi(puzzle.at(i).at(j)) != 0)
                    {
                        // Translate string of numbers into their corresponding
                        // coordinate on the 8-puzzle grid
                        // Get the column(x coord) the number should be in
                        int x = stoi(puzzle.at(i).at(j)) % puz_size;
                        // Equation only returns 0 for last column, so just manually set to puz_size
                        if(x == 0)
                        {
                            x = puz_size;
                        }

                        /*
                         Equation for y derived from:
                         r * y + x = num where, num = some number in the puzzle (not blank, 1-8 in this case)
                                                  x = x coordinate of num, solved in previous step
                                                  r = length of each row (in this case, puz_size)
                        */                        
                        int y = ((stoi(puzzle.at(i).at(j)) - x) / puz_size);
                        
                        // Subtracting 1 from x since the calculated x isn't an index
                        sum += abs(y - i) + abs((x - 1) - j);
                    }
                    else if(stoi(puzzle.at(i).at(j)) == 0)
                    {
                        // We already know position of 0 since it's stored in blankTile member of puzzleNode
                        // 0 is always in the last slot (bottom right) so its coords are always just (puz_size, puz_size)
                        sum += abs(this->blankTile.first - (puz_size - 1)) + abs(this->blankTile.second - (puz_size - 1));
                    }
                    current_num++;
                }
            }
            hn = sum;
        }
    }
    
    // Calculate total cost
    unsigned calcFn() const
    {
        return gn + hn;
    }
    
    // Overload the < operator in order to sort priority queue by lowest cost
    bool operator<(const puzzleNode& lhs) const
    {
        return !(this->calcFn() < lhs.calcFn());
    }
}; // End Struct
    
/*
  Operators:
    move blank up
    move blank down
    move blank left
    move blank right
  Just means swap blank tile with tile above, below, to left, or to right of it
*/
puzzleNode moveBlankUp(puzzleNode p)
{
    if(p.blankTile.second != 0)
    {
        // Swap positions of blank tile and tile above it in vector
        swap(p.puzzle.at(p.blankTile.first).at(p.blankTile.second - 1), p.puzzle.at(p.blankTile.first).at(p.blankTile.second));
        // Update the new position of the blank tile
        p.blankTile.second = p.blankTile.second - 1;
    }
    return p;
}
    
puzzleNode moveBlankDown(puzzleNode p)
{
    if(p.blankTile.second != puz_size - 1)
    {
        // Swap positions of blank tile and tile above it in vector
        swap(p.puzzle.at(p.blankTile.first).at(p.blankTile.second + 1), p.puzzle.at(p.blankTile.first).at(p.blankTile.second));
        // Update the new position of the blank tile
        p.blankTile.second = p.blankTile.second + 1;
    }
    return p;
}
    
puzzleNode moveBlankLeft(puzzleNode p)
{
    if(p.blankTile.first != 0)
    {
        // Swap positions of blank tile and tile above it in vector
        swap(p.puzzle.at(p.blankTile.first - 1).at(p.blankTile.second), p.puzzle.at(p.blankTile.first).at(p.blankTile.second));
        // Update the new position of the blank tile
        p.blankTile.first = p.blankTile.first - 1;
    }
    return p;
}
    
puzzleNode moveBlankRight(puzzleNode p)
{
    if(p.blankTile.first != puz_size - 1)
    {
        // Swap positions of blank tile and tile above it in vector
        swap(p.puzzle.at(p.blankTile.first + 1).at(p.blankTile.second), p.puzzle.at(p.blankTile.first).at(p.blankTile.second));
        // Update the new position of the blank tile
        p.blankTile.first = p.blankTile.first + 1;
    }
    return p;
}
    
// Main function for expanding nodes
void expandNodes(puzzleNode current, map<string, bool>& explored_nodes, priority_queue<puzzleNode>& nodes, int h)
{
    // Mark the current node as explored in map
    explored_nodes[current.puzzleString()] = true;
     
    /* 
    Start calculating operators and check with map to eliminate explored duplicates
    Map returns false if not previously explored
    Also have to set appropriate gn and hn costs
    */ 
    puzzleNode blank_up = moveBlankUp(current);
    if(!explored_nodes[blank_up.puzzleString()])
    {
        blank_up.setCosts(h);
        nodes.push(blank_up);
    }
    
    puzzleNode blank_down = moveBlankDown(current);
    if(!explored_nodes[blank_down.puzzleString()])
    {
        blank_down.setCosts(h);
        nodes.push(blank_down);
    }
    
    puzzleNode blank_left = moveBlankLeft(current);
    if(!explored_nodes[blank_left.puzzleString()])
    {
        blank_left.setCosts(h);
        nodes.push(blank_left);
    }
    
    puzzleNode blank_right = moveBlankRight(current);
    if(!explored_nodes[blank_right.puzzleString()])
    {
        blank_right.setCosts(h);
        nodes.push(blank_right);
    }
}

int main()
{   
    // Intro Menu Prompt
    int choice = -1;
    cout << "Welcome to 8-puzzle solver!" << endl;
    cout << "Type \"1\" to use a default puzzle, or \"2\" to enter your own puzzle." << endl;
    cin >> choice;
    
    while(choice != 1 && choice != 2)
    {
        cout << "Invalid number, please enter a \"1\" for a default puzzle or \"2\" to enter your own puzzle" << endl;
        cin >> choice;
    }
    cin.ignore(10, '\n');
    
    // 2D vector to store puzzle in
    vector< vector<string> > puzzle;
    
    // Use default puzzle
    if(choice == 1)
    {
        cout << "Using Default Puzzle." << endl;
        puzzle = {{"0", "1", "2"},
                  {"4", "5", "3"},
                  {"7", "8", "6"}};
    }
    // Enter custom puzzle
    else if(choice == 2)
    {
        // Get rows for puzzle from user
        string input;
        string s;
        cout << "Enter your puzzle, use a 0 to represent the blank space" << endl;
        cout << "Enter the first row, use spaces between numbers:\t";
        getline(cin, input);
        s += input + ' ';
        cout << "Enter the second row, use spaces between numbers:\t";
        getline(cin, input);
        s += input + ' ';
        cout << "Enter the third row, use spaces between numbers:\t";
        getline(cin, input);
        s += input + ' ';
        
        // Storing custom puzzle in puzzle vector
        vector<string> temp;
        unsigned pos = 0;
        for(unsigned i = 0; i < puz_size; ++i)
        {
            puzzle.push_back(temp);
            for(unsigned j = 0; j < puz_size; ++j)
            {
                puzzle.at(i).push_back(s.substr(pos, 1));
                pos += 2;
            }
        }
    }
    else
    {
        cout << "Something went wrong." << endl;
        exit(1);
    }
    
    // Select an algorithm to use
    choice = -1;
    cout << "Enter your choice of algorithm" << endl
         << "    1. Uniform Cost Search" << endl
         << "    2. A* with Misplaced Tile Heuristic" << endl
         << "    3. A* with Manhattan Distance Heuristic" << endl;
    cin >> choice;
    
    while(choice != 1 && choice != 2 && choice != 3)
    {
        cout << "Invalid number, please choose an appropriate algorithm" << endl;
        cin >> choice;
    }
    
    // Set up initial state
    puzzleNode initial(puzzle);
    // Stores the nodes to be explored
    priority_queue<puzzleNode> nodes;
    // Keeps track of previously explored nodes
    map<string, bool> explored_nodes;
    
    // Push the initial node into the queue
    nodes.push(initial);
    
    // Keep track of some statistics
    unsigned max_queue_size = nodes.size(); // Should initially be 1
    unsigned total_nodes_expanded = 0;
    
    // Main searching block
    do
    {
        // Calculate the current max nodes in the queue
        if(max_queue_size < nodes.size())
        {
            max_queue_size = nodes.size();
        }
        
        // Failed, couldn't find a solution
        if(nodes.empty())
        {
            cout << "Failed, unable to find solution." << endl;
            cout << "Search algorithm expanded a total of " << total_nodes_expanded << " nodes." << endl;
            cout << "The maximum number of nodes in the queue at any one time was " << max_queue_size << "." << endl;
            break;
        }
        
        // Pull out the best node to explore and then pop from queue
        puzzleNode current = nodes.top();
        nodes.pop();
        
        // Test for goal state
        if(current.isGoal())
        {
            cout << "Goal!!" << endl;
            cout << "To solve this problem, the search algorithm expanded a total of "
                 << total_nodes_expanded << " nodes." << endl;
            cout << "The maximum number of nodes in the queue at any one time was "
                 << max_queue_size << "." << endl;
            cout << "The depth of the goal node was " << current.gn << endl;
            break;
        }
        
        // Only need to output this for the first iteration
        // This won't be reached if the initial state = goal state
        if(current.puzzle == initial.puzzle)
        {
            cout << "Expanding state: " << endl;
            current.printPuzzleState();
        }
        else
        {
            cout << "The best state to expand with a g(n) = " << current.gn
                 << " and h(n) = " << current.hn << " is..." << endl;
            current.printPuzzleState();
            cout << "Expanding this node..." << endl;
        }
        
        // Explore the current node and push its children into queue
        // Also mark as being explored in map
        expandNodes(current, explored_nodes, nodes, choice);
          
        total_nodes_expanded++;
        
    } while(1);
    
    return 0;
}
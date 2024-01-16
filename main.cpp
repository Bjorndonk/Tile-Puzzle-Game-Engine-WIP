/*
TO DO:
 - Add "remove" as an available player input in the playBoard function
    - If the cursor is selecting a node that has been placed on the board, it will return the node to the player's inventory (if there is room).
 - Add "info" as an available player input in the playBoard function
    - Display detailed information about a selected node
 - Add a function to calculate if a node has its logical requirements met
*/

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
using namespace std;

// Defines a node struct
struct node {
    char nodeType = 'e';
        // s = standard
        // h = horizontal
        // v = vertical
        // b = blocked
        // e = empty
    char quantumGrouping = 'z';
        // z = no grouping
        // any other letter = grouping
    int numerator = 1;
    int denominator = 1;
    float value = numerator / denominator;
    bool placableNode = false;
    bool isMine = false;
};

// Defines a 2-Dimensional vector that stores board dimensions, inventory size, and the node data for both
struct board {
    int sizeX = 0;
    int sizeY = 0;
    vector<vector<node>> boardData;
    int inventorySize = 0;
    vector<node> inventory;
};

struct cursor {
    int posX = 0;
    int posY = 0;
};

// Nodes passed are of the form:
// s.z.000.001.0.0
// nodeType.quantumGrouping.numerator.denominator.placableNode.isMine
node createNode(string input) {
    node returnNode;
    returnNode.nodeType = input[0];
    returnNode.quantumGrouping = input[2];
    returnNode.numerator = stoi(input.substr(4,7));
    returnNode.denominator = stoi(input.substr(8,11));
    if (returnNode.denominator != 0) {
        returnNode.value = float(returnNode.numerator) / float(returnNode.denominator);
    }
    else {
        cout << "WARNING: Node denominator set to 0, cannot calculate value." << endl;
    }
    returnNode.placableNode = bool(input[12] - 48);
    returnNode.isMine = bool(input[14] - 48);
    return returnNode;
}

// Changes the dimension of the board
board setBoardDimensions(board board, int newX, int newY) {
    vector<vector<node>> newboardData(newY, vector<node> (newX, createNode("e.z.001.001.0.0")));
    board.boardData = newboardData;
    board.sizeX = newX;
    board.sizeY = newY;
    return board;
}

// Changes the inventory size
board setInventorySize(board board, int newSize) {
    vector<node> newInventory(newSize, createNode("e.z.001.001.0.0"));
    board.inventory = newInventory;
    board.inventorySize = newSize;
    return board;
}

board fillBoard(board passedBoard, string fileName) {
    // Setting Variables
    int lineNum = 0;
    int delimNum = 0;
    string currentLine = "";
    string currElement = "";
    int stringIndex = 0;
    board newBoard;
    // Opening File
    ifstream fin;
    fin.open(fileName);
    // this checks that the file has opened successfully
    if (fin.is_open() == false)
    {
        fin.close();
        return passedBoard;
    }
    // this runs until the file runs out of lines
    while (getline(fin, currentLine)) {
        // Checking first line to set inventory size
        if (lineNum == 0) {
            newBoard.inventorySize = stoi(currentLine);
            newBoard = setInventorySize(newBoard, newBoard.inventorySize);
        }
        // Checking second line to fill inventory
        else if (lineNum == 1) {
            delimNum = 0;
            while (delimNum < newBoard.inventorySize) {
                // cout << currentLine << " " << currElement << endl; // TEMP!!!
                currElement = currentLine.substr(0,15);
                newBoard.inventory[delimNum] = createNode(currElement);
                delimNum++;
                currentLine = currentLine.substr(16);
            }
        }
        // Checking third line to set sizeX
        else if (lineNum == 2) {
            newBoard.sizeX = stoi(currentLine);
        }
        // Checking fourth line to set sizeY
        else if (lineNum == 3) {
            newBoard.sizeY = stoi(currentLine);
            newBoard = setBoardDimensions(newBoard, newBoard.sizeX, newBoard.sizeY);
        }
        // Otherwise, it takes the info and fills the board using it
        else {
            delimNum = 0;
            while (delimNum < newBoard.sizeX) {
                // cout << currentLine << " " << currElement << endl; // TEMP!!!
                currElement = currentLine.substr(0,15);
                newBoard.boardData[lineNum - 4][delimNum] = createNode(currElement);
                delimNum++;
                currentLine = currentLine.substr(16);
            }
        }
        lineNum++;
    }
    fin.close();
    return newBoard;
}

cursor moveCursor(board board, cursor cursor, string input, bool isPlacing) {
    // move up
    if (input == "w") {
        // Player is in inventory
        if (cursor.posY == -1) {
            // player is placing
            if (isPlacing == true) {
                cout << "This move would place the cursor outside of the inventory boundaries." << endl;
            }
            // otherwise, player moves out of inventory back onto the board
            // cursor is further right than max board dimension
            else if (cursor.posX > board.sizeX - 1) {
                cursor.posX = board.sizeX - 1;
                cursor.posY = board.sizeY - 1;
            }
            // otherwise, the cursor moves smoothly onto the board and posX remains unchanged
            else {
                cursor.posY = board.sizeY - 1;
            }
        }
        // Player is in the board
        else {
            // move is valid
            if (cursor.posY > 0) {
                cursor.posY = cursor.posY - 1;
            }
            // move is invalid
            else {
                cout << "This move would place the cursor outside of the board boundaries." << endl;
            }
        }
    }
    // move left
    else if (input == "a") {
        // move is valid
        if (cursor.posX > 0) {
            cursor.posX = cursor.posX - 1;
        }
        // move is invalid
        else {
            cout << "This move would place the cursor outside of the board boundaries." << endl;
        }
    }
    // move right
    else if (input == "d") {
        // Player is in inventory
        if (cursor.posY == -1) {
            // move is valid
            if (cursor.posX < board.inventorySize - 1) {
                cursor.posX = cursor.posX + 1;
            }
            // move is invalid
            else {
                cout << "This move would place the cursor outside of the board boundaries." << endl;
            }
        }
        // Player is in boardData
        else {
            // move is valid
            if (cursor.posX < board.sizeX - 1) {
                cursor.posX = cursor.posX + 1;
            }
            // move is invalid
            else {
                cout << "This move would place the cursor outside of the board boundaries." << endl;
            }
        }
    }
    // move down
    else if (input == "s") {
        // Player is in Inventory
        if (cursor.posY == -1) {
            cout << "This move would place the cursor outside of the board boundaries." << endl;
        }
        // Player is in Board
        else {
            // Otherwise, if the cursor is vertically adjascent to the inventory
            if (cursor.posY == board.sizeY - 1) {
                // Player is in place mode
                if (isPlacing == true) {
                    cout << "This move would place the cursor outside of the board boundaries." << endl;
                }
                // Position on board is further right than inventory length
                else if (cursor.posX > board.inventorySize - 1) {
                    cursor.posX = board.inventorySize - 1;
                    cursor.posY = -1;
                }
                // otherwise perform a smooth cursor transition into inventory
                else {
                    cursor.posY = -1;
                }
            }
            // Otherwise, the player moves in the boardData
            else {
                cursor.posY = cursor.posY + 1;
            }
        }
    }
    // passed input is invalid
    else {
        cout << "Invalid cursor movement passed." << endl;
    }
    return cursor;
}

// Largely used for testing / debugging
void printNode(node input) {
    cout << "Node Type: " << input.nodeType << endl;
    cout << "Quantum Grouping: " << input.quantumGrouping << endl;
    cout << "Fraction: " << input.numerator << "/" << input.denominator << endl;
    cout << "Value: " << input.value << endl;
    cout << "Is Placable: " << input.placableNode << endl;
    cout << "Is Mine: " << input.isMine << endl;
    return;
}

// Prints the boardstate
void printBoardData(board board, cursor cursor) {
    for (unsigned int y = 0; y < ((4 * board.sizeY) + 1); y++) {
        string currRow = "";
        for (unsigned int x = 0; x < ((2 * board.sizeX) + 1); x++) {
            // Row being printed is an edge
            if (y % 4 == 0) {
                // Adding the diagonal to the string
                if (x % 2 == 0) {
                    if (((cursor.posX * 2 == x) || ((cursor.posX + 1) * 2 == x)) && ((cursor.posY * 4 == y) || ((cursor.posY + 1) * 4 == y))) {
                        currRow = currRow + '#';
                    }
                    else {
                        currRow = currRow + '+';
                    }
                }
                // Checking for cursor position
                else if ((x == (cursor.posX * 2) + 1) && ((y / 4 == cursor.posY) || (y / 4 == cursor.posY + 1)) && (cursor.posY >= 0)) {
                    currRow = currRow + "#####";
                }
                else {
                    currRow = currRow + "-----";
                }
            }
            // Row being printed is not an edge
            else {
                // Printing Column
                if (x % 2 == 0) {
                    // Checking for curson position
                    if ((y / 4 == cursor.posY) && ((x == 2 * cursor.posX) || (x == (2 * cursor.posX) + 2))) {
                        currRow = currRow + "#";
                    }
                    else {
                        currRow = currRow + "|";
                    }
                }
                // Printing empty node
                else if (board.boardData[y / 4][x / 2].nodeType == 'e') {
                    currRow = currRow + "     ";
                }
                // Printing blank node
                else if (board.boardData[y / 4][x / 2].nodeType == 'b') {
                    currRow = currRow + "/////";
                }
                // Printing top row of node
                else if (y % 4 == 1) {
                    // TOP LEFT CORNER
                    // Checking if the node is placable
                    if (board.boardData[y / 4][x / 2].placableNode == true) {
                        currRow = currRow + "*";
                    }
                    else {
                        currRow = currRow + " ";
                    }
                    // TOP CENTER
                    // Checking if fraction is needed
                    if (board.boardData[y / 4][x / 2].denominator == 1) {
                        currRow = currRow + "   ";
                    }
                    // Checking if numerator is 3 digits
                    else if (board.boardData[y / 4][x / 2].numerator / 100 > 0) {
                        currRow = currRow + to_string(board.boardData[y / 4][x / 2].numerator);
                    }
                    // Checking if numerator is 2 digits
                    else if (board.boardData[y / 4][x / 2].numerator / 10 > 0) {
                        currRow = currRow + " " + to_string(board.boardData[y / 4][x / 2].numerator);
                    }
                    // Otherwise numerator is 1 digit
                    else {
                        currRow = currRow + " " + to_string(board.boardData[y / 4][x / 2].numerator) + " ";
                    }
                    // TOP RIGHT CORNER
                    // Checking if the node is part of a quantum grouping
                    if (board.boardData[y / 4][x / 2].quantumGrouping == 'z') {
                        currRow = currRow + " ";
                    }
                    else {
                        currRow = currRow + board.boardData[y / 4][x / 2].quantumGrouping;
                    }
                }
                // Printing middle row of node
                else if (y % 4 == 2) {
                    // Checking if fraction is needed
                    if (board.boardData[y / 4][x / 2].denominator != 1) {
                        currRow = currRow + " --- ";
                    }
                    // Checking if node contains a non-fraction, non-singular mine
                    else if (board.boardData[y / 4][x / 2].isMine == true && board.boardData[y / 4][x / 2].numerator == 1) {
                        currRow = currRow + "  X  ";
                    }
                    // Otherwise, we place the node value in the center
                    else if (board.boardData[y / 4][x / 2].numerator / 100 > 0) {
                        currRow = currRow + " " + to_string(board.boardData[y / 4][x / 2].numerator) + " ";
                    }
                    else if (board.boardData[y / 4][x / 2].numerator / 10 > 0) {
                        currRow = currRow + "  " + to_string(board.boardData[y / 4][x / 2].numerator) + " ";
                    }
                    else{
                        currRow = currRow + "  " + to_string(board.boardData[y / 4][x / 2].numerator) + "  ";
                    }
                }
                // Printing bottom row of node
                else {
                    // BOTTOM LEFT CORNER
                    // Checking the type of node
                    if (board.boardData[y / 4][x / 2].nodeType == 's') {
                        currRow = currRow + " ";
                    }
                    else {
                        currRow = currRow + board.boardData[y / 4][x / 2].nodeType;
                    }
                    // BOTTOM CENTER
                    // Checking if fraction is needed
                    if (board.boardData[y / 4][x / 2].denominator == 1) {
                        currRow = currRow + "   ";
                    }
                    // Checking if numerator is 3 digits
                    else if (board.boardData[y / 4][x / 2].denominator / 100 > 0) {
                        currRow = currRow + to_string(board.boardData[y / 4][x / 2].denominator);
                    }
                    // Checking if numerator is 2 digits
                    else if (board.boardData[y / 4][x / 2].denominator / 10 > 0) {
                        currRow = currRow + " " + to_string(board.boardData[y / 4][x / 2].denominator);
                    }
                    // Otherwise numerator is 1 digit
                    else {
                        currRow = currRow + " " + to_string(board.boardData[y / 4][x / 2].denominator) + " ";
                    }
                    // BOTTOM RIGHT
                    // Only needs to be filled if the node is a non-singular mine
                    if ((board.boardData[y / 4][x / 2].denominator != 1 || board.boardData[y / 4][x / 2].numerator != 1) && board.boardData[y / 4][x / 2].isMine == true) {
                        currRow = currRow + "X";
                    }
                    else {
                        currRow = currRow + " ";
                    }
                }
            }
        }
        cout << currRow << endl;
    }
}

void printInventory(board board, cursor cursor) {
    //int cursor.posX, int cursor.posY,
    for (unsigned int y = 0; y < 5; y++) {
        string currRow = "";
        for (unsigned int x = 0; x < ((2 * board.inventorySize) + 1); x++) {
            // Row being printed is an edge
            if (y % 4 == 0) {
                // Adding the diagonal to the string
                if (x % 2 == 0) {
                    if (((cursor.posX * 2 == x) || ((cursor.posX + 1) * 2 == x)) && cursor.posY == -1) {
                        currRow = currRow + '#';
                    }
                    else {
                        currRow = currRow + '+';
                    }
                }
                // Checking for cursor position
                else if (((x == (cursor.posX * 2) + 1) && (cursor.posY == -1))) {
                    currRow = currRow + "#####";
                }
                else {
                    currRow = currRow + "-----";
                }
            }
            // Row being printed is not an edge
            else {
                // Printing Column
                if (x % 2 == 0) {
                    // Checking for curson position
                    if ((cursor.posY == -1) && ((x == 2 * cursor.posX) || (x == (2 * cursor.posX) + 2))) {
                        currRow = currRow + "#";
                    }
                    else {
                        currRow = currRow + "|";
                    }
                }
                // Printing empty node
                else if (board.inventory[x / 2].nodeType == 'e') {
                    currRow = currRow + "     ";
                }
                // Printing blank node
                else if (board.inventory[x / 2].nodeType == 'b') {
                    currRow = currRow + "/////";
                }
                // Printing top row of node
                else if (y % 4 == 1) {
                    // TOP LEFT CORNER
                    // Checking if the node is placable
                    if (board.inventory[x / 2].placableNode == true) {
                        currRow = currRow + "*";
                    }
                    else {
                        currRow = currRow + " ";
                    }
                    // TOP CENTER
                    // Checking if fraction is needed
                    if (board.inventory[x / 2].denominator == 1) {
                        currRow = currRow + "   ";
                    }
                    // Checking if numerator is 3 digits
                    else if (board.inventory[x / 2].numerator / 100 > 0) {
                        currRow = currRow + to_string(board.inventory[x / 2].numerator);
                    }
                    // Checking if numerator is 2 digits
                    else if (board.inventory[x / 2].numerator / 10 > 0) {
                        currRow = currRow + " " + to_string(board.inventory[x / 2].numerator);
                    }
                    // Otherwise numerator is 1 digit
                    else {
                        currRow = currRow + " " + to_string(board.inventory[x / 2].numerator) + " ";
                    }
                    // TOP RIGHT CORNER
                    // Checking if the node is part of a quantum grouping
                    if (board.inventory[x / 2].quantumGrouping == 'z') {
                        currRow = currRow + " ";
                    }
                    else {
                        currRow = currRow + board.inventory[x / 2].quantumGrouping;
                    }
                }
                // Printing middle row of node
                else if (y % 4 == 2) {
                    // Checking if fraction is needed
                    if (board.inventory[x / 2].denominator != 1) {
                        currRow = currRow + " --- ";
                    }
                    // Checking if node contains a non-fraction mine
                    else if (board.inventory[x / 2].isMine == true && board.inventory[x / 2].numerator == 1) {
                        currRow = currRow + "  X  ";
                    }
                    // Otherwise, we place the mine value in the center
                    else if (board.inventory[x / 2].numerator / 100 > 0) {
                        currRow = currRow + " " + to_string(board.inventory[x / 2].numerator) + " ";
                    }
                    else if (board.inventory[x / 2].numerator / 10 > 0) {
                        currRow = currRow + "  " + to_string(board.inventory[x / 2].numerator) + " ";
                    }
                    else{
                        currRow = currRow + "  " + to_string(board.inventory[x / 2].numerator) + "  ";
                    }
                }
                // Printing bottom row of node
                else {
                    // BOTTOM LEFT CORNER
                    // Checking the type of node
                    if (board.inventory[x / 2].nodeType == 's') {
                        currRow = currRow + " ";
                    }
                    else {
                        currRow = currRow + board.inventory[x / 2].nodeType;
                    }
                    // BOTTOM CENTER
                    // Checking if fraction is needed
                    if (board.inventory[x / 2].denominator == 1) {
                        currRow = currRow + "   ";
                    }
                    // Checking if numerator is 3 digits
                    else if (board.inventory[x / 2].denominator / 100 > 0) {
                        currRow = currRow + to_string(board.inventory[x / 2].denominator);
                    }
                    // Checking if numerator is 2 digits
                    else if (board.inventory[x / 2].denominator / 10 > 0) {
                        currRow = currRow + " " + to_string(board.inventory[x / 2].denominator);
                    }
                    // Otherwise numerator is 1 digit
                    else {
                        currRow = currRow + " " + to_string(board.inventory[x / 2].denominator) + " ";
                    }
                    // BOTTOM RIGHT
                    // Only needs to be filled if the node is a non-singular mine
                    if ((board.inventory[x / 2].denominator != 1 || board.inventory[x / 2].numerator != 1) && (board.inventory[x / 2].isMine == true)) {
                        currRow = currRow + "X";
                    }
                    else {
                        currRow = currRow + " ";
                    }
                }
            }
        }
        cout << currRow << endl;
    }
}

bool checkNodeRequirements(board passedBoard, node passedNode) {
    // Node is empty or blocked
    if (passedNode.nodeType == 'e' || passedNode.nodeType == 'b') {
        return true;
    }
    // This will have to be changed once quantum mines are implemented
    else if (passedNode.isMine == true) {
        return true;
    }
    // Standard node
    else if (passedNode.nodeType == 's') {

    }
    // Function returns false if node type has not been implemented properly
    return false;
}

void playBoard(board board, cursor activeCursor) {
    // defining variables
    string playerInput = "";
    cursor placementCursor;
    node temp;
    bool isPlacing = false;
    // printing initial board conditions
    cout << "\n\n\n\n\n\n\n\n\n\n";
    printBoardData(board, activeCursor);
    printInventory(board, activeCursor);
    while (playerInput != "quit") {

        // TAKING PLAYER INPUT

        getline(cin, playerInput);
        // Checking if player quit
        if (playerInput == "quit") {
            return;
        }
        // Printing space before calculating new boardstate
        cout << "\n\n\n\n\n\n\n\n\n\n";
        // Input moves activeCursor
        if ((playerInput == "w") || (playerInput == "a") || (playerInput == "s") || (playerInput == "d")) {
            activeCursor = moveCursor(board, activeCursor, playerInput, isPlacing);
        }
        // Input "help"
        else if (playerInput == "help") {
            cout << "w/a/s/d - Move the cursor up, left, down, or right, respectively." << endl;
            cout << "place - Select a node to place / position to place node." << endl;
            cout << "cancel - If placing a node, cancel node placement." << endl;
            cout << "quit - Exit the puzzle." << endl;
        }
        // Input "place"
        else if (playerInput == "place" || (playerInput == "" && isPlacing == true)) {
            // Placement selection is both not placable and not empty
            if (activeCursor.posY != -1 && board.boardData[activeCursor.posY][activeCursor.posX].placableNode == false && board.boardData[activeCursor.posY][activeCursor.posX].nodeType != 'e') {
                cout << "Selected node cannot be altered. Please select a different node." << endl;
            }
            else {
                // First Placement Call
                if (isPlacing == false) {
                    isPlacing = true;
                    placementCursor = activeCursor;
                    activeCursor.posX = 0;
                    // Active cursor needs to move to grid
                    if (activeCursor.posY == -1) {
                        activeCursor.posY = 0;
                    }
                    // Active cursor needs to move to inventory
                    else {
                        activeCursor.posY = -1;
                    }
                    
                }
                // Second Placement Call
                else {
                    // Active Cursor is in inventory
                    if (activeCursor.posY == -1) {
                        temp = board.boardData[placementCursor.posY][placementCursor.posX];
                        board.boardData[placementCursor.posY][placementCursor.posX] = board.inventory[activeCursor.posX];
                        board.inventory[activeCursor.posX] = temp;
                    }
                    // Active Cursor is in board
                    else {
                        temp = board.boardData[activeCursor.posY][activeCursor.posX];
                        board.boardData[activeCursor.posY][activeCursor.posX] = board.inventory[placementCursor.posX];
                        board.inventory[placementCursor.posX] = temp;
                    }
                    activeCursor = placementCursor;
                    cout << "Node placed." << endl;
                    isPlacing = false;
                }
            }
        }
        // Input "cancel"
        else if (playerInput == "cancel") {
            // Not currently placing
            if (isPlacing == false) {
                cout << "You are not currently placing a node." << endl;
            }
            // Currently placing
            else {
                cout << "Placement cancelled." << endl;
                isPlacing = false;
                activeCursor = placementCursor;
            }
        }
        // Input unrecognized command
        else {
            cout << "Command not recognized. Type \"help\" for a list of commands." << endl;
        }
        // PRINTING BOARD
        if (isPlacing == false) {
            printBoardData(board, activeCursor);
            printInventory(board, activeCursor);
        }
        else if (activeCursor.posY == -1) {
            cout << "Select a node to place. Type \"cancel\" to cancel placement. Type \"place\" or press enter to confirm node to place." << endl;
            printBoardData(board, placementCursor);
            printInventory(board, activeCursor);
        }
        else {
            cout << "Choose a location to place the selected node. Type \"cancel\" to cancel placement. Type \"place\" or press enter to confirm placement location." << endl;
            printBoardData(board, activeCursor);
            printInventory(board, placementCursor);
        }
    }
    return;
}

// s.z.000.000.0.0
// nodeType.quantumGrouping.numerator.denominator.placableNode.isMine
int main(int argc, char** argv)
{
    board testBoard;
    cursor testCursor;
    testBoard = fillBoard(testBoard, "testBoard.txt");

    testCursor.posX = 0;
    testCursor.posY = 0;
    

    playBoard(testBoard, testCursor);


    // printBoardData(testBoard, testCursor);
    // printInventory(testBoard, testCursor);
    
    /*
    printNode(testBoard2.boardData[0][0]);
    printNode(testBoard2.boardData[0][1]);
    printNode(testBoard2.boardData[1][0]);
    printNode(testBoard2.boardData[1][1]);
    */

    return 0;
}

/* Graveyarded Functions:

// Function is no longer useful
void initializeNodeDisplayData(vector<string>& nodeDisplayData, string fileName) {
    string currentLine = "";
    ifstream fin;
    fin.open(fileName);
    // this checks that the file has opened successfully
    if (fin.is_open() == false)
    {
        fin.close();
        cout << "File failed to open." << endl;
        return;
    }
    // this runs until the file runs out of lines
    while (getline(fin, currentLine)) {
        nodeDisplayData.push_back(currentLine.substr(0,5));
    }
    fin.close();
    return;
}

// prints the contents of the boardData
void printBoardData(board board) {
    for (int y = 0; y < board.sizeY; y++) {
        for (int x = 0; x < board.sizeX; x++) {
            cout << board.boardData[y][x] << " ";
        }
        cout << endl;
    }
}





*/
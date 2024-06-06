#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <iomanip>
#include <sstream>

using namespace std;

// Function to get current time as string
string getCurrentTime() {
    time_t now = time(0);
    tm *ltm = localtime(&now);
    stringstream ss;
    ss << 1900 + ltm->tm_year << "-"
       << setw(2) << setfill('0') << 1 + ltm->tm_mon << "-"
       << setw(2) << setfill('0') << ltm->tm_mday << " "
       << setw(2) << setfill('0') << ltm->tm_hour << ":"
       << setw(2) << setfill('0') << ltm->tm_min << ":"
       << setw(2) << setfill('0') << ltm->tm_sec;
    return ss.str();
}

// Function to log a message to the CSV file
void logMessage(const string& message) {
    ofstream logFile("logs.csv", ios::app);
    if (logFile.is_open()) {
        logFile << getCurrentTime() << "," << message << endl;
        logFile.close();
    } else {
        cerr << "Unable to open log file" << endl;
    }
}

// Function to save the game result to a CSV file
void saveGameResult(const string& result, bool reached2048) {
    ofstream resultFile("game_results.csv", ios::app);
    if (resultFile.is_open()) {
        resultFile << getCurrentTime() << "," << result << "," << (reached2048 ? "Yes" : "No") << endl;
        resultFile.close();
    } else {
        cerr << "Unable to open result file" << endl;
    }
}

// Function to initialize the game grid
vector<vector<int>> initializeGrid() {
    return {{0, 0, 0},
            {0, 0, 0},
            {0, 0, 0}};
}

// Function to add a new 2 or 4 tile to the grid
bool addNewTile(vector<vector<int>>& grid) {
    vector<pair<int, int>> emptyCells;
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            if (grid[i][j] == 0) {
                emptyCells.push_back({i, j});
            }
        }
    }
    if (!emptyCells.empty()) {
        int index = rand() % emptyCells.size();
        int value = (rand() % 2 + 1) * 2; // Either 2 or 4
        grid[emptyCells[index].first][emptyCells[index].second] = value;
        return true;
    }
    return false;
}

// Function to print the game grid
void printGrid(const vector<vector<int>>& grid) {
    for (const auto& row : grid) {
        for (int val : row) {
            cout << val << " | ";
        }
        cout << endl;
        cout << "-----------" << endl;
    }
}

// Function to check if the game is over
bool isGameOver(const vector<vector<int>>& grid) {
    // Check for any empty cell
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            if (grid[i][j] == 0) {
                return false;
            }
        }
    }
    // Check for possible merges
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 2; ++j) {
            if (grid[i][j] == grid[i][j + 1]) {
                return false;
            }
        }
    }
    for (int j = 0; j < 3; ++j) {
        for (int i = 0; i < 2; ++i) {
            if (grid[i][j] == grid[i + 1][j]) {
                return false;
            }
        }
    }
    return true;
}

// Function to swipe left
bool swipeLeft(vector<vector<int>>& grid) {
    bool moved = false;
    for (int i = 0; i < 3; ++i) {
        // Move all tiles to the left
        vector<int> newRow;
        for (int j = 0; j < 3; ++j) {
            if (grid[i][j] != 0) {
                newRow.push_back(grid[i][j]);
            }
        }
        while (newRow.size() < 3) {
            newRow.push_back(0);
        }
        // Merge adjacent tiles of the same value
        for (int j = 0; j < 2; ++j) {
            if (newRow[j] == newRow[j+1] && newRow[j] != 0) {
                newRow[j] *= 2;
                newRow[j+1] = 0;
                moved = true;
            }
        }
        // Move merged tiles to the left
        vector<int> finalRow;
        for (int j = 0; j < 3; ++j) {
            if (newRow[j] != 0) {
                finalRow.push_back(newRow[j]);
            }
        }
        while (finalRow.size() < 3) {
            finalRow.push_back(0);
        }
        if (grid[i] != finalRow) {
            moved = true;
            grid[i] = finalRow;
        }
    }
    return moved;
}

// Function to swipe right
bool swipeRight(vector<vector<int>>& grid) {
    // Reverse each row, then swipe left, then reverse again
    bool moved = false;
    for (auto& row : grid) {
        reverse(row.begin(), row.end());
    }
    moved = swipeLeft(grid);
    for (auto& row : grid) {
        reverse(row.begin(), row.end());
    }
    return moved;
}

// Function to swipe up
bool swipeUp(vector<vector<int>>& grid) {
    // Transpose the grid, then swipe left, then transpose again
    vector<vector<int>> transposedGrid = grid;
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            transposedGrid[i][j] = grid[j][i];
        }
    }
    bool moved = swipeLeft(transposedGrid);
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            grid[i][j] = transposedGrid[j][i];
        }
    }
    return moved;
}

// Function to swipe down
bool swipeDown(vector<vector<int>>& grid) {
    // Transpose the grid, reverse each row, swipe left, reverse again, then transpose again
    vector<vector<int>> transposedGrid = grid;
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            transposedGrid[i][j] = grid[j][i];
        }
    }
    for (auto& row : transposedGrid) {
        reverse(row.begin(), row.end());
    }
    bool moved = swipeLeft(transposedGrid);
    for (auto& row : transposedGrid) {
        reverse(row.begin(), row.end());
    }
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            grid[i][j] = transposedGrid[j][i];
        }
    }
    return moved;
}

// Main function to start the game
void playGame() {
    srand(time(0));
    vector<vector<int>> grid = initializeGrid();
    addNewTile(grid);
    addNewTile(grid);
    bool reached2048 = false;
    logMessage("User started the game");

    while (true) {
        system("cls||clear");
        printGrid(grid);
        if (reached2048) {
            cout << "Congratulations! You reached 2048. Do you want to continue? (Y/N): ";
            char choice;
            cin >> choice;
            if (choice == 'N' || choice == 'n') {
                cout << "Game Over!" << endl;
                logMessage("User ended the game after reaching 2048");
                saveGameResult("Won", true);
                break;
            } else {
                reached2048 = false; // Reset the flag to continue playing
            }
        }
        if (isGameOver(grid)) {
            cout << "Game Over! No more moves available." << endl;
            logMessage("Game over");
            saveGameResult("Lost", reached2048);
            break;
        }
        cout << "Enter swipe direction (up/down/left/right): ";
        string direction;
        cin >> direction;
        bool moved = false;
        if (direction == "left") {
            moved = swipeLeft(grid);
        } else if (direction == "right") {
            moved = swipeRight(grid);
        } else if (direction == "up") {
            moved = swipeUp(grid);
        } else if (direction == "down") {
            moved = swipeDown(grid);
        } else {
            cout << "Invalid direction! Please enter one of: up/down/left/right" << endl;
        }
        if (moved) {
            logMessage("User swiped " + direction);
            addNewTile(grid);
        }
    }
    cout << "Press Enter to exit..." << endl;
    cin.ignore();
    cin.get();
}

int main() {
    playGame();
    return 0;
}
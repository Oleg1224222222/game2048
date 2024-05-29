#include <iostream>
#include <vector>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iomanip> // For std::setw

using namespace std;

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
    cout << "-------------------" << endl;
    for (const auto& row : grid) {
        cout << "|";
        for (int val : row) {
            if (val == 0) {
                cout << setw(4) << " " << " |";
            } else {
                cout << setw(4) << val << " |";
            }
        }
        cout << endl;
        cout << "-------------------" << endl;
    }
}

// Function to check if the game is over
bool isGameOver(const vector<vector<int>>& grid) {
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            if (grid[i][j] == 0) {
                return false;
            }
            if (j < 2 && grid[i][j] == grid[i][j+1]) {
                return false;
            }
            if (i < 2 && grid[i][j] == grid[i+1][j]) {
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

// Function to save game statistics to a file
void saveGameStats(const vector<vector<int>>& grid, bool reached2048) {
    ofstream file("game_stats.txt", ios::app);
    if (!file.is_open()) {
        cout << "Error: Unable to open file for saving game stats." << endl;
        return;
    }
    file << "Game Grid:" << endl;
    for (const auto& row : grid) {
        for (int val : row) {
            file << val << " ";
        }
        file << endl;
    }
    file << "Reached 2048: " << (reached2048 ? "Yes" : "No") << endl;
    file << "--------------------------" << endl;
    file.close();
}

// Main function to start the game
void playGame() {
    srand(time(0));
    vector<vector<int>> grid = initializeGrid();
    addNewTile(grid);
    addNewTile(grid);
    bool reached2048 = false;
    while (true) {
        system("cls||clear");
        printGrid(grid);
        if (reached2048 || isGameOver(grid)) {
            cout << "Game Over!" << endl;
            saveGameStats(grid, reached2048); // Save game stats
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
            if (addNewTile(grid)) {
                // Check if 2048 is reached
                for (const auto& row : grid) {
                    for (int val : row) {
                        if (val == 2048) {
                            reached2048 = true;
                            cout << "Congratulations! You reached 2048. Do you want to continue? (Y/N): ";
                            char choice;
                            cin >> choice;
                            if (choice == 'N' || choice == 'n') {
                                cout << "Game Over!" << endl;
                                saveGameStats(grid, reached2048); // Save game stats
                                return;
                            }
                        }
                    }
                }
            } else {
                cout << "Game Over! No more moves possible." << endl;
                saveGameStats(grid, reached2048); // Save game stats
                break;
            }
        }
    }
    cout << "Press Enter to exit..." << endl;
    cin.ignore(); // Ignore the newline left in the input buffer
    cin.get(); // Wait for the user to press Enter
}

// Main function
int main() {
    playGame();
    return 0;
}

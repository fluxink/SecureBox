#include <iostream>
#include <vector>
#include <random>
#include <time.h>
#include <iomanip>
#include <cstdlib>

//===========================================================================
// # PROBLEM: Total Unlocking of the SecureBox
//===========================================================================
// SecureBox is a 2D grid (y rows × x columns) of integer values:
//     0 → fully unlocked
//     1 → partially locked
//     2 → fully locked
//
// Goal: Use toggle(x, y) operations to reach a fully unlocked state (all 0s).
//
//===========================================================================
// # SecureBox API
//===========================================================================

class SecureBox
{
private:
    std::vector<std::vector<uint8_t>> box;
    std::mt19937_64 rng;
    uint32_t xSize, ySize;

public:
    SecureBox(uint32_t x, uint32_t y) : xSize(x), ySize(y)
    {
        rng.seed(time(0));
        box.resize(y);
        for (auto &row : box)
            row.resize(x, 0);
        shuffle();
    }

    void toggle(uint32_t x, uint32_t y)
    {
        // Vertical (column)
        for (uint32_t row = 0; row < ySize; ++row)
            box[row][x] = (box[row][x] + 1) % 3;

        // Horizontal (row)
        for (uint32_t col = 0; col < xSize; ++col)
            box[y][col] = (box[y][col] + 1) % 3;

        // Center cell was incremented twice, fix it to be +1 total
        box[y][x] = (box[y][x] + 2) % 3;
    }

    bool isLocked() const
    {
        for (const auto &row : box)
            for (uint8_t cell : row)
                if (cell != 0)
                    return true;
        return false;
    }

    std::vector<std::vector<uint8_t>> getState() const
    {
        return box;
    }

    uint32_t getWidth() const { return xSize; }
    uint32_t getHeight() const { return ySize; }

private:
    void shuffle()
    {
        for (uint32_t t = rng() % 0x1000; t > 0; --t)
            toggle(rng() % xSize, rng() % ySize);
    }
};

//================================================================================
// Cross-platform Visualization Helper Functions
//================================================================================

void clearScreen()
{
    // ANSI escape sequence to clear screen and move cursor to top-left
    std::cout << "\033[2J\033[H";
    std::cout.flush();
}

void setColor(const std::string &colorCode)
{
    std::cout << colorCode;
}

void resetColor()
{
    std::cout << "\033[0m";
}

// ANSI color codes
const std::string RED = "\033[31m";
const std::string GREEN = "\033[32m";
const std::string YELLOW = "\033[33m";
const std::string BLUE = "\033[34m";
const std::string MAGENTA = "\033[35m";
const std::string CYAN = "\033[36m";
const std::string WHITE = "\033[37m";
const std::string BOLD = "\033[1m";
const std::string RESET = "\033[0m";

void displayBox(const SecureBox &box, const std::string &title = "SecureBox State")
{
    auto state = box.getState();

    std::cout << BOLD << CYAN << "\n"
              << title << RESET << "\n";
    std::cout << std::string(title.length(), '=') << "\n";

    // Column headers
    std::cout << "   ";
    for (uint32_t x = 0; x < box.getWidth(); ++x)
        std::cout << std::setw(3) << x;
    std::cout << "\n";

    // Grid with row headers
    for (uint32_t y = 0; y < box.getHeight(); ++y)
    {
        std::cout << std::setw(2) << y << " ";
        for (uint32_t x = 0; x < box.getWidth(); ++x)
        {
            uint8_t value = state[y][x];

            // Color coding: Green=0, Yellow=1, Red=2
            if (value == 0)
                std::cout << GREEN << "[" << (int)value << "]" << RESET;
            else if (value == 1)
                std::cout << YELLOW << "[" << (int)value << "]" << RESET;
            else
                std::cout << RED << "[" << (int)value << "]" << RESET;
        }
        std::cout << "\n";
    }

    std::cout << "\nLegend: " << GREEN << "[0]=Unlocked" << RESET
              << " " << YELLOW << "[1]=Partial" << RESET
              << " " << RED << "[2]=Locked" << RESET << "\n";

    if (box.isLocked())
        std::cout << "Status: " << RED << "LOCKED" << RESET << "\n\n";
    else
        std::cout << "Status: " << GREEN << "UNLOCKED" << RESET << "\n\n";
}

void displayProgress(int step, uint32_t x, uint32_t y)
{
    std::cout << BOLD << MAGENTA << "Step " << step << ": Toggle(" << x << ", " << y << ")" << RESET << "\n";
}

void waitForEnter(const std::string &message = "Press Enter to continue...")
{
    std::cout << CYAN << message << RESET;
    std::cin.get();
}

//================================================================================
// Matrix operations for Gaussian elimination over GF(3)
//================================================================================

int modInverse(int a, int mod)
{
    for (int i = 1; i < mod; ++i)
        if ((a * i) % mod == 1)
            return i;
    return 1;
}

std::vector<int> solveLinearSystem(std::vector<std::vector<int>> matrix, std::vector<int> target)
{
    int n = matrix.size();
    int m = matrix[0].size();

    // Augment matrix with target vector
    for (int i = 0; i < n; ++i)
        matrix[i].push_back(target[i]);

    // Gaussian elimination
    for (int col = 0, row = 0; col < m && row < n; ++col)
    {
        // Find pivot
        int pivot = -1;
        for (int i = row; i < n; ++i)
        {
            if (matrix[i][col] != 0)
            {
                pivot = i;
                break;
            }
        }

        if (pivot == -1)
            continue;

        // Swap rows
        if (pivot != row)
            std::swap(matrix[pivot], matrix[row]);

        // Make pivot 1
        int pivotVal = matrix[row][col];
        int inv = modInverse(pivotVal, 3);
        for (int j = col; j <= m; ++j)
            matrix[row][j] = (matrix[row][j] * inv) % 3;

        // Eliminate column
        for (int i = 0; i < n; ++i)
        {
            if (i != row && matrix[i][col] != 0)
            {
                int factor = matrix[i][col];
                for (int j = col; j <= m; ++j)
                    matrix[i][j] = (matrix[i][j] - factor * matrix[row][j] + 9) % 3;
            }
        }
        row++;
    }

    // Extract solution
    std::vector<int> solution(m, 0);
    for (int i = 0; i < std::min(n, m); ++i)
    {
        for (int j = 0; j < m; ++j)
        {
            if (matrix[i][j] == 1)
            {
                solution[j] = matrix[i][m];
                break;
            }
        }
    }

    return solution;
}

//================================================================================
// Main solving algorithm
//================================================================================

bool openBox(SecureBox &box)
{
    uint32_t width = box.getWidth();
    uint32_t height = box.getHeight();
    int totalCells = width * height;

    clearScreen();
    displayBox(box, "Initial SecureBox State");
    waitForEnter("Press Enter to start solving...");

    // Create effect matrix: each row represents what happens when we toggle position (i,j)
    std::vector<std::vector<int>> effectMatrix(totalCells, std::vector<int>(totalCells, 0));

    for (int toggleY = 0; toggleY < (int)height; ++toggleY)
    {
        for (int toggleX = 0; toggleX < (int)width; ++toggleX)
        {
            int toggleIndex = toggleY * width + toggleX;

            // Effect on column
            for (int y = 0; y < (int)height; ++y)
            {
                int cellIndex = y * width + toggleX;
                effectMatrix[cellIndex][toggleIndex] = (effectMatrix[cellIndex][toggleIndex] + 1) % 3;
            }

            // Effect on row
            for (int x = 0; x < (int)width; ++x)
            {
                int cellIndex = toggleY * width + x;
                effectMatrix[cellIndex][toggleIndex] = (effectMatrix[cellIndex][toggleIndex] + 1) % 3;
            }

            // Center cell gets extra increment
            int centerIndex = toggleY * width + toggleX;
            effectMatrix[centerIndex][toggleIndex] = (effectMatrix[centerIndex][toggleIndex] + 2) % 3;
        }
    }

    // Convert current state to target vector
    auto currentState = box.getState();
    std::vector<int> target(totalCells);
    for (int y = 0; y < (int)height; ++y)
    {
        for (int x = 0; x < (int)width; ++x)
        {
            int index = y * width + x;
            // We want to reach 0, so target = (3 - current) % 3
            target[index] = (3 - currentState[y][x]) % 3;
        }
    }

    // Solve the linear system
    std::cout << YELLOW << "Solving linear system..." << RESET << "\n";
    std::vector<int> solution = solveLinearSystem(effectMatrix, target);

    // Apply the solution with visualization
    int step = 1;
    bool foundMoves = false;

    for (int y = 0; y < (int)height; ++y)
    {
        for (int x = 0; x < (int)width; ++x)
        {
            int index = y * width + x;
            int toggleCount = solution[index];

            if (toggleCount > 0)
                foundMoves = true;

            for (int t = 0; t < toggleCount; ++t)
            {
                clearScreen();
                displayProgress(step++, x, y);
                box.toggle(x, y);
                displayBox(box, "SecureBox State After Toggle");

                if (box.isLocked())
                    waitForEnter();
                else
                {
                    std::cout << BOLD << GREEN << "SUCCESS! Box is now unlocked!" << RESET << "\n";
                    waitForEnter("Press Enter to finish...");
                    return true;
                }
            }
        }
    }

    if (!foundMoves)
    {
        clearScreen();
        std::cout << BOLD << GREEN << "Box was already unlocked or solution requires no moves!" << RESET << "\n";
        displayBox(box, "Final SecureBox State");
    }

    return !box.isLocked();
}

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        std::cout << "Usage: " << argv[0] << " <width> <height>\n";
        std::cout << "Example: " << argv[0] << " 4 3\n";
        return 1;
    }

    uint32_t x = std::atol(argv[1]);
    uint32_t y = std::atol(argv[2]);

    if (x == 0 || y == 0 || x > 10 || y > 10)
    {
        std::cout << RED << "Please use dimensions between 1 and 10." << RESET << "\n";
        return 1;
    }

    SecureBox box(x, y);
    bool state = openBox(box);

    clearScreen();
    displayBox(box, "Final SecureBox State");

    if (!state)
    {
        std::cout << BOLD << RED << "BOX: LOCKED!" << RESET << std::endl;
    }
    else
    {
        std::cout << BOLD << GREEN << "BOX: OPENED!" << RESET << std::endl;
    }

    return state ? 0 : 1;
}
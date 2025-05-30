#include <iostream>
#include <vector>
#include <random>
#include <time.h>
#include <iomanip>
#include <cstdlib>
#include <string>
#include <thread>
#include <chrono>

// OpenGL headers
#include <glad/gl.h>
#include <GLFW/glfw3.h>

//===========================================================================
// # PROBLEM: Total Unlocking of the SecureBox
//===========================================================================
// SecureBox is a 2D grid (y rows × x columns) of integer values:
//     0 → fully unlocked
//     1 → partially locked
//     2 → fully locked
//
// Goal: Use toggle(x, y) operations to reach a fully unlocked state (all 0s).
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
// Console fallback implementation
//================================================================================

void clearScreen()
{
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

const std::string RED = "\033[31m";
const std::string GREEN = "\033[32m";
const std::string YELLOW = "\033[33m";
const std::string BLUE = "\033[34m";
const std::string MAGENTA = "\033[35m";
const std::string CYAN = "\033[36m";
const std::string WHITE = "\033[37m";
const std::string BOLD = "\033[1m";
const std::string RESET = "\033[0m";

void displayBoxConsole(const SecureBox &box, const std::string &title = "SecureBox State")
{
    auto state = box.getState();

    std::cout << BOLD << CYAN << "\n"
              << title << RESET << "\n";
    std::cout << std::string(title.length(), '=') << "\n";

    std::cout << "   ";
    for (uint32_t x = 0; x < box.getWidth(); ++x)
        std::cout << std::setw(3) << x;
    std::cout << "\n";

    for (uint32_t y = 0; y < box.getHeight(); ++y)
    {
        std::cout << std::setw(2) << y << " ";
        for (uint32_t x = 0; x < box.getWidth(); ++x)
        {
            uint8_t value = state[y][x];

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

void waitForEnter(const std::string &message = "Press Enter to continue...")
{
    std::cout << CYAN << message << RESET;
    std::cin.get();
}

//================================================================================
// OpenGL implementation
//================================================================================

class OpenGLRenderer
{
private:
    GLFWwindow* window;
    GLuint shaderProgram;
    GLuint VAO, VBO;
    int windowWidth, windowHeight;
    bool shouldClose;
    
    const char* vertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec3 aColor;

out vec3 color;

uniform mat4 projection;

void main()
{
    gl_Position = projection * vec4(aPos, 0.0, 1.0);
    color = aColor;
}
)";

    const char* fragmentShaderSource = R"(
#version 330 core
in vec3 color;
out vec4 FragColor;

void main()
{
    FragColor = vec4(color, 1.0);
}
)";

public:
    OpenGLRenderer() : window(nullptr), shaderProgram(0), VAO(0), VBO(0), shouldClose(false) {}
    
    bool initialize(int width, int height)
    {
        windowWidth = width;
        windowHeight = height;
        
        if (!glfwInit())
        {
            std::cout << "Failed to initialize GLFW" << std::endl;
            return false;
        }

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

        window = glfwCreateWindow(width, height, "SecureBox Solver", NULL, NULL);
        if (!window)
        {
            std::cout << "Failed to create GLFW window" << std::endl;
            glfwTerminate();
            return false;
        }

        glfwMakeContextCurrent(window);
        glfwSetWindowUserPointer(window, this);
        glfwSetKeyCallback(window, keyCallback);

        int version = gladLoadGL(glfwGetProcAddress);
        if (version == 0)
        {
            std::cout << "Failed to initialize OpenGL context" << std::endl;
            glfwTerminate();
            return false;
        }

        std::cout << "Loaded OpenGL " << GLAD_VERSION_MAJOR(version) << "." << GLAD_VERSION_MINOR(version) << std::endl;

        if (!createShaders())
        {
            cleanup();
            return false;
        }

        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);

        glViewport(0, 0, width, height);
        return true;
    }

    void renderBox(const SecureBox &box, const std::string &title, int step = -1, int toggleX = -1, int toggleY = -1)
    {
        if (!window || glfwWindowShouldClose(window))
            return;

        glfwPollEvents();

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        auto state = box.getState();
        uint32_t width = box.getWidth();
        uint32_t height = box.getHeight();

        float cellSize = std::min(1.6f / width, 1.6f / height);
        float startX = -(width * cellSize) / 2.0f;
        float startY = (height * cellSize) / 2.0f;

        std::vector<float> vertices;

        for (uint32_t y = 0; y < height; ++y)
        {
            for (uint32_t x = 0; x < width; ++x)
            {
                float left = startX + x * cellSize;
                float right = left + cellSize * 0.9f;
                float top = startY - y * cellSize;
                float bottom = top - cellSize * 0.9f;

                uint8_t value = state[y][x];
                float r, g, b;
                
                if (value == 0) {
                    r = 0.0f; g = 1.0f; b = 0.0f; // Green
                } else if (value == 1) {
                    r = 1.0f; g = 1.0f; b = 0.0f; // Yellow
                } else {
                    r = 1.0f; g = 0.0f; b = 0.0f; // Red
                }

                // Highlight current toggle position
                if (step >= 0 && x == toggleX && y == toggleY) {
                    r = std::min(1.0f, r + 0.3f);
                    g = std::min(1.0f, g + 0.3f);
                    b = std::min(1.0f, b + 0.3f);
                }

                // Two triangles per cell
                float cellVertices[] = {
                    left,  bottom, r, g, b,
                    right, bottom, r, g, b,
                    right, top,    r, g, b,
                    
                    left,  bottom, r, g, b,
                    right, top,    r, g, b,
                    left,  top,    r, g, b
                };

                for (int i = 0; i < 30; ++i)
                    vertices.push_back(cellVertices[i]);
            }
        }

        glUseProgram(shaderProgram);
        
        // Set projection matrix (orthographic)
        float projection[16] = {
            1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f
        };
        
        GLuint projLoc = glGetUniformLocation(shaderProgram, "projection");
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, projection);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_DYNAMIC_DRAW);

        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(2 * sizeof(float)));
        glEnableVertexAttribArray(1);

        glDrawArrays(GL_TRIANGLES, 0, vertices.size() / 5);

        glfwSwapBuffers(window);
    }

    bool shouldCloseWindow()
    {
        return window && (glfwWindowShouldClose(window) || shouldClose);
    }

    void waitForSpace()
    {
        if (!window) return;
        
        shouldClose = false;
        while (!shouldCloseWindow())
        {
            glfwPollEvents();
            std::this_thread::sleep_for(std::chrono::milliseconds(16));
        }
        shouldClose = false;
    }

    void cleanup()
    {
        if (VAO) glDeleteVertexArrays(1, &VAO);
        if (VBO) glDeleteBuffers(1, &VBO);
        if (shaderProgram) glDeleteProgram(shaderProgram);
        if (window) glfwDestroyWindow(window);
        glfwTerminate();
    }

private:
    static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mode)
    {
        OpenGLRenderer* renderer = static_cast<OpenGLRenderer*>(glfwGetWindowUserPointer(window));
        if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
            glfwSetWindowShouldClose(window, GL_TRUE);
        if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
            renderer->shouldClose = true;
    }

    bool createShaders()
    {
        GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
        glCompileShader(vertexShader);

        GLint success;
        char infoLog[512];
        glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
            std::cout << "Vertex shader compilation failed: " << infoLog << std::endl;
            return false;
        }

        GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
        glCompileShader(fragmentShader);

        glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
            std::cout << "Fragment shader compilation failed: " << infoLog << std::endl;
            return false;
        }

        shaderProgram = glCreateProgram();
        glAttachShader(shaderProgram, vertexShader);
        glAttachShader(shaderProgram, fragmentShader);
        glLinkProgram(shaderProgram);

        glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
            std::cout << "Shader program linking failed: " << infoLog << std::endl;
            return false;
        }

        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        return true;
    }
};

//================================================================================
// Main solving algorithm with dual rendering support
//================================================================================

bool openBox(SecureBox &box, bool useOpenGL)
{
    uint32_t width = box.getWidth();
    uint32_t height = box.getHeight();
    int totalCells = width * height;

    OpenGLRenderer* renderer = nullptr;
    if (useOpenGL)
    {
        renderer = new OpenGLRenderer();
        if (!renderer->initialize(800, 600))
        {
            delete renderer;
            renderer = nullptr;
            useOpenGL = false;
            std::cout << "Falling back to console mode..." << std::endl;
        }
    }

    if (useOpenGL && renderer)
    {
        renderer->renderBox(box, "Initial SecureBox State");
        std::cout << "Press SPACE to continue, ESC to exit..." << std::endl;
        renderer->waitForSpace();
    }
    else
    {
        clearScreen();
        displayBoxConsole(box, "Initial SecureBox State");
        waitForEnter("Press Enter to start solving...");
    }

    // Create effect matrix
    std::vector<std::vector<int>> effectMatrix(totalCells, std::vector<int>(totalCells, 0));

    for (int toggleY = 0; toggleY < (int)height; ++toggleY)
    {
        for (int toggleX = 0; toggleX < (int)width; ++toggleX)
        {
            int toggleIndex = toggleY * width + toggleX;

            for (int y = 0; y < (int)height; ++y)
            {
                int cellIndex = y * width + toggleX;
                effectMatrix[cellIndex][toggleIndex] = (effectMatrix[cellIndex][toggleIndex] + 1) % 3;
            }

            for (int x = 0; x < (int)width; ++x)
            {
                int cellIndex = toggleY * width + x;
                effectMatrix[cellIndex][toggleIndex] = (effectMatrix[cellIndex][toggleIndex] + 1) % 3;
            }

            int centerIndex = toggleY * width + toggleX;
            effectMatrix[centerIndex][toggleIndex] = (effectMatrix[centerIndex][toggleIndex] + 2) % 3;
        }
    }

    auto currentState = box.getState();
    std::vector<int> target(totalCells);
    for (int y = 0; y < (int)height; ++y)
    {
        for (int x = 0; x < (int)width; ++x)
        {
            int index = y * width + x;
            target[index] = (3 - currentState[y][x]) % 3;
        }
    }

    std::cout << "Solving linear system..." << std::endl;
    std::vector<int> solution = solveLinearSystem(effectMatrix, target);

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
                if (useOpenGL && renderer)
                {
                    if (renderer->shouldCloseWindow())
                    {
                        renderer->cleanup();
                        delete renderer;
                        return false;
                    }
                    
                    std::cout << "Step " << step++ << ": Toggle(" << x << ", " << y << ") - Press SPACE to continue" << std::endl;
                    box.toggle(x, y);
                    renderer->renderBox(box, "SecureBox State After Toggle", step, x, y);
                    
                    if (!box.isLocked())
                    {
                        std::cout << "SUCCESS! Box is now unlocked!" << std::endl;
                        renderer->waitForSpace();
                        renderer->cleanup();
                        delete renderer;
                        return true;
                    }
                    renderer->waitForSpace();
                }
                else
                {
                    clearScreen();
                    std::cout << "Step " << step++ << ": Toggle(" << x << ", " << y << ")" << std::endl;
                    box.toggle(x, y);
                    displayBoxConsole(box, "SecureBox State After Toggle");

                    if (box.isLocked())
                        waitForEnter();
                    else
                    {
                        std::cout << "SUCCESS! Box is now unlocked!" << std::endl;
                        waitForEnter("Press Enter to finish...");
                        return true;
                    }
                }
            }
        }
    }

    if (renderer)
    {
        renderer->cleanup();
        delete renderer;
    }

    if (!foundMoves)
    {
        if (!useOpenGL)
        {
            clearScreen();
            std::cout << "Box was already unlocked or solution requires no moves!" << std::endl;
            displayBoxConsole(box, "Final SecureBox State");
        }
    }

    return !box.isLocked();
}

int main(int argc, char *argv[])
{
    if (argc < 3 || argc > 4)
    {
        std::cout << "Usage: " << argv[0] << " <width> <height> [--console]" << std::endl;
        std::cout << "Example: " << argv[0] << " 4 3" << std::endl;
        std::cout << "         " << argv[0] << " 4 3 --console" << std::endl;
        return 1;
    }

    uint32_t x = std::atol(argv[1]);
    uint32_t y = std::atol(argv[2]);
    bool forceConsole = (argc == 4 && std::string(argv[3]) == "--console");

    if (x == 0 || y == 0 || x > 10 || y > 10)
    {
        std::cout << "Please use dimensions between 1 and 10." << std::endl;
        return 1;
    }

    SecureBox box(x, y);
    bool useOpenGL = !forceConsole;
    
    if (useOpenGL)
        std::cout << "Attempting to use OpenGL rendering..." << std::endl;
    else
        std::cout << "Using console mode..." << std::endl;

    bool state = openBox(box, useOpenGL);

    if (!useOpenGL)
    {
        clearScreen();
        displayBoxConsole(box, "Final SecureBox State");
    }

    if (!state)
    {
        std::cout << "BOX: LOCKED!" << std::endl;
    }
    else
    {
        std::cout << "BOX: OPENED!" << std::endl;
    }

    return state ? 0 : 1;
}
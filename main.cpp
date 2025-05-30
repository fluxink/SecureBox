#include <iostream>
#include <vector>
#include <random>
#include <time.h>
#include <iomanip>
#include <cstdlib>
#include <string>
#include <thread>
#include <chrono>
#include <mutex>
#include <array>

// OpenGL headers
#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <algorithm>

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
    bool spacePressed;

    std::array<float, 2> nextMovePos;
    bool hasNextMove;
    
    // Animation state
    float currentTime;
    std::vector<std::vector<uint8_t>> currentBoxState;
    std::mutex stateMutex;
    
    // Multiple animation effects
    struct AnimationEffect {
        int step;
        int toggleX, toggleY;
        float startTime;
        float duration;
        bool active;
        
        AnimationEffect(int s, int x, int y, float start, float dur = 1.0f) 
            : step(s), toggleX(x), toggleY(y), startTime(start), duration(dur), active(true) {}
    };
    
    std::vector<AnimationEffect> activeEffects;
    
    const char* vertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aCellPos;

out vec3 color;
out vec2 cellPos;

uniform mat4 projection;

void main()
{
    gl_Position = projection * vec4(aPos, 0.0, 1.0);
    color = aColor;
    cellPos = aCellPos;
}
)";

    const char* fragmentShaderSource = R"(
#version 330 core
in vec3 color;
in vec2 cellPos;
out vec4 FragColor;

uniform float iTime;
uniform int numEffects;
uniform vec2 effectPositions[10];  // Max 10 concurrent effects
uniform float effectStartTimes[10];
uniform float effectDurations[10];
uniform vec2 nextMovePos;          // Position of the next move to highlight
uniform bool hasNextMove;          // Whether we have a next move to highlight

void main()
{
    // Base cell color
    vec3 finalColor = color;
        
    // Global wave/ripple effects across the entire grid
    float globalWaveEffect = 0.0;
    float globalRingEffect = 0.0;
    
    for (int i = 0; i < numEffects && i < 10; ++i) {
        vec2 effectPos = effectPositions[i];
        float effectTime = iTime - effectStartTimes[i];
        float duration = effectDurations[i];
        
        if (effectTime >= 0.0 && effectTime <= duration) {
            float progress = effectTime / duration;
            float falloff = smoothstep(1.0, 0.0, progress); // Fade out over time
            float dist = distance(cellPos, effectPos);
            
            // Global wave ripple effect using mask approach
            float waveSpeed = 6.0;
            float waveRadius = progress * 10.0;
            float waveWidth = 3.0;
            
            // Create smooth ripple mask based on distance
            float waveMask = smoothstep(waveWidth, 0.0, abs(dist - waveRadius)) * falloff;
            globalWaveEffect += waveMask * 0.3;
            
            // Expanding ring effect (as a separate visual layer)
            float ringRadius = progress * 5.0;
            float ringWidth = 0.4;
            float ringMask = smoothstep(ringWidth, 0.0, abs(dist - ringRadius)) * falloff;
            globalRingEffect += ringMask * 0.6;
            
            // Enhanced highlight for affected row/column cells
            if (abs(cellPos.x - effectPos.x) < 0.1 || abs(cellPos.y - effectPos.y) < 0.1) {
                float pulseFreq = 8.0;
                float pulse = sin(effectTime * pulseFreq) * falloff * 0.3 + 0.3;
                finalColor = mix(finalColor, vec3(1.0, 1.0, 0.8), pulse);
            }
        }
    }

    // Highlight the row and column of the next move with a subtle effect
    if (hasNextMove) {
        float highlightIntensity = 0.15;
        if (abs(cellPos.x - nextMovePos.x) < 0.1 || abs(cellPos.y - nextMovePos.y) < 0.1) {
            finalColor = mix(finalColor, vec3(1.0, 1.0, 1.0), highlightIntensity);
        }
    }
    
    // Apply global wave effect as a modifier
    finalColor *= (1.0 + globalWaveEffect);
    
    // Apply ring effect as an overlay
    finalColor += vec3(0.3, 0.5, 1.0) * globalRingEffect;
    
    // Clamp the final color
    finalColor = clamp(finalColor, 0.0, 1.0);
    FragColor = vec4(finalColor, 1.0);
}
)";

public:
    OpenGLRenderer() : window(nullptr), shaderProgram(0), VAO(0), VBO(0), 
                 shouldClose(false), spacePressed(false), currentTime(0.0f),
                 hasNextMove(false) {}
    
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

    // Render single frame with events
    void renderFrame()
    {
        auto currentTimePoint = std::chrono::high_resolution_clock::now();
        static auto startTime = currentTimePoint;
        auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(currentTimePoint - startTime);
        currentTime = elapsed.count() / 1000000.0f;
        
        // Clean up expired effects
        activeEffects.erase(
            std::remove_if(activeEffects.begin(), activeEffects.end(),
                [this](const AnimationEffect& effect) {
                    return (currentTime - effect.startTime) > effect.duration;
                }),
            activeEffects.end()
        );
        
        glfwPollEvents();
        render();
    }

    void updateBoxState(const SecureBox &box)
    {
        currentBoxState = box.getState();
    }

    void addAnimationEffect(int step, int toggleX, int toggleY, float duration = 1.5f)
    {
        activeEffects.emplace_back(step, toggleX, toggleY, currentTime, duration);
        
        // Limit number of concurrent effects
        if (activeEffects.size() > 10) {
            activeEffects.erase(activeEffects.begin());
        }
        
        std::cout << "Added animation effect at (" << toggleX << ", " << toggleY << "), total effects: " << activeEffects.size() << std::endl;
    }

    void clearAllEffects()
    {
        activeEffects.clear();
    }

    bool waitForSpace()
    {
        spacePressed = false;
        
        while (!shouldCloseWindow() && !spacePressed)
        {
            renderFrame();
            std::this_thread::sleep_for(std::chrono::milliseconds(16));
        }
        
        bool result = spacePressed;
        spacePressed = false;
        return result && !shouldCloseWindow();
    }

    bool checkSpacePressed()
    {
        bool result = spacePressed;
        if (result) {
            spacePressed = false;
            std::cout << "Space detected and consumed" << std::endl;
        }
        return result;
    }

    bool shouldCloseWindow()
    {
        return window && (glfwWindowShouldClose(window) || shouldClose);
    }

    void cleanup()
    {
        if (VAO) glDeleteVertexArrays(1, &VAO);
        if (VBO) glDeleteBuffers(1, &VBO);
        if (shaderProgram) glDeleteProgram(shaderProgram);
        if (window) glfwDestroyWindow(window);
        glfwTerminate();
    }

    void setNextMove(int x, int y) {
        nextMovePos = {(float)x, (float)y};
        hasNextMove = true;
    }

    void clearNextMove() {
        hasNextMove = false;
    }

private:
    void render()
    {
        if (currentBoxState.empty())
            return;

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        uint32_t width = currentBoxState[0].size();
        uint32_t height = currentBoxState.size();

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

                uint8_t value = currentBoxState[y][x];
                float r, g, b;
                
                if (value == 0) {
                    r = 0.0f; g = 1.0f; b = 0.0f; // Green
                } else if (value == 1) {
                    r = 1.0f; g = 1.0f; b = 0.0f; // Yellow
                } else {
                    r = 1.0f; g = 0.0f; b = 0.0f; // Red
                }

                float cellX = (float)x;
                float cellY = (float)y;

                // Two triangles per cell
                float cellVertices[] = {
                    left,  bottom, r, g, b, cellX, cellY,
                    right, bottom, r, g, b, cellX, cellY,
                    right, top,    r, g, b, cellX, cellY,
                    
                    left,  bottom, r, g, b, cellX, cellY,
                    right, top,    r, g, b, cellX, cellY,
                    left,  top,    r, g, b, cellX, cellY
                };

                for (int i = 0; i < 42; ++i)
                    vertices.push_back(cellVertices[i]);
            }
        }

        glUseProgram(shaderProgram);
        
        // Set time uniform
        GLuint timeLoc = glGetUniformLocation(shaderProgram, "iTime");
        glUniform1f(timeLoc, currentTime);
        
        // Set effect uniforms
        int numEffects = std::min((int)activeEffects.size(), 10);
        GLuint numEffectsLoc = glGetUniformLocation(shaderProgram, "numEffects");
        glUniform1i(numEffectsLoc, numEffects);
        
        if (numEffects > 0) {
            std::vector<float> positions(20, 0.0f); // 10 effects * 2 coordinates
            std::vector<float> startTimes(10, 0.0f);
            std::vector<float> durations(10, 1.0f);
            
            for (int i = 0; i < numEffects; ++i) {
                positions[i * 2] = (float)activeEffects[i].toggleX;
                positions[i * 2 + 1] = (float)activeEffects[i].toggleY;
                startTimes[i] = activeEffects[i].startTime;
                durations[i] = activeEffects[i].duration;
            }
            
            GLuint positionsLoc = glGetUniformLocation(shaderProgram, "effectPositions");
            glUniform2fv(positionsLoc, 10, positions.data());
            
            GLuint startTimesLoc = glGetUniformLocation(shaderProgram, "effectStartTimes");
            glUniform1fv(startTimesLoc, 10, startTimes.data());
            
            GLuint durationsLoc = glGetUniformLocation(shaderProgram, "effectDurations");
            glUniform1fv(durationsLoc, 10, durations.data());
        }
        
        // Set projection matrix
        float projection[16] = {
            1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f
        };

        GLuint nextMovePosLoc = glGetUniformLocation(shaderProgram, "nextMovePos");
        glUniform2f(nextMovePosLoc, nextMovePos[0], nextMovePos[1]);
                
        GLuint hasNextMoveLoc = glGetUniformLocation(shaderProgram, "hasNextMove");
        glUniform1i(hasNextMoveLoc, hasNextMove ? 1 : 0);
        
        GLuint projLoc = glGetUniformLocation(shaderProgram, "projection");
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, projection);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_DYNAMIC_DRAW);

        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(2 * sizeof(float)));
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(5 * sizeof(float)));
        glEnableVertexAttribArray(2);

        glDrawArrays(GL_TRIANGLES, 0, vertices.size() / 7);

        glfwSwapBuffers(window);
    }

    static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mode)
    {
        OpenGLRenderer* renderer = static_cast<OpenGLRenderer*>(glfwGetWindowUserPointer(window));
        if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        {
            renderer->shouldClose = true;
            glfwSetWindowShouldClose(window, GL_TRUE);
        }
        if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
        {
            renderer->spacePressed = true;
            std::cout << "Space key pressed!" << std::endl;
        }
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
// Modified main solving algorithm - single threaded with animation queue
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
        renderer->updateBoxState(box);
        std::cout << "Press SPACE to continue, ESC to exit..." << std::endl;
        
        if (!renderer->waitForSpace())
        {
            renderer->cleanup();
            delete renderer;
            return false;
        }
    }
    else
    {
        clearScreen();
        displayBoxConsole(box, "Initial SecureBox State");
        waitForEnter("Press Enter to start solving...");
    }

    // Create effect matrix and solve
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

    // Collect all moves
    struct Move {
        int x, y, count;
    };
    std::vector<Move> moves;
    
    for (int y = 0; y < (int)height; ++y)
    {
        for (int x = 0; x < (int)width; ++x)
        {
            int index = y * width + x;
            int toggleCount = solution[index];
            if (toggleCount > 0) {
                moves.push_back({x, y, toggleCount});
            }
        }
    }

    if (moves.empty())
    {
        std::cout << "Box was already unlocked or solution requires no moves!" << std::endl;
        if (renderer) {
            renderer->waitForSpace();
            renderer->cleanup();
            delete renderer;
        }
        return true;
    }

    int step = 1;
    size_t currentMove = 0;
    int currentToggleCount = 0;

    if (useOpenGL && renderer)
    {
        std::cout << "Press SPACE to apply next toggle (rapid presses create overlapping effects)" << std::endl;
        
        // Main interaction loop
        while (currentMove < moves.size() && !renderer->shouldCloseWindow())
        {
            if (currentMove < moves.size()) {
                renderer->setNextMove(moves[currentMove].x, moves[currentMove].y);
            }
            // Render frame and check for input
            renderer->renderFrame();
            
            if (renderer->checkSpacePressed())
            {
                auto& move = moves[currentMove];
                
                std::cout << "Step " << step++ << ": Toggle(" << move.x << ", " << move.y << ")" << std::endl;
                
                // Add animation effect (non-blocking)
                renderer->addAnimationEffect(step, move.x, move.y, 1.5f);
                
                // Apply toggle
                box.toggle(move.x, move.y);
                renderer->updateBoxState(box);
                
                currentToggleCount++;
                if (currentToggleCount >= move.count)
                {
                    currentMove++;
                    currentToggleCount = 0;
                }
                
                if (!box.isLocked())
                {
                    std::cout << "SUCCESS! Box is now unlocked!" << std::endl;
                    renderer->waitForSpace();
                    break;
                }
            }
            if (currentMove < moves.size()) {
                renderer->setNextMove(moves[currentMove].x, moves[currentMove].y);
            } else {
                renderer->clearNextMove();
            }
            
            std::this_thread::sleep_for(std::chrono::milliseconds(16)); // ~60 FPS
        }
    }
    else
    {
        // Console mode - sequential
        for (const auto& move : moves)
        {
            for (int t = 0; t < move.count; ++t)
            {
                clearScreen();
                std::cout << "Step " << step++ << ": Toggle(" << move.x << ", " << move.y << ")" << std::endl;
                box.toggle(move.x, move.y);
                displayBoxConsole(box, "SecureBox State After Toggle");

                if (box.isLocked())
                    waitForEnter();
                else
                {
                    std::cout << "SUCCESS! Box is now unlocked!" << std::endl;
                    waitForEnter("Press Enter to finish...");
                    break;
                }
            }
            if (!box.isLocked()) break;
        }
    }

    if (renderer)
    {
        renderer->cleanup();
        delete renderer;
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
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
// Enhanced OpenGL implementation with improved visual effects
//================================================================================

class OpenGLRenderer
{
private:
    GLFWwindow* window;
    GLuint shaderProgram;
    GLuint VAO, VBO;
    GLuint heightTexture;
    int windowWidth, windowHeight;
    bool shouldClose;
    bool spacePressed;

    std::array<float, 2> nextMovePos;
    bool hasNextMove;
    
    // Animation state
    float currentTime;
    std::vector<std::vector<uint8_t>> currentBoxState;
    std::vector<std::vector<float>> targetHeights;
    std::vector<std::vector<float>> currentHeights;
    std::mutex stateMutex;
    
    // Multiple animation effects
    struct AnimationEffect {
        int step;
        int toggleX, toggleY;
        float startTime;
        float duration;
        bool active;
        
        AnimationEffect(int s, int x, int y, float start, float dur = 2.0f) 
            : step(s), toggleX(x), toggleY(y), startTime(start), duration(dur), active(true) {}
    };
    
    std::vector<AnimationEffect> activeEffects;
    
    const char* vertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec2 aPos;
out vec2 fragCoord;

void main()
{
    fragCoord = (aPos + 1.0) * 0.5;
    gl_Position = vec4(aPos, 0.0, 1.0);
}
)";

    const char* fragmentShaderSource = R"(
#version 330 core
in vec2 fragCoord;
out vec4 FragColor;

uniform float iTime;
uniform vec2 iResolution;
uniform sampler2D heightMap;
uniform vec2 gridSize;
uniform int numEffects;
uniform vec2 effectPositions[10];
uniform float effectStartTimes[10];
uniform float effectDurations[10];
uniform vec2 nextMovePos;
uniform bool hasNextMove;

#define TIME        iTime
#define RESOLUTION  iResolution
#define PI          3.141592654
#define TAU         (2.0*PI)
#define ROT(a)      mat2(cos(a), sin(a), -sin(a), cos(a))

const vec3 LightDir0  = normalize(vec3(2.0, 2.0, 1.0));
const int   MaxIter   = 60;
const float Bottom    = 0.0;
const float MinHeight = 0.1;
const float MaxHeight = 4.0;
const float sz        = 0.45;
const float eps       = 1E-3;

float tanh_approx(float x) {
    float x2 = x*x;
    return clamp(x*(27.0 + x2)/(27.0+9.0*x2), -1.0, 1.0);
}

vec2 rayBox(vec3 ro, vec3 rd, vec3 boxSize, out vec3 outNormal) {
    vec3 m = 1.0/rd;
    vec3 n = m*ro;
    vec3 k = abs(m)*boxSize;
    vec3 t1 = -n - k;
    vec3 t2 = -n + k;
    float tN = max(max(t1.x, t1.y), t1.z);
    float tF = min(min(t2.x, t2.y), t2.z);
    if(tN > tF || tF < 0.0) return vec2(-1.0);
    outNormal = (tN > 0.0) ? step(vec3(tN), t1) : step(t2, vec3(tF));
    outNormal *= -sign(rd);
    return vec2(tN, tF);
}

float getHeight(vec2 p) {
    // Convert world position to grid coordinates
    vec2 gridCoord = (p + gridSize * 0.5) / gridSize;
    
    if (gridCoord.x < 0.0 || gridCoord.x > 1.0 || gridCoord.y < 0.0 || gridCoord.y > 1.0) {
        return 0.0;
    }
    
    float h = texture(heightMap, gridCoord).r;
    
    // Add wave effects from toggles
    float waveEffect = 0.0;
    for (int i = 0; i < numEffects && i < 10; ++i) {
        vec2 effectPos = effectPositions[i];
        float effectTime = TIME - effectStartTimes[i];
        float duration = effectDurations[i];
        
        if (effectTime >= 0.0 && effectTime <= duration) {
            float progress = effectTime / duration;
            vec2 worldEffectPos = (effectPos - gridSize * 0.5);
            float dist = length(p - worldEffectPos);
            
            // Ripple wave
            float wave = sin(dist * 8.0 - effectTime * 6.0) * exp(-dist * 2.0);
            wave *= smoothstep(1.0, 0.0, progress);
            waveEffect += wave * 0.15;
        }
    }
    
    h += waveEffect;
    return mix(MinHeight, MaxHeight, clamp(h, 0.0, 1.0));
}

vec3 getColor(vec2 p) {
    vec2 gridCoord = (p + gridSize * 0.5) / gridSize;
    
    if (gridCoord.x < 0.0 || gridCoord.x > 1.0 || gridCoord.y < 0.0 || gridCoord.y > 1.0) {
        return vec3(0.1, 0.1, 0.2);
    }
    
    float value = texture(heightMap, gridCoord).g; // Use green channel for cell values
    
    vec3 baseColor;
    if (value < 0.33) {
        baseColor = vec3(0.2, 0.8, 0.2); // Green for 0
    } else if (value < 0.67) {
        baseColor = vec3(0.9, 0.8, 0.2); // Yellow for 1
    } else {
        baseColor = vec3(0.9, 0.2, 0.2); // Red for 2
    }
    
    // Add highlight for next move
    if (hasNextMove) {
        vec2 worldNextPos = (nextMovePos - gridSize * 0.5);
        float distToNext = length(p - worldNextPos);
        if (distToNext < 0.6) {
            float pulse = 0.5 + 0.5 * sin(TIME * 4.0);
            baseColor = mix(baseColor, vec3(1.0, 1.0, 1.0), pulse * 0.3);
        }
    }
    
    return baseColor;
}

float cellTrace(vec3 ro, vec3 rd, float near, float far, out int iter, out vec2 cell, out vec2 boxi, out vec3 boxn) {
    vec2 rd2 = rd.xz;
    vec2 ird2 = 1.0/rd.xz;
    vec2 stp = step(vec2(0.0), rd2);
    
    float ct = near;
    iter = MaxIter;
    vec2 bi = vec2(-1.0);
    vec3 bn = vec3(0.0);
    vec2 np2 = vec2(0.0);
    float ft = far;
    
    for (int i = 0; i < MaxIter; ++i) {
        vec3 cp = ro + rd * ct;
        np2 = floor(cp.xz);
        float h = getHeight(np2);
        vec3 bdim = vec3(sz, h, sz);
        vec3 coff = vec3(np2.x + 0.5, h, np2.y + 0.5);
        vec3 bro = ro - coff;
        bi = rayBox(bro, rd, bdim, bn);
        
        if (bi.x > 0.0) {
            float bt = bi.x;
            if (bt >= far) {
                break;
            }
            ft = bt;
            iter = i;
            break;
        }
        
        // Step to next cell
        vec2 dif = np2 - cp.xz;
        dif += stp;
        dif *= ird2;
        float dt = min(dif.x, dif.y);
        ct += dt + eps;
        
        if (ct >= far) {
            break;
        }
    }
    cell = np2;
    boxi = bi;
    boxn = bn;
    return ft;
}

vec3 render(vec3 ro, vec3 rd) {
    vec3 sky = vec3(0.1, 0.1, 0.2);
    
    float skyt = 1E3;
    float bottom = -(ro.y - Bottom) / rd.y;
    float near = -(ro.y - MaxHeight) / rd.y;
    float far = bottom >= 0.0 ? bottom : skyt;
    
    int iter;
    vec2 cell;
    vec2 boxi;
    vec3 boxn;
    float ct = cellTrace(ro, rd, near, far, iter, cell, boxi, boxn);
    if (ct == skyt) {
        return sky;
    }
    
    vec3 p = ro + ct * rd;
    
    int siter;
    vec2 scell;
    vec2 sboxi;
    vec3 sboxn;
    float sfar = -(p.y - MaxHeight) / LightDir0.y;
    float sct = cellTrace((p - 2.0 * eps * rd), LightDir0, eps, sfar, siter, scell, sboxi, sboxn);
    
    vec3 n = vec3(0.0, 1.0, 0.0);
    vec3 bcol = vec3(0.5);
    
    if (iter < MaxIter) {
        n = boxn;
        bcol = getColor(cell);
        bcol *= smoothstep(0.0, 0.1, boxi.y - boxi.x);
    }
    
    float dif0 = max(dot(n, LightDir0), 0.0);
    dif0 = sqrt(dif0);
    float sf = siter < MaxIter ? tanh_approx(0.066 * sct) : 1.0;
    bcol *= mix(0.3, 1.0, dif0 * sf);
    
    vec3 col = bcol;
    col = mix(col, sky, 1.0 - exp(-0.05 * max(ct - 20.0, 0.0)));
    
    return col;
}

vec3 effect(vec2 p) {
    // Increased FOV for wider view
    const float fov = tan(TAU/8.0);
    
    // Calculate grid center and adjust camera position
    vec2 gridCenter = vec2(0.0, 0.0);
    float gridScale = max(gridSize.x, gridSize.y);
    
    // Position camera closer and at better angle
    vec3 ro = vec3(gridCenter.x, 4.0 + gridScale * 0.3, gridCenter.y - gridScale * 0.8);
    
    // Look at the center of the grid
    vec3 target = vec3(gridCenter.x, 0.0, gridCenter.y);
    vec3 ww = normalize(target - ro);
    vec3 up = vec3(0.0, 1.0, 0.0);
    vec3 uu = normalize(cross(up, ww));
    vec3 vv = cross(ww, uu);
    
    vec3 rd = normalize(p.x * uu + p.y * vv + fov * ww);
    
    vec3 col = render(ro, rd);
    col = clamp(col, 0.0, 1.0);
    col = sqrt(col);
    return col;
}

void main() {
    vec2 q = fragCoord;
    vec2 p = -1.0 + 2.0 * q;
    p.x *= iResolution.x / iResolution.y;
    vec3 col = effect(p);
    FragColor = vec4(col, 1.0);
}
)";

public:
    OpenGLRenderer() : window(nullptr), shaderProgram(0), VAO(0), VBO(0), heightTexture(0),
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

        window = glfwCreateWindow(width, height, "SecureBox 3D Solver", NULL, NULL);
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

        // Create fullscreen quad
        float quadVertices[] = {
            -1.0f, -1.0f,
             1.0f, -1.0f,
             1.0f,  1.0f,
            -1.0f, -1.0f,
             1.0f,  1.0f,
            -1.0f,  1.0f
        };

        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        // Create height texture
        glGenTextures(1, &heightTexture);
        glBindTexture(GL_TEXTURE_2D, heightTexture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        glViewport(0, 0, width, height);
        return true;
    }

    void updateBoxState(const SecureBox &box)
    {
        currentBoxState = box.getState();
        
        // Initialize height arrays if needed
        if (targetHeights.empty()) {
            targetHeights.resize(currentBoxState.size());
            currentHeights.resize(currentBoxState.size());
            for (size_t y = 0; y < currentBoxState.size(); ++y) {
                targetHeights[y].resize(currentBoxState[y].size());
                currentHeights[y].resize(currentBoxState[y].size());
            }
        }
        
        // Update target heights
        for (size_t y = 0; y < currentBoxState.size(); ++y) {
            for (size_t x = 0; x < currentBoxState[y].size(); ++x) {
                targetHeights[y][x] = currentBoxState[y][x] / 2.0f;
            }
        }
        
        updateHeightTexture();
    }

    void renderFrame()
    {
        auto currentTimePoint = std::chrono::high_resolution_clock::now();
        static auto startTime = currentTimePoint;
        auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(currentTimePoint - startTime);
        currentTime = elapsed.count() / 1000000.0f;
        
        // Animate heights towards targets
        if (!currentHeights.empty()) {
            bool changed = false;
            for (size_t y = 0; y < currentHeights.size(); ++y) {
                for (size_t x = 0; x < currentHeights[y].size(); ++x) {
                    float diff = targetHeights[y][x] - currentHeights[y][x];
                    if (abs(diff) > 0.01f) {
                        currentHeights[y][x] += diff * 0.05f; // Smooth interpolation
                        changed = true;
                    }
                }
            }
            if (changed) {
                updateHeightTexture();
            }
        }
        
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

    void addAnimationEffect(int step, int toggleX, int toggleY, float duration = 2.5f)
    {
        activeEffects.emplace_back(step, toggleX, toggleY, currentTime, duration);
        
        if (activeEffects.size() > 8) {
            activeEffects.erase(activeEffects.begin());
        }
        
        std::cout << "Added 3D wave animation at (" << toggleX << ", " << toggleY << ")" << std::endl;
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
            std::cout << "Space detected - applying move" << std::endl;
        }
        return result;
    }

    bool shouldCloseWindow()
    {
        return window && (glfwWindowShouldClose(window) || shouldClose);
    }

    void cleanup()
    {
        if (heightTexture) glDeleteTextures(1, &heightTexture);
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
    void updateHeightTexture()
    {
        if (currentHeights.empty()) return;
        
        int width = currentHeights[0].size();
        int height = currentHeights.size();
        
        std::vector<float> textureData(width * height * 3); // RGB
        
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                int index = (y * width + x) * 3;
                textureData[index] = currentHeights[y][x];     // R: height
                textureData[index + 1] = currentBoxState[y][x] / 2.0f; // G: cell value
                textureData[index + 2] = 0.0f;                 // B: unused
            }
        }
        
        glBindTexture(GL_TEXTURE_2D, heightTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, width, height, 0, GL_RGB, GL_FLOAT, textureData.data());
    }

    void render()
    {
        if (currentBoxState.empty()) return;

        glClearColor(0.05f, 0.05f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);
        
        // Set uniforms
        glUniform1f(glGetUniformLocation(shaderProgram, "iTime"), currentTime);
        glUniform2f(glGetUniformLocation(shaderProgram, "iResolution"), (float)windowWidth, (float)windowHeight);
        glUniform2f(glGetUniformLocation(shaderProgram, "gridSize"), (float)currentBoxState[0].size(), (float)currentBoxState.size());
        
        // Bind height texture
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, heightTexture);
        glUniform1i(glGetUniformLocation(shaderProgram, "heightMap"), 0);
        
        // Set effect uniforms
        int numEffects = std::min((int)activeEffects.size(), 10);
        glUniform1i(glGetUniformLocation(shaderProgram, "numEffects"), numEffects);
        
        if (numEffects > 0) {
            std::vector<float> positions(20, 0.0f);
            std::vector<float> startTimes(10, 0.0f);
            std::vector<float> durations(10, 1.0f);
            
            for (int i = 0; i < numEffects; ++i) {
                positions[i * 2] = (float)activeEffects[i].toggleX;
                positions[i * 2 + 1] = (float)activeEffects[i].toggleY;
                startTimes[i] = activeEffects[i].startTime;
                durations[i] = activeEffects[i].duration;
            }
            
            glUniform2fv(glGetUniformLocation(shaderProgram, "effectPositions"), 10, positions.data());
            glUniform1fv(glGetUniformLocation(shaderProgram, "effectStartTimes"), 10, startTimes.data());
            glUniform1fv(glGetUniformLocation(shaderProgram, "effectDurations"), 10, durations.data());
        }
        
        glUniform2f(glGetUniformLocation(shaderProgram, "nextMovePos"), nextMovePos[0], nextMovePos[1]);
        glUniform1i(glGetUniformLocation(shaderProgram, "hasNextMove"), hasNextMove ? 1 : 0);

        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);

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
// Main solving algorithm
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
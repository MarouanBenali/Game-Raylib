#include "raylib.h"

// Cell class definition
// Class representing a single cell in the maze
class Cell {
public:
    bool isWall;   // Indicates whether the cell is a wall
    bool visited;  // Indicates whether the cell has been visited during maze generation

    // Constructor to initialize a cell as a wall and unvisited by default
    Cell() : isWall(true), visited(false) {}
};

// Maze class definition
class Maze {
private:
    int width, height, cellSize;         // Dimensions of the maze and size of each cell
    Cell** grid;                         // 2D array to represent the maze grid
    int exitX, exitY;                    // Coordinates for the exit location
    Color mazeColor;                     // Color of the maze walls
    Texture2D exitTexture;               // Texture to represent the exit point

    // Checks if the given coordinates are inside the maze boundaries
    bool isInsideGrid(int x, int y) const {
        return x >= 0 && x < width && y >= 0 && y < height;
    }

    // Recursive function to generate the maze using depth-first search (DFS) with randomized directions
    void generateMazeRecursive(int x, int y) {
        // Mark the current cell as visited and remove its wall
        grid[y][x].visited = true;
        grid[y][x].isWall = false;

        // Define possible directions to move in the maze
        int directions[4][2] = {{0, -1}, {0, 1}, {-1, 0}, {1, 0}};

        // Randomize the order of directions to ensure randomized path generation
        for (int i = 0; i < 4; i++) {
            int j = GetRandomValue(i, 3); // Random index to swap with the current index
            int temp[2] = {directions[i][0], directions[i][1]};
            directions[i][0] = directions[j][0];
            directions[i][1] = directions[j][1];
            directions[j][0] = temp[0];
            directions[j][1] = temp[1];
        }

        // Explore each direction
        for (int i = 0; i < 4; i++) {
            int nx = x + directions[i][0] * 2; // Next cell in the direction
            int ny = y + directions[i][1] * 2;
            if (isInsideGrid(nx, ny) && !grid[ny][nx].visited) { // Check if the cell is within bounds and unvisited
                grid[y + directions[i][1]][x + directions[i][0]].isWall = false; // Remove wall between the two cells
                generateMazeRecursive(nx, ny); // Recursively generate the maze from the new cell
            }
        }
    }

public:
    // Constructor for the Maze class, initializes the maze with specified dimensions and properties
    Maze(int w, int h, int size, Color color, Texture2D exitTex) 
        : width(w), height(h), cellSize(size), mazeColor(color), exitTexture(exitTex) {
        // Allocate memory for the 2D maze grid
        grid = new Cell*[height];
        for (int i = 0; i < height; i++) {
            grid[i] = new Cell[width];
        }
        generateMaze(); // Generate the initial maze

        // Set the exit position near the bottom-right corner
        exitX = width - 2;
        exitY = height - 2;
        grid[exitY][exitX].isWall = false; // Ensure the exit is not a wall
    }

    // Destructor to clean up dynamically allocated memory
    ~Maze() {
        for (int i = 0; i < height; i++) {
            delete[] grid[i];
        }


        delete[] grid;
    }

    // Initiates the maze generation process
    void generateMaze() {
        generateMazeRecursive(1, 1);
    }

    // Checks if a given cell is a wall
    bool isWall(int x, int y) const {
        if (!isInsideGrid(x, y)) return true; // Treat out-of-bound coordinates as walls
        return grid[y][x].isWall;
    }

    // Draws the maze on the screen
    void draw() const {
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                if (grid[y][x].isWall) {
                    // Draw the walls of the maze
                    DrawRectangle(x * cellSize, y * cellSize, cellSize, cellSize, mazeColor);
                }
            }
        }

        // Draw the exit texture at the exit position
        Vector2 exitPosition = {(float)(exitX * cellSize), (float)(exitY * cellSize)};
        DrawTextureEx(exitTexture, exitPosition, 0.0f, (float)cellSize / exitTexture.width, WHITE);
    }

    // Checks if the given coordinates are at the exit position
    bool isExit(int x, int y) const {
        return x == exitX && y == exitY;
    }
};

/// Player class definition
class Player {
private:
    int x, y;                    // Player's current position in the maze
    int cellSize;               // Size of the player's representation (same as maze cell size)
    float moveCooldown;         // Minimum time interval between two moves
    float lastMoveTime;         // Timestamp of the last move to handle cooldown

public:
    // Constructor to initialize player's starting position, cell size, and movement properties
    Player(int startX, int startY, int size) 
        : x(startX), y(startY), cellSize(size), moveCooldown(0.2f), lastMoveTime(0.0f) {}

    // Handles player movement while considering cooldowns and walls
    void move(int dx, int dy, const Maze &maze) {
        float currentTime = GetTime(); // Get the current system time
        if (currentTime - lastMoveTime >= moveCooldown) { // Check if cooldown has passed
            int newX = x + dx; // Proposed new X position
            int newY = y + dy; // Proposed new Y position
            if (!maze.isWall(newX, newY)) { // Check if the move leads to a non-wall cell
                x = newX; // Move the player to the new position
                y = newY;
            }
            lastMoveTime = currentTime; // Update the last move time
        }
    }

    // Draws the player character at its current position
    void draw(Texture2D character) const {
        DrawTextureEx(character, {(float)(x * cellSize), (float)(y * cellSize)}, 0.0f, 
                      (float)cellSize / character.width, WHITE);
    }

    // Getter to access the player's current X coordinate
    int getX() const { return x; }

    // Getter to access the player's current Y coordinate
    int getY() const { return y; }
};

// Function to handle menu selection logic with keyboard input
int choix(int selectedButton, int nmbrButton) {
    // Move selection down
    if (IsKeyPressed(KEY_DOWN)) {
        selectedButton = (selectedButton + 1) % nmbrButton; // Wrap to the first button when exceeding the number of buttons
    }
    // Move selection up
    if (IsKeyPressed(KEY_UP)) {
        selectedButton = (selectedButton - 1 + nmbrButton) % nmbrButton; // Wrap to the last button when going below the first one
    }
    return selectedButton; // Return the updated selection index
}

// Function to handle character selection logic with keyboard input
int choix2(int selectedCharacter, int nmbrButton) {
    // Move character selection to the right
    if (IsKeyPressed(KEY_RIGHT)) {
        selectedCharacter = (selectedCharacter + 1) % nmbrButton; // Wrap to the first character when exceeding available options
    }
    // Move character selection to the left
    if (IsKeyPressed(KEY_LEFT)) {
        selectedCharacter = (selectedCharacter - 1 + nmbrButton) % nmbrButton; // Wrap to the last character when going below the first
    }
    return selectedCharacter; // Return the updated character selection index
}


int main() {
    // Initialize the game window
    InitWindow(0, 0, "Maze Game");
    InitAudioDevice();
    SetTargetFPS(60);

    // Enable fullscreen mode
    ToggleFullscreen();

    // Get screen dimensions
    float screenWidth = GetScreenWidth();
    float screenHeight = GetScreenHeight();

    // Menu options for difficulty selection
    const char *niveau[] = {"Facile", "Moyen", "Difficile", "Exit"};

    // Initial game state
    int selectedButton = 0;             // Currently selected menu button
    int selectedCharacter = 0;          // Currently selected character
    bool showCharacterSelection = false; // Whether to show character selection
    int difficulty = 1;                 // Game difficulty level

    // Load textures for visuals
    Texture2D background = LoadTexture("img/po.png");
    Texture2D mouseTexture = LoadTexture("img/ms.png");
    Texture2D manTexture = LoadTexture("img/hm.png");
    Texture2D cTexture = LoadTexture("img/c.png");
    Texture2D exitJnnTexture = LoadTexture("img/jnn.png");
    Texture2D exitFmTexture = LoadTexture("img/fm.png");
    Texture2D exitScTexture = LoadTexture("img/sc.png");

    // Load menu and game music
    Music menuMusic = LoadMusicStream("Audio/debut.mp3");
    Music gameMusic = LoadMusicStream("Audio/rr.mp3");
    SetMusicVolume(menuMusic, 0.5f);
    SetMusicVolume(gameMusic, 0.5f);

    // Game state control variables
    bool gameStarted = false;
    Maze* maze = nullptr;
    Player* player = nullptr;

    // Start playing menu music
    PlayMusicStream(menuMusic);

    while (!WindowShouldClose()) { // Main game loop
        if (!gameStarted) {
            UpdateMusicStream(menuMusic);

            // Menu navigation and difficulty selection
            if (!showCharacterSelection) {
                selectedButton = choix(selectedButton, 4); // Handle menu navigation
                if (IsKeyPressed(KEY_ENTER)) {
                    if (selectedButton == 3) { // Exit the game
                        break;
                    } else {
                        difficulty = selectedButton + 1; // Set difficulty
                        showCharacterSelection = true; // Show character selection screen
                    }
                }
            } else {
                // Handle character selection
                selectedCharacter = choix2(selectedCharacter, 3);
                if (IsKeyPressed(KEY_ENTER)) {
                    // Start the game with chosen difficulty and character
                    gameStarted = true;
                    int cellSize = (difficulty == 1) ? 50 : (difficulty == 2) ? 40 : 30;
                    int mazeWidth = screenWidth / cellSize;
                    int mazeHeight = screenHeight / cellSize;
                    Color mazeColor = (difficulty % 2 == 0) ? DARKGRAY : LIGHTGRAY;
                    Texture2D exitTexture;
                    
                    // Set the exit texture based on selected character
                    if (selectedCharacter == 0) {
                        exitTexture = exitJnnTexture;
                    } else if (selectedCharacter == 1) {
                        exitTexture = exitFmTexture;
                    } else {
                        exitTexture = exitScTexture;
                    }

                    // Create the maze and player
                    maze = new Maze(mazeWidth, mazeHeight, cellSize, mazeColor, exitTexture);
                    player = new Player(1, 1, cellSize);

                    StopMusicStream(menuMusic);
                    PlayMusicStream(gameMusic);
                }
            }

            // Drawing menu screen
            BeginDrawing();
            ClearBackground(RAYWHITE);

            // Draw background
            DrawTexturePro(background, {0, 0, (float)background.width, (float)background.height}, 
                           {0, 0, screenWidth, screenHeight}, {0, 0}, 0, WHITE);

            if (!showCharacterSelection) {
                // Draw menu screen with difficulty options
                DrawText("Choisissez le niveau du jeu", screenWidth / 2 - 350, 100, 50, BLUE);

                for (int i = 0; i < 4; i++) {
                    if (i == selectedButton) {
                        DrawEllipse(screenWidth / 2, 300 + i * 200, 320, 100, RED); // Highlight selected option
                    } else {
                        DrawEllipse(screenWidth / 2, 300 + i * 200, 300, 80, GOLD);
                    }
                    DrawText(niveau[i], screenWidth / 2 - 50, 280 + i * 200, 40, BLACK);
                }
            } else {
                // Draw character selection screen
                DrawText("Choisissez votre personnage :", screenWidth / 2 - 400, 100, 50, BLUE);

                DrawTexturePro(mouseTexture, {0, 0, 700, 600}, {screenWidth / 4 - 150, 400, 300, 300}, {0, 0}, 0, WHITE);
                DrawTexturePro(manTexture, {0, 0, 700, 600}, {screenWidth / 2 - 150, 400, 300, 300}, {0, 0}, 0, WHITE);
                DrawTexturePro(cTexture, {0, 0, 400, 400}, {3 * screenWidth / 4 - 150, 400, 300, 300}, {0, 0}, 0, WHITE);

                if (selectedCharacter == 0)
                    DrawCircle(screenWidth / 4, 750, 50, RED);
                else if (selectedCharacter == 1)
                    DrawCircle(screenWidth / 2, 750, 50, RED);
                else
                    DrawCircle(3 * screenWidth / 4, 750, 50, RED);
            }

            EndDrawing();
        } else {
            UpdateMusicStream(gameMusic);

            // Game logic: Handle player movement
            if (IsKeyDown(KEY_UP)) player->move(0, -1, *maze);
            if (IsKeyDown(KEY_DOWN)) player->move(0, 1, *maze);
            if (IsKeyDown(KEY_LEFT)) player->move(-1, 0, *maze);
            if (IsKeyDown(KEY_RIGHT)) player->move(1, 0, *maze);

            if (maze->isExit(player->getX(), player->getY())) {
                // Handle level completion
                delete maze;
                delete player;
                gameStarted = false;
                showCharacterSelection = false;
                StopMusicStream(gameMusic);
                PlayMusicStream(menuMusic);
            }

            // Drawing game screen
            BeginDrawing();
            ClearBackground(RAYWHITE);
            maze->draw(); // Draw the current state of the maze
            Texture2D playerTexture = (selectedCharacter == 0) ? mouseTexture : 
                                        (selectedCharacter == 1) ? manTexture : cTexture;
            player->draw(playerTexture); // Draw the player
            EndDrawing();
        }
    }

    // Cleanup textures, music, and game resources
    UnloadTexture(background);
    UnloadTexture(mouseTexture);
    UnloadTexture(manTexture);
    UnloadTexture(cTexture);
    UnloadTexture(exitJnnTexture);
    UnloadTexture(exitFmTexture);
    UnloadTexture(exitScTexture);
    UnloadMusicStream(menuMusic);
    UnloadMusicStream(gameMusic);

    if (maze) delete maze;
    if (player) delete player;

    CloseAudioDevice();
    CloseWindow();
    return 0;
}


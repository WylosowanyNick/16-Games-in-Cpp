#include <SFML/Graphics.hpp> // RenderWindow; Event; Texture; Sprite;
#include <Windows.h> // FreeConsole();
#include <cstdlib> // rand();

// Constant game variables
constexpr unsigned short tilesSize=18;

// Dependent-constant game variables
constexpr unsigned short numberOfRows=25, numberOfCols=40, numberOfEnemies=5,
screenWidth=tilesSize*numberOfCols, screenHeight=tilesSize*numberOfRows;
  
// ---------------------------------------------------------
// Structs and Classes
struct Vector2i { int x; int y; };

class Enemy
{
public:
  short x, y, dx, dy;
  float rotation, dRotation;
  char (&grid)[numberOfRows][numberOfCols];

  Enemy(char (&grid)[numberOfRows][numberOfCols])
    : x(0), y(0), dx(0), dy(0), rotation(0), dRotation(0), grid(grid) {}

  Enemy(short x, short y, short dx, short dy, float dRotation, char (&grid)[numberOfRows][numberOfCols])
    : x(x), y(y), dx(dx), dy(dy), rotation(0), dRotation(dRotation), grid(grid) {}

  bool Move()
  {
    char i=y/tilesSize, j=x/tilesSize, di=(y+dy)/tilesSize, dj=(x+dx)/tilesSize;
    if(grid[i][dj]==0) dx = -dx; // Bouncing vertical, blue grid
    if(grid[di][j]==0) dy = -dy; // Bouncing horizontal, blue grid

    x += dx; y += dy; // Movement
    rotation += dRotation; // Rotating enemy

    return (grid[y/tilesSize][x/tilesSize]==3); // Lost, when enemy runs on the green tiles
  }
};

// ---------------------------------------------------------
// Functions

void enemyMakesBlackTiles(char i, char j, char (&grid)[numberOfRows][numberOfCols])
{
  if(grid[i][j]==-2) grid[i][j]=-1;
  if(grid[i-1][j]==-2) enemyMakesBlackTiles(i-1, j, grid);
  if(grid[i][j+1]==-2) enemyMakesBlackTiles(i, j+1, grid);
  if(grid[i+1][j]==-2) enemyMakesBlackTiles(i+1, j, grid);
  if(grid[i][j-1]==-2) enemyMakesBlackTiles(i, j-1, grid);
}

// ---------------------------------------------------------
int main()
{
  FreeConsole(); // No command prompt window will be displayed

// ---------------------------------------------------------
// Game variables
  // Grid
  char grid[numberOfRows][numberOfCols];
  for(int i=0; i<numberOfRows; i++)
    for(int j=0; j<numberOfCols; j++)
      grid[i][j] = -1; // Black grid

  for(int i=0; i<numberOfRows; i++) // Borders of map
    { grid[i][0] = grid[i][numberOfCols-1] = 0; } // Left and right borders
  for(int i=0; i<numberOfCols; i++)
    { grid[0][i] = grid[numberOfRows-1][i] = 0; } // Top and bottom borders

  // Player
  Vector2i lastPlayerPos={0,0}, playerPos={0,0};
  
  // Others
  bool greenExists = false;

  // Enemies positions
  Enemy enemy[numberOfEnemies](grid);
  for(int i=0; i<numberOfEnemies; i++)
  {
    enemy[i].x=tilesSize+rand()%(screenWidth-tilesSize);
    enemy[i].y=tilesSize+rand()%(screenHeight-tilesSize);
    enemy[i].dx=4+rand()%3; enemy[i].dy=4+rand()%3;
    enemy[i].dRotation=7+rand()%3;
  }

  // Textures and Sprites
  sf::Texture textureTiles, textureGameOver, textureEnemy;
  textureTiles.loadFromFile("images/tiles.png");
  textureGameOver.loadFromFile("images/gameover.png");
  textureEnemy.loadFromFile("images/enemy.png");

  sf::Sprite tiles[8], gameover(textureGameOver), enemySprite(textureEnemy);
  for(int i=0; i<8; i++)
  {
    tiles[i].setTexture(textureTiles);
    tiles[i].setTextureRect(sf::IntRect(i*tilesSize,0,tilesSize,tilesSize));
  }
  gameover.setOrigin(gameover.getTextureRect().width/2,
      gameover.getTextureRect().height/2); // Centering Game Over sprite
  gameover.setPosition(screenWidth/2, screenHeight/2);

  enemySprite.setOrigin(enemySprite.getTextureRect().width/2,
      enemySprite.getTextureRect().height/2); // Centering enemySprite origin
 
// ---------------------------------------------------------
// Window handling (Game loop)
  sf::RenderWindow window(sf::VideoMode(screenWidth, screenHeight), "Xonix!");
  window.setFramerateLimit(30);

  while(window.isOpen())
  {
    // Window events handling
    sf::Event e;
    while(window.pollEvent(e))
    {
      if(e.type == sf::Event::Closed)
        window.close();
    }

    // Keyboard events (Player movement with the out-of-the board check)
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) { if(playerPos.y+1<numberOfRows) playerPos.y++; }
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) { if(playerPos.y-1>=0) playerPos.y--; }
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) { if(playerPos.x-1>=0) playerPos.x--; }
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) { if(playerPos.x+1<numberOfCols) playerPos.x++; }

// ---------------------------------------------------------
// Actions
    // Players actions
    if(grid[playerPos.y][playerPos.x]==3 &&
      (playerPos.x!=lastPlayerPos.x || playerPos.y!=lastPlayerPos.y))
      break; // Game Over (run into green zone)

    if(grid[playerPos.y][playerPos.x]==0 && greenExists)
    { // Player run into the blue zone. The map should be covered with the blue tiles.
      greenExists = false;

      // Black grids (marked as -1) transforms into temporary, -2 grids
      for(int i=0; i<numberOfRows; i++)
        for(int j=0; j<numberOfCols; j++)
          if(grid[i][j] == -1) grid[i][j] = -2;

      // Enemies mark their grids, to which they belong, as -1
      for(int i=0; i<numberOfEnemies; i++)
        enemyMakesBlackTiles(enemy[i].y/tilesSize, enemy[i].x/tilesSize, grid);

      // Rest of the temporary, -2 and green (3) grids are marked as blue tiles (0)
      for(int i=0; i<numberOfRows; i++)
        for(int j=0; j<numberOfCols; j++)
          if(grid[i][j]==-2 || grid[i][j]==3) grid[i][j] = 0;
    }

    // Player movement
    if(grid[playerPos.y][playerPos.x] == -1)
    {
      grid[playerPos.y][playerPos.x]=3;
      greenExists = true;
    }
    lastPlayerPos.x=playerPos.x; lastPlayerPos.y=playerPos.y;

    // Enemies movement
    for(int i=0; i<numberOfEnemies; i++)
      if(enemy[i].Move()) break; // Lost, when enemy runs on the green tiles

// ---------------------------------------------------------
// Rendering
    window.clear(sf::Color::Black);

    // Board rendering
    for(int i=0; i<numberOfRows; i++)
      for(int j=0; j<numberOfCols; j++)
      {
        if(grid[i][j] == -1) continue;

        sf::Sprite& s = tiles[grid[i][j]];
        s.setPosition(j*tilesSize, i*tilesSize);
        window.draw(s);
      }

    // Player rendering
    tiles[2].setPosition(playerPos.x*tilesSize, playerPos.y*tilesSize);
    window.draw(tiles[2]);

    // Enemies rendering
    for(int i=0; i<numberOfEnemies; i++)
    {
      enemySprite.setPosition(enemy[i].x, enemy[i].y);
      enemySprite.setRotation(enemy[i].rotation);
      window.draw(enemySprite);
    }

    window.display();
  }
// ---------------------------------------------------------
// Window handling (Lost game loop)
  while(window.isOpen())
  {
    // Window events handling
    sf::Event e;
    while(window.pollEvent(e))
    {
      if(e.type == sf::Event::Closed)
        window.close();
    }

    // Rendering
    window.clear(sf::Color::Black);

    // Board rendering
    for(int i=0; i<numberOfRows; i++)
      for(int j=0; j<numberOfCols; j++)
      {
        if(grid[i][j] == -1) continue;

        sf::Sprite& s = tiles[grid[i][j]];
        s.setPosition(j*tilesSize, i*tilesSize);
        window.draw(s);
      }

    // Player rendering
    tiles[2].setPosition(playerPos.x*tilesSize, playerPos.y*tilesSize);
    window.draw(tiles[2]);

    // Enemies rendering
    for(int i=0; i<numberOfEnemies; i++)
    {
      enemySprite.setPosition(enemy[i].x, enemy[i].y);
      window.draw(enemySprite);
    }

    // Game over sprite rendering
    window.draw(gameover);

    window.display();
  }
}


#include <SFML/Graphics.hpp> // Texture; Sprite; RenderWindow; Event;
#include <SFML/System/Clock.hpp> // Clock;
#include <Windows.h> // FreeConsole();
#include <ctime> // time();
#include <cstdlib> // srand();

using namespace sf;

// This function checks if there is a full, tetris row to delete.
// If there is, the index of the full row is returned. If not, -1 is 
// returned.
int CheckRowsToDelete(const int field[][10])
{
  for(int i=0; i<20; i++)
  {
    int j=0;
    for(; j<10; j++)
      if(field[i][j]==-1) break;

    if(j==10) return i;
  }
  
  return -1;
}

int main()
{
  FreeConsole(); // No command prompt window will be displayed

  // ---------------------------------------------------------
  // The game board data
  int field[20][10];
  for(int i=0; i<20; i++)
    for(int j=0; j<10; j++)
      field[i][j] = -1; // -1 means "empty tile"

  // ---------------------------------------------------------
  // Definition of the 8, colorful tiles and background
  Texture t1, t2;
  t1.loadFromFile("images/tiles.png");

  Sprite s[8]; // 8, colorful tiles
  for(int i=0; i<8; i++)
  {
    s[i].setTexture(t1);
    s[i].setTextureRect(IntRect(18*i, 0, 18, 18));
  }

  t2.loadFromFile("images/background.png"); // Background
  Sprite background(t2);

  // ---------------------------------------------------------
  // Definition of the game's figures. Each figure consists of
  // 4 tiles (squares) and has some color.
  struct Point { int x; int y; }; // Struct holding coords x and y
  struct Figure { Point coord[4]; int color; }; // Struct holding coords of 4 tiles (squares) of the figure and this figure color
  Figure figures[7]; // 7 possible figures in the game

  { // Definition of the game's figures
    int fig[7][4] =
    {
      1,3,5,7, // I
      2,4,5,7, // Z
      3,5,4,6, // S
      3,5,4,7, // T
      2,3,5,7, // L
      3,5,7,6, // J
      2,3,4,5, // O
    };

    for(int i=0; i<7; i++)
      for(int j=0; j<4; j++)
      {
        figures[i].coord[j].x = fig[i][j] % 2;
        figures[i].coord[j].y = fig[i][j] / 2;
      }
  };

  // ---------------------------------------------------------
  // Game variables and others
  srand(time(NULL)); // Random number generator initialization

  Clock clock; bool fig_set=false,check; Point p; int x,y,num; float delay=0.5;
  Figure fig=figures[rand()%7], fig_result; fig.color=rand()%8;
  
  // ---------------------------------------------------------
  // Window handling (Game loop)
  RenderWindow window(VideoMode(320, 480), "Tetris!");
  window.setFramerateLimit(60);
  while(window.isOpen())
  {
    // Window events handling
    Event e;
    while(window.pollEvent(e))
    {
      switch(e.type)
      {
      case Event::Closed: // Window closed event
        window.close();
        break;

  // ---------------------------------------------------------
  // Keyboard events handling
      case Event::KeyPressed:
        switch(e.key.code)
        {
  // Up arrow (Figure rotation)
        case Keyboard::Up:
          p = fig.coord[1]; // Center of the figure
          for(int i=0; i<4; i++) // Figure after rotation
          {
            x = fig.coord[i].y-p.y;
            y = fig.coord[i].x-p.x;
            fig_result.coord[i].x = p.x-x;
            fig_result.coord[i].y = p.y+y;
          }

          check = true; // Checking if the rotated figure fits into the board and doesn't colide with the other figures
          for(int i=0; i<4; i++)
            if(fig_result.coord[i].x<0 || fig_result.coord[i].x>9 ||
                field[fig_result.coord[i].y][fig_result.coord[i].x]!=-1)
              { check = false; break; }
          
          if(check) // Rotated figure fits into the board and doesn't coolide with the other figures -> Accepting the result
            for(int i=0; i<4; i++) fig.coord[i] = fig_result.coord[i];

          break;
  // Left arrow (move to the left)
        case Keyboard::Left:
          check = true; // Checking if the moved figure fits into the board and doesn't colide with the other figures
          for(int i=0; i<4; i++)
            if(fig.coord[i].x==0 || field[fig.coord[i].y][fig.coord[i].x-1]!=-1)
              { check = false; break; }
          
          if(check) // Moved figure fits into the board and doesn't coolide with the other figures -> Accepting the result
            for(int i=0; i<4; i++) fig.coord[i].x--;
          break;
  // Right arrow (move to the right)
        case Keyboard::Right:
          check = true; // Checking if the moved figure fits into the board and doesn't colide with the other figures
          for(int i=0; i<4; i++)
            if(fig.coord[i].x==9 || field[fig.coord[i].y][fig.coord[i].x+1]!=-1)
              { check = false; break; }
          
          if(check) // Moved figure fits into the board and doesn't coolide with the other figures -> Accepting the result
            for(int i=0; i<4; i++) fig.coord[i].x++;
          break;
  // Down arrow (move figure down fast)
        case Keyboard::Down:
          delay = 0.01;
          break;
        }
      }
    }
  // ---------------------------------------------------------
  // -Moving synchronically figure down.
  // -Checking if the figure reaches bottom (possibly composed of the other figures).
  //  -If no, skip to the next step (paragraph).
  //  -If yes:
  //   -put the figure on its place,
  //   -create new, random figure,
  //   -check, if there exist full, tetris rows,
  //    If yes, remove them properly,
  //   -check if newly created figure overlaps with the others,
  //    If yes, the game ends.
    if(delay < clock.getElapsedTime().asSeconds())
    { // -Moving synchronically figure down.
      for(int i=0; i<4; i++) fig.coord[i].y++;
      clock.restart();
      delay = 0.5;
    }
 
    for(int i=0; i<4; i++) // -Checking if the figure reaches bottom (possibly composed of the other figures).
      if(fig.coord[i].y==20 || field[fig.coord[i].y][fig.coord[i].x]!=-1)
        { fig_set = true; break; }

    if(fig_set) //  -If yes:
    {
      for(int i=0; i<4; i++) fig.coord[i].y--;

      for(int i=0; i<4; i++) //   -put the figure on its place,
        field[fig.coord[i].y][fig.coord[i].x] = fig.color;

      fig = figures[rand()%7]; //   -create new, random figure,
      fig.color = rand()%8;
      fig_set = false;

      //   -check, if there exist full, tetris rows,
      num = CheckRowsToDelete(field);
      while(num != -1)
      { //    If yes, remove them properly,
        for(int i=0; i<10; i++)
          field[num][i] = -1;
        
        for(int i=num; 0<i; i--)
          for(int j=0; j<10; j++)
            field[i][j] = field[i-1][j];
        
        num = CheckRowsToDelete(field);
      }

      //   -check if newly created figure overlaps with the others,
      check = false;
      for(int i=0; i<4; i++)
        if(field[fig.coord[i].y][fig.coord[i].x] != -1)
          { check = true; break; }
      
      if(check) //    If yes, the game ends.
        window.close();
    }

  // ---------------------------------------------------------
  // Rendering
    window.clear();
    window.draw(background);

    for(int i=0; i<4; i++) // Rendering the figure
    {
      s[fig.color].setPosition(fig.coord[i].x*18, fig.coord[i].y*18);
      s[fig.color].move(28, 31); // Necessary shift
      window.draw(s[fig.color]);
    }

    // Rendering the board
    for(int i=0; i<20; i++)
      for(int j=0; j<10; j++)
        if(field[i][j] != -1)
        {
          s[field[i][j]].setPosition(j*18, i*18);
          s[field[i][j]].move(28, 31); // Necessary shift
          window.draw(s[field[i][j]]);
        }

    window.display();
  }

  return 0;
}

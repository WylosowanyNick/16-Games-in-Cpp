#include <SFML/Graphics.hpp> // Texture; Sprite; RenderWindow; Event;
#include <Windows.h> // FreeConsole();
#include <ctime> // time();
#include <cstdlib> // srand();
#include <queue>

using namespace sf;

// This function allows to go arround given coordinate (x, y)
// using given number 'num'. If the result coordinate exceeds the
// boundries (X, Y), {-1, -1} is returned.
Vector2i clock_coord(int x, int y, int num, int X, int Y)
{
  Vector2i v;
  switch(num)
  {
  case 0:
    v.x=x; v.y=y-1; break;
  case 1:
    v.x=x+1; v.y=y-1; break;
  case 2:
    v.x=x+1; v.y=y; break;
  case 3:
    v.x=x+1; v.y=y+1; break;
  case 4:
    v.x=x; v.y=y+1; break;
  case 5:
    v.x=x-1; v.y=y+1; break;
  case 6:
    v.x=x-1; v.y=y; break;
  case 7:
    v.x=x-1; v.y=y-1; break;
  }

  if(v.y<0 || X<=v.x || Y<=v.y || v.x<0)
    return Vector2i(-1, -1);

  return v;
}

int main()
{
  FreeConsole(); // No command prompt window will be displayed

  // ---------------------------------------------------------
  // Game variables and others
  const int size = 32; // The tiles' (sprites') sizes
  const int X=20, Y=20, amount_of_bombs=50; // The board sizes and the number of the bombs
  bool lost_game=false;
  
  int tab_hidden[Y][X], // Invisible board numbers
  tab_visible[Y][X]; // Visible board numbers
  for(int i=0; i<Y; i++)
    for(int j=0; j<X; j++)
    { tab_hidden[i][j]=0; tab_visible[i][j]=10; }

  int X_times_Y = X*Y; srand(time(0));
  for(int k=0; k<amount_of_bombs; k++)
  { // Putting the bombs on the board
    int n = rand()%(X_times_Y - 1 - k);
    for(int i=0; i<X_times_Y; i++)
    {
      if(n>0 && tab_hidden[i/X][i%X]!=9) n--;
      if(!n && tab_hidden[i/X][i%X]!=9) { tab_hidden[i/X][i%X]=9; break; }
    }
  }

  for(int i=0; i<Y; i++)
    for(int j=0; j<X; j++)
      if(tab_hidden[i][j] == 9)
        for(int n=0; n<8; n++)
        { // Filling the invisible board with the digits
          Vector2i v = clock_coord(j, i, n, X, Y);
          if(v.x==-1 && v.y==-1) continue;
          
          if(tab_hidden[v.y][v.x] != 9)
            tab_hidden[v.y][v.x] += 1;
        }

  // ---------------------------------------------------------
  // Definition of the game sprites
  Texture t;
  t.loadFromFile("images/tiles.jpg");

  Sprite s[12];

  for(int i=0; i<12; i++)
  {
    s[i].setTexture(t);
    s[i].setTextureRect(IntRect(size*i, 0, size, size));
  }

  // ---------------------------------------------------------
  // Window handling (Game loop)
  RenderWindow window(VideoMode(size*X, size*Y), "Minesweeper!");
  window.setFramerateLimit(60);
  while (window.isOpen() && !lost_game)
  {
    // Window events handling
    Event e;
    while (window.pollEvent(e))
    switch (e.type)
    {
    case Event::Closed:
      window.close();
      break;
    
    case Event::MouseButtonPressed:
    {
  // Left mouse button -> Uncovering the field
      if(e.key.code == Mouse::Left)
      {
        Vector2i pos = Mouse::getPosition(window); // Mouse clicked position
        int x=pos.x/size, y=pos.y/size; // Board field coordinate

        if(tab_visible[y][x] == 11) // The field has flag -> Operation aborted
          continue;

        if(tab_hidden[y][x] != 9)
        { // Non bomb field was revealed
          if(tab_hidden[y][x] != 0)
            tab_visible[y][x] = tab_hidden[y][x]; // Normal uncovering the field
          else
          { // Uncovering all 'zero' fields
            tab_visible[y][x] = 0; // Normal uncovering the field

            Vector2i v(x, y);
            std::queue<Vector2i> q; // Queue of fields to uncover
            q.push(v);
            while(!q.empty())
            {
              v = q.front(); q.pop();
              
              for(int i=0; i<8; i++) // Uncovering around given coordinate
              {
                Vector2i v1(clock_coord(v.x, v.y, i, X, Y)); // Out of board uncovering -> Skipping
                if(v1.x==-1 && v1.y==-1)
                  continue;

                if(tab_visible[v1.y][v1.x] < 9) // Uncovering uncovered field -> Skipping
                  continue;
                
                if(tab_hidden[v1.y][v1.x] == 0)
                {
                  q.push(v1); // Next, 'zero' field to uncover around
                  tab_visible[v1.y][v1.x] = 0; // Uncovering 'zero'
                }

                if(tab_hidden[v1.y][v1.x] < 9)
                  tab_visible[v1.y][v1.x] = tab_hidden[v1.y][v1.x]; // Uncovering 'nonzero' field
              }
            }
          }
        }
        else
          lost_game = true; // One of the bomb was clicked
      }

  // Right mouse button -> Flagging/unflagging the field
      if(e.key.code == Mouse::Right)
      {
        Vector2i pos = Mouse::getPosition(window); // Mouse clicked position
        int x=pos.x/size, y=pos.y/size; // Board field coordinate

        if(9 < tab_visible[y][x]) // Visible board field can't be flagged/unflagged
        {
          if(tab_visible[y][x] != 11)
              tab_visible[y][x] = 11; // Flagging
          else
            tab_visible[y][x] = 10; // Unflagging
        }
      }
    }
    }
    
  // ---------------------------------------------------------
  // Rendering
    window.clear();

    for(int i=0; i<Y; i++)
      for(int j=0; j<X; j++)
      {
        s[tab_visible[i][j]].setPosition(size*j, size*i);
        window.draw(s[tab_visible[i][j]]);
      }

    window.display();
  }

  while(window.isOpen() && lost_game) // The game is lost, only rendering the board
    {
      Event e;
      while(window.pollEvent(e))
        if(e.type == Event::Closed) window.close();
      
      // Rendering
      window.clear();

      for(int i=0; i<Y; i++)
        for(int j=0; j<X; j++)
        {
          s[tab_hidden[i][j]].setPosition(size*j, size*i);
          window.draw(s[tab_hidden[i][j]]);
        }

      window.display();
    }
  
  return 0;
}

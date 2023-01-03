#include <SFML/Graphics.hpp> // Texture; Sprite; RenderWindow; Event;
#include <Windows.h> // FreeConsole();
#include <ctime> // time();
#include <cstdlib> // srand();
#include <math.h>

using namespace sf;

int main()
{
  FreeConsole(); // No command prompt window will be displayed

  // ---------------------------------------------------------
  // Definition of the game sprites
  const int size = 64; // tiles' size
  Texture t; t.loadFromFile("images/15.png");

  Sprite s[16];

  s[0].setTexture(t);
  s[0].setTextureRect(IntRect(3*size, 3*size, size, size));
  for(int i=1; i<16; i++)
  {
    s[i].setTexture(t);
    s[i].setTextureRect(IntRect((i-1)%4*size, (i-1)/4*size, size, size));
  }

  // ---------------------------------------------------------
  // Game variables and others
  // Digits on the board
  int tab[4][4];

  const int num_of_rows = sizeof(tab)/sizeof(tab[0]); // number of rows
  const int num_of_cols = sizeof(tab[0])/sizeof(int); // number of columns
  const int num_of_ele = num_of_rows*num_of_cols; // number of elements

  // ---------------------------------------------------------
  // Random board arrangement
  srand(time(0));
  for(int i=0; i<num_of_rows; i++)
    for(int j=0; j<num_of_cols; j++)
      tab[i][j] = -1;

  Vector2i zero_pos;
  {
  int n = rand()%(num_of_ele); // Zero location on the board
  tab[n/num_of_cols][n%num_of_cols] = 0;
  zero_pos.x = n%num_of_cols;
  zero_pos.y = n/num_of_cols;
  }

  for(int i=1; i<num_of_ele; i++)
  { // Random board arrangement
    int n = rand()%(num_of_ele - i);
    for(int j=0; j<num_of_ele; j++)
    {
      if(n && tab[j/num_of_cols][j%num_of_cols]==-1) n--;
      if(!n && tab[j/num_of_cols][j%num_of_cols]==-1) { tab[j/num_of_cols][j%num_of_cols]=i; break; }
    }
  }

  // ---------------------------------------------------------
  // Window handling (Game loop)
  RenderWindow window(VideoMode(size*num_of_cols, size*num_of_rows), "15-Puzzle!");
  window.setFramerateLimit(60);
  while(window.isOpen())
  {
    // Window events handling
    Event e;
    while(window.pollEvent(e))
    {
      switch(e.type)
      {
      case Event::Closed:
        window.close();
        break;

  // ---------------------------------------------------------
  // Left mouse button -> possible shifting the element
      case Event::MouseButtonPressed:
        if(e.key.code == Mouse::Left)
        {
          Vector2i pos = Mouse::getPosition(window);
          pos.x/=size; pos.y/=size; // Pressed element

          // Shift is vertical/horizontal (correct) Remcok tu był :)
          if((std::abs(zero_pos.x-pos.x)==1 && zero_pos.y==pos.y) || // vertical
          (zero_pos.x==pos.x && std::abs(zero_pos.y-pos.y)==1)) // horizontal
          {
            tab[zero_pos.y][zero_pos.x] = tab[pos.y][pos.x];
            tab[pos.y][pos.x] = 0;
            zero_pos.x=pos.x; zero_pos.y=pos.y;
          }
        }
      }
    }

  // ---------------------------------------------------------
  // Rendering
    window.clear();

    for(int i=0; i<num_of_rows; i++)
      for(int j=0; j<num_of_cols; j++)
      {
        s[tab[i][j]].setPosition(j*size, i*size);
        window.draw(s[tab[i][j]]);
      }

    window.display();
  }

  return 0;
}

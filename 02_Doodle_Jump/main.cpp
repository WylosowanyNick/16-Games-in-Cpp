#include <SFML/Graphics.hpp> // Texture; Sprite; RenderWindow; Event;
#include <Windows.h> // FreeConsole();
#include <ctime> // time();
#include <cstdlib> // srand();

using namespace sf;

int main()
{
  FreeConsole(); // No command prompt window will be displayed

  // ---------------------------------------------------------
  // Definition of the Background, Platform and Doodle sprites.
  Texture t1, t2, t3;
  t1.loadFromFile("images/background.png");
  t2.loadFromFile("images/platform.png");
  t3.loadFromFile("images/doodle.png");

  Sprite sBackground(t1), sPlat(t2), sDood(t3);

  // ---------------------------------------------------------
  // Game variables and others
  struct Point { int x, y; }; // Struct holding coords x and y
  Point plat[10]; // Array containing 10 platforms coords
  for(int i=0; i<10; i++)
  {
    plat[i].x = rand()%400;
    plat[i].y = rand()%533;
  }

  // Variables describing Doodle's movement
  int x=100, y=100, h=100; // h is the fixed constant denoting height. When Doodle jump over this height, the platforms and Doodle are properly moved to keep Doodle in the screen view.
  float dy=0; // dy is a velocity of the Doodle in y-axis

  srand(time(0)); // Random number generator initialization
 
  // ---------------------------------------------------------
  // Window handling (Game loop)
  RenderWindow window(VideoMode(400, 533), "Doodle Jump!");
  window.setFramerateLimit(60);
  while(window.isOpen())
  {
    // Window events handling
    Event e;
    while(window.pollEvent(e))
    {
      if(e.type == Event::Closed) window.close(); // Window closed event
    }
 
    // Keyboard events handling
    if(Keyboard::isKeyPressed(Keyboard::Left)) x -= 3;
    if(Keyboard::isKeyPressed(Keyboard::Right)) x += 3;
  // ---------------------------------------------------------
    // Doodle motion
    dy += 0.2; // Gravity acceleration
    y += dy; // Direct Euler Method
    if(y > 500) dy = -10; // Jump from the bottom of the screen

    for(int i=0; i<10; i++) // Jump from the platform
      if(x+50>plat[i].x && x+20<plat[i].x+68 &&
      y+70>plat[i].y && y+70<plat[i].y+14 && dy>0) dy=-10;

    // When Doodle jump over the height 'h', the platforms and
    // Doodle are properly moved to keep Doodle in the screen view.
    if(y<h)
      for(int i=0; i<10; i++)
      {
        y = h; // Keeping the Doodle in the screen view.
        plat[i].y -= dy; // Moving properly the platforms
        if(plat[i].y > 533) // Generating new platform when it leaves the screen view
        { plat[i].y = 0; plat[i].x = rand()%400; }
      }

  // ---------------------------------------------------------
  // Rendering
    window.clear();
    window.draw(sBackground); // Background
    sDood.setPosition(x, y);
    window.draw(sDood); // Doodle

    for(int i=0; i<10; i++)
    {
      sPlat.setPosition(plat[i].x, plat[i].y);
      window.draw(sPlat); // Platforms
    }

    window.display();  
  }

  return 0;
}

#include <SFML/Graphics.hpp> // Texture; Sprite; RenderWindow; Event;
#include <Windows.h> // FreeConsole();
#include <ctime> // time();
#include <cstdlib> // srand();

using namespace sf;

// Function checks if two sprites collide.
bool isCollide(Sprite& s1, Sprite& s2)
{ return s1.getGlobalBounds().intersects(s2.getGlobalBounds()); }

int main()
{
  FreeConsole(); // No command prompt window will be displayed

  // ---------------------------------------------------------
  // Definition of the block, Background, Ball and Paddle sprites
  Texture t1, t2, t3, t4;
  t1.loadFromFile("images/block01.png");
  t2.loadFromFile("images/background.jpg");
  t3.loadFromFile("images/ball.png");
  t4.loadFromFile("images/paddle.png");

  Sprite block[100], sBackground(t2), sBall(t3), sPaddle(t4);
 
  int n=0;
  for(int i=1; i<=10; i++)
    for(int j=1; j<=10; j++)
    {
      block[n].setTexture(t1);
      block[n].setPosition(i*43, j*20);
      n++;
    }

  sBall.setPosition(300, 300);
  sPaddle.setPosition(215, 440);
  // ---------------------------------------------------------
  // Game variables and others
  float dx=6, dy=5; // Velocity of the ball

  srand(time(0)); // Random number generator initialization
 
  // ---------------------------------------------------------
  // Window handling (Game loop)
  RenderWindow window(VideoMode(520, 450), "Arkanoid!");
  window.setFramerateLimit(60);
  while(window.isOpen())
  {
    // Window events handling
    Event e;
    while(window.pollEvent(e))
    {
      if (e.type == Event::Closed) window.close();
    }

    // Keyboard events handling
    if (Keyboard::isKeyPressed(Keyboard::Left)) sPaddle.move(-8, 0);
    if (Keyboard::isKeyPressed(Keyboard::Right)) sPaddle.move(8, 0);
  // ---------------------------------------------------------
  // Ball movement  
    Vector2f b = sBall.getPosition(); // Reflections from the bounds of screen
    if(b.x<0 || b.x>520) dx = -dx;
    if(b.y<0 || b.y>450) dy = -dy;

    // Reflections from the blocks
    sBall.move(dx, 0);
    for(int i=0; i<100; i++)
      if(isCollide(sBall, block[i])) { block[i].setPosition(-100, 0); dx=-dx; break; }
    
    sBall.move(0, dy);
    for(int i=0; i<100; i++)
      if(isCollide(sBall, block[i])) { block[i].setPosition(-100, 0); dy=-dy; break; }
    
    // Reflections from the Paddle
    if(isCollide(sPaddle, sBall)) dy = -dy;
    
  // ---------------------------------------------------------
  // Rendering
    window.clear();
    window.draw(sBackground); // Background

    for(int i=0; i<100; i++) window.draw(block[i]); // Blocks
    window.draw(sBall); // Ball
    window.draw(sPaddle); // Paddle

    window.display();
  }

  return 0;
}

#include <SFML/Graphics.hpp> // Texture; Sprite; RenderWindow; Event;
#include <Windows.h> // FreeConsole();
#include <ctime> // time();
#include <cstdlib> // srand();
#include <vector>
#include <string>

using namespace sf;

int main()
{
  FreeConsole(); // No command prompt window will be displayed

  // ---------------------------------------------------------
  // Game variables and others
  const int X=30, Y=20, size=16; // The board sizes and the size of the tiles
  const float delay=0.3; // The speed of the game

  std::vector<Vector2i> snake; // Snake coordinates
  for(int i=2; i>-1; i--)
  {
    Vector2i v; v.x=X/2; v.y=(Y-1)-i;
    snake.push_back(v);
  }

  Vector2i apple(X/2, Y/2), head_of_snake; // Apple and head_of_snake coordinates

  int dir=0; // Integer denoting direction of the snake's movement

  std::vector<Vector2i>::iterator it; Clock clock;

  // ---------------------------------------------------------
  // Definition of the game sprites
  Texture t1, t2, t3;
  t1.loadFromFile("images/green.png"); // Apple tile
  t2.loadFromFile("images/red.png"); // Snake tiles
  t3.loadFromFile("images/white.png"); // Board tiles

  Sprite sApple(t1), sSnake(t2), sBackground(t3);

  // ---------------------------------------------------------
  // Window handling (Game loop)
  RenderWindow window(VideoMode(size*X, size*Y), "Snake Game!");
  window.setFramerateLimit(60);
  while(window.isOpen())
  {
    // Window events handling
    Event e;
    while(window.pollEvent(e))
      if(e.type == Event::Closed) window.close();

  // ---------------------------------------------------------
  // Keyboard events handling and snake movement
    if(delay < clock.getElapsedTime().asSeconds())
    {
      // Keyboard events handling
      switch(e.key.code)
      {
        case Keyboard::Up:
          if(dir==2) break;
          dir = 0;
          break;
        case Keyboard::Right:
          if(dir==3) break;
          dir = 1;
          break;
        case Keyboard::Down:
          if(dir==0) break;
          dir = 2;
          break;
        case Keyboard::Left:
          if(dir==1) break;
          dir = 3;
          break;
      }

      // Snake movement
      head_of_snake = snake[0];
      switch(dir)
      {
      case 0:
        head_of_snake.y--;
        break;
      case 1:
        head_of_snake.x++;
        break;
      case 2:
        head_of_snake.y++;
        break;
      case 3:
        head_of_snake.x--;
        break;
      default:
        return -1;
      }

  // ---------------------------------------------------------
  // The game actions
      for(int i=0; i<snake.size(); i++) // The game ends, when the snake run into itself.
        if(snake[i] == head_of_snake)
          window.close();

      if(head_of_snake.y<0 || head_of_snake.x==X // The game ends, when the snake run into the game border
          || head_of_snake.y==Y || head_of_snake.x<0)
        window.close();

      if(head_of_snake == apple)
      { // The snake ate an apple
        it = snake.begin();
        snake.insert(it, head_of_snake);

        int n = rand()%(X*Y - 1 - snake.size()); // The apple appears on the screen in the location, where there is no snake
        bool tab[Y][X] = {false};
        for(int i=0; i<snake.size(); i++)
          tab[snake[i].y][snake[i].x] = true;
        
        for(int i=0; i<X*Y; i++)
        {
          if(!tab[i/X][i%X]) n--;
          if(!n)
          {
            apple.y = i/X;
            apple.x = i%X;
            break;
          }
        }  
      }
      else // Snake movement in the case, when it didn't eat the apple.
      {
        for(int i=snake.size()-1; i>0; i--)
          snake[i] = snake[i-1];
        
        snake[0] = head_of_snake;
      }

      clock.restart();
    }

  // ---------------------------------------------------------
  // Rendering
    window.clear();

    for(int i=0; i<X; i++)
      for(int j=0; j<Y; j++)
      { // Background
        sBackground.setPosition(size*i, size*j);
        window.draw(sBackground);
      }

    for(it=snake.begin(); it!=snake.end(); it++)
    { // Snake
      sSnake.setPosition(size*it->x, size*it->y);
      window.draw(sSnake);
    }

    sApple.setPosition(size*apple.x, size*apple.y); // Apple
    window.draw(sApple);

    // Points
    window.setTitle("Snake Game! Points: " + std::to_string(snake.size()));

    window.display();
  }

  return 0;
}

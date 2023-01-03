#include <SFML/Graphics.hpp> // Texture; Sprite; RenderWindow; Event; Keyboard;
#include <Windows.h> // FreeConsole();
#define _USE_MATH_DEFINES
#include <math.h> // std::atan2; std::abs; M_PI;

using namespace sf;

struct Moving_point
{
  float x, y, speed, angle;
  int num_checkpoint=0;

  Moving_point(float x=0, float y=0, float speed=0, float angle=0)
  {
    this->x = x;
    this->y = y;
    this->speed = speed;
    this->angle = angle;
  }

  void move()
  {
    x += std::sin(angle) * speed;
    y -= std::cos(angle) * speed;
  }
};

int main()
{
  FreeConsole(); // No command prompt window will be displayed

  // ---------------------------------------------------------
  // Definition of the game sprites
  Texture t1, t2;
  t1.loadFromFile("images/background.png");
  t2.loadFromFile("images/car.png");

  Sprite sBackground(t1), sCar(t2);

  // ---------------------------------------------------------
  // Game variables and others
  // Cars' parameters
  float maxSpeed=6, acc=0.2, deg_acc=0.06;
  Moving_point car(153, 900); // Player car

  const int num_cars=3; // Number of the opponents
  Moving_point cars[num_cars];
  for(int i=0; i<num_cars; i++)
  { cars[i].x=161; cars[i].y=1032+i*70; cars[i].speed=4+i; }

  // Checkpoints for the opponents cars'
  int checkpoints[10][2] = {{174, 281}, {623, 222}, {693, 1200}, {880, 1280}, {964, 870}, {1200, 810}, {1262, 1495}, {1114, 1632}, {250, 1631}, {170, 1540}};
  int num_checkpoints = sizeof(checkpoints)/sizeof(checkpoints[0]);

  // ---------------------------------------------------------
  // Window handling (Game loop)
  RenderWindow window(VideoMode(700, 700), "Top Down Racing!");
  window.setFramerateLimit(60);
  while(window.isOpen())
  {
    // Window events handling
    Event e;
    while(window.pollEvent(e))
    {
      if(e.type == Event::Closed)
        window.close();
    }

    // Keyboard events
    bool Up=false, Right=false, Down=false, Left=false;
    if(Keyboard::isKeyPressed(Keyboard::Up)) Up=true;
    if(Keyboard::isKeyPressed(Keyboard::Right)) Right=true;
    if(Keyboard::isKeyPressed(Keyboard::Down)) Down=true;
    if(Keyboard::isKeyPressed(Keyboard::Left)) Left=true;

  // ---------------------------------------------------------
  // Player's car movement
    if(Up && car.speed<maxSpeed) car.speed += acc;
    if(Down && -maxSpeed<car.speed) car.speed -= acc;

    if(!Up && !Down)
      if(0<car.speed) car.speed -= 0.5*acc;
      else car.speed += 0.5*acc;
    
    if(-0.7*acc<car.speed && car.speed<0.7*acc) car.speed=0;

    if(Left && car.speed!=0) car.angle -= deg_acc * car.speed / maxSpeed;
    if(Right && car.speed!=0) car.angle += deg_acc * car.speed / maxSpeed;

    car.move();

  // ---------------------------------------------------------
  // Opponents' cars movement
    for(int i=0; i<num_cars; i++)
    {
      // alpha is the direction of the opponent car
      float alpha = -std::atan2(cars[i].x-checkpoints[cars[i].num_checkpoint][0], cars[i].y-checkpoints[cars[i].num_checkpoint][1]);

      // Calculations are skipped, if the car is on the right way
      if(cars[i].angle == alpha)
      { cars[i].move(); continue; }

      // The case, in which alpha and car's destination are on the same side of the vertical line
      if((alpha<0 && cars[i].angle<0) || (0<alpha && 0<cars[i].angle))
      {
        if(cars[i].angle < alpha)
          cars[i].angle+=deg_acc*cars[i].speed/maxSpeed;
        else
          cars[i].angle-=deg_acc*cars[i].speed/maxSpeed;
      }
      else if((alpha < 0 && 0 < cars[i].angle) || (0 < alpha && cars[i].angle < 0))
      { // The case, in which alpha and car's destination are on the opposite sides of the vertical line
        // Checking which arc is shorter
        if(std::abs(alpha) + std::abs(cars[i].angle) < M_PI) // Shorter is upper arc
        {
          int sign = (0<alpha) ? 1 : -1;
          cars[i].angle += sign*deg_acc*cars[i].speed/maxSpeed;
        }
        else // Shorter is lower arc
        {
          int sign = (0<alpha) ? 1 : -1;
          cars[i].angle -= sign*deg_acc*cars[i].speed/maxSpeed;
          
          if(M_PI < std::abs(cars[i].angle)) // Adjusting cars[i].angle and alpha so that they are on the same site of the vertical line
            cars[i].angle = sign*M_PI;
        }
      }      

      // if cars[i].angle is close to alpha, make it alpha
      if(std::abs(cars[i].angle - alpha) < 0.03)
        cars[i].angle = alpha;

      cars[i].move();
    }

    // Changing the checkpoint of the opponents cars'
    for(int i=0; i<num_cars; i++)
    {
      float dx = checkpoints[cars[i].num_checkpoint][0]-cars[i].x;
      float dy = checkpoints[cars[i].num_checkpoint][1]-cars[i].y;
      if(dx*dx + dy*dy < 400.f)
        if(cars[i].num_checkpoint == num_checkpoints - 1)
          cars[i].num_checkpoint = 0;
        else
          cars[i].num_checkpoint++;
    }

  // ---------------------------------------------------------
  // Collisions
    // Player's car with the opponents'
    for(int i=0; i<num_cars; i++)
    {
      int dx = car.x - cars[i].x;
      int dy = car.y - cars[i].y;
      if(dx*dx + dy*dy < 0x790)
      {
        car.x += dx/10;
        car.y += dy/10;
        cars[i].x -= dx/10;
        cars[i].y -= dy/10;
      }
    }

    // Opponents' cars with opponents'
    for(int i=0; i<num_cars; i++)
      for(int j=0; j<num_cars; j++)
      {
        int dx = cars[i].x - cars[j].x;
        int dy = cars[i].y - cars[j].y;
        if(dx*dx + dy*dy < 0x790)
        {
          cars[i].x += dx/10;
          cars[i].y += dy/10;
          cars[j].x -= dx/10;
          cars[j].y -= dy/10;
        }
      }

  // ---------------------------------------------------------
  // Rendering
    window.clear();
    sBackground.setPosition(-car.x+350, -car.y+350);
    window.draw(sBackground); // Background

    sCar.setColor(Color::Red);
    sCar.setOrigin(22, 22);
    sCar.setPosition(350, 350);
    sCar.setRotation(car.angle*180/M_PI);
    window.draw(sCar); // Player's car
    
    for(int i=0; i<num_cars; i++)
    { // Opponents' cars
      sCar.setColor(Color::Green);
      sCar.setOrigin(22, 22);
      sCar.setPosition(cars[i].x-car.x+350, cars[i].y-car.y+350);
      sCar.setRotation(cars[i].angle*180/M_PI);
      window.draw(sCar);
    }

    window.display();
  }

  return 0;
}

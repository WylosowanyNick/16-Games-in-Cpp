#include <SFML/Graphics.hpp> // RenderWindow; Event;

// Constant game variables
const short screenWidth=1024, screenHeight=768;
const float segmentLength=50.f, segmentWidth=200.f, cameraDepth=300.f,
      hScreenWidth=screenWidth/2, hScreenHeight=screenHeight/2;

// ---------------------------------------------------------
// Struct holding (x, y) coordinates of the middle of the
// horizontal line and its length.
struct HorizontalLine { float x, y, length; };

// Draw trapeze on RenderWindow using specified parameters.
void drawTrapeze(sf::RenderWindow &w, const sf::Color& c,
    const HorizontalLine& l1, const HorizontalLine& l2)
{
  float hll1=l1.length/2, hll2=l2.length/2;
  sf::ConvexShape trapeze(4);
  trapeze.setFillColor(c);
  trapeze.setPoint(0, sf::Vector2f(l1.x-hll1, l1.y));
  trapeze.setPoint(1, sf::Vector2f(l2.x-hll2, l2.y));
  trapeze.setPoint(2, sf::Vector2f(l2.x+hll2, l2.y));
  trapeze.setPoint(3, sf::Vector2f(l1.x+hll1, l1.y));

  w.draw(trapeze);
}

struct Vector3f { float x,y,z; };

class HorizontalLineProjection
{
public:
  float x,y,z; // World coordinates of the middle of the horizontal line
  HorizontalLine projectedLine; //  Screen coordinates and screen length of the projected, horizontal, line

  HorizontalLineProjection()
    : x(0.f), y(0.f), z(0.f),
    projectedLine({0.f, 0.f, 0.f}) {}

  HorizontalLineProjection(float x, float y, float z)
    : x(x), y(y), z(z),
    projectedLine({0.f, 0.f, 0.f}) {}

  bool Project(Vector3f &cameraCoords)
  { // Perspective projection
    if(z-cameraCoords.z<=0.f) return false; // Invalid projection, skipping

    float scale = cameraDepth/(z-cameraCoords.z);
    projectedLine.x = scale*(x-cameraCoords.x) + hScreenWidth;
    projectedLine.y = -scale*(y-cameraCoords.y) + hScreenHeight;
    projectedLine.length = scale*segmentWidth;

    return true;
  }
};

int main()
{
  // Game variables
  Vector3f cameraCoords = { 0.f, 100.f, 0.f};

  // ---------------------------------------------------------
  // Window handling (Game loop)
  sf::RenderWindow window(sf::VideoMode(screenWidth, screenHeight), "Outrun!");
  window.setFramerateLimit(60);
  while(window.isOpen())
  {
    // Window events handling
    sf::Event e;
    while(window.pollEvent(e))
    {
      if(e.type == sf::Event::Closed)
        window.close();
    }

    // Keyboard events
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) cameraCoords.z += 1.f;
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) cameraCoords.x += 1.f;
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) cameraCoords.z -= 1.f;
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) cameraCoords.x -= 1.f;

  // ---------------------------------------------------------
  // Rendering
    window.clear(sf::Color::Green);

    float z=1.f;
    for(int i=0; i<50; i++)
    { // Road rendering
      float x0,y0,z0,x1,y1,z1;
      x0=y0=x1=y1=0.f;
      z0=z; z1=z0+segmentLength;
      HorizontalLineProjection l1(x0,y0,z0), l2(x1,y1,z1);
      if(l1.Project(cameraCoords) && l2.Project(cameraCoords))
      { // If valid projection, draw road Trapeze
        sf::Color c = (i%2) ? sf::Color(107,107,107) : sf::Color(105,105,105);
        drawTrapeze(window, c, l1.projectedLine, l2.projectedLine);
      }

      z += segmentLength;
    }

    window.display();
  }
}

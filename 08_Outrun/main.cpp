#include <SFML/Graphics.hpp> // RenderWindow; Event; Texture; Sprite;
#include <math.h>
#include <iostream>
#include <Windows.h> // FreeConsole();

// Constant game variables
const short screenWidth=768, screenHeight=768;
const float segmentLength=50.f, segmentWidth=400.f, segmentFullWidth=segmentWidth+50.f, cameraDepth=300.f,
      hScreenWidth=screenWidth/2, hScreenHeight=screenHeight/2;

const sf::Color darkGrassColor(0,153,51), lightGrassColor(102,255,102),
  blackColor(0,0,0), whiteColor(255,255,255),
  darkGrubColor(105,105,105), lightGrubColor(107,107,107);

// ---------------------------------------------------------
// Struct holding (x, y) coordinates of the middle of the
// horizontal line and its length.
struct HorizontalLine { float x, y, length; };

// Draw trapeze on RenderWindow using specified parameters.
void drawTrapeze(sf::RenderWindow &window, const sf::Color& color,
    const HorizontalLine& l1, const HorizontalLine& l2)
{
  float hll1=l1.length/2, hll2=l2.length/2;
  sf::ConvexShape trapeze(4);
  trapeze.setFillColor(color);
  trapeze.setPoint(0, sf::Vector2f(l1.x-hll1, l1.y));
  trapeze.setPoint(1, sf::Vector2f(l2.x-hll2, l2.y));
  trapeze.setPoint(2, sf::Vector2f(l2.x+hll2, l2.y));
  trapeze.setPoint(3, sf::Vector2f(l1.x+hll1, l1.y));

  window.draw(trapeze);
}

struct Vector3f { float x,y,z; };

class HorizontalLineProjection
{
public:
  float x,y,z,lineLength; // World coordinates of the middle of the horizontal line and its length
  HorizontalLine projectedLine; //  Screen coordinates and screen length of the projected, horizontal line

  HorizontalLineProjection()
    : x(0.f), y(0.f), z(0.f),
    projectedLine({0.f, 0.f, 0.f}) {}

  HorizontalLineProjection(float x, float y, float z, float lineLength)
    : x(x), y(y), z(z), lineLength(lineLength),
    projectedLine({0.f, 0.f, 0.f}) {}

  bool Project(const Vector3f &cameraCoords)
  { // Perspective projection
    if(z-cameraCoords.z<=0.f) return false; // Invalid projection, skipping

    float scale = cameraDepth/(z-cameraCoords.z);
    projectedLine.x = scale*(x-cameraCoords.x) + hScreenWidth;
    projectedLine.y = -scale*(y-cameraCoords.y) + hScreenHeight;
    projectedLine.length = scale*lineLength;

    return true;
  }
};

// Draw the Sprite on the right side of the Road
void drawSpriteOnTheRight(sf::RenderWindow& window, sf::Sprite& sprite, const HorizontalLineProjection& fl1)
{
  HorizontalLine projectedLine = fl1.projectedLine;
  sf::IntRect textureRect = sprite.getTextureRect();
  float scale = projectedLine.length/((float)textureRect.width);
  sprite.setScale(scale, scale);
  sprite.setPosition({projectedLine.x+projectedLine.length, projectedLine.y-sprite.getGlobalBounds().height});
  window.draw(sprite);
}

int main()
{
  FreeConsole(); // No command prompt window will be displayed

  // ---------------------------------------------------------
  // Game variables
  Vector3f cameraCoords = { 0.f, 100.f, 0.f};
  int beatenSegment=0; // Beaten road segment

  const int roadCoordsSize = 250; // Road
  Vector3f *roadCoords = new Vector3f[roadCoordsSize];
  { // roadCoords definition
    float x=0.f,dx=0.f;
    for(int i=0; i<40; i++)
    { // Right turn
      roadCoords[i].x=x;
      roadCoords[i].y=0.f;
      roadCoords[i].z=i*segmentLength;
      x += dx;
      dx += 1.f;
    }

    for(int i=40; i<80; i++)
    { // Right turn ending
      roadCoords[i].x=x;
      roadCoords[i].y=0.f;
      roadCoords[i].z=i*segmentLength;
      x += dx;
      dx -= 1.f;
    }

    for(int i=80; i<140; i++)
    { // Elevation
      roadCoords[i].x=x;
      roadCoords[i].y=200.f*sin(3.1415f*((float)i-80.f)/60.f);
      roadCoords[i].z=i*segmentLength;
    }

    for(int i=140; i<roadCoordsSize; i++)
    { // Straight road
      roadCoords[i].x=x;
      roadCoords[i].y=0.f;
      roadCoords[i].z=i*segmentLength;
    }
  }

  sf::Texture textureBackground;
  textureBackground.loadFromFile("images/background.png");
  sf::Sprite spriteBackground(textureBackground);

  sf::Texture textureTree;
  textureTree.loadFromFile("images/5.png");
  sf::Sprite spriteTree(textureTree);

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
        { delete roadCoords; window.close(); }
    }

    // Keyboard events
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) cameraCoords.z += 10.f;
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) cameraCoords.x += 8.f;
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) cameraCoords.z -= 10.f;
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) cameraCoords.x -= 8.f;

  // ---------------------------------------------------------
  // Rendering
    window.clear(sf::Color::Green);

    window.draw(spriteBackground);

    { // First segment beaten check
      float x0=roadCoords[beatenSegment].x,
        y0=roadCoords[beatenSegment].y,
        z0=roadCoords[beatenSegment].z,
        x1=roadCoords[beatenSegment+1].x,
        y1=roadCoords[beatenSegment+1].y,
        z1=roadCoords[beatenSegment+1].z;
      
      HorizontalLineProjection fl1(x0,y0,z0,segmentFullWidth), fl2(x1,y1,z1,segmentFullWidth),
        l1(x0,y0,z0,segmentWidth), l2(x1,y1,z1,segmentWidth);
      if(!(fl1.Project(cameraCoords) && fl2.Project(cameraCoords)
        && l1.Project(cameraCoords) && l2.Project(cameraCoords)))
        beatenSegment++; // The projection is invalid, the segment was beaten
    }

    { // Camera location based on the road height (interpolation)
      float percentOfSegmentBeaten = (cameraCoords.z-beatenSegment*segmentLength)/segmentLength;
      cameraCoords.y = 100.f + roadCoords[beatenSegment].y +
        percentOfSegmentBeaten*(roadCoords[beatenSegment+1].y-roadCoords[beatenSegment].y); 
    }

    for(int i=99; i>=0; i--) // Further things should be rendered earlier
    { // Road rendering
      int currentSegment = beatenSegment + i;
      if(currentSegment+1>=roadCoordsSize)
        { std::cout << "The Game Was Beaten" << std::endl; delete roadCoords; return 0; }
      
      float x0=roadCoords[currentSegment].x,
        y0=roadCoords[currentSegment].y,
        z0=roadCoords[currentSegment].z,
        x1=roadCoords[currentSegment+1].x,
        y1=roadCoords[currentSegment+1].y,
        z1=roadCoords[currentSegment+1].z;

      HorizontalLineProjection fl1(x0,y0,z0,segmentFullWidth), fl2(x1,y1,z1,segmentFullWidth),
        l1(x0,y0,z0,segmentWidth), l2(x1,y1,z1,segmentWidth);
      if(fl1.Project(cameraCoords) && fl2.Project(cameraCoords)
        && l1.Project(cameraCoords) && l2.Project(cameraCoords))
      { // If valid projection, draw road segment with its surrounding grass
        drawTrapeze(window, (currentSegment)%2 ? darkGrassColor : lightGrassColor, // Grass rendering
          {hScreenWidth, l1.projectedLine.y, screenWidth},
          {hScreenWidth, l2.projectedLine.y, screenWidth});

        // Grub rendering
        drawTrapeze(window, (currentSegment)%2 ? blackColor : whiteColor, fl1.projectedLine, fl2.projectedLine);

        // Road rendering
        drawTrapeze(window, (currentSegment)%2 ? darkGrubColor : lightGrubColor, l1.projectedLine, l2.projectedLine);

        // Sprites rendering
        if(!(currentSegment%5))
          drawSpriteOnTheRight(window, spriteTree, fl1);
      }
    }

    window.display();
  }

  delete roadCoords;
}

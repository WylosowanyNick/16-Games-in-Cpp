#include <SFML/Graphics.hpp> // RenderWindow; Event; Texture; Sprite;
#include <cstdlib> // rand(); abs();
#include <ctime> // time();
#include <utility> // swap();
#include <Windows.h> // FreeConsole();

using namespace sf;

// ---------------------------------------------------------------------------
// Constant game variables
constexpr short windowWidth=740, windowHeight=480, 
          numOfRows=8, numOfCols=8, numOfGems=7, gemTileSize=49,
          tileSize=54, xGridOffset=48, yGridOffset=24,
          animationSpeed=8;

constexpr unsigned char alphaDecreaseSpeed=10;

// ---------------------------------------------------------------------------
// Game class
class Game
{
public:
  Game()
  {
    srand(time(NULL));

    // Init grid
    for(char i=0; i<numOfRows; i++)
      for(char j=0; j<numOfCols; j++)
      {
        Piece& p = grid[i][j];
        p.kind = rand()%numOfGems;
        p.x = j*tileSize + xGridOffset;
        p.y = i*tileSize + yGridOffset;
      }

    // Init Textures and Sprites
    textureGems.loadFromFile("images/gems.png");
    textureCursor.loadFromFile("images/cursor.png");
    textureBackground.loadFromFile("images/background.png");

    for(char i=0; i<numOfGems; i++)
    {
      gem[i].setTexture(textureGems);
      gem[i].setTextureRect(IntRect(i*gemTileSize,0,gemTileSize,gemTileSize));
    }
    background.setTexture(textureBackground);

    // Init window
    window.create(VideoMode(windowWidth, windowHeight), "Bejeweled!");
    window.setFramerateLimit(30);

    iPrevPiece = jPrevPiece = -1;
    alphaOfPiecesToDelete = 255;

    // Delete Pieces
    while(GetPiecesToDelete())
      DeleteAnimation();
  }

  void Run()
  {
    while(Events())
    {
      Render();
    }
  }

private:
  bool Events()
  {
    Event e;
    while(window.pollEvent(e))
    {
      if(e.type == Event::Closed) { window.close(); return false; }

      if(e.type == Event::MouseButtonPressed &&
          e.mouseButton.button == Mouse::Button::Left)
      {
        // Clicked mouse coords relative to the grid
        int x=e.mouseButton.x-xGridOffset, y=e.mouseButton.y-yGridOffset;

        if(y>=0 && y<numOfRows*tileSize &&
            x>=0 && x<numOfCols*tileSize) // Checking if grid was clicked
        {
          int iPiece=y/tileSize, jPiece=x/tileSize;

          if(iPrevPiece == -1) // Init swapping
          { iPrevPiece=iPiece; jPrevPiece=jPiece; }
          else
          {
            if((abs(iPiece-iPrevPiece)==1 && jPiece==jPrevPiece) ||
              (iPiece==iPrevPiece && abs(jPiece-jPrevPiece)==1))
            { // Swapping
              if(SwappingAnimation(iPrevPiece, jPrevPiece, iPiece, jPiece))
                return false;

              if(!GetPiecesToDelete())
              { // There are no pieces to be deleted
                if(SwappingAnimation(iPrevPiece, jPrevPiece, iPiece, jPiece))
                  return false;
              }
              else
              { // There are some pieces to be deleted
                do { if(DeleteAnimation()) return false; } while(GetPiecesToDelete());
              }
              iPrevPiece = jPrevPiece = -1;
            }
            else // Reinit swapping
            { iPrevPiece=iPiece; jPrevPiece=jPiece; }
          }
        }
      }
    }

    return true;
  }
  
  void Render()
  {
    window.draw(background);

    Color colorOfPiecesToDelete(255, 255, 255, alphaOfPiecesToDelete),
      colorWhite(255, 255, 255, 255);
    for(char i=0; i<numOfRows; i++)
      for(char j=0; j<numOfCols; j++)
        if(gridToDelete[i][j])
        {
          Piece& p = grid[i][j];
          Sprite& g = gem[p.kind];
          g.setPosition(p.x, p.y);
          g.setColor(colorOfPiecesToDelete);
          window.draw(g);
        }
        else
        {
          Piece& p = grid[i][j];
          Sprite& g = gem[p.kind];
          g.setPosition(p.x, p.y);
          g.setColor(colorWhite);
          window.draw(g);
        }

    window.display();
  }

  char GetPiecesToDelete()
  {
    char numOfPiecesToDelete = 0;

    // Data Init
    memset(gridToDelete, 0, sizeof(gridToDelete));

    // Rows checking
    for(char i=0; i<numOfRows; i++)
      for(char j=0; j<numOfCols-2; j++)
      {
        if(grid[i][j].kind==grid[i][j+1].kind
            && grid[i][j+1].kind==grid[i][j+2].kind)
        {
          gridToDelete[i][j] = true;
          gridToDelete[i][j+1] = true;
          gridToDelete[i][j+2] = true;
        }
      }

    // Columns checking
    for(char i=0; i<numOfRows-2; i++)
      for(char j=0; j<numOfCols; j++)
      {
        if(grid[i][j].kind==grid[i+1][j].kind
            && grid[i+1][j].kind==grid[i+2][j].kind)
        {
          gridToDelete[i][j] = true;
          gridToDelete[i+1][j] = true;
          gridToDelete[i+2][j] = true;
        }
      }

    // Number of Pieces to Delete
    for(char i=0; i<numOfRows; i++)
      for(char j=0; j<numOfCols; j++)
        if(gridToDelete[i][j])
          numOfPiecesToDelete++;

    return numOfPiecesToDelete;
  }

  bool PollEvents()
  {
    Event e;
    while(window.pollEvent(e))
    {
      if(e.type == Event::Closed) { window.close(); return true; }
    }

    return false;
  }

  bool SwappingAnimation(int iPrevPiece, int jPrevPiece, int iPiece, int jPiece)
  {
    Piece &pP=grid[iPrevPiece][jPrevPiece], &p=grid[iPiece][jPiece];

    if(iPrevPiece == iPiece)
    { // Same row swapping
      short prevDestX=p.x, pDestX=pP.x, prevDirectionX=(prevDestX-pP.x>0);
      
      while(true)
      {
        if(PollEvents()) return true;

        if(prevDirectionX)
        { // Positive direction of previous piece
          if(pP.x+animationSpeed < prevDestX)
          {
            pP.x += animationSpeed;
            p.x -= animationSpeed;
          }
          else
          {
            pP.x = prevDestX;
            p.x = pDestX;
            break;
          }
        }
        else
        { // Negative direction of previous piece
          if(pP.x-animationSpeed > prevDestX)
          {
            pP.x -= animationSpeed;
            p.x += animationSpeed;
          }
          else
          {
            pP.x = prevDestX;
            p.x = pDestX;
            break;
          }
        }

        Render();
      }
    }
    else
    { // Same col swapping
      short prevDestY=p.y, pDestY=pP.y, prevDirectionY=(prevDestY-pP.y>0);
      
      while(true)
      {
        if(PollEvents()) return true;

        if(prevDirectionY)
        { // Positive direction of previous piece
          if(pP.y+animationSpeed < prevDestY)
          {
            pP.y += animationSpeed;
            p.y -= animationSpeed;
          }
          else
          {
            pP.y = prevDestY;
            p.y = pDestY;
            break;
          }
        }
        else
        { // Negative direction of previous piece
          if(pP.y-animationSpeed > prevDestY)
          {
            pP.y -= animationSpeed;
            p.y += animationSpeed;
          }
          else
          {
            pP.y = prevDestY;
            p.y = pDestY;
            break;
          }
        }

        Render();
      }
    }

    // Swapping in the data structure
    std::swap(pP.kind, p.kind);
    std::swap(pP.x, p.x);
    std::swap(pP.y, p.y);

    return false;
  }

  bool DeleteAnimation()
  {
    // Deletion through decreasing alpha
    while(true)
    {
      if(PollEvents()) return true;
      
      // Decreasing alphaOfPiecesToDelete
      if(alphaOfPiecesToDelete-alphaDecreaseSpeed > 0)
        alphaOfPiecesToDelete -= alphaDecreaseSpeed;
      else
      {
        alphaOfPiecesToDelete = 0;
        Render();
        break;
      }

      Render();
    }
    alphaOfPiecesToDelete = 255; // Reinit

    // Deletion in the data structure
    for(char i=0; i<numOfRows; i++)
      for(char j=0; j<numOfCols; j++)
        if(gridToDelete[i][j])
          grid[i][j].kind = rand()%numOfGems;

    for(char j=0; j<numOfCols; j++)
    {
      unsigned char numOfDeletedGems=0, numOfNondeletedGems; // in the considered col
      for(char i=0; i<numOfRows; i++)
        if(gridToDelete[i][j]) numOfDeletedGems++;
      numOfNondeletedGems = numOfRows - numOfDeletedGems;

      // Rearranging deleted and nondeleted gems in data structure
      Piece deletedGems[numOfRows], nondeletedGems[numOfRows];
      char deletedInd=0, nondeletedInd=0;
      for(char i=numOfRows-1; i>=0; i--)
        if(gridToDelete[i][j])
          deletedGems[deletedInd++] = grid[i][j];
        else
          nondeletedGems[nondeletedInd++] = grid[i][j];

      for(char i=0; i<numOfNondeletedGems; i++)
        grid[numOfRows-1-i][j] = nondeletedGems[i];

      for(char i=0; i<numOfDeletedGems; i++)
      {
        deletedGems[i].y = yGridOffset - (i+1)*tileSize;
        grid[numOfDeletedGems-1-i][j] = deletedGems[i];
      }
    }

    // Animation of falling from above pieces
    short yIndices[numOfRows]; // of gems
    for(char i=0; i<numOfRows; i++)
      yIndices[i] = yGridOffset + i*tileSize;
    while(true)
    {
      if(PollEvents()) return true;

      char numOfMovingPieces = 0;
      for(char i=0; i<numOfCols; i++)
        for(char j=0; j<numOfRows; j++)
        {
          if(grid[i][j].y+animationSpeed < yIndices[i])
          {
            grid[i][j].y += animationSpeed;
            numOfMovingPieces++;
          }
          else
            grid[i][j].y = yIndices[i];
        }

      Render();

      if(!numOfMovingPieces)
        break;
    }

    return false;
  }

private:
  struct Piece { char kind; short x, y; };
  Piece grid[numOfRows][numOfCols];
  bool gridToDelete[numOfRows][numOfCols];
  int iPrevPiece, jPrevPiece;
  unsigned char alphaOfPiecesToDelete;

  Texture textureGems, textureCursor, textureBackground;
  Sprite gem[numOfGems], background;
  RenderWindow window;
};

// ---------------------------------------------------------------------------
int main()
{
  FreeConsole(); // No command prompt window will be displayed

  Game game;

  game.Run();

  return 0;
}


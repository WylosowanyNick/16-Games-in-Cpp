# Minesweeper

The logic of the game is easy to understand reading comments in the source code. The only thing, that will be explained here, is "Uncovering all 'zero' fields" algorithm:

```cpp
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
```

Ok, so one has clicked a '0' field in the Minesweeper game. In the standard game one should expect that a big chunk of '0' fields to be somehow uncovered. But why something like that is happening in the first place. So one has clicked a '0' and it means, that in the position of this '0' there are no bomb around this '0'. So one can safely click fields around this zero to uncover them (as revealing non-bomb fields is the goal of the game). This process however can be automated (as it is so trivial and unnecessarly time consuming). So for this '0' position something should happen and thus this location is pushed to the queue to remember this postion. Nextly, a '0' position is considered. Around this position fields are uncovered in the proper way (see the source code). If during the process of the '0' field uncovering a next '0' was found, this next '0' should be simillary considered nextly (and thus the queue is needed to remember this piece of information). The rest is easy to understand reading the source code.
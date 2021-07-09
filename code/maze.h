#if !defined(MAZE_H)

#include "maze_platform.h"

#define Minimum(A, B) ((A < B) ? (A) : (B))
#define Maximum(A, B) ((A > B) ? (A) : (B))

//
//
//

struct memory_arena
{
    memory_index Size;
    u8 *Base;
    memory_index Used;
};

internal void
InitializeArena(memory_arena *Arena, memory_index Size, u8 *Base)
{
    Arena->Size = Size;
    Arena->Base = Base;
    Arena->Used = 0;
}

#define PushStruct(Arena, type) (type *)PushSize_(Arena, sizeof(type))
#define PushArray(Arena, Count, type) (type *)PushSize_(Arena, (Count)*sizeof(type))
void *
PushSize_(memory_arena *Arena, memory_index Size)
{
    Assert((Arena->Used + Size) <= Arena->Size);
    void *Result = Arena->Base + Arena->Used;
    Arena->Used += Size;
    
    return(Result);
}

#include "maze_math.h"
#include "maze_intrinsics.h"
#include "maze_random.h"
#include "maze_stack.h"

enum cell
{
    Cell_PathN = 0x01,
    Cell_PathE = 0x02,
    Cell_PathS = 0x04,
    Cell_PathW = 0x08,
    Cell_Visited = 0x10,
};

#define MAZE_WIDTH 40
#define MAZE_HEIGHT 25
#define MAZE_GENERATE_PER_FRAME 25

struct game_state
{
    memory_arena Arena;
    
    u32 PlayerIndexForController[ArrayCount(((game_input *)0)->Controllers)];
    
    s32 *Maze;
    
    maze_location_stack MazeLocationStack;
    s32 VisitedCells;
    maze_location MazeGoal;
    maze_location MazeCurrent;
    r32 MoveInterval;
    
    b32 GeneratingMaze;
};

#define MAZE_H
#endif

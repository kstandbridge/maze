#if !defined(MAZE_STACK_H)

struct maze_location
{
    s32 X;
    s32 Y;
};

struct maze_location_stack
{
    s32 Top;
    s32 Capacity;
    maze_location *Data;
};

inline maze_location
MazeLocation(s32 X, s32 Y)
{
    maze_location Result;
    
    Result.X = X;
    Result.Y = Y;
    
    return Result;
}

inline void
InitStack(maze_location_stack *Stack, memory_arena *Arena, s32 Capacity)
{
    Stack->Capacity = Capacity;
    Stack->Top = -1;
    Stack->Data = PushArray(Arena, Stack->Capacity, maze_location);
}

inline b32
IsStackFull(maze_location_stack *Stack)
{
    b32 Result = Stack->Top == Stack->Capacity - 1;
    
    return Result;
}


inline b32
IsStackEmpty(maze_location_stack *Stack)
{
    b32 Result = Stack->Top == -1;
    
    return Result;
}

inline void
PushStack(maze_location_stack *Stack, maze_location Item)
{
    Assert(!IsStackFull(Stack));
    
    Stack->Data[++Stack->Top] = Item;
}

inline maze_location
PopStack(maze_location_stack *Stack)
{
    Assert(!IsStackEmpty(Stack));
    
    maze_location Result = Stack->Data[Stack->Top--];
    
    return Result;
}

inline maze_location
PeekStack(maze_location_stack* Stack)
{
    Assert(!IsStackEmpty(Stack));
    
    maze_location Result = Stack->Data[Stack->Top];
    
    return Result;
}


#define MAZE_STACK_H
#endif

#include "maze.h"

internal void
GameOutputSound(game_state *GameState, game_sound_output_buffer *SoundBuffer, int ToneHz)
{
    s16 ToneVolume = 3000;
    int WavePeriod = SoundBuffer->SamplesPerSecond/ToneHz;
    
    s16 *SampleOut = SoundBuffer->Samples;
    for(int SampleIndex = 0;
        SampleIndex < SoundBuffer->SampleCount;
        ++SampleIndex)
    {
        // TODO(kstandbridge): Draw this out for people
#if 0
        r32 SineValue = sinf(GameState->tSine);
        s16 SampleValue = (s16)(SineValue * ToneVolume);
#else
        s16 SampleValue = 0;
#endif
        *SampleOut++ = SampleValue;
        *SampleOut++ = SampleValue;
        
#if 0
        GameState->tSine += 2.0f*Pi32*1.0f/(r32)WavePeriod;
        if(GameState->tSine > 2.0f*Pi32)
        {
            GameState->tSine -= 2.0f*Pi32;
        }
#endif
    }
}


internal void
DrawRectangle(game_offscreen_buffer *Buffer, v2 vMin, v2 vMax, r32 R, r32 G, r32 B)
{    
    s32 MinX = RoundReal32ToInt32(vMin.X);
    s32 MinY = RoundReal32ToInt32(vMin.Y);
    s32 MaxX = RoundReal32ToInt32(vMax.X);
    s32 MaxY = RoundReal32ToInt32(vMax.Y);
    
    if(MinX < 0)
    {
        MinX = 0;
    }
    
    if(MinY < 0)
    {
        MinY = 0;
    }
    
    if(MaxX > Buffer->Width)
    {
        MaxX = Buffer->Width;
    }
    
    if(MaxY > Buffer->Height)
    {
        MaxY = Buffer->Height;
    }
    
    u32 Color = ((RoundReal32ToUInt32(R * 255.0f) << 16) |
                 (RoundReal32ToUInt32(G * 255.0f) << 8) |
                 (RoundReal32ToUInt32(B * 255.0f) << 0));
    
    u8 *Row = ((u8 *)Buffer->Memory +
               MinX*Buffer->BytesPerPixel +
               MinY*Buffer->Pitch);
    for(int Y = MinY;
        Y < MaxY;
        ++Y)
    {
        u32 *Pixel = (u32 *)Row;
        for(int X = MinX;
            X < MaxX;
            ++X)
        {            
            *Pixel++ = Color;
        }
        
        Row += Buffer->Pitch;
    }
}

inline void
GenerateMaze(game_state *GameState)
{
    for(s32 Index = 0;
        Index < MAZE_GENERATE_PER_FRAME;
        ++Index)
    {
        if(!GameState->GeneratingMaze)
        {
            break;
        }
        s32 NextNeighbour = 0;
        s32 CellNeighbours[4] = {0};
        
        maze_location Top = PeekStack(&GameState->MazeLocationStack);
        
#define Offset(x, y) ((Top.Y + y) * MAZE_WIDTH + (Top.X + x))
        
        
        if(Top.Y > 0 && (GameState->Maze[Offset(0, -1)] & Cell_Visited) == 0)
        {
            CellNeighbours[NextNeighbour++] = Cell_PathN;
        }
        
        if(Top.X < MAZE_WIDTH - 1 && (GameState->Maze[Offset(1, 0)] & Cell_Visited) == 0)
        {
            CellNeighbours[NextNeighbour++] = Cell_PathE;
        }
        
        if(Top.Y < MAZE_HEIGHT - 1 && (GameState->Maze[Offset(0, 1)] & Cell_Visited) == 0)
        {
            CellNeighbours[NextNeighbour++] = Cell_PathS;
        }
        
        if(Top.X > 0 && (GameState->Maze[Offset(-1, 0)] & Cell_Visited) == 0)
        {
            CellNeighbours[NextNeighbour++] = Cell_PathW;
        }
        
        if(NextNeighbour > 0)
        {
            cell Cell = (cell)CellNeighbours[RandomChoice(NextNeighbour)];
            switch(Cell)
            {
                
                case Cell_PathN:
                {
                    GameState->Maze[Offset(0, -1)] |= Cell_Visited | Cell_PathS;
                    GameState->Maze[Offset(0, 0)] |= Cell_PathN;
                    PushStack(&GameState->MazeLocationStack, MazeLocation(Top.X + 0, Top.Y - 1));
                } break;
                
                case Cell_PathE:
                {
                    GameState->Maze[Offset(1, 0)] |= Cell_Visited | Cell_PathW;
                    GameState->Maze[Offset(0, 0)] |= Cell_PathE;
                    PushStack(&GameState->MazeLocationStack, MazeLocation(Top.X + 1, Top.Y + 0));
                } break;
                
                case Cell_PathS:
                {
                    GameState->Maze[Offset(0, 1)] |= Cell_Visited | Cell_PathN;
                    GameState->Maze[Offset(0, 0)] |= Cell_PathS;
                    PushStack(&GameState->MazeLocationStack, MazeLocation(Top.X + 0, Top.Y + 1));
                } break;
                
                case Cell_PathW:
                {
                    GameState->Maze[Offset(-1, 0)] |= Cell_Visited | Cell_PathE;
                    GameState->Maze[Offset(0, 0)] |= Cell_PathW;
                    PushStack(&GameState->MazeLocationStack, MazeLocation(Top.X - 1, Top.Y + 0));
                } break;
                
                default:
                {
                    InvalidCodePath;
                }
            }
            
            GameState->VisitedCells++;
        }
        else
        {
            PopStack(&GameState->MazeLocationStack);
        }
        
        GameState->GeneratingMaze = (GameState->VisitedCells < MAZE_WIDTH * MAZE_HEIGHT);
    }
}

inline void
RenderMaze(game_state *GameState, game_offscreen_buffer *Buffer)
{
    r32 CellWidth = (r32)Buffer->Width / MAZE_WIDTH;
    r32 CellHeight = (r32)Buffer->Height / MAZE_HEIGHT;
    
    for(s32 Y = 0;
        Y < MAZE_HEIGHT;
        ++Y)
    {
        for(s32 X = 0;
            X < MAZE_WIDTH;
            ++X)
        {
            v2 Min = V2(X*CellWidth, Y*CellHeight);
            v2 Max = V2((X + 1)*CellWidth, (Y + 1)*CellHeight);
            DrawRectangle(Buffer, Min, Max, 0, 0, 0);
            if(GameState->Maze[Y*MAZE_WIDTH + X] & Cell_Visited)
            {
                DrawRectangle(Buffer, Min, Max - V2(CellWidth/3, CellHeight/3), 1, 1, 1);
            }
            else
            {
                DrawRectangle(Buffer, Min, Max - V2(CellWidth/3, CellHeight/3), 0, 0, 1);
            }
            
            if(GameState->Maze[Y*MAZE_WIDTH + X] & Cell_PathN)
            {
                DrawRectangle(Buffer, Min - V2(0, CellHeight/3), Max - V2(CellWidth/3, CellHeight), 1, 1, 1);
            }
            
            if(GameState->Maze[Y*MAZE_WIDTH + X] & Cell_PathS)
            {
                DrawRectangle(Buffer, Min + V2(0, CellHeight/2), Max - V2(CellWidth/3, 0), 1, 1, 1);
            }
            
            if(GameState->Maze[Y*MAZE_WIDTH + X] & Cell_PathW)
            {
                DrawRectangle(Buffer, Min - V2(CellWidth/3, 0), Max - V2(CellWidth, CellHeight/3), 1, 1, 1);
            }
            
            if(GameState->Maze[Y*MAZE_WIDTH + X] & Cell_PathE)
            {
                DrawRectangle(Buffer, Min + V2(CellWidth/2, 0), Max - V2(0, CellHeight/3), 1, 1, 1);
            }
            
            if(GameState->GeneratingMaze)
            {
                maze_location Top = PeekStack(&GameState->MazeLocationStack);
                if(Top.X == X && Top.Y == Y)
                {
                    DrawRectangle(Buffer, Min, Max - V2(CellWidth/3, CellHeight/3), 0, 1, 0);
                }
            }
            else
            {
                if(GameState->MazeGoal.X == X && GameState->MazeGoal.Y == Y)
                {
                    DrawRectangle(Buffer, Min, Max - V2(CellWidth/3, CellHeight/3), 1, 1, 0);
                }
                
                if(GameState->MazeCurrent.X == X && GameState->MazeCurrent.Y == Y)
                {
                    DrawRectangle(Buffer, Min, Max - V2(CellWidth/3, CellHeight/3), 0.3f, 0, 0.3f);
                }
            }
            
        }
    }
}

extern "C" GAME_UPDATE_AND_RENDER(GameUpdateAndRender)
{    
    Assert((&Input->Controllers[0].Terminator - &Input->Controllers[0].Buttons[0]) ==
           (ArrayCount(Input->Controllers[0].Buttons)));
    Assert(sizeof(game_state) <= Memory->PermanentStorageSize);
    
    game_state *GameState = (game_state *)Memory->PermanentStorage;
    if(!Memory->IsInitialized)
    {
        InitializeArena(&GameState->Arena, Memory->PermanentStorageSize - sizeof(game_state),
                        (u8 *)Memory->PermanentStorage + sizeof(game_state));
        
        GameState->Maze = PushArray(&GameState->Arena, MAZE_WIDTH*MAZE_HEIGHT, s32);
        InitStack(&GameState->MazeLocationStack, &GameState->Arena, MAZE_WIDTH*MAZE_HEIGHT);
        
        for(s32 Y = 0;
            Y < MAZE_HEIGHT;
            ++Y)
        {
            for(s32 X = 0;
                X < MAZE_WIDTH;
                ++X)
            {
                GameState->Maze[Y*MAZE_WIDTH + X] = 0;
            }
        }
        
        s32 RandomX = RandomChoice(MAZE_WIDTH);
        s32 RandomY = RandomChoice(MAZE_HEIGHT);
        GameState->Maze[RandomY * MAZE_WIDTH + RandomX] = Cell_Visited;
        GameState->VisitedCells = 1;
        PushStack(&GameState->MazeLocationStack, MazeLocation(RandomX, RandomY));
        GameState->GeneratingMaze = true;
        GameState->MazeGoal = MazeLocation(RandomChoice(MAZE_WIDTH),
                                           RandomChoice(MAZE_HEIGHT));
        GameState->MazeCurrent = MazeLocation(RandomChoice(MAZE_WIDTH),
                                              RandomChoice(MAZE_HEIGHT));
        Memory->IsInitialized = true;
    }
    
    if(GameState->GeneratingMaze)
    {
        GenerateMaze(GameState);
    }
    else
    {
        s32 MoveX = 0;
        s32 MoveY = 0;
        
        for (s32 ControllerIndex = 0; ControllerIndex < ArrayCount(Input->Controllers); ++ControllerIndex)
        {
            game_controller_input *Controller = GetController(Input, ControllerIndex);
            if (Controller->IsAnalog)
            {
                if(Controller->StickAverageX > 0)
                {
                    MoveX = 1;
                }
                else if(Controller->StickAverageX < 0) 
                {
                    MoveX = -1;
                }
                if(Controller->StickAverageY < 0)
                {
                    MoveY = 1;
                }
                if(Controller->StickAverageY > 0)
                {
                    MoveY = -1;
                }
            }
            else
            {
                // NOTE(kstandbridge): Use digital movement tuning
                
                if(Controller->MoveUp.EndedDown)
                {
                    MoveY = -1;
                }
                
                if(Controller->MoveDown.EndedDown)
                {
                    MoveY = 1;
                }
                
                if(Controller->MoveLeft.EndedDown)
                {
                    MoveX = -1;
                }
                if(Controller->MoveRight.EndedDown)
                {
                    MoveX = 1;
                }
            }
        }
        
        if(GameState->MoveInterval < 0.0f)
        {
            GameState->MoveInterval = 0.15;
            if(MoveX == 1 && GameState->Maze[GameState->MazeCurrent.Y*MAZE_WIDTH + GameState->MazeCurrent.X] & Cell_PathE)
            {
                GameState->MazeCurrent.X += 1;
            }
            
            if(MoveX == -1 && GameState->Maze[GameState->MazeCurrent.Y*MAZE_WIDTH + GameState->MazeCurrent.X] & Cell_PathW)
            {
                GameState->MazeCurrent.X -= 1;
            }
            
            
            if(MoveY == -1 && GameState->Maze[GameState->MazeCurrent.Y*MAZE_WIDTH + GameState->MazeCurrent.X] & Cell_PathN)
            {
                GameState->MazeCurrent.Y -= 1;
            }
            
            if(MoveY == 1 && GameState->Maze[GameState->MazeCurrent.Y*MAZE_WIDTH + GameState->MazeCurrent.X] & Cell_PathS)
            {
                GameState->MazeCurrent.Y += 1;
            }
        }
        GameState->MoveInterval -= Input->dtForFrame;
    }
    
    RenderMaze(GameState, Buffer);
    
    if(GameState->MazeGoal.X == GameState->MazeCurrent.X &&
       GameState->MazeGoal.Y == GameState->MazeCurrent.Y)
    {
        // NOTE(kstandbridge): Winner!
        Memory->IsInitialized = false;
    }
}

extern "C" GAME_GET_SOUND_SAMPLES(GameGetSoundSamples)
{
    game_state *GameState = (game_state *)Memory->PermanentStorage;
    GameOutputSound(GameState, SoundBuffer, 400);
}

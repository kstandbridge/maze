#if !defined(MAZE_MATH_H)

union v2
{
    struct
    {
        r32 X, Y;
    };
    r32 E[2];
};

inline v2
V2(r32 X, r32 Y)
{
    v2 Result;
    
    Result.X = X;
    Result.Y = Y;
    
    return(Result);
}

inline v2
operator*(r32 A, v2 B)
{
    v2 Result;
    
    Result.X = A*B.X;
    Result.Y = A*B.Y;
    
    return(Result);
}

inline v2
operator*(v2 B, r32 A)
{
    v2 Result = A*B;
    
    return(Result);
}

inline v2 &
operator*=(v2 &B, r32 A)
{
    B = A * B;
    
    return(B);
}

inline v2
operator-(v2 A)
{
    v2 Result;
    
    Result.X = -A.X;
    Result.Y = -A.Y;
    
    return(Result);
}

inline v2
operator+(v2 A, v2 B)
{
    v2 Result;
    
    Result.X = A.X + B.X;
    Result.Y = A.Y + B.Y;
    
    return(Result);
}

inline v2 &
operator+=(v2 &A, v2 B)
{
    A = A + B;
    
    return(A);
}

inline v2
operator-(v2 A, v2 B)
{
    v2 Result;
    
    Result.X = A.X - B.X;
    Result.Y = A.Y - B.Y;
    
    return(Result);
}

inline r32
Square(r32 A)
{
    r32 Result = A*A;
    
    return(Result);
}

inline r32
Inner(v2 A, v2 B)
{
    r32 Result = A.X*B.X + A.Y*B.Y;
    
    return(Result);
}

inline r32
LengthSq(v2 A)
{
    r32 Result = Inner(A, A);
    
    return(Result);
}

struct rectangle2
{
    v2 Min;
    v2 Max;
};

inline v2
GetMinCorner(rectangle2 Rect)
{
    v2 Result = Rect.Min;
    return(Result);
}

inline v2
GetMaxCorner(rectangle2 Rect)
{
    v2 Result = Rect.Max;
    return(Result);
}

inline v2
GetCenter(rectangle2 Rect)
{
    v2 Result = 0.5f*(Rect.Min + Rect.Max);
    return(Result);
}

inline rectangle2
RectMinMax(v2 Min, v2 Max)
{
    rectangle2 Result;
    
    Result.Min = Min;
    Result.Max = Max;
    
    return(Result);
}

inline rectangle2
RectMinDim(v2 Min, v2 Dim)
{
    rectangle2 Result;
    
    Result.Min = Min;
    Result.Max = Min + Dim;
    
    return(Result);
}

inline rectangle2
RectCenterHalfDim(v2 Center, v2 HalfDim)
{
    rectangle2 Result;
    
    Result.Min = Center - HalfDim;
    Result.Max = Center + HalfDim;
    
    return(Result);
}

inline rectangle2
RectCenterDim(v2 Center, v2 Dim)
{
    rectangle2 Result = RectCenterHalfDim(Center, 0.5f*Dim);
    
    return(Result);
}

inline b32
IsInRectangle(rectangle2 Rectangle, v2 Test)
{
    b32 Result = ((Test.X >= Rectangle.Min.X) &&
                  (Test.Y >= Rectangle.Min.Y) &&
                  (Test.X < Rectangle.Max.X) &&
                  (Test.Y < Rectangle.Max.Y));
    
    return(Result);
}

#define MAZE_MATH_H
#endif

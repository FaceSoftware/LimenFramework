// Copyright FaceSoftware. All rights reserved.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
template<typename T>
class LIMENCORE_API TGridArray
{
public:	
	TGridArray() : Elements(nullptr)
	{
	}

	~TGridArray()
	{
	}
	
	void Reserve(FIntVector2 Size)
	{
		if (Elements != nullptr)
		{
			T* Temp = Elements;
			Elements = new T[Size.X * Size.Y];
		}
	}
	
	int32 CoordinateToIndex(const FIntVector2 Coordinate) const
	{
		check(Coordinate.X >= 0)
		check(Coordinate.Y >= 0)
		check(Coordinate.X < Size.X)
		check(Coordinate.Y < Size.Y)
		return Coordinate.Y * Size.X + Coordinate.X;
	}
	FIntVector2 IndexToCoordinate(const int32 Index) const
	{
		return FIntVector2(Index % Size.X, Index / Size.X);
	}
	
protected:
	
private:
	T* Elements;
	FIntVector2 Size;
};

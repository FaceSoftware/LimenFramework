// Copyright Face Software. All Rights Reserved.


#include "Namespaces/LimenCoreMath.h"


FLimenStringNumber::FLimenStringNumber() {}

FLimenStringNumber::FLimenStringNumber(const FString& InStr)
{
	FromString(InStr);
}

FLimenStringNumber::FLimenStringNumber(const FLimenStringNumber& Other)
{
	IntegerPart = Other.IntegerPart;
	DecimalPart = Other.DecimalPart;
	bIsNegative = Other.bIsNegative;
}

void FLimenStringNumber::FromString(const FString& InStr)
{
	FString Str = InStr.TrimStartAndEnd();

	bIsNegative = false;
	if (Str.StartsWith("-"))
	{
		bIsNegative = true;
		Str.RightChopInline(1);
	}
	else if (Str.StartsWith("+"))
	{
		Str.RightChopInline(1);
	}

	TArray<FString> Parts;
	Str.ParseIntoArray(Parts, TEXT("."));
	IntegerPart = Parts.Num() > 0 ? Parts[0] : TEXT("0");
	DecimalPart = Parts.Num() > 1 ? Parts[1] : TEXT("0");

	Normalize();
}

void FLimenStringNumber::Normalize()
{
	IntegerPart = TrimLeadingZeros(IntegerPart);
	DecimalPart = TrimTrailingZeros(DecimalPart);

	if (IntegerPart.IsEmpty()) IntegerPart = TEXT("0");
	if (DecimalPart.IsEmpty()) DecimalPart = TEXT("0");

	if (IntegerPart == TEXT("0") && DecimalPart == TEXT("0"))
	{
		bIsNegative = false;
	}
}

FString FLimenStringNumber::ToString() const
{
	FString Str = bIsNegative ? TEXT("-") : TEXT("");
	Str += IntegerPart;
	if (!DecimalPart.IsEmpty() && DecimalPart != TEXT("0"))
	{
		Str += TEXT(".") + DecimalPart;
	}
	return Str;
}

FString FLimenStringNumber::TrimLeadingZeros(const FString& Str)
{
	int32 i = 0;
	while (i < Str.Len() - 1 && Str[i] == '0') ++i;
	return Str.Mid(i);
}

FString FLimenStringNumber::TrimTrailingZeros(const FString& Str)
{
	int32 i = Str.Len();
	while (i > 1 && Str[i - 1] == '0') --i;
	return Str.Left(i);
}

bool FLimenStringNumber::IsLessThan(const FString& A, const FString& B)
{
	if (A.Len() != B.Len())
		return A.Len() < B.Len();
	return A < B;
}

void FLimenStringNumber::AlignDecimals(FString& ADec, FString& BDec)
{
	int32 MaxLen = FMath::Max(ADec.Len(), BDec.Len());
	ADec = ADec + FString::ChrN(MaxLen - ADec.Len(), '0');
	BDec = BDec + FString::ChrN(MaxLen - BDec.Len(), '0');
}

FString FLimenStringNumber::AddStrings(const FString& A, const FString& B)
{
	FString Result;
	int32 Carry = 0;
	int32 i = A.Len() - 1, j = B.Len() - 1;

	while (i >= 0 || j >= 0 || Carry)
	{
		int32 DigitA = i >= 0 ? A[i--] - '0' : 0;
		int32 DigitB = j >= 0 ? B[j--] - '0' : 0;
		int32 Sum = DigitA + DigitB + Carry;
		Result.InsertAt(0, TCHAR('0' + (Sum % 10)));
		Carry = Sum / 10;
	}

	return Result;
}

FString FLimenStringNumber::SubtractStrings(const FString& A, const FString& B)
{
	FString Result;
	int32 Borrow = 0;
	int32 i = A.Len() - 1, j = B.Len() - 1;

	while (i >= 0)
	{
		int32 DigitA = A[i--] - '0';
		int32 DigitB = j >= 0 ? B[j--] - '0' : 0;
		int32 Sub = DigitA - DigitB - Borrow;

		if (Sub < 0)
		{
			Sub += 10;
			Borrow = 1;
		}
		else
		{
			Borrow = 0;
		}

		Result.InsertAt(0, TCHAR('0' + Sub));
	}

	return TrimLeadingZeros(Result);
}

FString FLimenStringNumber::MultiplyStrings(const FString& A, const FString& B)
{
	TArray<int32> Result;
	Result.AddZeroed(A.Len() + B.Len());

	for (int32 i = A.Len() - 1; i >= 0; --i)
	{
		for (int32 j = B.Len() - 1; j >= 0; --j)
		{
			int32 Mul = (A[i] - '0') * (B[j] - '0');
			int32 p1 = i + j, p2 = i + j + 1;
			int32 Sum = Mul + Result[p2];

			Result[p2] = Sum % 10;
			Result[p1] += Sum / 10;
		}
	}

	FString Out;
	for (int32 Num : Result)
	{
		Out += FString::FromInt(Num);
	}

	return TrimLeadingZeros(Out);
}

FString FLimenStringNumber::DivideStrings(const FString& A, const FString& B, int32 Precision)
{
	if (B == "0")
		return "NaN";

	FString Dividend = A + FString::ChrN(Precision, '0'); // shift for decimal
	FString Result;
	FString Current;

	for (int32 i = 0; i < Dividend.Len(); ++i)
	{
		Current += Dividend[i];

		Current = TrimLeadingZeros(Current);
		int32 Count = 0;

		while (!IsLessThan(Current, B))
		{
			Current = SubtractStrings(Current, B);
			++Count;
		}

		Result += FString::FromInt(Count);
	}

	if (Result.Len() <= Precision)
	{
		Result = "0." + FString::ChrN(Precision - Result.Len(), '0') + Result;
	}
	else
	{
		Result.InsertAt(Result.Len() - Precision, '.');
	}

	return TrimTrailingZeros(Result);
}

FLimenStringNumber FLimenStringNumber::operator+(const FLimenStringNumber& Other) const
{
	// Same sign? Just add and carry the sign.
	if (bIsNegative == Other.bIsNegative)
	{
		// [existing same-sign addition code here…]
	}

	// Mixed signs: compute |A| and |B|
	FLimenStringNumber AbsA = *this;  AbsA.bIsNegative = false;
	FLimenStringNumber AbsB = Other;  AbsB.bIsNegative = false;

	// Decide which magnitude is larger
	bool bALarger = AbsA.IntegerPart.Len() > AbsB.IntegerPart.Len() ||
					(AbsA.IntegerPart.Len() == AbsB.IntegerPart.Len() && AbsA.IntegerPart > AbsB.IntegerPart) ||
					(AbsA.IntegerPart == AbsB.IntegerPart && AbsA.DecimalPart.Len() > AbsB.DecimalPart.Len()) ||
					(AbsA.IntegerPart == AbsB.IntegerPart && AbsA.DecimalPart == AbsB.DecimalPart);

	FLimenStringNumber Result;
	if (bALarger)
	{
		// |A| > |B|  → result magnitude = |A| - |B|, sign = sign of A
		Result = AbsA;
		Result.IntegerPart = SubtractStrings(AbsA.IntegerPart, AbsB.IntegerPart);               // integer part
		// subtract decimals with borrow if needed… or reuse existing Subtraction with aligned decimals
		// (you can call a helper that handles both integer+decimal subtraction)
		Result.bIsNegative = bIsNegative;
	}
	else
	{
		// |B| >= |A| → result magnitude = |B| - |A|, sign = sign of Other
		Result = AbsB;
		Result.IntegerPart = SubtractStrings(AbsB.IntegerPart, AbsA.IntegerPart);
		Result.bIsNegative = Other.bIsNegative;
	}

	Result.Normalize();
	return Result;
}

FLimenStringNumber FLimenStringNumber::operator-(const FLimenStringNumber& Other) const
{
	FLimenStringNumber NegOther = Other;
	NegOther.bIsNegative = !Other.bIsNegative;
	return *this + NegOther;
}

FLimenStringNumber FLimenStringNumber::operator*(const FLimenStringNumber& Other) const
{
	FString A = IntegerPart + DecimalPart;
	FString B = Other.IntegerPart + Other.DecimalPart;
	int32 TotalDecimals = DecimalPart.Len() + Other.DecimalPart.Len();

	FString Mul = MultiplyStrings(A, B);

	FString IntPart = Mul.Left(Mul.Len() - TotalDecimals);
	FString DecPart = Mul.Right(TotalDecimals);

	FLimenStringNumber Result;
	Result.IntegerPart = IntPart.IsEmpty() ? "0" : TrimLeadingZeros(IntPart);
	Result.DecimalPart = TrimTrailingZeros(DecPart);
	Result.bIsNegative = bIsNegative != Other.bIsNegative;
	Result.Normalize();
	return Result;
}

FLimenStringNumber FLimenStringNumber::operator/(const FLimenStringNumber& Other) const
{
	// 1) Build raw integer strings
	const FString ARaw = IntegerPart + DecimalPart;
	const FString BRaw = Other.IntegerPart + Other.DecimalPart;

	// 2) Calculate how many positions to shift the decimal
	//    (B has D₂ decimals, A has D₁ decimals)
	int32 Shift = Other.DecimalPart.Len() - DecimalPart.Len();

	// 3) Do the long division with extra precision
	//    (we’ll get a "." in the right spot for `Precision` digits)
	constexpr int32 Precision = 20;
	FString Div = DivideStrings(ARaw, BRaw, Precision);

	// 4) Apply the shift: move the decimal point SHIFT places
	int32 DotPos = Div.Find(TEXT("."), ESearchCase::IgnoreCase, ESearchDir::FromStart);
	if (DotPos == INDEX_NONE)
	{
		// no dot? treat as integer
		Div += TEXT(".");
		DotPos = Div.Len() - 1;
	}

	int32 NewDotPos = DotPos + Shift;
	// clamp within [0..len]
	NewDotPos = FMath::Clamp(NewDotPos, 0, Div.Len() - 1);

	// remove old dot and re-insert
	Div.RemoveAt(DotPos, 1);
	Div.InsertAt(NewDotPos, TEXT("."));

	// 5) Build result
	FLimenStringNumber Result(Div);
	Result.bIsNegative = bIsNegative != Other.bIsNegative;
	Result.Normalize();
	return Result;
}

bool FLimenStringNumber::operator==(const FLimenStringNumber& Other) const
{
	return bIsNegative == Other.bIsNegative &&
		   IntegerPart == Other.IntegerPart &&
		   DecimalPart == Other.DecimalPart ;
}

bool FLimenStringNumber::operator!=(const FLimenStringNumber& Other) const
{
	return !(*this == Other);
}

bool FLimenStringNumber::operator<(const FLimenStringNumber& Other) const
{
	// Handle sign
	if (bIsNegative != Other.bIsNegative)
		return bIsNegative;

	// Same sign: compare magnitude
	bool bBothNegative = bIsNegative;
	// compare integer lengths
	if (IntegerPart.Len() != Other.IntegerPart.Len())
		return bBothNegative
			? IntegerPart.Len() > Other.IntegerPart.Len()
			: IntegerPart.Len() < Other.IntegerPart.Len();

	// integer parts equal length
	if (IntegerPart != Other.IntegerPart)
		return bBothNegative
			? IntegerPart > Other.IntegerPart
			: IntegerPart < Other.IntegerPart;

	// compare decimals
	FString DecA = DecimalPart, DecB = Other.DecimalPart;
	AlignDecimals(DecA, DecB);
	if (DecA != DecB)
		return bBothNegative
			? DecA > DecB
			: DecA < DecB;

	return false; // equal
}

bool FLimenStringNumber::operator>(const FLimenStringNumber& Other) const
{
	return Other < *this;
}

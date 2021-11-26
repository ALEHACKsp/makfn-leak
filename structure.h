#pragma once
#include "stdafx.h"
#include "xor.h"
#include <Windows.h>
#include <psapi.h>
#include <rpcndr.h>
#include "Basic.h"

uintptr_t GFnBase, GFnSize = 0;

uintptr_t GOffset_OGI = 0;
uintptr_t GOffset_LocalPlayers = 0;
uintptr_t GOffset_PlayerController = 0;
uintptr_t GOffset_MyHUD = 0;
uintptr_t GOffset_Canvas = 0;
uintptr_t GOffset_Font = 0;
#define M_PI		3.14159265358979323846264338327950288419716939937510582f
#define E(str) _xor_(str).c_str()
unsigned char* g_pSpoofGadget = 0;
#define SPOOFER_MODULE E("ntdll.dll")
#define GLOBAL_DEBUG_FLAG
#define GLOBAL_UNLOAD_FLAG
#ifdef GLOBAL_DEBUG_FLAG
#define DEBUG_ENABLE true
#else
#define NODPRINTF
#define DEBUG_ENABLE false
#endif

struct FHitResult
{
	char memes[0x1000];
};

#define COLLINMENU_COLOR_1 Colors::Black

ULONGLONG tStarted = 0;
ULONGLONG tEnded = 0;

bool HOOKED = true;

float AimbotKey = VK_RBUTTON;
float WeakSpotAimbotKey = VK_CAPITAL;

#define s c_str()

#define null NULL
#define DEBUG_USE_MBOX false
#define DEBUG_USE_LOGFILE true
#define DEBUG_USE_CONSOLE false
#define DEBUG_LOG_PROCESSEVENT_CALLS false


#define RVA(addr, size) ((PBYTE)(addr + *(DWORD*)(addr + ((size) - 4)) + size))

#define _ZeroMemory(x, y) (memset(x, 0, y));

#ifdef NODPRINTF
#define dprintf(x)
#else
#define dprintf printf
#endif





#define INRANGE(x,a,b)    (x >= a && x <= b) 
#define getBits( x )    (INRANGE((x&(~0x20)),'A','F') ? ((x&(~0x20)) - 'A' + 0xA) : (INRANGE(x,'0','9') ? x - '0' : 0))
#define getByte( x )    (getBits(x[0]) << 4 | getBits(x[1]))


uintptr_t GPawn;

int g_MX = 0;
int g_MY = 0;

int g_ScreenWidth = 0;
int g_ScreenHeight = 0;

bool bAimbotActivated = false;


uintptr_t FindPattern(const char* pattern)
{
	char* pat = const_cast<char*>(pattern);
	uintptr_t firstMatch = 0;
	auto b = GFnBase;
	uintptr_t rangeEnd = b + GFnSize;

	for (auto pCur = b; pCur < rangeEnd; pCur++)
	{
		if (!*pat)
			return firstMatch;

		if (*(BYTE*)pat == '\?' || *(BYTE*)pCur == getByte(pat))
		{
			if (!firstMatch)
				firstMatch = pCur;

			if (!pat[2])
				return firstMatch;

			if (*(BYTE*)pat == '\?')
				pat += 2;
			else
				pat += 3;
		}
		else
		{
			pat = const_cast<char*>(pattern);
			firstMatch = 0;
		}
	}

	return 0;
}

struct keys
{
	bool mouse[4] = {};
	bool key[256] = {};
	float mouse_wheel = 0.f;
	int16_t mouseX = 0;
	int16_t mouseY = 0;
};

keys* k;

template<class T>
struct TArray
{


public:

	friend struct FString;

	inline TArray()
	{
		Data = nullptr;
		Count = Max = 0;
	};

	inline int Num() const
	{
		return Count;
	};

	inline T& operator[](int i)
	{
		return Data[i];
	};

	inline const T& operator[](int i) const
	{
		return Data[i];
	};

	inline bool IsValid() const
	{
		return Data != nullptr;
	}

	inline bool IsValidIndex(int i) const
	{
		return i < Num();
	}

	void Clear()
	{
		Data = nullptr;
		Count = Max = 0;
	};

	inline void Add(T InputData)
	{
		Data = (T*)realloc(Data, sizeof(T) * (Count + 1));
		Data[Count++] = InputData;
		Max = Count;
	};

	T* Data;
	int32_t Count;
	int32_t Max;
};

struct FString : private TArray<wchar_t>
{
public:

	inline FString()
	{
	};

	FString(const wchar_t* other)
	{
		Max = Count = *other ? (int32_t)std::wcslen(other) + 1 : 0;

		if (Count)
		{
			Data = const_cast<wchar_t*>(other);
		}
	};

	inline bool IsValid() const
	{
		return Data != nullptr;
	}

	inline const wchar_t* c_str() const
	{
		return Data;
	}

	std::string ToString() const
	{

		auto length = std::wcslen(Data);

		std::string str(length, '\0');

		std::use_facet<std::ctype<wchar_t>>(std::locale()).narrow(Data, Data + length, '?', &str[0]);

		return str;
	}

};

using tGetPathName = void(__fastcall*)(void* _this, FString* fs, uint64_t zeroarg);
tGetPathName GGetPathName = 0;
void* GGetObjectClass = 0;

string WideToAnsi(const wchar_t* inWide)
{
	static char outAnsi[0x1000];

	int i = 0;
	for (; inWide[i / 2] != L'\0'; i += 2)
		outAnsi[i / 2] = ((const char*)inWide)[i];
	outAnsi[i / 2] = '\0';

	return outAnsi;
}

void __forceinline WideToAnsi(wchar_t* inWide, char* outAnsi)
{
	int i = 0;
	for (; inWide[i / 2] != L'\0'; i += 2)
		outAnsi[i / 2] = ((const char*)inWide)[i];
	outAnsi[i / 2] = '\0';
}

void __forceinline AnsiToWide(char* inAnsi, wchar_t* outWide)
{
	int i = 0;
	for (; inAnsi[i] != '\0'; i++)
		outWide[i] = (wchar_t)(inAnsi)[i];
	outWide[i] = L'\0';
}

wstring AnsiToWide(const char* inAnsi)
{
	static wchar_t outWide[0x1000];

	int i = 0;
	for (; inAnsi[i] != '\0'; i++)
		outWide[i] = (wchar_t)(inAnsi)[i];
	outWide[i] = L'\0';

	return outWide;
}

struct FVector2D
{
	float                                              X;                                                        // 0x0000(0x0004) (Edit, BlueprintVisible, ZeroConstructor, SaveGame, IsPlainOldData)
	float                                              Y;                                                        // 0x0004(0x0004) (Edit, BlueprintVisible, ZeroConstructor, SaveGame, IsPlainOldData)

	inline FVector2D()
		: X(0), Y(0)
	{ }

	inline FVector2D(float x, float y)
		: X(x),
		Y(y)
	{ }

	__forceinline FVector2D operator-(const FVector2D& V) {
		return FVector2D(X - V.X, Y - V.Y);
	}

	__forceinline FVector2D operator+(const FVector2D& V) {
		return FVector2D(X + V.X, Y + V.Y);
	}

	__forceinline FVector2D operator*(float Scale) const {
		return FVector2D(X * Scale, Y * Scale);
	}

	__forceinline FVector2D operator/(float Scale) const {
		const float RScale = 1.f / Scale;
		return FVector2D(X * RScale, Y * RScale);
	}

	__forceinline FVector2D operator+(float A) const {
		return FVector2D(X + A, Y + A);
	}

	__forceinline FVector2D operator-(float A) const {
		return FVector2D(X - A, Y - A);
	}

	__forceinline FVector2D operator*(const FVector2D& V) const {
		return FVector2D(X * V.X, Y * V.Y);
	}

	__forceinline FVector2D operator/(const FVector2D& V) const {
		return FVector2D(X / V.X, Y / V.Y);
	}

	__forceinline float operator|(const FVector2D& V) const {
		return X * V.X + Y * V.Y;
	}

	__forceinline float operator^(const FVector2D& V) const {
		return X * V.Y - Y * V.X;
	}

	__forceinline FVector2D& operator+=(const FVector2D& v) {
		(*this);
		(v);
		X += v.X;
		Y += v.Y;
		return *this;
	}

	__forceinline FVector2D& operator-=(const FVector2D& v) {
		(*this);
		(v);
		X -= v.X;
		Y -= v.Y;
		return *this;
	}

	__forceinline FVector2D& operator*=(const FVector2D& v) {
		(*this);
		(v);
		X *= v.X;
		Y *= v.Y;
		return *this;
	}

	__forceinline FVector2D& operator/=(const FVector2D& v) {
		(*this);
		(v);
		X /= v.X;
		Y /= v.Y;
		return *this;
	}

	__forceinline bool operator==(const FVector2D& src) const {
		(src);
		(*this);
		return (src.X == X) && (src.Y == Y);
	}

	__forceinline bool operator!=(const FVector2D& src) const {
		(src);
		(*this);
		return (src.X != X) || (src.Y != Y);
	}

	__forceinline float Size() const {
		return sqrt(X * X + Y * Y);
	}

	__forceinline float SizeSquared() const {
		return X * X + Y * Y;
	}

	__forceinline float Dot(const FVector2D& vOther) const {
		const FVector2D& a = *this;

		return (a.X * vOther.X + a.Y * vOther.Y);
	}

	__forceinline FVector2D Normalize() {
		FVector2D vector;
		float length = this->Size();

		if (length != 0) {
			vector.X = X / length;
			vector.Y = Y / length;
		}
		else
			vector.X = vector.Y = 0.0f;

		return vector;
	}

	__forceinline float DistanceFrom(const FVector2D& Other) const {
		const FVector2D& a = *this;
		float dx = (a.X - Other.X);
		float dy = (a.Y - Other.Y);

		return sqrt((dx * dx) + (dy * dy));
	}

};

uintptr_t GPlayerCameraManager = 0;
uintptr_t GController = 0;
uintptr_t GWorld = 0;



void cFixName(char* Name)
{
	for (int i = 0; Name[i] != '\0'; i++)
	{
		if (Name[i] == '_')
		{
			if (Name[i + 1] == '0' ||
				Name[i + 1] == '1' ||
				Name[i + 1] == '2' ||
				Name[i + 1] == '3' ||
				Name[i + 1] == '4' ||
				Name[i + 1] == '5' ||
				Name[i + 1] == '6' ||
				Name[i + 1] == '7' ||
				Name[i + 1] == '8' ||
				Name[i + 1] == '9')
				Name[i] = '\0';
		}
	}

	return;
}

void FreeObjName(__int64 address)
{
	static uintptr_t addr = 0;

	if (!addr) {
		addr = FindPattern(E("48 85 C9 0F 84 ? ? ? ? 53 48 83 EC 20 48 89 7C 24 30 48 8B D9 48 8B 3D ? ? ? ? 48 85 FF 0F 84 ? ? ? ? 48 8B 07 4C 8B 40 30 48 8D 05 ? ? ? ? 4C 3B C0"));;

		if (!addr) {
			exit(0);
		}
	}

	auto func = reinterpret_cast<__int64(__fastcall*)(__int64 a1)>(addr);

	func(address);
}


std::string fGetNameByIndex(int Index)
{
	static uintptr_t addr = 0;

	if (!addr) {
		addr = FindPattern(E("48 89 5C 24 ? 48 89 74 24 ? 55 57 41 56 48 8D AC 24 ? ? ? ? 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 85 ? ? ? ? 45 33 F6 48 8B F2 44 39 71 04 0F 85 ? ? ? ? 8B 19 0F B7 FB E8 ? ? ? ? 8B CB 48 8D 54 24"));
		if (!addr) {
			exit(0);
		}
	}

	auto fGetNameByIdx = reinterpret_cast<FString * (__fastcall*)(int*, FString*)>(addr);

	FString result;
	fGetNameByIdx(&Index, &result);

	if (result.c_str() == NULL) return (char*)"";

	auto tmp = result.ToString();

	char return_string[1024] = {};
	for (size_t i = 0; i < tmp.size(); i++)
	{
		return_string[i] += tmp[i];
	}

	FreeObjName((uintptr_t)result.c_str());

	cFixName(return_string);

	return std::string(return_string);
}






std::string GetById(int id)
{

	return fGetNameByIndex(id);
}


struct FName
{
	union
	{
		struct
		{
			int32_t ComparisonIndex;
			int32_t Number;
		};

		uint64_t CompositeComparisonValue;
	};

	inline FName()
		: ComparisonIndex(0),
		Number(0)
	{
	};

	inline FName(int32_t i)
		: ComparisonIndex(i),
		Number(0)
	{
	};

	inline std::string GetName() const
	{
		return GetById(ComparisonIndex);
	};

	inline bool operator==(const FName& other) const
	{
		return ComparisonIndex == other.ComparisonIndex;
	};
};

// Enum Engine.EFontCacheType
enum class EFontCacheType : uint8_t
{
	Offline = 0,
	Runtime = 1,
	EFontCacheType_MAX = 2
};
// Enum Engine.EFontImportCharacterSet
enum class EFontImportCharacterSet : uint8_t
{
	FontICS_Default = 0,
	FontICS_Ansi = 1,
	FontICS_Symbol = 2,
	FontICS_MAX = 3
};
// Enum SlateCore.EFontHinting
enum class EFontHinting : uint8_t
{
	Default = 0,
	Auto = 1,
	AutoLight = 2,
	Monochrome = 3,
	None = 4,
	EFontHinting_MAX = 5
};
// Enum SlateCore.EFontLoadingPolicy
enum class EFontLoadingPolicy : uint8_t
{
	LazyLoad = 0,
	Stream = 1,
	Inline = 2,
	EFontLoadingPolicy_MAX = 3
};

class UObject
{
public:
	void** Vtable;                                                   // 0x0000(0x0000) NOT AUTO-GENERATED PROPERTY
	int32_t                                            ObjectFlags;                                              // 0x0000(0x0000) NOT AUTO-GENERATED PROPERTY
	int32_t                                            InternalIndex;                                            // 0x0000(0x0000) NOT AUTO-GENERATED PROPERTY
	class UClass* Class;                                                    // 0x0000(0x0000) NOT AUTO-GENERATED PROPERTY
	FName                                              Name;                                                     // 0x0000(0x0000) NOT AUTO-GENERATED PROPERTY
	class UObject* Outer;                                                    // 0x0000(0x0000) NOT AUTO-GENERATED PROPERTY

	static inline TUObjectArray GetGlobalObjects()
	{
		static FUObjectArray* GObjects = NULL;

		if (!GObjects)
			GObjects = (FUObjectArray*)((DWORD64)GetModuleHandleW(NULL) + 0x9A39CC0);

		return GObjects->ObjObjects;
	}

	std::string GetName() const;

	std::string GetFullName() const;

	template<typename T>
	static T* FindObject(const std::string& name)
	{
		for (int i = 0; i < GetGlobalObjects().Num(); ++i)
		{
			auto object = GetGlobalObjects().GetByIndex(i);

			if (object == nullptr)
			{
				continue;
			}

			if (object->GetFullName() == name)
			{
				return static_cast<T*>(object);
			}
		}
		return nullptr;
	}

	static UClass* FindClass(const std::string& name)
	{
		return FindObject<UClass>(name);
	}

	template<typename T>
	static T* GetObjectCasted(std::size_t index)
	{
		return static_cast<T*>(GetGlobalObjects().GetByIndex(index));
	}


	static UClass* StaticClass()
	{
		static UClass* ptr = NULL;
		if (!ptr)
			ptr = UObject::FindClass(_xor_("Class CoreUObject.Object"));

		return ptr;
	}

};


template<class TEnum>
class TEnumAsByte
{
public:
	inline TEnumAsByte()
	{
	}

	inline TEnumAsByte(TEnum _value)
		: value(static_cast<uint8_t>(_value))
	{
	}

	explicit inline TEnumAsByte(int32_t _value)
		: value(static_cast<uint8_t>(_value))
	{
	}

	explicit inline TEnumAsByte(uint8_t _value)
		: value(_value)
	{
	}

	inline operator TEnum() const
	{
		return (TEnum)value;
	}

	inline TEnum GetValue() const
	{
		return (TEnum)value;
	}

private:
	uint8_t value;
};

// ScriptStruct CoreUObject.LinearColor
// 0x0010
struct FLinearColor
{
	float                                              R;                                                        // 0x0000(0x0004) (Edit, BlueprintVisible, ZeroConstructor, SaveGame, IsPlainOldData)
	float                                              G;                                                        // 0x0004(0x0004) (Edit, BlueprintVisible, ZeroConstructor, SaveGame, IsPlainOldData)
	float                                              B;                                                        // 0x0008(0x0004) (Edit, BlueprintVisible, ZeroConstructor, SaveGame, IsPlainOldData)
	float                                              A;                                                        // 0x000C(0x0004) (Edit, BlueprintVisible, ZeroConstructor, SaveGame, IsPlainOldData)

	inline FLinearColor()
		: R(0), G(0), B(0), A(0)
	{ }

	inline FLinearColor(float r, float g, float b, float a)
		: R(r),
		G(g),
		B(b),
		A(a)
	{ }

};

// ScriptStruct Engine.FontImportOptionsData
// 0x00B0
struct FFontImportOptionsData
{
	struct FString                                     FontName;                                                 // 0x0000(0x0010) (Edit, ZeroConstructor)
	float                                              Height;                                                   // 0x0010(0x0004) (Edit, ZeroConstructor, IsPlainOldData)
	unsigned char                                      bEnableAntialiasing : 1;                                  // 0x0014(0x0001) (Edit)
	unsigned char                                      bEnableBold : 1;                                          // 0x0014(0x0001) (Edit)
	unsigned char                                      bEnableItalic : 1;                                        // 0x0014(0x0001) (Edit)
	unsigned char                                      bEnableUnderline : 1;                                     // 0x0014(0x0001) (Edit)
	unsigned char                                      bAlphaOnly : 1;                                           // 0x0014(0x0001) (Edit)
	unsigned char                                      UnknownData00[0x3];                                       // 0x0015(0x0003) MISSED OFFSET
	TEnumAsByte<EFontImportCharacterSet>               CharacterSet;                                             // 0x0018(0x0001) (Edit, ZeroConstructor, IsPlainOldData)
	unsigned char                                      UnknownData01[0x7];                                       // 0x0019(0x0007) MISSED OFFSET
	struct FString                                     Chars;                                                    // 0x0020(0x0010) (Edit, ZeroConstructor)
	struct FString                                     UnicodeRange;                                             // 0x0030(0x0010) (Edit, ZeroConstructor)
	struct FString                                     CharsFilePath;                                            // 0x0040(0x0010) (Edit, ZeroConstructor)
	struct FString                                     CharsFileWildcard;                                        // 0x0050(0x0010) (Edit, ZeroConstructor)
	unsigned char                                      bCreatePrintableOnly : 1;                                 // 0x0060(0x0001) (Edit)
	unsigned char                                      bIncludeASCIIRange : 1;                                   // 0x0060(0x0001) (Edit)
	unsigned char                                      UnknownData02[0x3];                                       // 0x0061(0x0003) MISSED OFFSET
	struct FLinearColor                                ForegroundColor;                                          // 0x0064(0x0010) (Edit, ZeroConstructor, IsPlainOldData)
	unsigned char                                      bEnableDropShadow : 1;                                    // 0x0074(0x0001) (Edit)
	unsigned char                                      UnknownData03[0x3];                                       // 0x0075(0x0003) MISSED OFFSET
	int                                                TexturePageWidth;                                         // 0x0078(0x0004) (Edit, ZeroConstructor, IsPlainOldData)
	int                                                TexturePageMaxHeight;                                     // 0x007C(0x0004) (Edit, ZeroConstructor, IsPlainOldData)
	int                                                XPadding;                                                 // 0x0080(0x0004) (Edit, ZeroConstructor, IsPlainOldData)
	int                                                YPadding;                                                 // 0x0084(0x0004) (Edit, ZeroConstructor, IsPlainOldData)
	int                                                ExtendBoxTop;                                             // 0x0088(0x0004) (Edit, ZeroConstructor, IsPlainOldData)
	int                                                ExtendBoxBottom;                                          // 0x008C(0x0004) (Edit, ZeroConstructor, IsPlainOldData)
	int                                                ExtendBoxRight;                                           // 0x0090(0x0004) (Edit, ZeroConstructor, IsPlainOldData)
	int                                                ExtendBoxLeft;                                            // 0x0094(0x0004) (Edit, ZeroConstructor, IsPlainOldData)
	unsigned char                                      bEnableLegacyMode : 1;                                    // 0x0098(0x0001) (Edit)
	unsigned char                                      UnknownData04[0x3];                                       // 0x0099(0x0003) MISSED OFFSET
	int                                                Kerning;                                                  // 0x009C(0x0004) (Edit, ZeroConstructor, IsPlainOldData)
	unsigned char                                      bUseDistanceFieldAlpha : 1;                               // 0x00A0(0x0001) (Edit)
	unsigned char                                      UnknownData05[0x3];                                       // 0x00A1(0x0003) MISSED OFFSET
	int                                                DistanceFieldScaleFactor;                                 // 0x00A4(0x0004) (Edit, ZeroConstructor, IsPlainOldData)
	float                                              DistanceFieldScanRadiusScale;                             // 0x00A8(0x0004) (Edit, ZeroConstructor, IsPlainOldData)
	unsigned char                                      UnknownData06[0x4];                                       // 0x00AC(0x0004) MISSED OFFSET
};


// ScriptStruct SlateCore.FontData
// 0x0020
struct FFontData
{
	struct FString                                     FontFilename;                                             // 0x0000(0x0010) (ZeroConstructor)
	EFontHinting                                       Hinting;                                                  // 0x0010(0x0001) (ZeroConstructor, IsPlainOldData)
	EFontLoadingPolicy                                 LoadingPolicy;                                            // 0x0011(0x0001) (ZeroConstructor, IsPlainOldData)
	unsigned char                                      UnknownData00[0x2];                                       // 0x0012(0x0002) MISSED OFFSET
	int                                                SubFaceIndex;                                             // 0x0014(0x0004) (ZeroConstructor, IsPlainOldData)
	class UObject* FontFaceAsset;                                            // 0x0018(0x0008) (ZeroConstructor, IsPlainOldData)
};

// ScriptStruct SlateCore.TypefaceEntry
// 0x0028
struct FTypefaceEntry
{
	struct FName                                       Name;                                                     // 0x0000(0x0008) (ZeroConstructor, IsPlainOldData)
	struct FFontData                                   Font;                                                     // 0x0008(0x0020)
};

// ScriptStruct SlateCore.Typeface
// 0x0010
struct FTypeface
{
	TArray<struct FTypefaceEntry>                      Fonts;                                                    // 0x0000(0x0010) (ZeroConstructor)
};
// ScriptStruct SlateCore.CompositeFallbackFont
// 0x0018
struct FCompositeFallbackFont
{
	struct FTypeface                                   Typeface;                                                 // 0x0000(0x0010)
	float                                              ScalingFactor;                                            // 0x0010(0x0004) (ZeroConstructor, IsPlainOldData)
	unsigned char                                      UnknownData00[0x4];                                       // 0x0014(0x0004) MISSED OFFSET
};

// ScriptStruct SlateCore.CompositeFont
// 0x0038
struct FCompositeFont
{
	struct FTypeface                                   DefaultTypeface;                                          // 0x0000(0x0010)
	struct FCompositeFallbackFont                      FallbackTypeface;                                         // 0x0010(0x0018)
	TArray<struct FCompositeSubFont>                   SubTypefaces;                                             // 0x0028(0x0010) (ZeroConstructor)
};


class UFont : public UObject
{
public:
	unsigned char                                      UnknownData00[0x8];                                       // 0x0028(0x0008) MISSED OFFSET
	EFontCacheType                                     FontCacheType;                                            // 0x0030(0x0001) (Edit, ZeroConstructor, IsPlainOldData)
	unsigned char                                      UnknownData01[0x7];                                       // 0x0031(0x0007) MISSED OFFSET
	TArray<struct FFontCharacter>                      Characters;                                               // 0x0038(0x0010) (Edit, ZeroConstructor)
	TArray<class UTexture2D*>                          Textures;                                                 // 0x0048(0x0010) (ZeroConstructor)
	int                                                IsRemapped;                                               // 0x0058(0x0004) (ZeroConstructor, IsPlainOldData)
	float                                              EmScale;                                                  // 0x005C(0x0004) (Edit, ZeroConstructor, IsPlainOldData)
	float                                              Ascent;                                                   // 0x0060(0x0004) (Edit, ZeroConstructor, IsPlainOldData)
	float                                              Descent;                                                  // 0x0064(0x0004) (Edit, ZeroConstructor, IsPlainOldData)
	float                                              Leading;                                                  // 0x0068(0x0004) (Edit, ZeroConstructor, IsPlainOldData)
	int                                                Kerning;                                                  // 0x006C(0x0004) (Edit, ZeroConstructor, IsPlainOldData)
	struct FFontImportOptionsData                      ImportOptions;                                            // 0x0070(0x00B0) (Edit)
	int                                                NumCharacters;                                            // 0x0120(0x0004) (ZeroConstructor, Transient, IsPlainOldData)
	unsigned char                                      UnknownData02[0x4];                                       // 0x0124(0x0004) MISSED OFFSET
	TArray<int>                                        MaxCharHeight;                                            // 0x0128(0x0010) (ZeroConstructor, Transient)
	float                                              ScalingFactor;                                            // 0x0138(0x0004) (Edit, ZeroConstructor, IsPlainOldData)
	int                                                LegacyFontSize;                                           // 0x013C(0x0004) (Edit, ZeroConstructor, IsPlainOldData)
	struct FName                                       LegacyFontName;                                           // 0x0140(0x0008) (Edit, ZeroConstructor, IsPlainOldData)
	struct FCompositeFont                              CompositeFont;                                            // 0x0148(0x0038)
	unsigned char                                      UnknownData03[0x50];                                      // 0x0180(0x0050) MISSED OFFSET

	static UClass* StaticClass()
	{
		static UClass* ptr = NULL;
		if (!ptr)
			ptr = UObject::FindClass(_xor_("Class Engine.Font"));

		return ptr;
	}

};

class UField : public UObject
{
public:
	class UField* Next;                                                     // 0x0000(0x0000) NOT AUTO-GENERATED PROPERTY

	static UClass* StaticClass()
	{
		static UClass* ptr = NULL;
		if (!ptr)
			ptr = UObject::FindClass(_xor_("Class CoreUObject.Field"));

		return ptr;
	}

};

class UProperty : public UField
{
public:
	unsigned char                                      UnknownData00[0x40];                                      // 0x0030(0x0040) MISSED OFFSET
	int32_t Offset; //0x0044

	static UClass* StaticClass()
	{
		static UClass* ptr = NULL;
		if (!ptr)
			ptr = UObject::FindClass(_xor_("Class CoreUObject.Property"));

		return ptr;
	}

};;

class UStruct : public UField
{
public:
	char                                               pad_0030[0x10];                                           // 0x0000(0x0000) NOT AUTO-GENERATED PROPERTY
	class UStruct* SuperField;                                               // 0x0000(0x0000) NOT AUTO-GENERATED PROPERTY
	class UField* Children;                                                 // 0x0000(0x0000) NOT AUTO-GENERATED PROPERTY
	void* ChildProperties;                                          // 0x0000(0x0000) NOT AUTO-GENERATED PROPERTY
	int32_t                                            PropertySize;                                             // 0x0000(0x0000) NOT AUTO-GENERATED PROPERTY
	int32_t                                            MinAlignment;                                             // 0x0000(0x0000) NOT AUTO-GENERATED PROPERTY
	char                                               pad_0060[0x50];                                           // 0x0000(0x0000) NOT AUTO-GENERATED PROPERTY

	static UClass* StaticClass()
	{
		static UClass* ptr = NULL;
		if (!ptr)
			ptr = UObject::FindClass(_xor_("Class CoreUObject.Struct"));

		return ptr;
	}

};

class UClass : public UStruct
{
public:
	unsigned char                                      UnknownData00[0x188];                                     // 0x00B0(0x0188) MISSED OFFSET

	static UClass* StaticClass()
	{
		static UClass* ptr = NULL;
		if (!ptr)
			ptr = UObject::FindClass(_xor_("Class CoreUObject.Class"));

		return ptr;
	}

};

std::string UObject::GetName() const
{
	std::string name(Name.GetName());
	if (Name.Number > 0)
	{
		name += '_' + std::to_string(Name.Number);
	}

	auto pos = name.rfind('/');
	if (pos == std::string::npos)
	{
		return name;
	}

	return name.substr(pos + 1);
}

std::string UObject::GetFullName() const
{
	std::string name;

	if (Class != nullptr)
	{
		std::string temp;
		for (auto p = Outer; p; p = p->Outer)
		{
			temp = p->GetName() + "." + temp;
		}

		name = Class->GetName();
		name += " ";
		name += temp;
		name += GetName();
	}

	return name;
}

std::string GetById(int id);



template<typename ElementType, int32_t MaxTotalElements, int32_t ElementsPerChunk> class GObjsTStaticIndirectArrayThreadSafeRead
{
public:

	static int32_t Num()
	{
		return 65535;
	}

	bool IsValidIndex(int32_t index) const
	{
		return index >= 0 && index < Num() && GetById(index) != nullptr;
	}

	ElementType* const GetById(int32_t index) const
	{
		return GetItemPtr(index);
	}

private:

	ElementType* const GetItemPtr(int32_t Index) const
	{
		int32_t ChunkIndex = Index / ElementsPerChunk;
		int32_t SubIndex = Index % ElementsPerChunk;
		auto pGObjects = (uintptr_t*)*Chunks;
		if (IsBadReadPtr(pGObjects, 0x8))
			return nullptr;
		auto chunk = pGObjects[ChunkIndex];
		if (IsBadReadPtr((void*)chunk, 0x8))
			return nullptr;
		return &((ElementType*)chunk)[SubIndex];
	}

	enum
	{
		ChunkTableSize = (MaxTotalElements + ElementsPerChunk - 1) / ElementsPerChunk
	};

	ElementType*** Chunks[ChunkTableSize];
};

using TObjectEntryArray = GObjsTStaticIndirectArrayThreadSafeRead<FUObjectItem, 2 * 1024 * 1024, 0x10400>;

TObjectEntryArray* GObjects;

uintptr_t FindSpooferFromModuleBase(const char* mod)
{
	auto spooferMod = (uintptr_t)GetModuleHandleA(mod);
	spooferMod += 0x1000;
	while (true)
	{
		if (*(UINT8*)(spooferMod) == 0xFF && *(UINT8*)(spooferMod + 1) == 0x23)
			return spooferMod;
		spooferMod++;
	}
	return 0;
}

uintptr_t FindSpooferFromModule(void* mod)
{
	auto spooferMod = (uintptr_t)mod;
	spooferMod += 0x1000;
	while (true)
	{
		if (*(UINT8*)(spooferMod) == 0xFF && *(UINT8*)(spooferMod + 1) == 0x23)
			return spooferMod;
		spooferMod++;
	}
	return 0;
}

uintptr_t GOffset_bHit = 0;

bool MemoryBlocksEqual(void* b1, void* b2, UINT32 size)
{
	uintptr_t p1 = (uintptr_t)b1;
	uintptr_t p2 = (uintptr_t)b2;
	UINT32 off = 0;

	while (off != size)
	{
		if (*(UINT8*)(p1 + off) != *(UINT8*)(p2 + off))
			return false;
		off++;
	}

	return true;
}

uintptr_t TraceToModuleBaseAndGetSpoofGadget(void* func)
{
	auto ptr = (uintptr_t)func;
	auto hdrSig = E("This program cannot be run in DOS mode");

	while (true)
	{
		if (MemoryBlocksEqual((void*)ptr, hdrSig, sizeof(hdrSig) - 1))
			break;
		ptr--;
	}

	char mz[] = { 0x4D, 0x5A };

	while (true)
	{
		if (MemoryBlocksEqual((void*)ptr, mz, sizeof(mz)))
			break;
		ptr--;
	}

	// we're at module base now.

	ptr += 0x1000;

	while (true)
	{
		if (*(UINT8*)(ptr) == 0xFF && *(UINT8*)(ptr + 1) == 0x23)
			return ptr;
		ptr++;
	}

	return 0;
}


#define CHECK_VALID(x)

double mytan(double x)
{
	return (sin(x) / cos(x));
}

struct FRotator {
	float                                              Pitch;                                                    // 0x0000(0x0004) (CPF_Edit, CPF_BlueprintVisible, CPF_ZeroConstructor, CPF_SaveGame, CPF_IsPlainOldData)
	float                                              Yaw;                                                      // 0x0004(0x0004) (CPF_Edit, CPF_BlueprintVisible, CPF_ZeroConstructor, CPF_SaveGame, CPF_IsPlainOldData)
	float                                              Roll;                                                     // 0x0008(0x0004) (CPF_Edit, CPF_BlueprintVisible, CPF_ZeroConstructor, CPF_SaveGame, CPF_IsPlainOldData)

	inline FRotator()
		: Pitch(0), Yaw(0), Roll(0) {
	}

	inline FRotator(float x, float y, float z)
		: Pitch(x),
		Yaw(y),
		Roll(z) {
	}

	__forceinline FRotator operator+(const FRotator& V) {
		return FRotator(Pitch + V.Pitch, Yaw + V.Yaw, Roll + V.Roll);
	}

	__forceinline FRotator operator-(const FRotator& V) {
		return FRotator(Pitch - V.Pitch, Yaw - V.Yaw, Roll - V.Roll);
	}

	__forceinline FRotator operator*(float Scale) const {
		return FRotator(Pitch * Scale, Yaw * Scale, Roll * Scale);
	}

	__forceinline FRotator operator/(float Scale) const {
		const float RScale = 1.f / Scale;
		return FRotator(Pitch * RScale, Yaw * RScale, Roll * RScale);
	}

	__forceinline FRotator operator+(float A) const {
		return FRotator(Pitch + A, Yaw + A, Roll + A);
	}

	__forceinline FRotator operator-(float A) const {
		return FRotator(Pitch - A, Yaw - A, Roll - A);
	}

	__forceinline FRotator operator*(const FRotator& V) const {
		return FRotator(Pitch * V.Pitch, Yaw * V.Yaw, Roll * V.Roll);
	}

	__forceinline FRotator operator/(const FRotator& V) const {
		return FRotator(Pitch / V.Pitch, Yaw / V.Yaw, Roll / V.Roll);
	}

	__forceinline float operator|(const FRotator& V) const {
		return Pitch * V.Pitch + Yaw * V.Yaw + Roll * V.Roll;
	}

	__forceinline FRotator& operator+=(const FRotator& v) {
		CHECK_VALID(*this);
		CHECK_VALID(v);
		Pitch += v.Pitch;
		Yaw += v.Yaw;
		Roll += v.Roll;
		return *this;
	}

	__forceinline FRotator& operator-=(const FRotator& v) {
		CHECK_VALID(*this);
		CHECK_VALID(v);
		Pitch -= v.Pitch;
		Yaw -= v.Yaw;
		Roll -= v.Roll;
		return *this;
	}

	__forceinline FRotator& operator*=(const FRotator& v) {
		CHECK_VALID(*this);
		CHECK_VALID(v);
		Pitch *= v.Pitch;
		Yaw *= v.Yaw;
		Roll *= v.Roll;
		return *this;
	}

	__forceinline FRotator& operator/=(const FRotator& v) {
		CHECK_VALID(*this);
		CHECK_VALID(v);
		Pitch /= v.Pitch;
		Yaw /= v.Yaw;
		Roll /= v.Roll;
		return *this;
	}

	__forceinline float operator^(const FRotator& V) const {
		return Pitch * V.Yaw - Yaw * V.Pitch - Roll * V.Roll;
	}

	__forceinline bool operator==(const FRotator& src) const {
		CHECK_VALID(src);
		CHECK_VALID(*this);
		return (src.Pitch == Pitch) && (src.Yaw == Yaw) && (src.Roll == Roll);
	}

	__forceinline bool operator!=(const FRotator& src) const {
		CHECK_VALID(src);
		CHECK_VALID(*this);
		return (src.Pitch != Pitch) || (src.Yaw != Yaw) || (src.Roll != Roll);
	}

	__forceinline float Size() const {
		return sqrt(Pitch * Pitch + Yaw * Yaw + Roll * Roll);
	}


	__forceinline float SizeSquared() const {
		return Pitch * Pitch + Yaw * Yaw + Roll * Roll;
	}

	__forceinline float Dot(const FRotator& vOther) const {
		const FRotator& a = *this;

		return (a.Pitch * vOther.Pitch + a.Yaw * vOther.Yaw + a.Roll * vOther.Roll);
	}

	__forceinline float ClampAxis(float Angle) {
		// returns Angle in the range (-360,360)
		Angle = fmod(Angle, 360.f);

		if (Angle < 0.f) {
			// shift to [0,360) range
			Angle += 360.f;
		}

		return Angle;
	}

	__forceinline float NormalizeAxis(float Angle) {
		// returns Angle in the range [0,360)
		Angle = ClampAxis(Angle);

		if (Angle > 180.f) {
			// shift to (-180,180]
			Angle -= 360.f;
		}

		return Angle;
	}

	__forceinline void Normalize() {
		Pitch = NormalizeAxis(Pitch);
		Yaw = NormalizeAxis(Yaw);
		Roll = NormalizeAxis(Roll);
	}

	__forceinline FRotator GetNormalized() const {
		FRotator Rot = *this;
		Rot.Normalize();
		return Rot;
	}
};

typedef struct _D3DMATRIX {
	union {
		struct {
			float        _11, _12, _13, _14;
			float        _21, _22, _23, _24;
			float        _31, _32, _33, _34;
			float        _41, _42, _43, _44;

		};
		float m[4][4];
	};
} D3DMATRIX;

// ScriptStruct CoreUObject.Vector
// 0x000C
struct FVector
{
	float                                              X;                                                        // 0x0000(0x0004) (Edit, BlueprintVisible, ZeroConstructor, SaveGame, IsPlainOldData)
	float                                              Y;                                                        // 0x0004(0x0004) (Edit, BlueprintVisible, ZeroConstructor, SaveGame, IsPlainOldData)
	float                                              Z;                                                        // 0x0008(0x0004) (Edit, BlueprintVisible, ZeroConstructor, SaveGame, IsPlainOldData)

	inline FVector()
		: X(0), Y(0), Z(0)
	{ }

	inline FVector(float x, float y, float z)
		: X(x),
		Y(y),
		Z(z)
	{ }

	__forceinline FVector operator-(const FVector& V) {
		return FVector(X - V.X, Y - V.Y, Z - V.Z);
	}

	__forceinline FVector operator+(const FVector& V) {
		return FVector(X + V.X, Y + V.Y, Z + V.Z);
	}

	__forceinline FVector operator*(float Scale) const {
		return FVector(X * Scale, Y * Scale, Z * Scale);
	}

	__forceinline FVector operator/(float Scale) const {
		const float RScale = 1.f / Scale;
		return FVector(X * RScale, Y * RScale, Z * RScale);
	}

	__forceinline FVector operator+(float A) const {
		return FVector(X + A, Y + A, Z + A);
	}

	__forceinline FVector operator-(float A) const {
		return FVector(X - A, Y - A, Z - A);
	}

	__forceinline FVector operator*(const FVector& V) const {
		return FVector(X * V.X, Y * V.Y, Z * V.Z);
	}

	__forceinline FVector operator/(const FVector& V) const {
		return FVector(X / V.X, Y / V.Y, Z / V.Z);
	}

	__forceinline float operator|(const FVector& V) const {
		return X * V.X + Y * V.Y + Z * V.Z;
	}

	__forceinline float operator^(const FVector& V) const {
		return X * V.Y - Y * V.X - Z * V.Z;
	}

	__forceinline FVector& operator+=(const FVector& v) {
		(*this);
		(v);
		X += v.X;
		Y += v.Y;
		Z += v.Z;
		return *this;
	}

	__forceinline FVector& operator-=(const FVector& v) {
		(*this);
		(v);
		X -= v.X;
		Y -= v.Y;
		Z -= v.Z;
		return *this;
	}

	__forceinline FVector& operator*=(const FVector& v) {
		(*this);
		(v);
		X *= v.X;
		Y *= v.Y;
		Z *= v.Z;
		return *this;
	}

	__forceinline FVector& operator/=(const FVector& v) {
		(*this);
		(v);
		X /= v.X;
		Y /= v.Y;
		Z /= v.Z;
		return *this;
	}

	__forceinline bool operator==(const FVector& src) const {
		(src);
		(*this);
		return (src.X == X) && (src.Y == Y) && (src.Z == Z);
	}

	__forceinline bool operator!=(const FVector& src) const {
		(src);
		(*this);
		return (src.X != X) || (src.Y != Y) || (src.Z != Z);
	}

	__forceinline float Size() const {
		return sqrt(X * X + Y * Y + Z * Z);
	}

	__forceinline float Size2D() const {
		return sqrt(X * X + Y * Y);
	}

	__forceinline float SizeSquared() const {
		return X * X + Y * Y + Z * Z;
	}

	__forceinline float SizeSquared2D() const {
		return X * X + Y * Y;
	}

	__forceinline float Dot(const FVector& vOther) const {
		const FVector& a = *this;

		return (a.X * vOther.X + a.Y * vOther.Y + a.Z * vOther.Z);
	}

	__forceinline float DistanceFrom(const FVector& Other) const {
		const FVector& a = *this;
		float dx = (a.X - Other.X);
		float dy = (a.Y - Other.Y);
		float dz = (a.Z - Other.Z);

		return (sqrt((dx * dx) + (dy * dy) + (dz * dz)));
	}

	__forceinline FVector Normalize() {
		FVector vector;
		float length = this->Size();

		if (length != 0) {
			vector.X = X / length;
			vector.Y = Y / length;
			vector.Z = Z / length;
		}
		else
			vector.X = vector.Y = 0.0f;
		vector.Z = 1.0f;

		return vector;
	}

};

D3DMATRIX _inline MatrixMultiplication(D3DMATRIX pM1, D3DMATRIX pM2);


float GetWeaponBulletSpeed(uint64_t cwep);
FVector get_velocity(uint64_t root_comp);
FVector get_acceleration(uint64_t target);

struct FCameraCacheEntry {
	FVector Location;
	FRotator Rotation;
	float FOV;
	//0xEB0
};

static inline void _out_buffer(char character, void* buffer, size_t idx, size_t maxlen)
{
	if (idx < maxlen)
	{
		((char*)buffer)[idx] = character;
	}
}



// ScriptStruct CoreUObject.Plane
// 0x0004 (0x0010 - 0x000C)
struct alignas(16) FPlane : public FVector
{
	float                                              W;                                                        // 0x000C(0x0004) (CPF_Edit, CPF_BlueprintVisible, CPF_ZeroConstructor, CPF_SaveGame, CPF_IsPlainOldData)
};


// ScriptStruct CoreUObject.Matrix
// 0x0040
struct FMatrix
{
	struct FPlane                                      XPlane;                                                   // 0x0000(0x0010) (CPF_Edit, CPF_BlueprintVisible, CPF_SaveGame, CPF_IsPlainOldData)
	struct FPlane                                      YPlane;                                                   // 0x0010(0x0010) (CPF_Edit, CPF_BlueprintVisible, CPF_SaveGame, CPF_IsPlainOldData)
	struct FPlane                                      ZPlane;                                                   // 0x0020(0x0010) (CPF_Edit, CPF_BlueprintVisible, CPF_SaveGame, CPF_IsPlainOldData)
	struct FPlane                                      WPlane;                                                   // 0x0030(0x0010) (CPF_Edit, CPF_BlueprintVisible, CPF_SaveGame, CPF_IsPlainOldData)
};

typedef FMatrix* (*tGetBoneMatrix)(UObject*, FMatrix*, int);
tGetBoneMatrix GetBoneMatrix;

using tTraceVisibility = bool(__fastcall*)(UObject*, FVector&, FVector&, bool, bool, FVector*, FVector*, FName*, FHitResult*);

tTraceVisibility GTraceVisibilityFn = 0;

bool VisibilityCheck(UObject* mesh, FVector& TraceStart, FVector& TraceEnd, bool bTraceComplex, bool bShowTrace, FVector* HitLocation, FVector* HitNormal, FName* BoneName, FHitResult* OutHit)
{
	return GTraceVisibilityFn(mesh, TraceStart, TraceEnd, bTraceComplex, bShowTrace, HitLocation, HitNormal, BoneName, OutHit);
}

enum Bones : uint8_t
{
	Root = 0,
	attach = 1,
	pelvis = 2,
	spine_01 = 3,
	spine_02 = 4,
	Spine_03 = 5,
	spine_04 = 6,
	spine_05 = 7,
	clavicle_l = 8,
	upperarm_l = 9,
	lowerarm_l = 10,
	Hand_L = 11,
	index_metacarpal_l = 12,
	index_01_l = 13,
	index_02_l = 14,
	index_03_l = 15,
	middle_metacarpal_l = 16,
	middle_01_l = 17,
	middle_02_l = 18,
	middle_03_l = 19,
	pinky_metacarpal_l = 20,
	pinky_01_l = 21,
	pinky_02_l = 22,
	pinky_03_l = 23,
	ring_metacarpal_l = 24,
	ring_01_l = 25,
	ring_02_l = 26,
	ring_03_l = 27,
	thumb_01_l = 28,
	thumb_02_l = 29,
	thumb_03_l = 30,
	weapon_l = 31,
	lowerarm_twist_01_l = 32,
	lowerarm_twist_02_l = 33,
	upperarm_twist_01_l = 34,
	upperarm_twist_02_l = 35,
	clavicle_r = 36,
	upperarm_r = 37,
	lowerarm_r = 38,
	hand_r = 39,
	index_metacarpal_r = 40,
	index_01_r = 41,
	index_02_r = 42,
	index_03_r = 43,
	middle_metacarpal_r = 44,
	middle_01_r = 45,
	middle_02_r = 46,
	middle_03_r = 47,
	pinky_metacarpal_r = 48,
	pinky_01_r = 49,
	pinky_02_r = 50,
	pinky_03_r = 51,
	ring_metacarpal_r = 52,
	ring_01_r = 53,
	ring_02_r = 54,
	ring_03_r = 55,
	thumb_01_r = 56,
	thumb_02_r = 57,
	thumb_03_r = 58,
	weapon_r = 59,
	lowerarm_twist_01_r = 60,
	lowerarm_twist_02_r = 61,
	upperarm_twist_01_r = 62,
	upperarm_twist_02_r = 63,
	neck_01 = 64,
	neck_02 = 65,
	HEAD = 66,
	thigh_l = 67,
	calf_l = 68,
	calf_twist_01_l = 69,
	calf_twist_02_l = 70,
	foot_l = 71,
	ball_l = 72,
	thigh_twist_01_l = 73,
	thigh_r = 74,
	calf_r = 75,
	calf_twist_01_r = 76,
	calf_twist_02_r = 77,
	foot_r = 78,
	ball_r = 79,
	thigh_twist_01_r = 80,
	ik_foot_root = 81,
	ik_foot_l = 82,
	ik_foot_r = 83,
	ik_hand_root = 84,
	ik_hand_gun = 85,
	ik_hand_l = 86,
	ik_hand_r = 87,
	spine_05_weapon_r = 88,
	spine_05_weapon_r_ik_hand_gun = 89,
	spine_05_weapon_r_ik_hand_l = 90,
	spine_05_upperarm_r = 91,
	spine_05_upperarm_r_lowerarm_r = 92,
	spine_05_upperarm_r_lowerarm_r_hand_r = 93
};

float bestFOV = 0.f;
FRotator idealAngDelta;

float AimbotFOV = 90;

void AimbotBeginFrame()
{
	bestFOV = AimbotFOV;
	idealAngDelta = { 0,0,0 };
}

bool W2S(FVector inWorldLocation, FVector2D& outScreenLocation);

bool g_Menu = 1;

bool read(void* data, uint64_t address, DWORD size)
{
	if (address <= 0x10000 || address >= 0x7FFFFFFFFFF) // 0x7FFFFFFFFFF //|| address >= 0x7FFFFFFFF00
		return false;

	if (IsBadReadPtr((void*)address, (UINT_PTR)size))
		return false;

	memcpy(data, (void*)address, size);
	return true;
}

//string GetObjectFullNameA(UObject* obj);

template <typename T>
T read(uint64_t address)
{
	T tmp;
	if (read(&tmp, address, sizeof(tmp)))
		return tmp;
	else
		return T();
}

//uint32_t GetGNameID(UObject* obj)
//{
//	if (!obj)
//		return 0;
//	return read<uint32_t>((uint64_t)(obj + offsetof(UObject, UObject::name)));
//}
//
//uint64_t GetGNameID64(uint64_t obj)
//{
//	if (!obj)
//		return 0;
//	return read<uint64_t>(obj + offsetof(UObject, UObject::name));
//}

//using tGetNameFromId = uintptr_t(__fastcall*)(uint64_t* ID, void* buffer);
//tGetNameFromId GGetNameFromId = 0;
//UObject** pGEngine = 0;
//UObject* GEngine = 0;
tGetBoneMatrix GGetBoneMatrix = 0;
FVector BoneToWorld(Bones boneid, uint64_t mesh);

FVector GetBone3D(UObject* _this, int bone)
{
	return BoneToWorld((Bones)bone, (uint64_t)_this);

	//FMatrix vMatrix;
	//spoof_call(g_pSpoofGadget, GGetBoneMatrix, _this, &vMatrix, bone);
	//return vMatrix.WPlane;
}

bool B2S(UObject* _this, int bone, FVector2D& outScrLoc)
{
	FVector tmp = GetBone3D(_this, bone);
	return W2S(tmp, outScrLoc);
}

//wstring GetGNameByIdW(uint64_t id)
//{
//	if (!id || id >= 1000000)
//		return E(L"None_X0");
//
//	static wchar_t buff[0x10000];
//	_ZeroMemory(buff, sizeof(buff));
//
//	auto v47 = spoof_call(g_pSpoofGadget, GGetNameFromId, &id, (void*)buff);
//	if (IsBadReadPtr((void*)v47, 0x8))
//	{
//		//dprintf(E("Getgnbyid bad result"));
//		return E(L"None_X1");
//	}
//	if (v47 && *((DWORD*)v47 + 2))
//	{
//		//dprintf(E("Getgnbyid gud result"));
//		return *(const wchar_t**)v47;
//	}
//	else
//	{
//		//dprintf(E("Getgnbyid bad result #2"));
//		return E(L"None_X2");
//	}
//}
//
//string GetGNameByIdA(uint64_t id)
//{
//	auto str = GetGNameByIdW(id);
//	//dprintf(E("123234434"));
//	return WideToAnsi(str.c_str());
//}

//void PrintNames(int end)
//{
//	if (end == 0)
//		end = 500000;
//
//	dprintf(E("Looping through names.."));
//	for (int i = 0; i < end; i++)
//	{
//		auto name = GetGNameByIdA(i);
//		dprintf(E("[%d] -> %s"), i, name.c_str());
//	}
//	dprintf(E("Done.."));
//}

//wstring GetObjectNameW(UObject* obj);
//
//string GetObjectNameA(UObject* obj)
//{
//	auto name = GetObjectNameW(obj);
//	return WideToAnsi(name.c_str());
//}

void GetAll(UObject* _this, vector<UObject*>* memes)
{
	memes->push_back(_this);

	if (!((UStruct*)_this)->Children)
		return;

	//dprintf("2");

	int cnt = 0;
	auto child = read<UStruct*>((uint64_t)_this + offsetof(UStruct, UStruct::Children));
	//dprintf("3");
	for (; child != nullptr; child = read<UStruct*>((uintptr_t)child + offsetof(UField, Next)))
	{
		if (cnt >= 100)
			return;
		//dprintf("4X");
		if (IsBadReadPtr(child, 0x8) || IsBadReadPtr(child->Class, 0x8))
			return;
		memes->push_back(child);
		cnt++;
	}
	//dprintf("5");
}

//void PrintObjects(int max)
//{
//	dprintf(E("Looping through objects..."));
//
//	if (max == 0)
//		max = GObjects->Num();
//
//	for (int i = 0; i < max; i++)
//	{
//		auto objItem = GObjects->GetById(i);
//
//		dprintf(E(" Obj #%d"), i);
//
//		if (!objItem || !objItem->Object)
//		{
//			continue;
//		}
//
//		auto obj_x = objItem->Object;
//
//		vector<UObject*> objs;
//		GetAll(obj_x, &objs);
//
//		for (int x = 0; x < objs.size(); x++)
//		{
//			auto obj = objs[x];
//			auto name = GetObjectFullNameA(obj);
//			dprintf(E("\t %d -> 0x%p -> %s"), x, obj, name.c_str());
//		}
//	}
//	dprintf(E("Done.."));
//}

uintptr_t GetGObjects()
{
	auto ss = FindPattern(E("48 8B 05 ? ? ? ? 48 8B 0C C8 48 8B 04 D1"));
	ss = (uintptr_t)RVA(ss, 7);
	if (!ss)
		return 0;

	ss += 7;

	return (ss + 7 + *(int32_t*)(ss + 3));
}

uintptr_t ResolveRelativeReference(uintptr_t address, uint32_t offset = 0)
{
	if (address)
	{
		address += offset;

		if (*reinterpret_cast<BYTE*>(address) == 0xE9 || *reinterpret_cast<BYTE*>(address) == 0xE8)
		{
			auto displacement = *reinterpret_cast<uint32_t*>(address + 1);
			auto ret = address + displacement + 5;

			if (displacement & 0x80000000)
				ret -= 0x100000000;

			return ret;
		}
		else if (*reinterpret_cast<BYTE*>(address + 1) == 0x05)
		{
			auto displacement = *reinterpret_cast<uint32_t*>(address + 2);
			auto ret = address + displacement + 6;

			if (displacement & 0x80000000)
				ret -= 0x100000000;

			return ret;
		}
		else
		{
			auto displacement = *reinterpret_cast<uint32_t*>(address + 3);
			auto ret = address + displacement + 3 + sizeof(uint32_t);

			if (displacement & 0x80000000)
				ret -= 0x100000000;

			return ret;
		}
	}
	else
	{
		return 0;
	}
}

uint64_t SigScanSimple(uint64_t base, uint32_t size, PBYTE sig, int len)
{
	for (size_t i = 0; i < size; i++)
		if (MemoryBlocksEqual((void*)(base + i), sig, len))
			return base + i;
	return NULL;
}

UINT32 UWorldOffset = 0x9C23450;

typedef void(*tUE4ProcessEvent)(UObject*, UObject*, void*);

tUE4ProcessEvent GoPE = 0;

template<typename Fn>
inline Fn GetVFunction(const void* instance, std::size_t index)
{
	auto vtable = *reinterpret_cast<const void***>(const_cast<void*>(instance));
	return reinterpret_cast<Fn>(vtable[index]);
}

//wstring GetObjectNameW(UObject* obj)
//{
//	if (!obj)
//		return E(L"None_X4");
//
//	if (IsBadReadPtr(obj, sizeof(UObject)))
//		return E(L"None_X5");
//
//	auto id = obj->name.ComparisonIndex;
//	return GetGNameByIdW(id);
//}

uint32_t GetVtableSize(void* object)
{
	auto vtable = *(void***)(object);
	int i = 0;

	for (; vtable[i]; i++)
		__noop();

	return i;
}

void freememory(uintptr_t Ptr, int Length = 8)
{
	VirtualFree((LPVOID)Ptr, (SIZE_T)0, (DWORD)MEM_RELEASE);
}

template <typename T>
std::string GetObjectName(T Object, bool GetOuter = false)
{
	static uintptr_t GetObjectName;

	if (!GetObjectName)
		GetObjectName = FindPattern(E("48 89 5C 24 ? 48 89 74 24 ? 55 57 41 56 48 8D AC 24 ? ? ? ? 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 85 ? ? ? ? 45 33 F6 48 8B F2 44 39 71 04 0F 85 ? ? ? ? 8B 19 0F B7 FB E8 ? ? ? ? 8B CB 48 8D 54 24"));

	auto UFUNGetObjectNameByIndex = reinterpret_cast<FString * (__fastcall*)(DWORD*, FString*)>(GetObjectName);

	DWORD ObjectIndex = *(DWORD*)((PBYTE)Object + 0x18);
	if (!ObjectIndex || ObjectIndex >= MAXDWORD) return E("");

	if (!GetOuter)
	{
		FString A;
		std::string B = "";
		UFUNGetObjectNameByIndex(&ObjectIndex, &A);
		B = A.ToString();
		return B;
	}

	std::string Ret;

	UObject* Object_ = (UObject*)Object;
	for (int i = 0; Object_; Object_ = Object_->Outer, i++)
	{
		FString Result;

		UFUNGetObjectNameByIndex(&ObjectIndex, &Result);

		std::string tmp = Result.ToString();

		if (tmp.c_str() == 0) return E("");

		freememory((__int64)Result.c_str(), tmp.size() + 8);

		char return_string[1024] = {};
		for (size_t i = 0; i < tmp.size(); i++)
		{
			return_string[i] += tmp[i];
		}

		Ret = return_string + std::string(i > 0 ? "." : "") + Ret;
	}

	return Ret;
}

vector<void*>* GHookedObjects;

D3DMATRIX Matrix(FRotator rot, FVector origin = { 0, 0, 0 })
{
	float radPitch = rot.Pitch * M_PI / 180.f;
	float radYaw = rot.Yaw * M_PI / 180.f;
	float radRoll = rot.Roll * M_PI / 180.f;

	float SP = sin(radPitch);
	float CP = cos(radPitch);
	float SY = sin(radYaw);
	float CY = cos(radYaw);
	float SR = sin(radRoll);
	float CR = cos(radRoll);

	D3DMATRIX matrix;
	matrix.m[0][0] = CP * CY;
	matrix.m[0][1] = CP * SY;
	matrix.m[0][2] = SP;
	matrix.m[0][3] = 0.f;

	matrix.m[1][0] = SR * SP * CY - CR * SY;
	matrix.m[1][1] = SR * SP * SY + CR * CY;
	matrix.m[1][2] = -SR * CP;
	matrix.m[1][3] = 0.f;

	matrix.m[2][0] = -(CR * SP * CY + SR * SY);
	matrix.m[2][1] = CY * SR - CR * SP * SY;
	matrix.m[2][2] = CR * CP;
	matrix.m[2][3] = 0.f;

	matrix.m[3][0] = origin.X;
	matrix.m[3][1] = origin.Y;
	matrix.m[3][2] = origin.Z;
	matrix.m[3][3] = 1.f;

	return matrix;
}

bool IsObjectHooked(void* obj)
{
	for (auto x : *GHookedObjects)
	{
		if (x == obj)
			return true;
	}

	return false;
}

void SwapVtable(void* obj, uint32_t index, void* hook)
{
	if (!IsObjectHooked(obj))
	{
		auto currVt = *(void**)(obj);
		auto size = GetVtableSize(obj);
		dprintf(E("VT has %d functions"), size);
		auto newVt = new uintptr_t[size];
		memcpy(newVt, currVt, size * 0x8);
		newVt[index] = (uintptr_t)hook;
		*(uintptr_t**)(obj) = newVt;
		GHookedObjects->push_back(obj);
	}
	else
	{
		dprintf(E("0x%p is already hooked.."), obj);
	}
}

class UFunction : public UStruct
{
public:
	int32_t FunctionFlags; //0x0088
	int16_t RepOffset; //0x008C
	int8_t NumParms; //0x008E
	char pad_0x008F[0x1]; //0x008F
	int16_t ParmsSize; //0x0090
	int16_t ReturnValueOffset; //0x0092
	int16_t RPCId; //0x0094
	int16_t RPCResponseId; //0x0096
	class UProperty* FirstPropertyToInit; //0x0098
	UFunction* EventGraphFunction; //0x00A0
	int32_t EventGraphCallOffset; //0x00A8
	char pad_0x00AC[0x4]; //0x00AC
	void* Func; //0x00B0
};

UFunction* S_ReceiveDrawHUD()
{
	static UFunction* ass = 0;
	if (!ass)
		ass = UObject::FindObject<UFunction>(E("Function Engine.HUD.ReceiveDrawHUD"));
	return ass;
}

UObject* GHUD;
UObject* GCanvas;

UFont* GetFont()
{
	static UFont* font = 0;
	if (!font)
	{
		/*dprintf(E("GetFont init: GEngine: %s"), GetObjectName(GEngine).c_str());
		font = *(UObject**)((uintptr_t)(GEngine)+DGOffset_Font);
		dprintf(E("GetFont init: font: %s"), GetObjectName(font).c_str());
		*/
		font = UObject::FindObject<UFont>("Font Roboto.Roboto");
	}

	return font;
}

TArray<UObject*>* GActorArray = 0;

struct UCanvas_K2_DrawText_Params
{
	class UFont* RenderFont;                                               // (Parm, ZeroConstructor, IsPlainOldData)
	struct FString                                     RenderText;                                               // (Parm, ZeroConstructor)
	struct FVector2D                                   ScreenPosition;                                           // (Parm, ZeroConstructor, IsPlainOldData)
	struct FVector2D                                   Scale;                                                    // (Parm, ZeroConstructor, IsPlainOldData)
	struct FLinearColor                                RenderColor;                                              // (Parm, ZeroConstructor, IsPlainOldData)
	float                                              Kerning;                                                  // (Parm, ZeroConstructor, IsPlainOldData)
	struct FLinearColor                                ShadowColor;                                              // (Parm, ZeroConstructor, IsPlainOldData)
	struct FVector2D                                   ShadowOffset;                                             // (Parm, ZeroConstructor, IsPlainOldData)
	bool                                               bCentreX;                                                 // (Parm, ZeroConstructor, IsPlainOldData)
	bool                                               bCentreY;                                                 // (Parm, ZeroConstructor, IsPlainOldData)
	bool                                               bOutlined;                                                // (Parm, ZeroConstructor, IsPlainOldData)
	struct FLinearColor                                OutlineColor;                                             // (Parm, ZeroConstructor, IsPlainOldData)
};

void ProcessEvent(UObject* obj, class UFunction* function, void* parms)
{
	if (!function)
		return;
	auto func = GetVFunction<void(__thiscall*)(UObject*, class UFunction*, void*)>(obj, PROCESSEVENT_INDEX);

	func(obj, function, parms);
}

void Render_Toggle(FVector2D& loc_ref, const wchar_t* name, bool* on);

FVector GPawnLocation;

void K2_DrawText(UObject* _this, class UFont* RenderFont, const class FString& RenderText, const struct FVector2D& ScreenPosition, const struct FVector2D& Scale, const struct FLinearColor& RenderColor, float Kerning, const struct FLinearColor& ShadowColor, const struct FVector2D& ShadowOffset, bool bCentreX, bool bCentreY, bool bOutl, const struct FLinearColor& OutlineColor)
{
	static UFunction* fn = nullptr;

	if (!fn)
		fn = UObject::FindObject<UFunction>(E("Function Engine.Canvas.K2_DrawText"));

	UCanvas_K2_DrawText_Params params;
	params.RenderFont = RenderFont;
	params.RenderText = RenderText;
	params.ScreenPosition = ScreenPosition;
	params.Scale = Scale;
	params.RenderColor = RenderColor;
	params.Kerning = Kerning;
	params.ShadowColor = ShadowColor;
	params.ShadowOffset = ShadowOffset;
	params.bCentreX = bCentreX;
	params.bCentreY = bCentreY;
	params.bOutlined = bOutl;
	params.OutlineColor = OutlineColor;

	ProcessEvent(_this, fn, &params);
}

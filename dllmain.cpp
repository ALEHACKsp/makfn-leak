#include "stdafx.h"
#include "structure.h"


UObject* Closest;
uintptr_t MmSize;
uintptr_t CurrentTime;
UObject* CameraManager;
uintptr_t LastTimePEHookCalled;
uintptr_t MmBase;
	

FVector2D K2_StrLen(UObject* canvas, class UObject* RenderFont, const struct FString& RenderText)
{
	static UFunction* fn = 0; if (!fn) fn = UObject::FindObject<UFunction>(E("Function Engine.Canvas.K2_StrLen"));

	struct
	{
		class UObject* RenderFont;
		struct FString                 RenderText;
		struct FVector2D               ReturnValue;
	} params;

	params.RenderFont = RenderFont;
	params.RenderText = RenderText;

	ProcessEvent(canvas, fn, &params);

	return params.ReturnValue;
}


inline D3DMATRIX matrix(FRotator rot, FVector origin = FVector(0, 0, 0))
{

	float radPitch = (rot.Pitch * float(M_PI) / 180.f);
	float radYaw = (rot.Yaw * float(M_PI) / 180.f);
	float radRoll = (rot.Roll * float(M_PI) / 180.f);

	float SP = sinf(radPitch);
	float CP = cosf(radPitch);
	float SY = sinf(radYaw);
	float CY = cosf(radYaw);
	float SR = sinf(radRoll);
	float CR = cosf(radRoll);

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

FVector GetCameraLocation(UObject* _this)
{
	static UFunction* fn = 0; if (!fn) fn = UObject::FindObject<UFunction>(E("Function Engine.Controller.LineOfSightTo"));

	struct
	{
		struct FVector ReturnValue;
	} params;

	ProcessEvent(_this, fn, &params);

	return params.ReturnValue;
}

float GetFOVAngle(UObject* _this)
{
	static UFunction* fn = 0; if (!fn) fn = UObject::FindObject<UFunction>(E("Function Engine.PlayerCameraManager.GetFOVAngle"));

	struct
	{
		float ReturnValue;
	} params;

	ProcessEvent(_this, fn, &params);

	return params.ReturnValue;
}

inline FVector xWorldToScreen(FVector worldloc, FRotator camrot)
{
	FVector screenloc = FVector(0, 0, 0);
	FRotator rot = camrot;

	D3DMATRIX tempMatrix = matrix(rot);
	FVector vAxisX, vAxisY, vAxisZ;

	vAxisX = FVector(tempMatrix.m[0][0], tempMatrix.m[0][1], tempMatrix.m[0][2]);
	vAxisY = FVector(tempMatrix.m[1][0], tempMatrix.m[1][1], tempMatrix.m[1][2]);
	vAxisZ = FVector(tempMatrix.m[2][0], tempMatrix.m[2][1], tempMatrix.m[2][2]);

	FVector vDelta = worldloc - GetCameraLocation((UObject*)GCanvas);
	FVector vTransformed = FVector(vDelta.Dot(vAxisY), vDelta.Dot(vAxisZ), vDelta.Dot(vAxisX));

	float fov_angle = GetFOVAngle((UObject*)GCanvas);
	float ScreenCenterX = static_cast<float>(g_ScreenWidth) / 2.0f;
	float ScreenCenterY = static_cast<float>(g_ScreenHeight) / 2.0f;

	if (vTransformed.Z < 1.f || mytan(fov_angle * (float)M_PI / 360.f) == 0.f) return FVector(0, 0, 0);

	screenloc.X = ScreenCenterX + vTransformed.X * (ScreenCenterX / mytan(fov_angle * (float)M_PI / 360.f)) / vTransformed.Z;
	screenloc.Y = ScreenCenterY - vTransformed.Y * (ScreenCenterX / mytan(fov_angle * (float)M_PI / 360.f)) / vTransformed.Z;

	return screenloc;

}

/*
bool xWorldToScreen(FVector WorldLocation, FVector2D& outLocScreen)
{

	if (WorldLocation.Size() == 0.0f)
		return false;

	FVector2D Screenlocation;

	FRotator rot = camrot;
	D3DMATRIX tempMatrix = matrix(rot);

	auto vAxisX = FVector(tempMatrix.m[0][0], tempMatrix.m[0][1], tempMatrix.m[0][2]);
	auto vAxisY = FVector(tempMatrix.m[1][0], tempMatrix.m[1][1], tempMatrix.m[1][2]);
	auto vAxisZ = FVector(tempMatrix.m[2][0], tempMatrix.m[2][1], tempMatrix.m[2][2]);

	FVector vDelta = WorldLocation - CameraCacheL->Location;
	FVector vTransformed = FVector(vDelta.Dot(vAxisY), vDelta.Dot(vAxisZ), vDelta.Dot(vAxisX));

	if (vTransformed.Z < 1.f)
		vTransformed.Z = 1.f;

	float FovAngle = CameraCacheL->FOV;
	float ScreenCenterX = static_cast<float>(g_ScreenWidth) / 2.0f;
	float ScreenCenterY = static_cast<float>(g_ScreenHeight) / 2.0f;
	auto f = (ScreenCenterX / mytan(FovAngle * M_PI / 360.0f));

	Screenlocation.X = ScreenCenterX + vTransformed.X * f / vTransformed.Z;
	Screenlocation.Y = ScreenCenterY - vTransformed.Y * f / vTransformed.Z;

	outLocScreen = Screenlocation;

	return true;
}*/

void xDrawText(const wchar_t* str, FVector2D pos, FLinearColor clr, float box_center_offset = 0.0f)
{
	auto font = GetFont();

	auto name_w = K2_StrLen(GCanvas, (UObject*)font, str).X;

	if (box_center_offset != -1.0f)
	{
		pos.X -= name_w / 2;
		pos.X += box_center_offset;
	}
	else
	{
		pos.X -= name_w;
	}

	K2_DrawText(GCanvas, font, str, pos, FVector2D(1.0f, 1.0f), clr, 1.0f, FLinearColor(0, 0, 0, 255), FVector2D(), false, false, G::Outline, FLinearColor(0, 0, 0, 1.0f));
}

uintptr_t GOffset_Levels = 0;
uintptr_t GOffset_Actors = 0;
uintptr_t GOffset_RootComponent = 0;
uintptr_t GOffset_ComponentLocation;

UObject* SC_FortPlayerPawn()
{
	static UClass* obj = 0;
	if (!obj)
		obj = UObject::FindObject<UClass>(E("Class FortniteGame.FortPlayerPawnAthena"));
	return obj;
}

bool ProjectWorldLocationToScreen(UObject* _this, const struct FVector& WorldLocation, bool bPlayerViewportRelative, struct FVector2D* ScreenLocation)
{
	static UFunction* fn = 0; if (!fn) fn = UObject::FindObject<UFunction>(E("Function Engine.PlayerController.ProjectWorldLocationToScreen"));

	struct
	{
		struct FVector                 WorldLocation;
		struct FVector2D               ScreenLocation;
		bool                           bPlayerViewportRelative;
		bool                           ReturnValue;
	} params;

	params.WorldLocation = WorldLocation;
	params.bPlayerViewportRelative = bPlayerViewportRelative;

	ProcessEvent(_this, fn, &params);

	if (ScreenLocation != nullptr)
		*ScreenLocation = params.ScreenLocation;

	return params.ReturnValue;
}

FCameraCacheEntry* GCameraCache = nullptr;

bool xWorldToScreen(FVector WorldLocation, FVector2D& outLocScreen)
{
	auto CameraCacheL = GCameraCache;

	if (WorldLocation.Size() == 0.0f)
		return false;

	FVector2D Screenlocation;

	D3DMATRIX tempMatrix = Matrix(CameraCacheL->Rotation);

	auto vAxisX = FVector(tempMatrix.m[0][0], tempMatrix.m[0][1], tempMatrix.m[0][2]);
	auto vAxisY = FVector(tempMatrix.m[1][0], tempMatrix.m[1][1], tempMatrix.m[1][2]);
	auto vAxisZ = FVector(tempMatrix.m[2][0], tempMatrix.m[2][1], tempMatrix.m[2][2]);

	FVector vDelta = WorldLocation - CameraCacheL->Location;
	FVector vTransformed = FVector(vDelta.Dot(vAxisY), vDelta.Dot(vAxisZ), vDelta.Dot(vAxisX));

	if (vTransformed.Z < 1.f)
		vTransformed.Z = 1.f;

	float FovAngle = CameraCacheL->FOV;
	float ScreenCenterX = static_cast<float>(g_ScreenWidth) / 2.0f;
	float ScreenCenterY = static_cast<float>(g_ScreenHeight) / 2.0f;
	auto f = (ScreenCenterX / mytan(FovAngle * M_PI / 360.0f));

	Screenlocation.X = ScreenCenterX + vTransformed.X * f / vTransformed.Z;
	Screenlocation.Y = ScreenCenterY - vTransformed.Y * f / vTransformed.Z;

	outLocScreen = Screenlocation;

	return true;
}

bool W2S(FVector inWorldLocation, FVector2D& outScreenLocation)
{
	if (!G::UseEngineW2S)
		return xWorldToScreen(inWorldLocation, outScreenLocation);
	else
		return ProjectWorldLocationToScreen((UObject*)GController, inWorldLocation, false, &outScreenLocation);
}



bool Object_IsA(UObject* obj, UObject* cmp);

float GetDistanceTo(UObject* _this, class UObject* OtherActor)
{
	static UFunction* fn = 0; if (!fn) fn = UObject::FindObject<UFunction>(E("Function Engine.Actor.GetDistanceTo"));

	struct
	{
		class UObject* OtherActor;
		float                          ReturnValue;
	} params;

	params.OtherActor = OtherActor;

	ProcessEvent(_this, fn, &params);

	return params.ReturnValue;
}

int __forceinline GetDistanceMeters(FVector& location)
{
	return (int)(location.DistanceFrom(GPawnLocation) / 100);
}

int Dist(UObject* other)
{
	if (!GPawn)
		return 0;

	return (int)(GetDistanceTo((UObject*)GPawn, other) / 100);
}

bool LineOfSightTo(UObject* _this, class UObject* Other, const struct FVector& ViewPoint, bool bAlternateChecks)
{
	static UFunction* fn = 0; if (!fn) fn = UObject::FindObject<UFunction>(E("Function Engine.Controller.LineOfSightTo"));

	struct
	{
		class UObject* Other;
		struct FVector                 ViewPoint;
		bool                           bAlternateChecks;
		bool                           ReturnValue;
	} params;

	params.Other = Other;
	params.ViewPoint = ViewPoint;
	params.bAlternateChecks = bAlternateChecks;

	ProcessEvent(_this, fn, &params);

	return params.ReturnValue;
}

uintptr_t GOffset_BlockingHit;
uintptr_t GOffset_PlayerCameraManager;
uintptr_t GOffset_TeamIndex = 0;

// ScriptStruct Engine.HitResult
// 0x0088

// Enum Engine.EDrawDebugTrace
enum class EDrawDebugTrace : uint8_t
{
	EDrawDebugTrace__None = 0,
	EDrawDebugTrace__ForOneFrame = 1,
	EDrawDebugTrace__ForDuration = 2,
	EDrawDebugTrace__Persistent = 3,
	EDrawDebugTrace__EDrawDebugTrace_MAX = 4
};


// Enum Engine.ETraceTypeQuery
enum class ETraceTypeQuery : uint8_t
{
	TraceTypeQuery1 = 0,
	TraceTypeQuery2 = 1,
	TraceTypeQuery3 = 2,
	TraceTypeQuery4 = 3,
	TraceTypeQuery5 = 4,
	TraceTypeQuery6 = 5,
	TraceTypeQuery7 = 6,
	TraceTypeQuery8 = 7,
	TraceTypeQuery9 = 8,
	TraceTypeQuery10 = 9,
	TraceTypeQuery11 = 10,
	TraceTypeQuery12 = 11,
	TraceTypeQuery13 = 12,
	TraceTypeQuery14 = 13,
	TraceTypeQuery15 = 14,
	TraceTypeQuery16 = 15,
	TraceTypeQuery17 = 16,
	TraceTypeQuery18 = 17,
	TraceTypeQuery19 = 18,
	TraceTypeQuery20 = 19,
	TraceTypeQuery21 = 20,
	TraceTypeQuery22 = 21,
	TraceTypeQuery23 = 22,
	TraceTypeQuery24 = 23,
	TraceTypeQuery25 = 24,
	TraceTypeQuery26 = 25,
	TraceTypeQuery27 = 26,
	TraceTypeQuery28 = 27,
	TraceTypeQuery29 = 28,
	TraceTypeQuery30 = 29,
	TraceTypeQuery31 = 30,
	TraceTypeQuery32 = 31,
	TraceTypeQuery_MAX = 32,
	ETraceTypeQuery_MAX = 33
};

// Function Engine.KismetSystemLibrary.LineTraceSingle
struct UKismetSystemLibrary_LineTraceSingle_Params
{
	class UObject* WorldContextObject;                                       // (Parm, ZeroConstructor, IsPlainOldData)
	struct FVector                                     Start;                                                    // (ConstParm, Parm, IsPlainOldData)
	struct FVector                                     End;                                                      // (ConstParm, Parm, IsPlainOldData)
	TEnumAsByte<ETraceTypeQuery>                       TraceChannel;                                             // (Parm, ZeroConstructor, IsPlainOldData)
	bool                                               bTraceComplex;                                            // (Parm, ZeroConstructor, IsPlainOldData)
	TArray<class AActor*>                              ActorsToIgnore;                                           // (ConstParm, Parm, OutParm, ZeroConstructor, ReferenceParm)
	TEnumAsByte<EDrawDebugTrace>                       DrawDebugType;                                            // (Parm, ZeroConstructor, IsPlainOldData)
	struct FHitResult                                  OutHit;                                                   // (Parm, OutParm, IsPlainOldData)
	bool                                               bIgnoreSelf;                                              // (Parm, ZeroConstructor, IsPlainOldData)
	struct FLinearColor                                TraceColor;                                               // (Parm, IsPlainOldData)
	struct FLinearColor                                TraceHitColor;                                            // (Parm, IsPlainOldData)
	float                                              DrawTime;                                                 // (Parm, ZeroConstructor, IsPlainOldData)
	bool                                               ReturnValue;                                              // (Parm, OutParm, ZeroConstructor, ReturnParm, IsPlainOldData)
};

bool LineTraceSingle(UObject* k2, class UObject* WorldContextObject, const struct FVector& Start, const struct FVector& End, TEnumAsByte<ETraceTypeQuery> TraceChannel, bool bTraceComplex, TArray<class AActor*> ActorsToIgnore, TEnumAsByte<EDrawDebugTrace> DrawDebugType, bool bIgnoreSelf, const struct FLinearColor& TraceColor, const struct FLinearColor& TraceHitColor, float DrawTime, struct FHitResult* OutHit)
{
	static UFunction* fn = nullptr;
	if (!fn) fn = UObject::FindObject<UFunction>(E("Function Engine.KismetSystemLibrary.LineTraceSingle"));

	UKismetSystemLibrary_LineTraceSingle_Params params;
	params.WorldContextObject = WorldContextObject;
	params.Start = Start;
	params.End = End;
	params.TraceChannel = TraceChannel;
	params.bTraceComplex = bTraceComplex;
	params.ActorsToIgnore = ActorsToIgnore;
	params.DrawDebugType = DrawDebugType;
	params.bIgnoreSelf = bIgnoreSelf;
	params.TraceColor = TraceColor;
	params.TraceHitColor = TraceHitColor;
	params.DrawTime = DrawTime;

	auto flags = fn->FunctionFlags;

	ProcessEvent(k2, fn, &params);

	fn->FunctionFlags = flags;

	if (OutHit != nullptr)
		*OutHit = params.OutHit;

	return params.ReturnValue;
}


bool TraceVisibility(UObject* mesh, FVector& p1, FVector& p2)
{
	FVector hitLoc;
	FVector hitNormal;
	static FHitResult kek;
	FName boneName;
	return VisibilityCheck(mesh, p1, p2, true, false, &hitLoc, &hitNormal, &boneName, &kek);
}

bool IsVisible(UObject* actor)
{
	return LineOfSightTo((UObject*)GController, actor, FVector{ 0, 0, 0 }, true);
}

namespace Colors
{
	FLinearColor AliceBlue = { 0.941176534f, 0.972549081f, 1.000000000f, 1.000000000f };
	FLinearColor AntiqueWhite = { 0.980392218f, 0.921568692f, 0.843137324f, 1.000000000f };
	FLinearColor Aqua = { 0.000000000f, 1.000000000f, 1.000000000f, 1.000000000f };
	FLinearColor Aquamarine = { 0.498039246f, 1.000000000f, 0.831372619f, 1.000000000f };
	FLinearColor Azure = { 0.941176534f, 1.000000000f, 1.000000000f, 1.000000000f };
	FLinearColor Beige = { 0.960784376f, 0.960784376f, 0.862745166f, 1.000000000f };
	FLinearColor Bisque = { 1.000000000f, 0.894117713f, 0.768627524f, 1.000000000f };
	FLinearColor Black = { 0.000000000f, 0.000000000f, 0.000000000f, 1.000000000f };
	FLinearColor BlanchedAlmond = { 1.000000000f, 0.921568692f, 0.803921640f, 1.000000000f };
	FLinearColor Blue = { 0.000000000f, 0.000000000f, 1.000000000f, 1.000000000f };
	FLinearColor BlueViolet = { 0.541176498f, 0.168627456f, 0.886274576f, 1.000000000f };
	FLinearColor Brown = { 0.647058845f, 0.164705887f, 0.164705887f, 1.000000000f };
	FLinearColor BurlyWood = { 0.870588303f, 0.721568644f, 0.529411793f, 1.000000000f };
	FLinearColor CadetBlue = { 0.372549027f, 0.619607866f, 0.627451003f, 1.000000000f };
	FLinearColor Chartreuse = { 0.498039246f, 1.000000000f, 0.000000000f, 1.000000000f };
	FLinearColor Chocolate = { 0.823529482f, 0.411764741f, 0.117647067f, 1.000000000f };
	FLinearColor Coral = { 1.000000000f, 0.498039246f, 0.313725501f, 1.000000000f };
	FLinearColor CornflowerBlue = { 0.392156899f, 0.584313750f, 0.929411829f, 1.000000000f };
	FLinearColor Cornsilk = { 1.000000000f, 0.972549081f, 0.862745166f, 1.000000000f };
	FLinearColor Crimson = { 0.862745166f, 0.078431375f, 0.235294133f, 1.000000000f };
	FLinearColor Cyan = { 0.000000000f, 1.000000000f, 1.000000000f, 1.000000000f };
	FLinearColor DarkBlue = { 0.000000000f, 0.000000000f, 0.545098066f, 1.000000000f };
	FLinearColor DarkCyan = { 0.000000000f, 0.545098066f, 0.545098066f, 1.000000000f };
	FLinearColor DarkGoldenrod = { 0.721568644f, 0.525490224f, 0.043137256f, 1.000000000f };
	FLinearColor DarkGray = { 0.662745118f, 0.662745118f, 0.662745118f, 1.000000000f };
	FLinearColor DarkGreen = { 0.000000000f, 0.392156899f, 0.000000000f, 1.000000000f };
	FLinearColor DarkKhaki = { 0.741176486f, 0.717647076f, 0.419607878f, 1.000000000f };
	FLinearColor DarkMagenta = { 0.545098066f, 0.000000000f, 0.545098066f, 1.000000000f };
	FLinearColor DarkOliveGreen = { 0.333333343f, 0.419607878f, 0.184313729f, 1.000000000f };
	FLinearColor DarkOrange = { 1.000000000f, 0.549019635f, 0.000000000f, 1.000000000f };
	FLinearColor DarkOrchid = { 0.600000024f, 0.196078449f, 0.800000072f, 1.000000000f };
	FLinearColor DarkRed = { 0.545098066f, 0.000000000f, 0.000000000f, 1.000000000f };
	FLinearColor DarkSalmon = { 0.913725555f, 0.588235319f, 0.478431404f, 1.000000000f };
	FLinearColor DarkSeaGreen = { 0.560784340f, 0.737254918f, 0.545098066f, 1.000000000f };
	FLinearColor DarkSlateBlue = { 0.282352954f, 0.239215702f, 0.545098066f, 1.000000000f };
	FLinearColor DarkSlateGray = { 0.184313729f, 0.309803933f, 0.309803933f, 1.000000000f };
	FLinearColor DarkTurquoise = { 0.000000000f, 0.807843208f, 0.819607913f, 1.000000000f };
	FLinearColor DarkViolet = { 0.580392182f, 0.000000000f, 0.827451050f, 1.000000000f };
	FLinearColor DeepPink = { 1.000000000f, 0.078431375f, 0.576470613f, 1.000000000f };
	FLinearColor DeepSkyBlue = { 0.000000000f, 0.749019623f, 1.000000000f, 1.000000000f };
	FLinearColor DimGray = { 0.411764741f, 0.411764741f, 0.411764741f, 1.000000000f };
	FLinearColor DodgerBlue = { 0.117647067f, 0.564705908f, 1.000000000f, 1.000000000f };
	FLinearColor Firebrick = { 0.698039234f, 0.133333340f, 0.133333340f, 1.000000000f };
	FLinearColor FloralWhite = { 1.000000000f, 0.980392218f, 0.941176534f, 1.000000000f };
	FLinearColor ForestGreen = { 0.133333340f, 0.545098066f, 0.133333340f, 1.000000000f };
	FLinearColor Fuchsia = { 1.000000000f, 0.000000000f, 1.000000000f, 1.000000000f };
	FLinearColor Gainsboro = { 0.862745166f, 0.862745166f, 0.862745166f, 1.000000000f };
	FLinearColor GhostWhite = { 0.972549081f, 0.972549081f, 1.000000000f, 1.000000000f };
	FLinearColor Gold = { 1.000000000f, 0.843137324f, 0.000000000f, 1.000000000f };
	FLinearColor Goldenrod = { 0.854902029f, 0.647058845f, 0.125490203f, 1.000000000f };
	FLinearColor Gray = { 0.501960814f, 0.501960814f, 0.501960814f, 1.000000000f };
	FLinearColor Green = { 0.000000000f, 0.501960814f, 0.000000000f, 1.000000000f };
	FLinearColor GreenYellow = { 0.678431392f, 1.000000000f, 0.184313729f, 1.000000000f };
	FLinearColor Honeydew = { 0.941176534f, 1.000000000f, 0.941176534f, 1.000000000f };
	FLinearColor HotPink = { 1.000000000f, 0.411764741f, 0.705882370f, 1.000000000f };
	FLinearColor IndianRed = { 0.803921640f, 0.360784322f, 0.360784322f, 1.000000000f };
	FLinearColor Indigo = { 0.294117659f, 0.000000000f, 0.509803951f, 1.000000000f };
	FLinearColor Ivory = { 1.000000000f, 1.000000000f, 0.941176534f, 1.000000000f };
	FLinearColor Khaki = { 0.941176534f, 0.901960850f, 0.549019635f, 1.000000000f };
	FLinearColor Lavender = { 0.901960850f, 0.901960850f, 0.980392218f, 1.000000000f };
	FLinearColor LavenderBlush = { 1.000000000f, 0.941176534f, 0.960784376f, 1.000000000f };
	FLinearColor LawnGreen = { 0.486274540f, 0.988235354f, 0.000000000f, 1.000000000f };
	FLinearColor LemonChiffon = { 1.000000000f, 0.980392218f, 0.803921640f, 1.000000000f };
	FLinearColor LightBlue = { 0.678431392f, 0.847058892f, 0.901960850f, 1.000000000f };
	FLinearColor LightCoral = { 0.941176534f, 0.501960814f, 0.501960814f, 1.000000000f };
	FLinearColor LightCyan = { 0.878431439f, 1.000000000f, 1.000000000f, 1.000000000f };
	FLinearColor LightGoldenrodYellow = { 0.980392218f, 0.980392218f, 0.823529482f, 1.000000000f };
	FLinearColor LightGreen = { 0.564705908f, 0.933333397f, 0.564705908f, 1.000000000f };
	FLinearColor LightGray = { 0.827451050f, 0.827451050f, 0.827451050f, 1.000000000f };
	FLinearColor LightPink = { 1.000000000f, 0.713725507f, 0.756862819f, 1.000000000f };
	FLinearColor LightSalmon = { 1.000000000f, 0.627451003f, 0.478431404f, 1.000000000f };
	FLinearColor LightSeaGreen = { 0.125490203f, 0.698039234f, 0.666666687f, 1.000000000f };
	FLinearColor LightSkyBlue = { 0.529411793f, 0.807843208f, 0.980392218f, 1.000000000f };
	FLinearColor LightSlateGray = { 0.466666698f, 0.533333361f, 0.600000024f, 1.000000000f };
	FLinearColor LightSteelBlue = { 0.690196097f, 0.768627524f, 0.870588303f, 1.000000000f };
	FLinearColor LightYellow = { 1.000000000f, 1.000000000f, 0.878431439f, 1.000000000f };
	FLinearColor Lime = { 0.000000000f, 1.000000000f, 0.000000000f, 1.000000000f };
	FLinearColor LimeGreen = { 0.196078449f, 0.803921640f, 0.196078449f, 1.000000000f };
	FLinearColor Linen = { 0.980392218f, 0.941176534f, 0.901960850f, 1.000000000f };
	FLinearColor Magenta = { 1.000000000f, 0.000000000f, 1.000000000f, 1.000000000f };
	FLinearColor Maroon = { 0.501960814f, 0.000000000f, 0.000000000f, 1.000000000f };
	FLinearColor MediumAquamarine = { 0.400000036f, 0.803921640f, 0.666666687f, 1.000000000f };
	FLinearColor MediumBlue = { 0.000000000f, 0.000000000f, 0.803921640f, 1.000000000f };
	FLinearColor MediumOrchid = { 0.729411781f, 0.333333343f, 0.827451050f, 1.000000000f };
	FLinearColor MediumPurple = { 0.576470613f, 0.439215720f, 0.858823597f, 1.000000000f };
	FLinearColor MediumSeaGreen = { 0.235294133f, 0.701960802f, 0.443137288f, 1.000000000f };
	FLinearColor MediumSlateBlue = { 0.482352972f, 0.407843173f, 0.933333397f, 1.000000000f };
	FLinearColor MediumSpringGreen = { 0.000000000f, 0.980392218f, 0.603921592f, 1.000000000f };
	FLinearColor MediumTurquoise = { 0.282352954f, 0.819607913f, 0.800000072f, 1.000000000f };
	FLinearColor MediumVioletRed = { 0.780392230f, 0.082352944f, 0.521568656f, 1.000000000f };
	FLinearColor MidnightBlue = { 0.098039225f, 0.098039225f, 0.439215720f, 1.000000000f };
	FLinearColor MintCream = { 0.960784376f, 1.000000000f, 0.980392218f, 1.000000000f };
	FLinearColor MistyRose = { 1.000000000f, 0.894117713f, 0.882353008f, 1.000000000f };
	FLinearColor Moccasin = { 1.000000000f, 0.894117713f, 0.709803939f, 1.000000000f };
	FLinearColor NavajoWhite = { 1.000000000f, 0.870588303f, 0.678431392f, 1.000000000f };
	FLinearColor Navy = { 0.000000000f, 0.000000000f, 0.501960814f, 1.000000000f };
	FLinearColor OldLace = { 0.992156923f, 0.960784376f, 0.901960850f, 1.000000000f };
	FLinearColor Olive = { 0.501960814f, 0.501960814f, 0.000000000f, 1.000000000f };
	FLinearColor OliveDrab = { 0.419607878f, 0.556862772f, 0.137254909f, 1.000000000f };
	FLinearColor Orange = { 1.000000000f, 0.647058845f, 0.000000000f, 1.000000000f };
	FLinearColor OrangeRed = { 1.000000000f, 0.270588249f, 0.000000000f, 1.000000000f };
	FLinearColor Orchid = { 0.854902029f, 0.439215720f, 0.839215755f, 1.000000000f };
	FLinearColor PaleGoldenrod = { 0.933333397f, 0.909803987f, 0.666666687f, 1.000000000f };
	FLinearColor PaleGreen = { 0.596078455f, 0.984313786f, 0.596078455f, 1.000000000f };
	FLinearColor PaleTurquoise = { 0.686274529f, 0.933333397f, 0.933333397f, 1.000000000f };
	FLinearColor PaleVioletRed = { 0.858823597f, 0.439215720f, 0.576470613f, 1.000000000f };
	FLinearColor PapayaWhip = { 1.000000000f, 0.937254965f, 0.835294187f, 1.000000000f };
	FLinearColor PeachPuff = { 1.000000000f, 0.854902029f, 0.725490212f, 1.000000000f };
	FLinearColor Peru = { 0.803921640f, 0.521568656f, 0.247058839f, 1.000000000f };
	FLinearColor Pink = { 1.000000000f, 0.752941251f, 0.796078503f, 1.000000000f };
	FLinearColor Plum = { 0.866666734f, 0.627451003f, 0.866666734f, 1.000000000f };
	FLinearColor PowderBlue = { 0.690196097f, 0.878431439f, 0.901960850f, 1.000000000f };
	FLinearColor Purple = { 0.501960814f, 0.000000000f, 0.501960814f, 1.000000000f };
	FLinearColor Red = { 1.000000000f, 0.000000000f, 0.000000000f, 1.000000000f };
	FLinearColor RosyBrown = { 0.737254918f, 0.560784340f, 0.560784340f, 1.000000000f };
	FLinearColor RoyalBlue = { 0.254901975f, 0.411764741f, 0.882353008f, 1.000000000f };
	FLinearColor SaddleBrown = { 0.545098066f, 0.270588249f, 0.074509807f, 1.000000000f };
	FLinearColor Salmon = { 0.980392218f, 0.501960814f, 0.447058856f, 1.000000000f };
	FLinearColor SandyBrown = { 0.956862807f, 0.643137276f, 0.376470625f, 1.000000000f };
	FLinearColor SeaGreen = { 0.180392161f, 0.545098066f, 0.341176480f, 1.000000000f };
	FLinearColor SeaShell = { 1.000000000f, 0.960784376f, 0.933333397f, 1.000000000f };
	FLinearColor Sienna = { 0.627451003f, 0.321568638f, 0.176470593f, 1.000000000f };
	FLinearColor Silver = { 0.752941251f, 0.752941251f, 0.752941251f, 1.000000000f };
	FLinearColor SkyBlue = { 0.529411793f, 0.807843208f, 0.921568692f, 1.000000000f };
	FLinearColor SlateBlue = { 0.415686309f, 0.352941185f, 0.803921640f, 1.000000000f };
	FLinearColor SlateGray = { 0.439215720f, 0.501960814f, 0.564705908f, 1.000000000f };
	FLinearColor Snow = { 1.000000000f, 0.980392218f, 0.980392218f, 1.000000000f };
	FLinearColor SpringGreen = { 0.000000000f, 1.000000000f, 0.498039246f, 1.000000000f };
	FLinearColor SteelBlue = { 0.274509817f, 0.509803951f, 0.705882370f, 1.000000000f };
	FLinearColor Tan = { 0.823529482f, 0.705882370f, 0.549019635f, 1.000000000f };
	FLinearColor Teal = { 0.000000000f, 0.501960814f, 0.501960814f, 1.000000000f };
	FLinearColor Thistle = { 0.847058892f, 0.749019623f, 0.847058892f, 1.000000000f };
	FLinearColor Tomato = { 1.000000000f, 0.388235331f, 0.278431386f, 1.000000000f };
	FLinearColor Transparent = { 0.000000000f, 0.000000000f, 0.000000000f, 0.000000000f };
	FLinearColor Turquoise = { 0.250980407f, 0.878431439f, 0.815686345f, 1.000000000f };
	FLinearColor Violet = { 0.933333397f, 0.509803951f, 0.933333397f, 1.000000000f };
	FLinearColor Wheat = { 0.960784376f, 0.870588303f, 0.701960802f, 1.000000000f };
	FLinearColor White = { 1.000000000f, 1.000000000f, 1.0f, 1.000000000f };
	FLinearColor WhiteSmoke = { 0.960784376f, 0.960784376f, 0.960784376f, 1.000000000f };
	FLinearColor Yellow = { 1.000000000f, 1.000000000f, 0.000000000f, 1.000000000f };
	FLinearColor YellowGreen = { 0.603921592f, 0.803921640f, 0.196078449f, 1.000000000f };
};

// ScriptStruct CoreUObject.Quat
// 0x0010
struct alignas(16) FQuat
{
	float                                              X;                                                        // 0x0000(0x0004) (CPF_Edit, CPF_BlueprintVisible, CPF_ZeroConstructor, CPF_SaveGame, CPF_IsPlainOldData)
	float                                              Y;                                                        // 0x0004(0x0004) (CPF_Edit, CPF_BlueprintVisible, CPF_ZeroConstructor, CPF_SaveGame, CPF_IsPlainOldData)
	float                                              Z;                                                        // 0x0008(0x0004) (CPF_Edit, CPF_BlueprintVisible, CPF_ZeroConstructor, CPF_SaveGame, CPF_IsPlainOldData)
	float                                              W;                                                        // 0x000C(0x0004) (CPF_Edit, CPF_BlueprintVisible, CPF_ZeroConstructor, CPF_SaveGame, CPF_IsPlainOldData)
};

// ScriptStruct CoreUObject.Transform
// 0x0030
struct alignas(16) FTransform
{
	struct FQuat                                       Rotation;                                                 // 0x0000(0x0010) (CPF_Edit, CPF_BlueprintVisible, CPF_SaveGame, CPF_IsPlainOldData)
	struct FVector                                     Translation;                                              // 0x0010(0x000C) (CPF_Edit, CPF_BlueprintVisible, CPF_SaveGame, CPF_IsPlainOldData)
	unsigned char                                      UnknownData00[0x4];                                       // 0x001C(0x0004) MISSED OFFSET
	struct FVector                                     Scale3D;                                                  // 0x0020(0x000C) (CPF_Edit, CPF_BlueprintVisible, CPF_SaveGame, CPF_IsPlainOldData)
	unsigned char                                      UnknownData01[0x4];                                       // 0x002C(0x0004) MISSED OFFSET
};

uintptr_t GOffset_Mesh = 0;
uintptr_t GOffset_Weapon = 0;
uintptr_t GOffset_WeaponData = 0;
uintptr_t GOffset_DisplayName = 0;
uintptr_t GOffset_ViewportClient = 0;

struct UCanvas_K2_DrawLine_Params
{
	FVector2D                                   ScreenPositionA;                                          // (Parm, IsPlainOldData)
	FVector2D                                   ScreenPositionB;                                          // (Parm, IsPlainOldData)
	float                                              Thickness;                                                // (Parm, ZeroConstructor, IsPlainOldData)
	struct FLinearColor                                RenderColor;                                              // (Parm, IsPlainOldData)
};

void K2_DrawLine(UObject* _this, FVector2D ScreenPositionA, FVector2D ScreenPositionB, FLOAT Thickness, FLinearColor Color)
{
	static uintptr_t addr = NULL;
	if (!addr)
	{
		addr = FindPattern(E("40 55 48 8D 6C 24 ? 48 81 EC ? ? ? ? 4C 89 45 DF F3 0F 10 4D ?"));
		if (!addr) return;
	}

	if (!_this) return;

	auto K2_DrawLine = reinterpret_cast<VOID(__fastcall*)(PVOID, FVector2D, FVector2D, FLOAT, FLinearColor)>(addr);

	return K2_DrawLine((PVOID)_this, ScreenPositionA, ScreenPositionB, Thickness, Color);
}

//void K2_DrawLine(UObject* _this, const struct FVector2D& ScreenPositionA, const struct FVector2D& ScreenPositionB, float Thickness, const struct FLinearColor& RenderColor)
//{
//	static UFunction* fn = nullptr;
//
//	if (!fn)
//		fn = (UFunction*)FindObject(E("Function Engine.Canvas.K2_DrawLine"));
//
//	UCanvas_K2_DrawLine_Params params;
//	params.ScreenPositionA = ScreenPositionA;
//	params.ScreenPositionB = ScreenPositionB;
//	params.Thickness = Thickness;
//	params.RenderColor = RenderColor;
//
//	ProcessEvent(_this, fn, &params);
//}

//void K2_DrawLine(UObject* _this, const struct FVector2D& ScreenPositionA, const struct FVector2D& ScreenPositionB, float Thickness, const struct FLinearColor& RenderColor)
//{
//	auto a1 = ScreenPositionA;
//	a1.X += 1;
//	a1.Y += 1;
//	auto b1 = ScreenPositionB;
//	b1.X += 1;
//	b1.Y += 1;
//
//	auto a2 = ScreenPositionA;
//	a2.X -= 1;
//	a2.Y -= 1;
//	auto b2 = ScreenPositionB;
//	b2.X -= 1;
//	b2.Y -= 1;
//
//	K2_DrawLine_Internal(_this, a1, b1, Thickness, Colors::Gray);
//	K2_DrawLine_Internal(_this, ScreenPositionA, ScreenPositionB, Thickness, RenderColor);
//	K2_DrawLine_Internal(_this, a2, b2, Thickness, Colors::Gray);
//}



UFunction* FindFunction(const char* memes)
{
	return UObject::FindObject<UFunction>(memes);
}



uintptr_t GOffset_Pawn;

uintptr_t GetWorld()
{
	return *(uint64_t*)(GFnBase + UWorldOffset);
}

struct FMinimalViewInfo
{
	FCameraCacheEntry cache;
	/*struct FVector                                     Location;                                                 // 0x0000(0x000C) (Edit, BlueprintVisible, IsPlainOldData)
	struct FRotator                                    Rotation;                                                 // 0x000C(0x000C) (Edit, BlueprintVisible, IsPlainOldData)
	float                                              FOV;                                                      // 0x0018(0x0004) (Edit, BlueprintVisible, ZeroConstructor, IsPlainOldData)
	*/float                                              DesiredFOV;                                               // 0x001C(0x0004) (ZeroConstructor, Transient, IsPlainOldData)
	float                                              OrthoWidth;                                               // 0x0020(0x0004) (Edit, BlueprintVisible, ZeroConstructor, IsPlainOldData)
	float                                              OrthoNearClipPlane;                                       // 0x0024(0x0004) (Edit, BlueprintVisible, ZeroConstructor, IsPlainOldData)
	float                                              OrthoFarClipPlane;                                        // 0x0028(0x0004) (Edit, BlueprintVisible, ZeroConstructor, IsPlainOldData)
	float                                              AspectRatio;                                              // 0x002C(0x0004) (Edit, BlueprintVisible, ZeroConstructor, IsPlainOldData)
	unsigned char                                      bConstrainAspectRatio : 1;                                // 0x0030(0x0001) (Edit, BlueprintVisible)
	unsigned char                                      bUseFieldOfViewForLOD : 1;                                // 0x0030(0x0001) (Edit, BlueprintVisible)
	unsigned char                                      UnknownData00[0x3];                                       // 0x0031(0x0003) MISSED OFFSET
	/*TEnumAsByte<ECameraProjectionM>                    ProjectionMode;                                           // 0x0034(0x0001) (Edit, BlueprintVisible, ZeroConstructor, IsPlainOldData)
	unsigned char                                      UnknownData01[0x3];                                       // 0x0035(0x0003) MISSED OFFSET
	float                                              PostProcessBlendWeight;                                   // 0x0038(0x0004) (BlueprintVisible, ZeroConstructor, IsPlainOldData)
	unsigned char                                      UnknownData02[0x4];                                       // 0x003C(0x0004) MISSED OFFSET
	struct FPostProcessSettings                        PostProcessSettings;                                      // 0x0040(0x0520) (BlueprintVisible)
	struct FVector2D                                   OffCenterProjectionOffset;                                // 0x0560(0x0008) (Edit, BlueprintVisible, DisableEditOnTemplate, Transient, EditConst, IsPlainOldData)
	unsigned char                                      UnknownData03[0x8];                                       // 0x0568(0x0008) MISSED OFFSET
	*/
};

bool get_camera(FMinimalViewInfo* view, uint64_t player_camera_mgr)
{
	auto player_camera_mgr_VTable = read<uintptr_t>(player_camera_mgr);
	if (!player_camera_mgr_VTable)
		return false;
	if (IsBadCodePtr(*(FARPROC*)(player_camera_mgr_VTable + 0x670)))
		return false;

	//(*(void(__fastcall **)(__int64, void*))(player_camera_mgr_VTable + 0x630))(player_camera_mgr, view);
	auto func = (*(void(__fastcall**)(uint64_t, void*))(player_camera_mgr_VTable + 0x670));

	func(player_camera_mgr, (void*)view);

	return (view[0].cache.Location.Size() != 0 && view[0].cache.Rotation.Size() != 0);
}

void PreRender()
{
	GWorld = GetWorld();

	if (GWorld && GCanvas)
	{
		auto GameInstance = read<uint64_t>(GWorld + DGOffset_OGI);
		auto LocalPlayers = read<uint64_t>(GameInstance + DGOffset_LocalPlayers);
		auto ULocalPlayer = read<uint64_t>(LocalPlayers);
		GController = read<uint64_t>(ULocalPlayer + DGOffset_PlayerController);
		GPlayerCameraManager = read<uint64_t>(GController + DGOffset_PlayerCameraManager);
		if (GPlayerCameraManager)
		{
			static FMinimalViewInfo* view = nullptr;
			if (!view) view = (FMinimalViewInfo*)new BYTE[1024 * 50];

			if (get_camera(view, GPlayerCameraManager))
			{
				GCameraCache = &view[0].cache;
				GPawn = read<uint64_t>(GController + DGOffset_Pawn);
				GPawnLocation = GCameraCache->Location;
				GMyTeamId = GetTeamId((UObject*)GPawn);
				if (GController)
					Render();
			}
		}
	}
}

void HkProcessEvent(UObject* _this, UFunction* fn, void* parms)
{
	if (!_this || !fn)
		return GoPE(_this, (UObject*)fn, parms);

	if (fn == S_ReceiveDrawHUD())
	{
		auto canvas = read<UObject*>((uintptr_t)_this + DGOffset_Canvas);

		GHUD = _this;
		GCanvas = canvas;

		PreRender();

		K2_DrawLine(canvas, FVector2D(1920, 1080), FVector2D(50, 50), 100.0f, FLinearColor(255, 255, 255, 255));

	}

	return GoPE(_this, (UObject*)fn, parms);
}

int find_last_of(std::string _this, char c)
{
	auto last = -1;
	for (int i = 0; i < _this.length(); i++)
	{
		auto cCurrent = _this[i];
		if (cCurrent == c)
			last = i;
	}
	return last;
}

//string GetObjectFullNameA(UObject* obj)
//{
//	if (IsBadReadPtr(obj, sizeof(UObject)))
//		return E("None_X9");
//
//	if (IsBadReadPtr(obj->Class, sizeof(UClass)))
//		return E("None_X10");
//
//	auto objName = GetObjectNameA(obj);
//	auto className = GetObjectNameA(obj->Class);
//
//	std::string temp;
//	std::string name;
//	//dprintf("14");
//	int memes = 0;
//	for (auto p = obj->Outer; !IsBadReadPtr(p, 0x8); p = p->Outer)
//	{
//		memes++;
//		if (memes >= 100)
//			return E("None_X13");
//		//dprintf("14.5");
//		std::string temp2;
//		auto outerName = GetObjectNameA(p);
//		temp2 = outerName;
//		temp2.append(E("."));
//		temp2.append(temp);
//		temp = temp2;
//	}
//	//dprintf("16");
//
//	std::string shit;
//
//	shit.append(temp);
//	shit.append(objName);
//
//	auto last = find_last_of(shit, '/');
//	if (last != -1)
//		shit.push_back(last + 1);
//
//	name.append(className);
//	name.append(E(" "));
//	name.append(shit);
//
//	//name.append(" ");
//	//name.append(myitoa(last));
//
//	return name;
//}

uintptr_t GOffset_ComponentVelocity = 0;
uintptr_t GOffset_MovementComponent = 0;
uintptr_t GOffset_Acceleration = 0;

struct AFortWeapon_GetProjectileSpeed_Params
{
	float                                              ReturnValue;                                              // (Parm, OutParm, ZeroConstructor, ReturnParm, IsPlainOldData)
};

void CallUFunction(UObject* obj, UFunction* fn, void* params)
{
	if (!obj || !fn)
		return;
	static int FunctionFlags_Offset = 0x88; //tried from 0x80 - 0x8e
	/*{
		char buf[40];
		sprintf_s(buf, "trying 0x%x\r\n", FunctionFlags_Offset);
		write_debug_log(buf);
	}*/

	auto flags = *(uint32_t*)((uint64_t)fn + FunctionFlags_Offset);
	//auto flags = fn->FunctionFlags;
	//fn->FunctionFlags |= 0x400;
	*(uint32_t*)((uint64_t)fn + FunctionFlags_Offset) |= 0x400;
	ProcessEvent(obj, fn, params);
	*(uint32_t*)((uint64_t)fn + FunctionFlags_Offset) = flags;

	//fn->FunctionFlags = flags;
}

void predict_hit(UObject* enemy, uint64_t local_weapon, FVector& pos, float distance)
{
	auto weapon_speed = GetWeaponBulletSpeed(local_weapon);
	if (weapon_speed == 0.f)
		return;

	auto rc = read<uint64_t>((uintptr_t)enemy + DGOffset_RootComponent);

	if (!rc)
		return;

	float travel_time = distance / weapon_speed;
	auto velocity = get_velocity(rc);
	auto acceleration = get_acceleration((uintptr_t)enemy);
	velocity.X *= travel_time;
	velocity.Y *= travel_time;
	velocity.Z *= travel_time;
	if (acceleration.Size())
	{
		acceleration.X /= 2.f;
		acceleration.Y /= 2.f;
		acceleration.Z /= 2.f;
		acceleration.X *= pow(travel_time, 2.f);
		acceleration.Y *= pow(travel_time, 2.f);
		acceleration.Z *= pow(travel_time, 2.f);
	}

	pos += velocity + acceleration;

	double gravity = cached_bullet_gravity_scale * cached_world_gravity;

	pos.Z -= .5 * (gravity * travel_time * travel_time);
}

float GetWeaponBulletSpeed(uint64_t cwep)
{
	if (!cwep)
		return 0.f;
	//Function FortniteGame.FortWeapon.GetProjectileSpeed
	static UFunction* f = nullptr;
	if (!f)
		f = FindFunction(E("Function FortniteGame.FortWeapon.GetProjectileSpeed"));
	if (f)
	{
		AFortWeapon_GetProjectileSpeed_Params ret{};
		CallUFunction((UObject*)cwep, f, &ret);
		return ret.ReturnValue;
	}
	else
		return 0.f;
}

bool Object_IsA(UObject* obj, UObject* cmp)
{
	if (!cmp)
		return false;

	UINT i = 0;

	//dprintf(E(""));
	//dprintf(E("-> IsA %p (%s)"), cmp, GetObjectFullNameA(cmp).c_str());
	//dprintf(E(""));

	for (auto super = read<uint64_t>((uint64_t)obj + offsetof(UObject, UObject::Class)); super; super = read<uint64_t>(super + offsetof(UStruct, UStruct::SuperField)))
	{
		//dprintf(E("SF # %d -> 0x%p -> %s"), i, super, GetObjectFullNameA((UObject*)super).c_str());
		if (super == (uint64_t)cmp)
		{
			//dprintf(E("IsA: positive result"));
			return true;
		}
		i++;
	}

	//dprintf(E(""));
	//dprintf(E("-> IsA: bad result"));
	//dprintf(E(""));

	return false;
}

bool bLogFindObject = true;




UClass* SC_AHUD()
{
	static UClass* obj = 0;
	if (!obj)
		obj = UObject::FindObject<UClass>(E("Class Engine.HUD"));
	return obj;
}

using tUE4PostRender = void(*)(UObject* _this, UObject* canvas);

tUE4PostRender GoPR = 0;

FVector get_velocity(uint64_t root_comp)
{
	return read<FVector>(root_comp + DGOffset_ComponentVelocity);
	/*
	if (!actor)
		return FVector();
	static UFunction* f = nullptr;
	if (!f)
		f = FindObject<UFunction>("Function Engine.Actor.GetVelocity");
	if (f) {
		AActor_GetVelocity_Params ret{};
		CallUFunction((UObject*)actor, f, &ret);
		return ret.ReturnValue;
	}
	else
		return FVector();*/
}

FVector get_acceleration(uint64_t target)
{
	if (auto char_movement = read<uint64_t>(target + DGOffset_MovementComponent)) {
		return read<FVector>(char_movement + DGOffset_Acceleration);
	}
	else
		return { 0, 0, 0 };
}

void HkPostRender(UObject* _this, UObject* canvas)
{
	if (!HOOKED)
		return GoPR(_this, canvas);

	tStarted = GetTickCount64();
	GCanvas = canvas;
	dprintf("hooking prerender");
	//MwMenuDraw();
	PreRender();
	dprintf("hooked prerender");
	tEnded = GetTickCount64();


	auto delta = (tEnded - tStarted);

	static auto old_delta = 0;

	if (delta == 0)
	{
		delta = old_delta;
	}
	else
	{
		old_delta = delta;
	}

	if (GCanvas && G::ShowTimeConsumed)
	{
		static char time_buff[256];
		static wchar_t time_buff_wide[256];
		//sprintf(time_buff, E("PostRender: time consumed: %d"), delta);
		AnsiToWide(time_buff, time_buff_wide);
		K2_DrawText(GCanvas, GetFont(), time_buff_wide, { 30, 260 }, FVector2D(1.0f, 1.0f), Colors::LightGreen, 1.0f, FLinearColor(0, 0, 0, 255), FVector2D(), false, false, true, FLinearColor(0, 0, 0, 1.0f));
	}

	GoPR(_this, canvas);
}

void HookPE()//rendering
{
	dprintf(E("Hooking PE"));

	auto UWorld = GetWorld();

	auto GameInstance = read<uint64_t>(UWorld + DGOffset_OGI);
	auto LocalPlayers = read<uint64_t>(GameInstance + DGOffset_LocalPlayers);
	auto ULocalPlayer = read<uint64_t>(LocalPlayers);

	auto UViewportClient = read<uint64_t>(ULocalPlayer + DGOffset_ViewportClient);

	if (!UWorld || !ULocalPlayer || !UViewportClient)
	{
		dprintf(E("Bad world/localplayer/viewportshit"));
		return;
	}

	dprintf(E("ViewportClient name: %s"), GetObjectName((UObject*)UViewportClient).c_str());
	auto vpVt = *(void***)(UViewportClient);
	GoPR = (tUE4PostRender)vpVt[POSTRENDER_INDEX];
	SwapVtable((void*)UViewportClient, POSTRENDER_INDEX, HkPostRender);
	dprintf(E("Hooked the viewport client!"));
}

int g_MenuW = 510;
int g_MenuH = 480;


//uintptr_t GetGetGNameById()
//{
//	uintptr_t cs = 0;
//	int addy = 0;
//
//	cs = FindPattern(E("E8 ? ? ? ? 83 7C 24 ? ? 48 0F 45 7C 24 ? EB 0E"));
//	if (!cs)
//	{
//		dprintf(E("SS Fail (1)"));
//		cs = FindPattern(E("48 83 C3 20 E8 ? ? ? ?"));
//		addy = 4;
//	}
//	if (!cs)
//	{
//		dprintf(E("SS Fail (2)"));
//		addy = 0;
//		cs = FindPattern(E("E8 ? ? ? ? 48 8B D0 48 8D 4C 24 ? E8 ? ? ? ? 48 8B D8 E8 ? ? ? ?"));
//	}
//
//	if (!cs)
//	{
//		dprintf(E("SS Fail (3)"));
//		return 0;
//	}
//
//	cs += addy;
//
//	return cs;
//}

uintptr_t GetOffset(string propName)
{
	bLogFindObject = false;
	auto prop = UObject::FindObject<UObject>(propName.c_str());
	auto off = ((UProperty*)prop)->Offset;
	dprintf(E("Offset: %s -> 0x%X"), propName.c_str(), off);
	bLogFindObject = true;
	return off;
}

bool g_Chineese = false;
bool g_Russian = false;
bool g_Korean = false;

void drawFilledRect(const FVector2D& initial_pos, float w, float h, const FLinearColor& color);

void RegisterButtonControl(const FVector2D initial_pos, float w, float h, const FLinearColor color, int tabIndex = -1, bool* boundBool = nullptr)
{
	drawFilledRect(initial_pos, w, h, color);
	UControl bounds;
	bounds.Origin = initial_pos;
	bounds.Size = { w, h };
	if (tabIndex != -1)
	{
		if (g_Menu)
		bounds.bIsMenuTabControl = true;
		bounds.BoundMenuTabIndex = tabIndex;
	}
	else
	{
		if (g_Menu)
		bounds.BoundBool = boundBool;
		bounds.bIsMenuTabControl = false;
	}
	g_ControlBoundsList->push_back(bounds);
}


void S2(UFont* font, FVector2D sp, FLinearColor color, const wchar_t* string)
{
	if (g_Menu)
	K2_DrawText(GCanvas, font, string, sp, FVector2D(1, 1), color, 1.0f, FLinearColor(0, 0, 0, 255), FVector2D(), false, false, true, FLinearColor(0, 0, 0, 0.f));
}

FVector2D g_Clientarea;

FVector2D g_MenuInitialPos = { 400, 400 };

int g_MenuIndex = 1;

FLinearColor SkeetMenuOutline = COLLINMENU_COLOR_1;

void MenuDrawTabs()
{
	
}

FLinearColor SkeetMenuBg = { 1 , 1 , 1 , 1.000000000f };

void MenuCheckbox(FVector2D sp, const wchar_t* text, bool* shittobind)
{
	auto color = *shittobind ? Colors::Green : Colors::SlateGray;
	sp.X += 3;
	FLinearColor gayshit = { 0.06f, 0.06f, 0.06f, 1.000000000f };
	if (g_Menu)
	RegisterButtonControl(sp + g_Clientarea, 15, 15, gayshit, -1, shittobind);
	drawFilledRect(sp + g_Clientarea + 3, 9, 9, color);
	K2_DrawText(GCanvas, GetFont(), text, sp + g_Clientarea + FVector2D({ 20, -2 }), FVector2D(1.0f, 1.0f), Colors::White, 1.0f, FLinearColor(0, 0, 0, 255), FVector2D(), false, false, true, FLinearColor(0, 0, 0, 0));
}

void MenuSlider(FVector2D sp, const wchar_t* text, int* shittobind, int min, int max);

void RegisterSliderControl(FVector2D initial_pos, float w, float h, const FLinearColor color, int* boundShit, int min, int max)
{
	if (g_Menu)
	drawFilledRect(initial_pos, w, h, color);
	UControl bounds;
	initial_pos.Y -= 10;
	h += 10;
	bounds.Origin = initial_pos;
	bounds.Size = { w, h };
	bounds.BoundMenuTabIndex = 0;
	bounds.bIsMenuTabControl = false;
	bounds.pBoundRangeValue = boundShit;
	bounds.RangeValueMin = min;
	bounds.RangeValueMax = max;
	bounds.bIsRangeSlider = true;
	g_ControlBoundsList->push_back(bounds);
}

float g_SliderScale;



void MenuSlider(FVector2D sp, const wchar_t* text, int* shittobind, int min, int max)
{
	if (g_Menu)
	g_SliderScale = 462;//g_MenuW * 0.5;

	auto g_Canvas = GCanvas;
	sp.Y += 30;
	RegisterSliderControl(sp + g_Clientarea, g_SliderScale, 4, Colors::White, shittobind, min, max);
	auto sp1 = sp + g_Clientarea + FVector2D((*shittobind) * ((g_SliderScale) / (max - min)), 0);
	auto sp2 = sp1;
	sp2.Y -= 5;
	sp1.Y += 5;
	auto fMain = GetFont();

	auto textpos = g_Clientarea + sp + FVector2D({ 0, -26 });
	if (g_Menu)
	K2_DrawLine(GCanvas, sp1, sp2, 5, SkeetMenuOutline);

}	

void Render_Slider(const wchar_t* name, float minimum, float maximum, float* val, FVector2D* loc);



void drawRect(const FVector2D initial_pos, float w, float h, const FLinearColor color, float thickness = 1.f)
{
	if (g_Menu)
	K2_DrawLine(GCanvas, initial_pos, FVector2D(initial_pos.X + w, initial_pos.Y), thickness, color);
	K2_DrawLine(GCanvas, initial_pos, FVector2D(initial_pos.X, initial_pos.Y + h), thickness, color);
	K2_DrawLine(GCanvas, FVector2D(initial_pos.X + w, initial_pos.Y), FVector2D(initial_pos.X + w, initial_pos.Y + h), thickness, color);
	K2_DrawLine(GCanvas, FVector2D(initial_pos.X, initial_pos.Y + h), FVector2D(initial_pos.X + w, initial_pos.Y + h), thickness, color);
}

void drawFilledRect(const FVector2D& initial_pos, float w, float h, const FLinearColor& color)
{
	if (g_Menu)
	for (float i = 0.f; i < h; i += 1.f)
		K2_DrawLine(GCanvas, FVector2D(initial_pos.X, initial_pos.Y + i), FVector2D(initial_pos.X + w, initial_pos.Y + i), 1.f, color);
}

void K2_DrawBox(UObject* canvas, const struct FVector2D& ScreenPosition, const struct FVector2D& ScreenSize, float Thickness)
{
	static UFunction* fn = 0; if (!fn) fn = FindFunction(E("Function Engine.Canvas.K2_DrawBox"));

	struct
	{
		struct FVector2D               ScreenPosition;
		struct FVector2D               ScreenSize;
		float                          Thickness;
	} params;
	if (g_Menu)
	params.ScreenPosition = ScreenPosition;
	params.ScreenSize = ScreenSize;
	params.Thickness = Thickness;

	ProcessEvent(canvas, fn, &params);
}

void MwMenuDraw()
{
	if (g_Menu)
#define MENU_OUTLINE_THICC 2
	//drawRect(
	//	g_MenuInitialPos - MENU_OUTLINE_THICC,  // initialpos
	//	g_MenuW - 1 + MENU_OUTLINE_THICC,
	//	g_MenuH - 1 + MENU_OUTLINE_THICC,
	//	SkeetMenuOutline,
	//	MENU_OUTLINE_THICC);

	drawFilledRect(g_MenuInitialPos, g_MenuW - MENU_OUTLINE_THICC, g_MenuH - MENU_OUTLINE_THICC, SkeetMenuBg);
	MenuDrawTabs();
	MenuDrawItemsFor(g_MenuIndex);
	if (g_MX && g_MY)
	{
		if (g_Menu)
		K2_DrawBox(GCanvas, { (float)g_MX - 2, (float)g_MY - 1 }, { 4, 4 }, 4);
	}
	//dprintf("MenuDrawing5\n");
	//dprintf(E("5"));
	//MenuCheckbox({ 0, (20 * 43) - 180 }, E(L"Включить русский язык"), &g_Russian);
	//MenuCheckbox({ 0, (20 * 44) - 180 }, E(L"한국어 사용 가능"), &g_Korean);
	//MenuCheckbox({ 0 , (20 * 45) - 180 }, E(L"选中文"), &g_Chineese);
}

void PatchFuncRet0(void* fn)
{
	uint8_t patch[] = { 0x31, 0xC0, 0xC3 };
	DWORD old;
	DWORD old2;
	VirtualProtect(fn, 3, PAGE_EXECUTE_READWRITE, &old);
	memcpy(fn, patch, 3);
	VirtualProtect(fn, 3, old, &old2);
}

void PatchFwFuncs()
{
	printf(E("Patching FW funcs"));

	void* funcs[] = { EnumWindows };
	for (auto x : funcs)
		PatchFuncRet0(x);

	printf(E("Done"));
}



uintptr_t GetGEngine()
{
	auto ss = FindPattern(E("48 8B 0D ? ? ? ? 45 0F B6 C6 F2 0F 10 0D ? ? ? ? 66 0F 5A C9 48 8B 11 FF 92 ? ? ? ? "));
	if (!ss)
	{
		dprintf(E("SS Failed (1) !"));
		return ss;
	}

	return (*(int32_t*)(ss + 3) + ss + 7);
}

LRESULT HkWndProcInternal(uintptr_t unk, HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

using tWndProc = decltype(&HkWndProcInternal);

tWndProc G_oWndProc = 0;

#define GET_X_LPARAM(lp)                        ((int)(short)LOWORD(lp))
#define GET_Y_LPARAM(lp)                        ((int)(short)HIWORD(lp))

int g_MenuDragStartX = 0;
int g_MenuDragStartY = 0;

bool bDragging = 0;

void MenuProcessClick(int x, int y)
{
	if (g_Menu)
	if (!g_ControlBoundsList)
		dprintf(E("No controlbounds list"));

	if (g_ControlBoundsList && g_Menu)
	{
		//dprintf(E("g_ControlBounds list size %d"), g_ControlBoundsList->size());
		for (auto fuck = 0; fuck < g_ControlBoundsList->size(); fuck++)
		{
			auto bi = g_ControlBoundsList->at(fuck);
			if (bi.ContainsPoint({ (float)x, (float)y }))
			{
				if (bi.bIsMenuTabControl)
				{
					g_MenuIndex = bi.BoundMenuTabIndex;
				}
				else if (bi.BoundBool)
				{
					*bi.BoundBool = !*bi.BoundBool;

					if ((bi.BoundBool == &g_Russian) && *bi.BoundBool
						&& (g_Chineese || g_Korean)) // wanna enable russian but chineese is enabled
					{
						g_Korean = false;
						g_Chineese = false;
					}
					else if ((bi.BoundBool == &g_Chineese) && *bi.BoundBool && (g_Russian || g_Korean))
					{
						g_Korean = false;
						g_Russian = false;
					}
					else if ((bi.BoundBool == &g_Korean) && *bi.BoundBool && (g_Chineese || g_Russian))
					{
						g_Russian = false;
						g_Chineese = false;
					}
				}
				else if (bi.pBoundRangeValue)
				{
					auto how_far_clicked = g_MX - bi.Origin.X;
					if (how_far_clicked <= 0)
						continue;

					how_far_clicked *= (bi.RangeValueMax - bi.RangeValueMin) / (g_SliderScale);

					auto delta = how_far_clicked - *bi.pBoundRangeValue;
					auto willbe = *bi.pBoundRangeValue + delta;
					if (willbe >= bi.RangeValueMin && willbe <= bi.RangeValueMax)
						*bi.pBoundRangeValue = willbe;
				}
			}
		}
	}
}

bool IsInMenu(int x, int y)
{
	if (g_Menu)
	return (x >= g_MenuInitialPos.X) && (x <= g_MenuInitialPos.X + g_MenuW) && (y >= g_MenuInitialPos.Y) && (y <= g_MenuInitialPos.Y + g_MenuH);
}

WNDPROC G_oWndProcUnsafe = 0;

using tCallWindowProcW = decltype(&CallWindowProcW);
tCallWindowProcW fnCallWindowProcW;

LRESULT
WINAPI
myCallWindowProcW(
	_In_ WNDPROC lpPrevWndFunc,
	_In_ HWND hWnd,
	_In_ UINT Msg,
	_In_ WPARAM wParam,
	_In_ LPARAM lParam)
{
	return fnCallWindowProcW(lpPrevWndFunc, hWnd, Msg, wParam, lParam);
}

void SetIgnoreLookInput(bool bNewLookInput)
{
	static UFunction* fn = 0; if (!fn) fn = FindFunction(E("Function Engine.Controller.SetIgnoreLookInput"));

	struct
	{
		bool                           bNewLookInput;
	} params;

	params.bNewLookInput = bNewLookInput;

	ProcessEvent((UObject*)GController, fn, &params);
}

int myceilf(float num)
{
	int inum = (int)num;
	if (num == (float)inum) {
		return inum;
	}
	return inum + 1;
}

void ResetIgnoreLookInput()
{
	static UFunction* fn = 0; if (!fn) fn = fn = FindFunction(E("Function Engine.Controller.ResetIgnoreLookInput"));

	struct
	{
	} params;

	ProcessEvent((UObject*)GController, fn, &params);
}

#define M_PI 3.14159265358979323846264338327950288419716939937510582f
#define D2R(d) (d / 180.f) * M_PI
#define MAX_SEGMENTS 180

void Render_Line(FVector2D one, FVector2D two, FLinearColor color)
{
	if (g_Menu)
	K2_DrawLine(GCanvas, one, two, 1, color);
}

void Render_Circle(FVector2D pos, int r, FLinearColor color)
{
	float circum = M_PI * 2.f * r;
	int seg = myceilf(circum);
	if (g_Menu)
	if (seg > MAX_SEGMENTS) seg = MAX_SEGMENTS;

	float theta = 0.f;
	float step = 180.f / seg;

	for (size_t i = 0; i < seg; ++i)
	{
		theta = i * step;
		auto delta = FVector2D(round(r * sin(D2R(theta))), round(r * cos(D2R(theta))));
		Render_Line(pos + delta, pos - delta, color);
	}
}

void Render_Clear(FVector2D one, FVector2D two, FLinearColor color)
{
	if (g_Menu)
	for (auto x = one.X; x < two.X; x += 1.f)
	{
		K2_DrawLine(GCanvas, FVector2D(x, one.Y), FVector2D(x, two.Y), 1.f, color);
	}
}


void Render_PointArray(size_t count, FVector2D* ary, FLinearColor color)
{
	if (g_Menu)
	for (size_t i = 1; i < count; ++i)
		Render_Line(ary[i - 1], ary[i], color);
}

void Render_CircleOutline(FVector2D pos, int r, FLinearColor outline)
{
	float circum = M_PI * 2.f * r;
	int seg = myceilf(circum);
	if (g_Menu)
	if (seg > MAX_SEGMENTS) seg = MAX_SEGMENTS;

	float theta = 0.f;
	float step = 360.f / seg;

	FVector2D points[MAX_SEGMENTS] = {};

	for (size_t i = 0; i < seg; ++i)
	{
		theta = i * step;
		points[i] = FVector2D(pos.X + roundf(r * sin(D2R(theta))), pos.Y + roundf(r * cos(D2R(theta))));
	}

	Render_PointArray(seg, points, outline);
}

void Render_CircleOutlined(FVector2D pos, int r, FLinearColor fill, FLinearColor outline)
{
	if (g_Menu)
	Render_Circle(pos, r, fill);
	Render_CircleOutline(pos, r, outline);
}

void Render_MenuText(const wchar_t* text, FLinearColor col, FVector2D loc, bool centered)
{
	if (g_Menu)
	//	ctx->Canvas->K2_DrawText(ctx->menu_font(), _X(L"Colors"), FVector2D(tabx + tab_width / 2.f - 2.f, menu_y + 31), (i == tab_index) ? FLinearColor(1.f, 1.f, 1.f, 1.f) : menu_color1, 1.f, FLinearColor(), FVector2D(), true, true, true, FLinearColor(0, 0, 0, 1.f));

	K2_DrawText(GCanvas, GetFont(), text, loc, FVector2D(1.0f, 1.0f), Colors::Black, 1.0f, FLinearColor(), FVector2D(), centered, centered, false, FLinearColor(0, 0, 0, 1.f));
}

void Render_Slider(const wchar_t* name, float minimum, float maximum, float* val, FVector2D* loc)
{
	//auto menu_color1 = FLinearColor(0.8f, 0.f, 0.4f, 1.f);
	auto kinda_white = FLinearColor(0.8f, 0.8f, 0.8f, 1.f);

	constexpr float _width = 180 + 19;

	//ctx->Canvas->K2_DrawText(ctx->menu_font(), name, FVector2D(loc->X + 6, loc->Y + 10), FLinearColor(255, 255, 255, 255), 1.f, FLinearColor(), FVector2D(), false, true, true, FLinearColor(0, 0, 0, 255));
	Render_MenuText(name, Colors::White, FVector2D(loc->X + 6, loc->Y), false);

	loc->X += 6.f;

	bool hover = k->mouseX > loc->X && k->mouseX < (loc->X + _width) && k->mouseY > loc->Y && k->mouseY < (loc->Y + 30);
	if (k->mouse[0] && hover)
	{
		float ratio = (float)(k->mouseX - loc->X) / _width;
		if (ratio < 0.f) ratio = 0.f;
		if (ratio > 1.f) ratio = 1.f;
		*val = minimum + ((maximum - minimum) * ratio);
	}

	int xpos = ((*val - minimum) / (maximum - minimum)) * _width;
	loc->Y += 24.f;

	Render_Circle(*loc, 3, COLLINMENU_COLOR_1);
	if (g_Menu)
	Render_Clear(FVector2D(loc->X, loc->Y - 3), FVector2D(loc->X + xpos, loc->Y + 3), COLLINMENU_COLOR_1);

	Render_Clear(FVector2D(loc->X + xpos, loc->Y - 3), FVector2D(loc->X + _width, loc->Y + 3), kinda_white);
	if (g_Menu)
	Render_Circle(FVector2D(loc->X + _width, loc->Y), 3, kinda_white);
	if (g_Menu)
	Render_CircleOutlined(FVector2D(loc->X + xpos, loc->Y), 5, Colors::White, Colors::Black);

	loc->Y -= 24.f;

	wchar_t wstr[16] = {};
	char __str[16] = {};
	sprintf(__str, E("%0.1f"), *val);
	AnsiToWide(__str, wstr);
	//ctx->Canvas->K2_DrawText(ctx->menu_font(), FString(wstr), FVector2D(loc->X + _width - 20.f, loc->Y + 10), FLinearColor(255, 255, 255, 255), 1.f, FLinearColor(), FVector2D(), false, true, false, FLinearColor());
	Render_MenuText(wstr, Colors::White, FVector2D(loc->X + _width - 30.0f, loc->Y + 33), false);

	loc->X -= 6.f;
	loc->Y += 35.0f;
	loc->Y += 13.0f;
}

void Render_Toggle(FVector2D& loc_ref, const wchar_t* name, bool* on)
{
	//auto menu_color1 = FLinearColor(0.8f, 0.f, 0.4f, 1.f);

	//loc_nonp += g_Clientarea;

	auto loc = &loc_ref;


	bool hover = k->mouseX > loc->X && k->mouseX < (loc->X + 64) && k->mouseY > loc->Y && k->mouseY < (loc->Y + 20);
	if (k->mouse[0] && hover)
	{
		*on = !*on;
		k->mouse[0] = true;
	}

	FLinearColor col = *on ? COLLINMENU_COLOR_1 : FLinearColor(1.f, 1.f, 1.f, 1.f);
	if (g_Menu)
	Render_Circle(FVector2D(loc->X + 10, loc->Y + 10), 6, col);
	if (g_Menu)
	Render_Circle(FVector2D(loc->X + 18, loc->Y + 10), 6, col);
	if (g_Menu)
	Render_Clear(FVector2D(loc->X + 10, loc->Y + 4), FVector2D(loc->X + 18, loc->Y + 16), col);

	if (*on)
	{
		if (g_Menu)
		Render_CircleOutlined(FVector2D(loc->X + 18, loc->Y + 10), 5, hover ? FLinearColor(0.8f, 0.8f, 0.8f, 1.f) : FLinearColor(1, 1, 1, 1), FLinearColor(0, 0, 0, 1.f));
		if (g_Menu)
		Render_Line(FVector2D(loc->X + 9, loc->Y + 8), FVector2D(loc->X + 9, loc->Y + 12), FLinearColor(0, 0, 0, 1.f));
	}
	else
	{
		if (g_Menu)
		Render_CircleOutlined(FVector2D(loc->X + 10, loc->Y + 10), 5, hover ? FLinearColor(0.8f, 0.8f, 0.8f, 1.f) : FLinearColor(1, 1, 1, 1), FLinearColor(0, 0, 0, 1.f));
		if (g_Menu)
		Render_CircleOutline(FVector2D(loc->X + 19, loc->Y + 10), 2, FLinearColor(0, 0, 0, 1.f));
	}
	if (g_Menu)
	//ctx->Canvas->K2_DrawText(ctx->menu_font(), name, FVector2D(loc->X + 32, loc->Y + 10), FLinearColor(255, 255, 255, 255), 1.f, FLinearColor(), FVector2D(), false, true, true, FLinearColor(0, 0, 0, 255));
	Render_MenuText(name, Colors::White, FVector2D(loc->X + 32, loc->Y + 2), false);

	loc->Y += 25.0f;
}

void WndProcHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_MBUTTONDOWN:
	{
		int button = 0;
		//if (msg == WM_LBUTTONDOWN) button = 0;
		if (msg == WM_RBUTTONDOWN) button = 1;
		if (msg == WM_MBUTTONDOWN) button = 2;
		k->mouse[button] = true;
		return;
	}
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
	case WM_MBUTTONUP:
	{
		int button = 0;
		//if (msg == WM_LBUTTONUP) button = 0;
		if (msg == WM_RBUTTONUP) button = 1;
		if (msg == WM_MBUTTONUP) button = 2;
		k->mouse[button] = false;
		return;
	}
	case WM_MOUSEWHEEL:
		k->mouse_wheel += GET_WHEEL_DELTA_WPARAM(wParam) > 0 ? +1.0f : -1.0f;
		return;
	case WM_MOUSEMOVE:
		k->mouseX = (signed short)(lParam);
		k->mouseY = (signed short)(lParam >> 16);
		return;
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
		if (wParam < 256)
			k->key[wParam] = true;
		return;
	case WM_KEYUP:
	case WM_SYSKEYUP:
		if (wParam < 256)
			k->key[wParam] = false;
		return;
		//case WM_CHAR:
		//	// You can also use ToAscii()+GetKeyboardState() to retrieve characters.
		//	if (wParam > 0 && wParam < 0x10000)
		//		io.AddInputCharacter((unsigned short)wParam);
		//	return 0;
	}
}

LRESULT HkWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	auto x = 0;
	auto y = 0;


	if (!HOOKED)
		goto fok_u;

	if (!k)
	{
		auto fuck_cpp = new uint8_t[sizeof(keys)];
		k = (keys*)fuck_cpp;
	}

	WndProcHandler(hWnd, msg, wParam, lParam);

	switch (msg)
	{

	case WM_SIZE:
		if (wParam != SIZE_MINIMIZED)
		{
			dprintf(E("Resize event"));
			RECT rect;
			if (GHGameWindow && GetWindowRect(GHGameWindow, &rect))
			{
				g_ScreenWidth = rect.right - rect.left;
				g_ScreenHeight = rect.bottom - rect.top;
				dprintf(E("Resized %d %d"), g_ScreenWidth, g_ScreenHeight);
			}
		}
		break;

	case WM_MOUSEMOVE:
		g_MX = GET_X_LPARAM(lParam);
		g_MY = GET_Y_LPARAM(lParam);
		if (bDragging)
		{
			auto newX = g_MenuInitialPos.X + g_MX - g_MenuDragStartX;
			auto newY = g_MenuInitialPos.Y + g_MY - g_MenuDragStartY;
			if (newX >= g_ScreenWidth - g_MenuW)
				newX = g_ScreenWidth - g_MenuW;
			if (newY >= g_ScreenHeight - g_MenuH)
				newY = g_ScreenHeight - g_MenuH;
			if (newX <= 0)
				newX = 0;
			if (newY <= 0)
				newY = 0;

			g_MenuInitialPos.X = newX;
			g_MenuInitialPos.Y = newY;
			g_MenuDragStartX = g_MX;
			g_MenuDragStartY = g_MY;
		}
		break;

	case WM_LBUTTONUP:
		bDragging = false;
		x = GET_X_LPARAM(lParam);
		y = GET_Y_LPARAM(lParam);
		//dprintf(E("Processing un-click at %d %d"), x, y);
		MenuProcessClick(x, y);
		break;

	case WM_LBUTTONDOWN:
		if (IsInMenu(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)))
		{
			bDragging = true;
			g_MenuDragStartX = GET_X_LPARAM(lParam);
			g_MenuDragStartY = GET_Y_LPARAM(lParam);

			//dprintf(E("Processing click at %d %d"), g_MenuDragStartX, g_MenuDragStartY);
		}
		break;
	}
	//if (VK_INSERT)
	//{
	////	g_Menu = !g_Menu;

	//	if (g_Menu)
	//	{
	//		if (!IsBadReadPtr((void*)GController, 0x8))
	//		{
	//			SetIgnoreLookInput(true);
	//		}
	//	}
	//	else
	//	{
	//		if (!IsBadReadPtr((void*)GController, 0x8))
	//		{
	//			SetIgnoreLookInput(false);
	//		}
	//	}
	//}
		if (msg == WM_KEYUP)
	{
		auto nVirtKey = (int)wParam;
		if (nVirtKey == VK_F8)
		{
		//	dprintf(E("MENU HOTKEY HIT"));
			g_Menu = !g_Menu;
			if (g_Menu)
			{
				if (!IsBadReadPtr((void*)GController, 0x8))
				{
					SetIgnoreLookInput(true);
				}
			}
			else
			{
				if (!IsBadReadPtr((void*)GController, 0x8))
				{
					SetIgnoreLookInput(false);
				}
			}
		}
		/*else if (nVirtKey == VK_F8)
		{
			g_Menu = !g_Menu;

		}*/
		else if (nVirtKey == VK_PRIOR)
		{
			if (G::LootTier != 5)
				G::LootTier++;
		}
		else if (nVirtKey == VK_NEXT)
		{
			if (G::LootTier != 1)
				G::LootTier--;
		}
		else if (nVirtKey == VK_NUMPAD5)
		{
#ifdef GLOBAL_UNLOAD_FLAG
			HOOKED = false;
#endif
		}
		else if (nVirtKey == VK_F1)
		{
			G::Chests = !G::Chests;
		}
		else if (nVirtKey == VK_F2)
		{
			G::CollisionDisableOnAimbotKey = !G::CollisionDisableOnAimbotKey;
		}
	}

	if (g_Menu && (msg == WM_LBUTTONDOWN || msg == WM_LBUTTONUP || msg == WM_LBUTTONDBLCLK || msg == WM_MOUSEMOVE))
	{
		if (IsInMenu(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)))
			return 0x0;
	}

fok_u:

	return myCallWindowProcW(G_oWndProcUnsafe, hWnd, msg, wParam, lParam);
}

LRESULT HkWndProcInternal(uintptr_t unk, HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	HkWndProc(hWnd, msg, wParam, lParam);

	return G_oWndProc(unk, hWnd, msg, wParam, lParam);
}

void HookWndProcSafe()
{
	auto callSite = FindPattern(E("48 8B 0D ? ? ? ? 4C 8B CF 44 8B C6 48 89 5C 24 20 48 8B D5 E8 ? ? ? ?"));
	if (callSite)
	{
		printf(E("Found call site"));
		callSite += 21;
		auto func = ResolveRelativeReference(callSite, 0);
		printf(E("Function offset: 0x%p\n"), func - GFnBase);
		uint8_t origBytes[12];
		uint8_t jmp[] = { 0x48, 0xB8, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xFF, 0xE0 };
		memcpy(origBytes, (void*)func, 12);

		DWORD old;
		DWORD old2;

		auto trampoline = GFnBase + 0x1100;
		uint8_t trampolineBytes[24];
		memcpy(trampolineBytes, origBytes, 12);
		*(uintptr_t*)(jmp + 2) = (uintptr_t)func + 12;
		memcpy((void*)((uintptr_t)trampolineBytes + 12), jmp, 12);

		VirtualProtect((void*)trampoline, 24, PAGE_EXECUTE_READWRITE, &old);
		memcpy((void*)trampoline, trampolineBytes, 24);
		//myVirtualProtect((void*)trampoline, 24, old, &old2);

		VirtualProtect((void*)func, 12, PAGE_EXECUTE_READWRITE, &old);
		*(uintptr_t*)(jmp + 2) = (uintptr_t)HkWndProcInternal;
		memcpy((void*)func, jmp, 12);
		VirtualProtect((void*)func, 12, old, &old2);

		printf(E("Hook done!"));
		G_oWndProc = (tWndProc)trampoline;
	}
	else
	{
		printf(E("HookWndProc: FindPattern fucked up"));
	}
}

//void HookWndProcUnsafe()
//{
//	G_oWndProcUnsafe = (WNDPROC)GetWindowLongPtrA(GHGameWindow, GWLP_WNDPROC);
//	dprintf(E("G_oWndProcUnsafe: 0x%p"), G_oWndProcUnsafe);
//	SetWindowLongPtrA(GHGameWindow, GWLP_WNDPROC, (LONG_PTR)HkWndProc);
//	dprintf(E("SetWindowLongPtr done!"));
//}

void HookWndProcUnsafe()
{
	G_oWndProcUnsafe = reinterpret_cast<WNDPROC>(SetWindowLongPtr(GHGameWindow, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(HkWndProc)));
}

void InitializeWindowData()
{
	g_ControlBoundsList = new vector<UControl>();
	RECT rect;
	GHGameWindow = FindWindowA(E("UnrealWindow"), 0);

	dprintf(E("GHGameWindow: 0x%X"), GHGameWindow);

	if (GetWindowRect(GHGameWindow, &rect))
	{
		g_ScreenWidth = rect.right - rect.left;
		g_ScreenHeight = rect.bottom - rect.top;
	}

	fnCallWindowProcW = CallWindowProcW;

}



uintptr_t GetTraceVisibilityFn()
{
	return FindPattern(E("48 8B C4 48 89 58 20 55 56 57 41 54 41 55 41 56 41 57 48 8D 6C 24 ? 48 81 EC ? ? ? ? 0F 29 70 B8 0F 29 78 A8 44 0F 29 40 ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 45 20 45 8A E9"));

}


void main()
{

	GHookedObjects = new vector<void*>();

	MODULEINFO info;
	//spoof_call(g_pSpoofGadget, K32GetModuleInformation, GetCurrentProcess(), GetModuleHandle(0), &info, (DWORD)sizeof(info));
	K32GetModuleInformation(GetCurrentProcess(), GetModuleHandle(0), &info, (DWORD)sizeof(info));

	GFnBase = (uintptr_t)info.lpBaseOfDll;
	GFnSize = (uintptr_t)info.SizeOfImage;

	dprintf(E("GFnBase: 0x%p, GFnSize: 0x%X"), GFnBase, GFnSize);

	g_pSpoofGadget = (unsigned char*)FindSpooferFromModule((void*)GFnBase);
	dprintf(E("New Spoof gadget: 0x%p"), g_pSpoofGadget);

	//PatchFwFuncs();

	GObjects = (TObjectEntryArray*)(GetGObjects());

	if (!GObjects)
	{
		dprintf(E("Failed to initialize GObjects!"));
		MessageBoxA(0, "Woah", "", 0);
		return;
	}
	else
		dprintf(E("GObjects OK"));

	//GGetNameFromId = (tGetNameFromId)(GetGetGNameById());

	//if (!GGetNameFromId)
	//{
	//	dprintf(E("Failed to initialize GetGNameById!"));
	//	return;
	//}
	//else
	//	dprintf(E("GetNameShit OK"));

	//pGEngine = (UObject**)GetGEngine();

	//if (pGEngine)
	//{
	//	dprintf(E("GEngine OK"));
	//}
	//else
	//{
	//	dprintf(E("No GEngine!"));
	//	return;
	//}

	//GTraceVisibilityFn = (tTraceVisibility)GetTraceVisibilityFn();


	//GEngine = *pGEngine;

#ifdef GLOBAL_DEBUG_FLAG
	//48 89 05 ? ? ? ? 48 8B 8B ? ? ? ?
	auto pUWorldRefFunc = FindPattern(E("48 8B 05 ? ? ? ? 4D 8B C2"));
	pUWorldRefFunc = reinterpret_cast<decltype(pUWorldRefFunc)>(RVA(pUWorldRefFunc, 7));


	// 40 57 48 83 EC 50 48 8B 49 08 8B FA 48 83 C1 30

	if (!pUWorldRefFunc)
	{
		dprintf(E("No UWorld ref func!"));
	//	MessageBoxA(0, "Woah1", "", 0);
		return;
	}
	else
	{
		dprintf((E("UWorld ref found!")));
	}

#else

	UWorldOffset = DGOffset_GWorld;

#endif
	InitializeWindowData();
	HookWndProcUnsafe();
	HookPE();
}

bool __stdcall DllMain(void* hModule, unsigned long ul_reason_for_call, void* lpReserved)
{
	if (ul_reason_for_call == 1)
	{
		//AllocConsole();
		//FILE* fileptr;
		//freopen_s(&fileptr, "CONOUT$", "w", stdout);
		//freopen_s(&fileptr, "CONOUT$", "w", stderr);
		//freopen_s(&fileptr, "CONIN$", "r", stdin);

		g_pSpoofGadget = (unsigned char*)0x1; // will crash if called with

		g_pSpoofGadget = (unsigned char*)TraceToModuleBaseAndGetSpoofGadget(LoadLibraryA);
		if (!g_pSpoofGadget)
			g_pSpoofGadget = (unsigned char*)0x1;

		//InitializeRoutinePtrs();

		dprintf(E("g_pSpoofGadget: 0x%p, DllBase: 0x%p"), g_pSpoofGadget, hModule);

		//NiggerPE(hModule);

		dprintf(E("Preparations are done, calling main()"));

		main();

		dprintf(E("main() has returned.."));

		return TRUE;
	}

	return 1;
}
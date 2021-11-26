#include "stdafx.h"
#include "structure.h"
#include "dllmain.cpp"

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


uintptr_t GetGetBoneMatrix()
{
	auto ss = FindPattern(E("E8 ? ? ? ? 0F 10 48 30"));
	if (!ss) ss = FindPattern(E("E8 ? ? ? ? 48 8B 8C 24 ? ? ? ? 0F 28 00"));
	if (!ss) ss = FindPattern(E("E8 ? ? ? ? 4C 8B 8D ? ? ? ? 48 8D 4D F0"));

	if (!ss)
		return ss;

	return ss;
}

struct FName GetBoneName(UObject* _this, int BoneIndex)
{
	static UFunction* fn = 0; if (!fn) fn = UObject::FindObject<UFunction>(E("Function Engine.SkinnedMeshComponent.GetBoneName"));

	struct
	{
		int                            BoneIndex;
		struct FName                   ReturnValue;
	} params;

	params.BoneIndex = BoneIndex;

	ProcessEvent(_this, fn, &params);

	return params.ReturnValue;
}

bool __forceinline point_valid(FVector2D& val)
{
	return val.X > 0 && val.X < (float)g_ScreenWidth&& val.Y > 0 && val.Y < (float)g_ScreenHeight;
}

void DrawBones(UObject* mesh, Bones* arr, int sz, FLinearColor clr, FVector2D& topleft, FVector2D& downright, float b_thicc = 1.0f)
{
	for (int i = 1; i < sz; i++)
	{
		FVector2D spPrev;
		FVector2D spNext;

		FVector previousBone = GetBone3D(mesh, arr[i - 1]);

		if (previousBone == FVector())
			continue;

		if (!W2S(previousBone, spPrev))
			continue;

		FVector nextBone = GetBone3D(mesh, arr[i]);

		if (nextBone == FVector())
			continue;

		if (!W2S(nextBone, spNext))
			continue;

		if (previousBone.DistanceFrom(nextBone) > 100)
			continue;

		auto x = spPrev;

		if (x.X > downright.X)
			downright.X = x.X;

		if (x.Y > downright.Y)
			downright.Y = x.Y;

		if (x.X < topleft.X)
			topleft.X = x.X;

		if (x.Y < topleft.Y)
			topleft.Y = x.Y;

		x = spNext;

		if (x.X > downright.X)
			downright.X = x.X;

		if (x.Y > downright.Y)
			downright.Y = x.Y;

		if (x.X < topleft.X)
			topleft.X = x.X;

		if (x.Y < topleft.Y)
			topleft.Y = x.Y;

		if (G::Skeletons)
			K2_DrawLine((UObject*)GCanvas, spPrev, spNext, b_thicc, clr);
	}
}

void DrawBox(FVector2D& topleft, FVector2D& downright, FLinearColor clr = Colors::Red)
{
	//xDrawText(E(L"T L"), topleft, clr);
	//xDrawText(E(L"D R"), downright, clr);

	float thicc = G::PlayerBoxThicc;

	if (!G::CornerBox)
	{
		K2_DrawLine(GCanvas, topleft, { downright.X, topleft.Y }, thicc, clr);
		K2_DrawLine(GCanvas, topleft, { topleft.X , downright.Y }, thicc, clr);
		K2_DrawLine(GCanvas, downright, { topleft.X , downright.Y }, thicc, clr);
		K2_DrawLine(GCanvas, downright, { downright.X, topleft.Y }, thicc, clr);
	}
	else
	{
		auto h = downright.Y - topleft.Y;
		auto w = downright.X - topleft.X;

		auto downleft = FVector2D{ topleft.X, downright.Y };
		auto topright = FVector2D{ downright.X, topleft.Y };

		thicc = G::CornerBoxThicc;

		K2_DrawLine(GCanvas, topleft, { topleft.X, topleft.Y + h * G::CornerBoxScale }, thicc, clr);
		K2_DrawLine(GCanvas, topleft, { topleft.X + w * G::CornerBoxScale, topleft.Y }, thicc, clr);

		K2_DrawLine(GCanvas, downright, { downright.X, downright.Y - h * G::CornerBoxScale }, thicc, clr);
		K2_DrawLine(GCanvas, downright, { downright.X - w * G::CornerBoxScale, downright.Y }, thicc, clr);

		K2_DrawLine(GCanvas, downleft, { downleft.X, downleft.Y - h * G::CornerBoxScale }, thicc, clr);
		K2_DrawLine(GCanvas, downleft, { downleft.X + w * G::CornerBoxScale, downright.Y }, thicc, clr);

		K2_DrawLine(GCanvas, topright, { topright.X, topright.Y + h * G::CornerBoxScale }, thicc, clr);
		K2_DrawLine(GCanvas, topright, { topright.X - w * G::CornerBoxScale, topright.Y }, thicc, clr);
	}
}

struct FString GetPlayerName(UObject* player)
{
	static UFunction* fn = 0; if (!fn) fn = FindFunction(E("Function Engine.PlayerState.GetPlayerName"));

	struct
	{
		struct FString                 ReturnValue;
	} params;


	ProcessEvent(player, fn, &params);

	auto ret = params.ReturnValue;
	return ret;
}

uintptr_t GOffset_PlayerState = 0;

class FTextData {
public:
	char pad_0x0000[0x28];  //0x0000
	wchar_t* Name;          //0x0028 
	__int32 Length;         //0x0030 

};

struct FText {
	FTextData* Data;
	char UnknownData[0x10];

	wchar_t* Get() const {
		if (Data) {
			return Data->Name;
		}

		return nullptr;
	}
};

class UControl
{
public:

	FVector2D Origin;
	FVector2D Size;
	bool* BoundBool = nullptr;
	bool bIsMenuTabControl;
	bool bIsRangeSlider;
	int RangeValueMin;
	int RangeValueMax;
	int* pBoundRangeValue;
	int BoundMenuTabIndex;

	bool ContainsPoint(FVector2D pt)
	{
		auto extent = Origin + Size;
		return (pt.X > Origin.X && pt.Y > Origin.Y && pt.X < extent.X&& pt.Y < extent.Y);
	}
};

UClass* SC_Pickaxe()
{
	static UClass* memes = 0;
	if (!memes)
		memes = UObject::FindObject<UClass>(E("Class FortniteGame.AthenaPickaxeItemDefinition"));
	return memes;
}

uintptr_t GOffset_PrimaryPickupItemEntry = 0;
uintptr_t GOffset_Tier = 0;
uintptr_t GOffset_ItemDefinition = 0;

FText QueryDroppedItemNameAndTier(uint64_t item, BYTE* tier)
{
	auto definition = read<uint64_t>(item + DGOffset_PrimaryPickupItemEntry + DGOffset_ItemDefinition);
	if (definition)
	{
		*tier = read<BYTE>(definition + DGOffset_Tier);
		return read<FText>(definition + DGOffset_DisplayName);
	}
	else
		return FText{};
}

void MwMenuDraw();

vector<UControl>* g_ControlBoundsList;


D3DMATRIX _inline MatrixMultiplication(D3DMATRIX pM1, D3DMATRIX pM2)
{
	D3DMATRIX pOut;
	pOut._11 = pM1._11 * pM2._11 + pM1._12 * pM2._21 + pM1._13 * pM2._31 + pM1._14 * pM2._41;
	pOut._12 = pM1._11 * pM2._12 + pM1._12 * pM2._22 + pM1._13 * pM2._32 + pM1._14 * pM2._42;
	pOut._13 = pM1._11 * pM2._13 + pM1._12 * pM2._23 + pM1._13 * pM2._33 + pM1._14 * pM2._43;
	pOut._14 = pM1._11 * pM2._14 + pM1._12 * pM2._24 + pM1._13 * pM2._34 + pM1._14 * pM2._44;
	pOut._21 = pM1._21 * pM2._11 + pM1._22 * pM2._21 + pM1._23 * pM2._31 + pM1._24 * pM2._41;
	pOut._22 = pM1._21 * pM2._12 + pM1._22 * pM2._22 + pM1._23 * pM2._32 + pM1._24 * pM2._42;
	pOut._23 = pM1._21 * pM2._13 + pM1._22 * pM2._23 + pM1._23 * pM2._33 + pM1._24 * pM2._43;
	pOut._24 = pM1._21 * pM2._14 + pM1._22 * pM2._24 + pM1._23 * pM2._34 + pM1._24 * pM2._44;
	pOut._31 = pM1._31 * pM2._11 + pM1._32 * pM2._21 + pM1._33 * pM2._31 + pM1._34 * pM2._41;
	pOut._32 = pM1._31 * pM2._12 + pM1._32 * pM2._22 + pM1._33 * pM2._32 + pM1._34 * pM2._42;
	pOut._33 = pM1._31 * pM2._13 + pM1._32 * pM2._23 + pM1._33 * pM2._33 + pM1._34 * pM2._43;
	pOut._34 = pM1._31 * pM2._14 + pM1._32 * pM2._24 + pM1._33 * pM2._34 + pM1._34 * pM2._44;
	pOut._41 = pM1._41 * pM2._11 + pM1._42 * pM2._21 + pM1._43 * pM2._31 + pM1._44 * pM2._41;
	pOut._42 = pM1._41 * pM2._12 + pM1._42 * pM2._22 + pM1._43 * pM2._32 + pM1._44 * pM2._42;
	pOut._43 = pM1._41 * pM2._13 + pM1._42 * pM2._23 + pM1._43 * pM2._33 + pM1._44 * pM2._43;
	pOut._44 = pM1._41 * pM2._14 + pM1._42 * pM2._24 + pM1._43 * pM2._34 + pM1._44 * pM2._44;

	return pOut;
}

#define x X
#define y Y
#define z Z
#define w W

D3DMATRIX ToMatrixWithScale(const FVector& translation, const FVector& scale, const FQuat& rot)
{
	D3DMATRIX m;
	m._41 = translation.x;
	m._42 = translation.y;
	m._43 = translation.z;

	float x2 = rot.x + rot.x;
	float y2 = rot.y + rot.y;
	float z2 = rot.z + rot.z;

	float xx2 = rot.x * x2;
	float yy2 = rot.y * y2;
	float zz2 = rot.z * z2;
	m._11 = (1.0f - (yy2 + zz2)) * scale.x;
	m._22 = (1.0f - (xx2 + zz2)) * scale.y;
	m._33 = (1.0f - (xx2 + yy2)) * scale.z;

	float yz2 = rot.y * z2;
	float wx2 = rot.w * x2;
	m._32 = (yz2 - wx2) * scale.z;
	m._23 = (yz2 + wx2) * scale.y;

	float xy2 = rot.x * y2;
	float wz2 = rot.w * z2;
	m._21 = (xy2 - wz2) * scale.y;
	m._12 = (xy2 + wz2) * scale.x;

	float xz2 = rot.x * z2;
	float wy2 = rot.w * y2;
	m._31 = (xz2 + wy2) * scale.z;
	m._13 = (xz2 - wy2) * scale.x;

	m._14 = 0.0f;
	m._24 = 0.0f;
	m._34 = 0.0f;
	m._44 = 1.0f;

	return m;
}
#undef x
#undef y
#undef z
#undef w

FVector BoneToWorld(Bones boneid, uint64_t bone_array, FTransform& ComponentToWorld)
{
	if (bone_array == NULL)
		return { 0, 0, 0 };
	auto bone = read<FTransform>(bone_array + (boneid * sizeof(FTransform)));
	if (bone.Translation == FVector())
		return { 0, 0, 0 };
	auto matrix = MatrixMultiplication(ToMatrixWithScale(bone.Translation, bone.Scale3D, bone.Rotation), ToMatrixWithScale(ComponentToWorld.Translation, ComponentToWorld.Scale3D, ComponentToWorld.Rotation));
	return FVector(matrix._41, matrix._42, matrix._43);
}

FVector BoneToWorld(Bones boneid, uint64_t mesh)
{
	if (mesh == NULL)
		return { 0, 0, 0 };
	uint64_t bone_array = read<uint64_t>(mesh + MESH_BONE_ARRAY); //offsetof(Classes::USkeletalMeshComponent, )
	if (bone_array == 0)
		return { 0, 0, 0 };
	auto ComponentToWorld = read<FTransform>(mesh + MESH_COMPONENT_TO_WORLD); //offsetof(Classes::USceneComponent, Classes::USceneComponent::ComponentToWorld)
	auto bone = read<FTransform>(bone_array + (boneid * sizeof(FTransform)));
	if (bone.Translation == FVector() || ComponentToWorld.Translation == FVector())
		return { 0, 0, 0 };
	auto matrix = MatrixMultiplication(ToMatrixWithScale(bone.Translation, bone.Scale3D, bone.Rotation), ToMatrixWithScale(ComponentToWorld.Translation, ComponentToWorld.Scale3D, ComponentToWorld.Rotation));
	return FVector(matrix._41, matrix._42, matrix._43);
}

uintptr_t g_VehSelected = 0;

HWND GHGameWindow = 0;

int GetTeamId(UObject* actor)
{
	auto playerState = read<UObject*>((uintptr_t)actor + DGOffset_PlayerState);

	if (playerState)
	{
		return read<int>((uint64_t)playerState + DGOffset_TeamIndex);
	}

	return 0;
}

float cached_bullet_gravity_scale = 0.f, cached_world_gravity = 0.f;

int GMyTeamId = 0;

uintptr_t GOffset_GravityScale = 0;
uintptr_t GOffset_Searched = 0;

bool AController_SetControlRotation(FRotator rot, uint64_t controller)
{
	auto VTable = read<uintptr_t>(controller);
	if (!VTable)
		return false;

	auto func = (*(void(__fastcall**)(uint64_t, void*))(VTable + 0x688));
	func(controller, (void*)&rot);

	return true;
}

FRotator Clamp(FRotator r)
{
	if (r.Yaw > 180.f)
		r.Yaw -= 360.f;
	else if (r.Yaw < -180.f)
		r.Yaw += 360.f;

	if (r.Pitch > 180.f)
		r.Pitch -= 360.f;
	else if (r.Pitch < -180.f)
		r.Pitch += 360.f;

	if (r.Pitch < -89.f)
		r.Pitch = -89.f;
	else if (r.Pitch > 89.f)
		r.Pitch = 89.f;

	r.Roll = 0.f;

	return r;
}

#define M_RADPI	57.295779513082f
__forceinline FRotator calc_angle(FVector& Src, FVector& Dst)
{
	FVector Delta = Src - Dst;
	FRotator AimAngles;
	float Hyp = sqrt(powf(Delta.X, 2.f) + powf(Delta.Y, 2.f));
	AimAngles.Yaw = atanf(Delta.Y / Delta.X) * M_RADPI;
	AimAngles.Pitch = (atanf(Delta.Z / Hyp) * M_RADPI) * -1.f;
	if (Delta.X >= 0.f) AimAngles.Yaw += 180.f;
	//AimAngles.Roll = 0.f;
	return AimAngles;
}

void SetViewAngles(FRotator ang)
{
	auto angls = Clamp(ang);
	angls.Roll = 0.0f;
	AController_SetControlRotation(ang, GController);
}

void AimToTarget(FRotator ang)
{
	//if (bestFOV >= AimbotFOV)
		//return;

	bAimbotActivated = true;

	SetViewAngles(ang);
}

#define DEG2RAD(x)  ( (float)(x) * (float)(M_PI_F / 180.f) )
#define M_PI_F		((float)(M_PI))
#define RAD2DEG(x)  ( (float)(x) * (float)(180.f / M_PI_F) )


UClass* SC_BuildingContainer()
{
	static UClass* bc = 0;
	if (!bc) bc = UObject::FindObject<UClass>(E("Class FortniteGame.BuildingContainer"));
	return bc;
}

bool GetActorEnableCollision(UObject* a)
{
	static UFunction* fn = 0; if (!fn) fn = FindFunction(E("Function Engine.Actor.GetActorEnableCollision"));

	struct
	{
		bool                           ReturnValue;
	} params;


	ProcessEvent(a, fn, &params);

	return params.ReturnValue;
}

void SetActorEnableCollision(UObject* a, bool bNewActorEnableCollision)
{
	static UFunction* fn = 0; if (!fn) fn = FindFunction(E("Function Engine.Actor.SetActorEnableCollision"));

	struct
	{
		bool                           bNewActorEnableCollision;
	} params;

	params.bNewActorEnableCollision = bNewActorEnableCollision;

	ProcessEvent(a, fn, &params);
}

uintptr_t GOffset_VehicleSkeletalMesh = 0;
uintptr_t GOffset_Visible = 0;

SHORT myGetAsyncKeyState(int kode)
{
	return GetAsyncKeyState(kode);
}

#pragma pack(push, 1)
// Function Engine.PrimitiveComponent.SetAllPhysicsLinearVelocity
struct UPrimitiveComponent_SetAllPhysicsLinearVelocity_Params
{
	struct FVector                                     NewVel;                                                   // (Parm, IsPlainOldData)
	bool                                               bAddToCurrent;                                            // (Parm, ZeroConstructor, IsPlainOldData)
};

// Function Engine.PrimitiveComponent.SetEnableGravity
struct UPrimitiveComponent_SetEnableGravity_Params
{
	bool                                               bGravityEnabled;                                          // (Parm, ZeroConstructor, IsPlainOldData)
};

// Function Engine.Actor.K2_SetActorRotation
struct AActor_K2_SetActorRotation_Params
{
	struct FRotator                                    NewRotation;                                              // (Parm, IsPlainOldData)
	bool                                               bTeleportPhysics;                                         // (Parm, ZeroConstructor, IsPlainOldData)
	bool                                               ReturnValue;                                              // (Parm, OutParm, ZeroConstructor, ReturnParm, IsPlainOldData)
};
#pragma pack(pop)

void SetAllPhysicsAngularVelocity(uint64_t primitive_component, const struct FVector& NewVel, bool bAddToCurrent)
{
	static UFunction* fn = nullptr;

	if (!fn)
		fn = FindFunction(E("Function Engine.PrimitiveComponent.SetAllPhysicsAngularVelocity"));
	UPrimitiveComponent_SetAllPhysicsLinearVelocity_Params params;
	params.NewVel = NewVel;
	params.bAddToCurrent = bAddToCurrent;

	ProcessEvent((UObject*)primitive_component, fn, &params);
}


void SetAllPhysicsLinearVelocity(uint64_t primitive_component, const struct FVector& NewVel, bool bAddToCurrent)
{
	static UFunction* fn = nullptr;

	if (!fn)
		fn = FindFunction(E("Function Engine.PrimitiveComponent.SetAllPhysicsLinearVelocity"));
	UPrimitiveComponent_SetAllPhysicsLinearVelocity_Params params;
	params.NewVel = NewVel;
	params.bAddToCurrent = bAddToCurrent;

	ProcessEvent((UObject*)primitive_component, fn, &params);
}

void SetEnableGravity(uint64_t primitive_component, bool bEnable)
{
	static UFunction* fn = nullptr;

	if (!fn)
		fn = FindFunction(E("Function Engine.PrimitiveComponent.SetEnableGravity"));
	UPrimitiveComponent_SetEnableGravity_Params params;
	params.bGravityEnabled = bEnable;
	ProcessEvent((UObject*)primitive_component, fn, &params);
}

void ProcessVehicle(uintptr_t pawn)
{
	auto rc = read<uintptr_t>(pawn + DGOffset_RootComponent);

	if (!rc)
		return;

	auto loc = read<FVector>(rc + DGOffset_ComponentLocation);

	if (GetDistanceMeters(loc) > 10)
		return;

	auto veh_mesh = *(uintptr_t*)(pawn + DGOffset_VehicleSkeletalMesh);

	if (veh_mesh)
	{
		if (myGetAsyncKeyState(VK_CAPITAL) & 1)
		{
			SetActorEnableCollision((UObject*)pawn, false);

			float coeff = (60.0f * 60);

			if (myGetAsyncKeyState(VK_SHIFT))
			{
				coeff *= 2;
			}

			SetAllPhysicsAngularVelocity(veh_mesh, { 0, 0, 0 }, false);
			SetAllPhysicsLinearVelocity(veh_mesh, { 0, 0, 0 }, false);
			SetEnableGravity(veh_mesh, false);
			bool bKp = false;

			if (myGetAsyncKeyState('Q') & 0x8000)
			{
				SetAllPhysicsLinearVelocity(veh_mesh, { 0.f, 0.f, coeff / 2 }, true);
				bKp = true;
			}

			if (myGetAsyncKeyState('E') & 0x8000)
			{
				SetAllPhysicsLinearVelocity(veh_mesh, { 0.f, 0.f, -(coeff / 2) }, true);
				bKp = true;
			}

			if (myGetAsyncKeyState(0x57))
			{
				FVector vel;
				auto yaw = GCameraCache->Rotation.Yaw;
				float theta = 2.f * M_PI * (yaw / 360.f);

				vel.X = (coeff * cos(theta));
				vel.Y = (coeff * sin(theta));
				vel.Z = 0.f;

				SetAllPhysicsLinearVelocity(veh_mesh, vel, true);
				bKp = true;
			}
			if (myGetAsyncKeyState(0x53))
			{
				FVector vel;
				auto yaw = GCameraCache->Rotation.Yaw;
				float theta = 2.f * M_PI * (yaw / 360.f);

				vel.X = -(coeff * cos(theta));
				vel.Y = -(coeff * sin(theta));

				SetAllPhysicsLinearVelocity(veh_mesh, vel, true); //{ -80.f, 0.f, 0.f }
				bKp = true;
			}
			if (myGetAsyncKeyState(0x41)) // A
			{
				FVector vel;
				auto yaw = GCameraCache->Rotation.Yaw;
				float theta = 2.f * M_PI * (yaw / 360.f);

				vel.X = (coeff * sin(theta));
				vel.Y = -(coeff * cos(theta));

				SetAllPhysicsLinearVelocity(veh_mesh, vel, true); //{ -80.f, 0.f, 0.f }
				bKp = true;
			}
			if (myGetAsyncKeyState(0x44)) // D
			{
				FVector vel;
				auto yaw = GCameraCache->Rotation.Yaw;
				float theta = 2.f * M_PI * (yaw / 360.f);

				vel.X = -(coeff * sin(theta));
				vel.Y = (coeff * cos(theta));

				SetAllPhysicsLinearVelocity(veh_mesh, vel, true); //{ -80.f, 0.f, 0.f }
				bKp = true;
			}

			if (!bKp || myGetAsyncKeyState(VK_SPACE))
			{
				SetAllPhysicsLinearVelocity(veh_mesh, { 0.0, 0.0, 0.0 }, false);
			}
		}
		else
		{
			SetEnableGravity(veh_mesh, true);
			SetActorEnableCollision((UObject*)pawn, true);
		}
	}
}

bool K2_SetActorLocation(UObject* a, const struct FVector& NewLocation, bool bSweep, bool bTeleport, struct FHitResult* SweepHitResult)
{
	static UFunction* fn = nullptr;
	if (!fn) fn = FindFunction(E("Function Engine.Actor.K2_SetActorLocation"));

	// Function Engine.Actor.K2_SetActorLocation
	struct AActor_K2_SetActorLocation_Params
	{
		struct FVector                                     NewLocation;                                              // (Parm, IsPlainOldData)
		bool                                               bSweep;                                                   // (Parm, ZeroConstructor, IsPlainOldData)
		struct FHitResult                                  SweepHitResult;                                           // (Parm, OutParm, IsPlainOldData)
		bool                                               bTeleport;                                                // (Parm, ZeroConstructor, IsPlainOldData)
		bool                                               ReturnValue;                                              // (Parm, OutParm, ZeroConstructor, ReturnParm, IsPlainOldData)
	};

	AActor_K2_SetActorLocation_Params params;

	params.NewLocation = NewLocation;
	params.bSweep = bSweep;
	params.bTeleport = bTeleport;

	auto flags = fn->FunctionFlags;

	ProcessEvent(a, fn, &params);

	fn->FunctionFlags = flags;

	if (SweepHitResult != nullptr)
		*SweepHitResult = params.SweepHitResult;

	return params.ReturnValue;
}

void Render()
{
	//dprintf("RENDERER CHECK");
	FLinearColor clr(1, 1, 1, 1);

	if (g_ControlBoundsList && g_ControlBoundsList->size())
		g_ControlBoundsList->clear();
	else
		g_ControlBoundsList = new vector<UControl>();

	int closestEnemyDist = 9999999;
	FVector closestEnemyAss;
	if (G::RefreshEach1s)
	{
		ULONGLONG tLastTimeRefreshd = 0;
		if (GetTickCount64() - tLastTimeRefreshd >= 1000)
		{
			//dprintf("RENDERER CHECK3");
			RECT rect;
			if (GHGameWindow && GetWindowRect(GHGameWindow, &rect))
			{
				g_ScreenWidth = rect.right - rect.left;
				g_ScreenHeight = rect.bottom - rect.top;
			}
			//dprintf("RENDERER CHECK4");
			tLastTimeRefreshd = GetTickCount64();

			dprintf(E("Refreshed: g_ScrenWidth: %d px, height: %d px"), g_ScreenWidth, g_ScreenHeight);
		}
	}

	if (G::EnableHack)
	{
		if (G::AimbotEnable)
			AimbotBeginFrame();

		static UClass* VehicleSK_class = nullptr;
		if (!VehicleSK_class)
			VehicleSK_class = UObject::FindObject<UClass>(E("Class FortniteGame.FortAthenaSKVehicle"));

		bool bInExplosionRadius = false;
		bool bEnemyClose = false;

		auto UWorld = GWorld;
		if (!UWorld)
		{
			//xDrawText(E(L"NO WORLD!"), { 600, 600 }, clr);
		}

		auto levels = *(TArray<UObject*>*)((uintptr_t)UWorld + DGOffset_Levels);

		if (!levels.Num())
		{
			//xDrawText(E(L"NO LEVEL #1!"), { 600, 600 }, clr);
			return;
		}

		auto bCaps = false;
		if (G::WeakSpotAimbot)
		{
			if (GetAsyncKeyState((int)WeakSpotAimbotKey))
			{
				bCaps = true;

				K2_DrawText(GCanvas, GetFont(), E(L"WeakSpot aimbot is ON"), { 30, 800 }, FVector2D(1.0f, 1.0f), Colors::Red, 1.0f, FLinearColor(0, 0, 0, 255), FVector2D(), false, false, true, FLinearColor(0, 0, 0, 1.0f));
			}
		}


		for (int levelIndex = 0; (G::LootEnable ? (levelIndex != levels.Num()) : levelIndex != 1); levelIndex++)
		{
			auto level = levels[levelIndex];
			if (!level)
			{
				return;
			}
			GActorArray = (TArray<UObject*>*)((uintptr_t)level + DGOffset_Actors);
			auto actors = *GActorArray;
			static UClass* supply_class = nullptr;
			static UClass* trap_class = nullptr;
			static UClass* fortpickup_class = nullptr;
			static UClass* BuildingContainer_class = nullptr;
			static UClass* Chests_class = nullptr;
			static UClass* AB_class = nullptr;
			static UClass* GolfCarts_class = nullptr;
			static UClass* Rifts_class = nullptr;

			if (!supply_class)
				supply_class = UObject::FindObject<UClass>(E("Class FortniteGame.FortAthenaSupplyDrop"));

			if (!trap_class)
				trap_class = UObject::FindObject<UClass>(E("Class FortniteGame.BuildingTrap"));

			if (!fortpickup_class)
				fortpickup_class = UObject::FindObject<UClass>(E("Class FortniteGame.FortPickup"));

			if (!Rifts_class)
				Rifts_class = UObject::FindObject<UClass>(E("Class FortniteGame.FortAthenaRiftPortal"));

			static UClass* projectiles_class = nullptr;

			static UClass* weakspot_class = nullptr;

			if (!weakspot_class)
				weakspot_class = UObject::FindObject<UClass>(E("Class FortniteGame.BuildingWeakSpot"));

			if (!projectiles_class)
				projectiles_class = UObject::FindObject<UClass>(E("Class FortniteGame.FortProjectileBase"));

			for (int i = 0; i < actors.Num(); i++)
			{
				auto actor = actors[i];

				if (!actor)
					continue;
		
				if (actor == (UObject*)GPawn)
					continue;
				if (G::EspLoot && Object_IsA((UObject*)actor, fortpickup_class))
				{
					auto rc = *(UObject**)((uintptr_t)actor + DGOffset_RootComponent);

					if (!rc)
						continue;

					FVector loc = *(FVector*)((uintptr_t)rc + DGOffset_ComponentLocation);

					FVector2D sp;

					auto dist = GetDistanceMeters(loc);

					if (dist > G::RenderDist)
						continue;

					if (dist > G::LootRenderDist)
						continue;

					if (W2S(loc, sp))
					{
						static char buf[512];
						static wchar_t wmemes[512];
						BYTE tier;
						auto name = QueryDroppedItemNameAndTier((uintptr_t)actor, &tier);
						if (name.Get() && tier > 0)
						{
							auto color = Colors::LightYellow;
							sprintf(buf, E("[ %s %d m ]"), WideToAnsi(name.Get()).c_str(), dist);
							AnsiToWide(buf, wmemes);
							xDrawText(wmemes, sp, color);
						}
					}
				}
				else if (G::Chests && Object_IsA(actor, SC_BuildingContainer()))
				{

					FVector2D sp;

					auto rc = *(UObject**)((uintptr_t)actor + DGOffset_RootComponent);

					if (!rc)
						continue;

					FVector loc = *(FVector*)((uintptr_t)rc + DGOffset_ComponentLocation);

					auto dist = GetDistanceMeters(loc);

					if (dist > G::RenderDist)
						continue;

					if (dist > G::ChestsRdist)
						continue;



					if (W2S(loc, sp))
					{
						char memes[128];
						wchar_t wmemes[128];

						auto _class = actor->Class;
						auto drawColor = Colors::Yellow;
						auto drawName = E("Chest");

						static UClass* Ammoboxes_class = 0;

						bool bDraw = true;

						if (G::Chests)
						{
							if (!Chests_class)
							{
								auto className = GetObjectName(_class);
								if (MemoryBlocksEqual((void*)className.c_str(), (void*)E("Tiered_Chest"), 12))
								{
									Chests_class = _class;
									dprintf(E("Chests class: %s"), className.c_str());
									bDraw = true;
								}
							}
							else if (Object_IsA(actor, Chests_class))
							{
								bDraw = true;
							}
						}

						if (!bDraw && G::LootTier <= 1)
						{
							if (!Ammoboxes_class)
							{
								auto className = GetObjectName(_class);
								if (MemoryBlocksEqual((void*)className.c_str(), (void*)E("Tiered_Ammo"), 11))
								{
									Ammoboxes_class = _class;
									dprintf(E("AmmoBoxes class: %s"), className.c_str());
									bDraw = true;
									drawName = E("Ammo");
									drawColor = Colors::White;
								}
							}
							else if (Object_IsA(actor, Ammoboxes_class))
							{
								drawName = E("Ammo");
								bDraw = true;
								drawColor = Colors::White;
							}
						}

						if (!bDraw)
							continue;

						sprintf(memes, E("[ %s %d m ]"), drawName, GetDistanceMeters(loc));
						AnsiToWide(memes, wmemes);
						xDrawText(wmemes, sp, drawColor);
					}
				}

				else if (levelIndex == 0 && Object_IsA(actor, SC_FortPlayerPawn()))
				{

					auto rc = *(UObject**)((uintptr_t)actor + DGOffset_RootComponent);

					if (!rc)
						continue;

					FVector loc = *(FVector*)((uintptr_t)rc + DGOffset_ComponentLocation);

					auto dist = GetDistanceMeters(loc);

					if (dist > G::RenderDist)
						continue;

					FVector2D sp;

					if (W2S(loc, sp))
					{
						auto mesh = *(UObject**)((uintptr_t)actor + DGOffset_Mesh);

						if (!mesh)
							continue;

						auto bone = GetBone3D(mesh, !G::Baim ? (Bones)66 : Bones::spine_02);

						static Bones p1[] = // left arm - neck - right arm
						{
							Bones(55),
							Bones(92),
							Bones(91),
							Bones(36),
							Bones(9),
							Bones(10),
							Bones(27)
						};

						static Bones p2[] = // head-spine-pelvis
						{
							(Bones)66,
							(Bones)64,
							(Bones)2
						};

						static Bones p3[] = // left leg - pelvis - right leg
						{
							Bones(79),
							Bones(83),
							Bones(75),
							Bones(74),
							Bones(2),
							Bones(67),
							Bones(68),
							Bones(82),
							Bones(72)
						};

						if (levelIndex == 0 && G::EspRifts && Object_IsA(actor, Rifts_class))
						{
							auto rc = *(UObject**)((uintptr_t)actor + DGOffset_RootComponent);

							if (!rc)
								continue;

							FVector loc = *(FVector*)((uintptr_t)rc + DGOffset_ComponentLocation);

							FVector2D sp;

							auto dist = GetDistanceMeters(loc);

							if (dist > G::RenderDist)
								continue;

							if (W2S(loc, sp))
							{
								static char memes[128];
								static wchar_t wmemes[128];
								sprintf(memes, E(" Rift %d m "), GetDistanceMeters(loc));
								AnsiToWide(memes, wmemes);
								xDrawText(wmemes, sp, Colors::Green);
							}
						}
						else if (levelIndex == 0 && (G::FlyingCars || G::EspVehicles) && Object_IsA(actor, VehicleSK_class))
						{
							auto rc = *(UObject**)((uintptr_t)actor + DGOffset_RootComponent);

							if (!rc)
								continue;

							FVector loc = *(FVector*)((uintptr_t)rc + DGOffset_ComponentLocation);

							FVector2D sp;

							auto dist = GetDistanceMeters(loc);

							if (dist > G::RenderDist)
								continue;

							if (W2S(loc, sp))
							{
								static char memes[128];
								static wchar_t wmemes[128];
								sprintf(memes, E(" Vehicle %d m "), GetDistanceMeters(loc));
								AnsiToWide(memes, wmemes);
								xDrawText(wmemes, sp, Colors::Cyan);
							}

							if (G::FlyingCars)
							{
								ProcessVehicle((uintptr_t)actor);
							}

						}
						else if (levelIndex == 0 && G::EspSupplyDrops && Object_IsA(actor, supply_class))
						{
							auto rc = *(UObject**)((uintptr_t)actor + DGOffset_RootComponent);

							if (!rc)
								continue;

							FVector loc = *(FVector*)((uintptr_t)rc + DGOffset_ComponentLocation);

							FVector2D sp;

							auto dist = Dist((UObject*)actor);

							if (dist > G::RenderDist)
								continue;

							if (dist > G::LootRenderDist)
								continue;

							if (W2S(loc, sp))
							{
								char memes[128];
								wchar_t wmemes[128];
								sprintf(memes, E(" Supplies! %d m "), GetDistanceMeters(loc));
								AnsiToWide(memes, wmemes);
								xDrawText(wmemes, sp, Colors::Cyan);
							}
						}
						else if (levelIndex == 0 && G::EspTraps && Object_IsA(actor, trap_class))
						{
							auto rc = *(UObject**)((uintptr_t)actor + DGOffset_RootComponent);

							if (!rc)
								continue;

							FVector loc = *(FVector*)((uintptr_t)rc + DGOffset_ComponentLocation);

							FVector2D sp;

							auto dist = GetDistanceMeters(loc);

							if (dist > 30)
								continue;

							if (W2S(loc, sp))
							{
								char memes[128];
								wchar_t wmemes[128];
								sprintf(memes, E(" TRAP %d m "), GetDistanceMeters(loc));
								AnsiToWide(memes, wmemes);
								xDrawText(wmemes, sp, Colors::Red);
							}
						}
						else if (Object_IsA(actor, projectiles_class))
						{
							auto rc = *(UObject**)((uintptr_t)actor + DGOffset_RootComponent);

							if (!rc)
								continue;

							FVector loc = *(FVector*)((uintptr_t)rc + DGOffset_ComponentLocation);

							FVector2D sp;

							auto dist = GetDistanceMeters(loc);

							if (dist > G::RenderDist)
								continue;

							if (dist > 50)
								continue;

							if (W2S(loc, sp))
							{
								static char memes[128];
								static wchar_t wmemes[128];
								sprintf(memes, E(" PROJECTILE %d m "), dist);
								AnsiToWide(memes, wmemes);
								xDrawText(wmemes, sp, Colors::Red);

								if (dist < 20 && !bInExplosionRadius)
								{
									xDrawText(E(L"YOU'RE IN EXPLOSION RADIUS!"), { (float)(g_ScreenWidth / 2) , (float)(g_ScreenHeight - 200) }, Colors::Red);
									bInExplosionRadius = true;
								}
							}

							if (G::ProjectileTpEnable)
							{
								FHitResult xxxx;
								K2_SetActorLocation(actor, closestEnemyAss, true, true, &xxxx);
							}
						}
						else if (G::WeakSpotAimbot && bCaps && Object_IsA(actor, weakspot_class))
						{
							auto rc = *(UObject**)((uintptr_t)actor + DGOffset_RootComponent);

							if (!rc)
								continue;

							FVector loc = *(FVector*)((uintptr_t)rc + DGOffset_ComponentLocation);

							FVector2D sp;

							auto dist = GetDistanceMeters(loc);

							if (dist > 5)
								continue;

							if (!(*(bool*)((uintptr_t)actor + DGOffset_bHit)))
							{
								continue;
							}

							if (W2S(loc, sp))
							{
								xDrawText(E(L" x "), sp, Colors::Cyan);
								//EvaluateTarget(loc);
							}
						}
					}
				}

				bAimbotActivated = false;

				if (G::AimbotEnable)
				{
					if (bCaps)
					{
						auto old = AimbotFOV;
						AimbotFOV = 90;
						AimbotFOV = old;
					}
					else
					{
						if (GetAsyncKeyState((int)AimbotKey))
						{
							auto angle = calc_angle(GCameraCache->Location, closestEnemyAss);

							bAimbotActivated = true;
							AimToTarget(angle);
						}
					}
				}
			}

			static char tier_data[256];
			static wchar_t tier_data_wide[256];

			auto color = Colors::LightYellow;

			switch ((int)G::LootTier)
			{
			case 0:
			case 1:
				break;
			case 2:
				color = Colors::LightGreen;
				break;
			case 3:
				color = Colors::DarkCyan;
				break;
			case 4:
				color = Colors::Purple;
				break;
			case 5:
				color = Colors::Orange;
				break;
			}

			sprintf(tier_data, E("Current loot tier (Page Up/Down): %d"), (int)G::LootTier);
			AnsiToWide(tier_data, tier_data_wide);
			K2_DrawText(GCanvas, GetFont(), tier_data_wide, { 30, 220 }, FVector2D(1.0f, 1.0f), color, 1.0f, FLinearColor(0, 0, 0, 255), FVector2D(), false, false, true, FLinearColor(0, 0, 0, 1.0f));

			//sprintf(tier_data, E("Render distance: overall: %d m, loot: %d m"), (int)G::RenderDist, (int)G::LootRenderDist);
		//	AnsiToWide(tier_data, tier_data_wide);
		//	K2_DrawText(GCanvas, GetFont(), tier_data_wide, { 30, 240 }, FVector2D(1.0f, 1.0f), Colors::White, 1.0f, FLinearColor(0, 0, 0, 255), FVector2D(), false, false, true, FLinearColor(0, 0, 0, 1.0f));

			if (closestEnemyDist < 10000)
			{
				//sprintf(tier_data, E("CLOSEST ENEMY: %d m"), closestEnemyDist);
			//	AnsiToWide(tier_data, tier_data_wide);
			//	K2_DrawText(GCanvas, GetFont(), tier_data_wide, { 30, (float)(g_ScreenHeight / 2) }, FVector2D(1.0f, 1.0f), closestEnemyDist < 50 ? Colors::Red : Colors::White, 1.0f, FLinearColor(0, 0, 0, 255), FVector2D(), false, false, true, FLinearColor(0, 0, 0, 1.0f));
			}
			if (g_Menu);
			MwMenuDraw();
		}
		}
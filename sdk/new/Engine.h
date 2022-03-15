#pragma once
#include <math.h>
#pragma warning( disable : 4244 )

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <cstddef>
#include <cstdint>
#include <cmath>
#include "../../global.h"
#include "../../Offsets.h"
#define M_PI 3.14159265
#define RAD_TO_DEG(x) x * (180.f / M_PI)
#define DEG_TO_RAD(x) x * (M_PI / 180.f)

struct Matrix {
	float matrix[16];
};

inline Matrix GetViewMatrix()
{
	uintptr_t view_renderer = *(uintptr_t*)(dwbase + Offsets::view_render);
	if (view_renderer)
	{
		uintptr_t view_matrix_ = *(uintptr_t*)(view_renderer + Offsets::view_matrix);
		if (view_matrix_)
		{
			Matrix view_matrix = *(Matrix*)(view_matrix_);

			return view_matrix;
		}
	}
}

struct Vector4 {
	float x, y, z, w;
};

struct Vector2 {
	float x, y;

	Vector2 operator-(Vector2 ape)
	{
		return { x - ape.x, y - ape.y };
	}

	Vector2 operator+(Vector2 ape)
	{
		return { x + ape.x, y + ape.y };
	}

	Vector2 operator*(float ape)
	{
		return { x * ape, y * ape };
	}

	Vector2 operator/(float ape)
	{
		return { x / ape, y / ape };
	}

	Vector2 operator/=(float ape)
	{
		x /= ape;
		y /= ape;

		return *this;
	}

	Vector2 operator+=(Vector2 ape)
	{
		return { x += ape.x, y += ape.y };
	}

	Vector2 operator-=(Vector2 ape)
	{
		return { x -= ape.x, y -= ape.y };
	}

	void Normalize()
	{
		if (x > 89.0f)
			x -= 180.f;

		if (x < -89.0f)
			x += 180.f;

		if (y > 180.f)
			y -= 360.f;

		if (y < -180.f)
			y += 360.f;
	}

	float Length2D()
	{
		return sqrt((x * x) + (y * y));
	}

	float Dist2D(Vector2 ape)
	{
		return (*this - ape).Length2D();
	}
};

struct Vector3 {
	float x, y, z;

	float distance(Vector3 vec)
	{
		return sqrt(
			pow(vec.x - x, 2) +
			pow(vec.y - y, 2)
		);
	}

	Vector3() : x(0), y(0), z(0) {}

	Vector3(float X, float Y, float Z) : x(X), y(Y), z(Z) {}

	Vector3 operator-(Vector3 ape)
	{
		return { x - ape.x, y - ape.y, z - ape.z };
	}

	Vector3 operator+(Vector3 ape)
	{
		return { x + ape.x, y + ape.y, z + ape.z };
	}

	Vector3 operator*(float ape)
	{
		return { x * ape, y * ape, z * ape };
	}

	Vector3 operator/(float ape)
	{
		return { x / ape, y / ape, z / ape };
	}

	Vector3 operator/=(float ape)
	{
		x /= ape;
		y /= ape;
		z /= ape;

		return *this;
	}

	Vector3 operator+=(Vector3 ape)
	{
		return { x += ape.x, y += ape.y, z += ape.z };
	}

	Vector3 operator-=(Vector3 ape)
	{
		return { x -= ape.x, y -= ape.y, z -= ape.z };
	}

	void Normalize()
	{
		while (x > 89.0f)
			x -= 180.f;

		while (x < -89.0f)
			x += 180.f;

		while (y > 180.f)
			y -= 360.f;

		while (y < -180.f)
			y += 360.f;
	}

	float Length()
	{
		return sqrt((x * x) + (y * y) + (z * z));
	}

	float Length2D()
	{
		return sqrt((x * x) + (y * y));
	}

	float DistTo(Vector3 ape)
	{
		return (*this - ape).Length();
	}

	float Dist2D(Vector3 ape)
	{
		return (*this - ape).Length2D();
	}


	float Distance(Vector3 pos)
	{
		float Dist = sqrt((x - pos.x) * (x - pos.x)
			+ (y - pos.y) * (y - pos.y)
			+ (z - pos.z) * (z - pos.z));
		return Dist * 0.01905f;
	}

	Vector3 CalcAngle(Vector3 dst, float height)
	{
		Vector3 angle, e = { dst.x - x, dst.y - y, dst.z - z };
		float eh = sqrt(e.x * e.x + e.y * e.y);
		angle.x = (atan2(-e.z, eh) * 180 / 3.14159f) + height;
		angle.y = (atan2(e.y, e.x) * 180 / 3.14159f);
		angle.z = 0;
		return angle;
	}

	bool world_to_screen(Vector2& screen) {
		float* m_vMatrix = GetViewMatrix().matrix;
		float w = m_vMatrix[12] * x + m_vMatrix[13] * y + m_vMatrix[14] * z + m_vMatrix[15];

		if (w < 0.01f)
			return false;

		screen.x = m_vMatrix[0] * x + m_vMatrix[1] * y + m_vMatrix[2] * z + m_vMatrix[3];
		screen.y = m_vMatrix[4] * x + m_vMatrix[5] * y + m_vMatrix[6] * z + m_vMatrix[7];

		float invw = 1.0f / w;
		screen.x *= invw;
		screen.y *= invw;

		float x = GetSystemMetrics(SM_CXSCREEN) / 2;
		float y = GetSystemMetrics(SM_CYSCREEN) / 2;

		x += 0.5 * screen.x * GetSystemMetrics(SM_CXSCREEN) + 0.5;
		y -= 0.5 * screen.y * GetSystemMetrics(SM_CYSCREEN) + 0.5;

		screen.x = x;
		screen.y = y;

		if (screen.x > GetSystemMetrics(SM_CXSCREEN) || screen.x < 0 || screen.y > GetSystemMetrics(SM_CYSCREEN) || screen.y < 0)
			return false;

		return true;
	}
};

enum class SignonState : int32_t {
	None,
	Challenge,
	Connected,
	StateNew,
	Prespawn,
	GettingData,
	Spawn,
	FirstSnap,
	Full,
	ChangeLevel,
};

struct NameEntry {
	uint64_t name1;
	uint64_t name2;
};

enum HelmetTypes
{
	HELMSLOT_NONE = 0,
	HELMSLOT_GREY = 1,
	HELMSLOT_BLUE = 2,
	HELMSLOT_PURPLE = 3,
	HELMSLOT_GOLDEN = 4,
};
enum ArmorTypes
{
	ARMORSLOT_NONE = 0,
	ARMORSLOT_GREY = 1,
	ARMORSLOT_BLUE = 2,
	ARMORSLOT_PURPLE = 3,
	ARMORSLOT_GOLDEN = 4,
	ARMORSLOT_MYTHIC = 5
};

enum WeaponIDS
{
	MEELE = 18,

	// Sniper Ammo
	SENTINEL = 1,
	CHARGE_RIFLE = 86,
	LONGBOW = 88,

	// Light Ammo
	RE_45 = 85,
	P2020 = 101,
	R301 = 0,
	R99 = 100,
	ALTERNATOR = 84,

	//Heavy Ammo
	CAR_MP = 107,
	RAMPAGE = 14,
	_30_30 = 106,
	WINGMAN = 104,
	PROWLER = 98,
	HEMLOK = 93,
	FLATLINE = 91,

	// Elektro Ammo
	TRIPLE_TAKE = 103,
	DEVOTION = 87,
	HAVOC = 89,
	L_STAR = 95,

	// Pump Ammo
	MOSAMBIK = 97,
	PEACEKEEPER = 99,
	EVA_8_AUTO = 90,
	MASTIFF = 96,

	// BOW
	BOCEK = 2,

	//MYTHIC
	KRABER = 94,
	G7_SCOUT = 92,
	VOLT = 105,
	SPITFIRE = 102
};

struct WeaponSkinIDs
{
	enum WINGMAN
	{
		DEFAULT = 0,
		MIDNIGHT = 1,
		EYE_OF_THE_STORM = 2,
		TRIAL_BY_FIRE = 3,
		CODE_OF_HONOR = 4,
		OFF_THE_GRID = 5,
		BREAK_THE_ENEMY = 6,
		ARTIC_BLASTER = 7,
		ETERNAL_LIFE = 8,
		CRIMSON_KING = 9,
		DRAGONS_WING = 10,
		GLADIATORS_TROPHY = 11,

	};
};

//Player Stuff
struct colorRGB
{
	float r, g, b;
};

struct glowMode
{
	BYTE GeneralGlowMode, BorderGlowMode, BorderSize, TransparentLevel;
};

struct glowFade
{
	int a, b;
	float c, d, e, f;
};

struct Bone {
	BYTE thing[0xCC];
	float x;
	BYTE thing2[0xC];
	float y;
	BYTE thing3[0xC];
	float z;
};
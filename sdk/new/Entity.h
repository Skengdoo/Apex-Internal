#pragma once
#include <string>
#include "LocalPlayer.h"

inline double GetCrossDistance(double x1, double y1, double x2, double y2)
{
	return sqrt(pow((x2 - x1), 2) + pow((y2 - y1), 2));
}

class BaseEntity {
private:
	DWORD64 EntityList;
	DWORD64 BaseEnt;
	DWORD64 EntityHandle;
	int Index;

	std::string Identifier;

	DWORD64 atIndex(int index)
	{
		Index = index;
		return *(DWORD64*)(EntityList + (index << 5)); //Returns Entity Address
	}


#define NUM_ENT_ENTRIES			(1 << 12)
#define ENT_ENTRY_MASK			(NUM_ENT_ENTRIES - 1)

public:

	DWORD64 Entity;
	LPCSTR IdentifierC = Identifier.c_str();
	glowMode glowStyle = { 101, 101, 46, 90 }; //{ 101, 101, 46, 90 }; //Default Glow is Outline

	explicit BaseEntity(int index)
	{
		EntityList = dwbase + Offsets::cl_entitylist;
		DWORD64 BaseEnt = *(DWORD64*)(EntityList);
		Entity = atIndex(index);
		//if (Entity != 0)
			//EntityHandle = *(DWORD64*)(Entity + 0x580);
	}

	int ItemID()
	{
		return *(int*)(Entity + Offsets::Classes::CPropSurvival::m_customScriptInt);
	}

	std::string GetItemType()
	{
		int id = this->ItemID();

		if (id != 0)
		{
			if (id >= 1 && id <= 131)
				return "Weapon";
			else if (id >= 120 && id <= 124)
				return "Ammo";
			else if (id >= 120 && id <= 125)
				return "Healing";
			else if (id >= 165 && id <= 166)
				return "Shield";
			else if (id >= 137 && id <= 140)
				return "Helmet";
			else if (id >= 171 && id <= 179)
				return "Body Shield";
			else if (id >= 181 && id <= 184)
				return "EVO Shield";
			else if (id >= 185 && id <= 188)
				return "Backpack";
			else if (id >= 189 && id <= 191)
				return "Grenade";
			else if (id >= 192 && id <= 201)
				return "Scope";
			else if (id >= 202 && id <= 244)
				return "Attachment";
			else
				return "";
		}
		else
			return "";
	}

	bool IsDummy()
	{
		//int id = *(int*)(Entity + Offsets::Classes::CPropSurvival::m_customScriptInt);
		return (bool)(strcmp(IdentifierC, "npc_dummie"));//id == 412653360 ? true : false;
	}

	bool IsDrone()
	{
		return !isPlayer() && !IsDummy() ? true : false;
	}

	bool isPlayer()
	{
		return (bool)strcmp(IdentifierC, "player");
	}

	bool IsAlive()
	{
		return *(int*)(Entity + Offsets::Classes::CPlayer::m_iHealth) != 0;
	}

	std::string identifier()
	{
		//return *(std::string*)(Entity + 0x589);
	}

	void PredictPosition(LocalPlayer pLocal, Vector3* bonePos)
	{
		uintptr_t actWeaponID = *(uintptr_t*)(pLocal.Entity + Offsets::Classes::CPlayer::m_latestPrimaryWeapons) & 0xFFFF;
		uintptr_t currentWeapon = *(uintptr_t*)(EntityList + (actWeaponID << 5));

		if (currentWeapon != 0)
		{
			float bulletSpeed = *(float*)(currentWeapon + Offsets::Classes::CWeaponX::m_bulletSpeed);
			float bulletGravity = *(float*)(currentWeapon + Offsets::Classes::CWeaponX::m_bulletGravity);

			if (bulletSpeed > 1.f)
			{
				Vector3 muzzle = *(Vector3*)(pLocal.Entity + 0x1f20);
				float time = bonePos->DistTo(muzzle) / bulletSpeed;
				bonePos->z += (700.f * bulletGravity * 0.5f) * (time * time);
				Vector3 velDelta = (*(Vector3*)(Entity + Offsets::Classes::CPlayer::m_vecAbsOrigin - 0xC) * time);
				bonePos->x += velDelta.x;
				bonePos->y += velDelta.y;
				bonePos->z += velDelta.z;
			}
		}
	}

	Vector3 Position() {
		return *(Vector3*)(Entity + Offsets::Classes::CPlayer::m_vecAbsOrigin);
	}

	Vector2 Position2D() {
		Vector3 pos = *(Vector3*)(Entity + Offsets::Classes::CPlayer::m_vecAbsOrigin);
		Vector2 _2d_;
		pos.world_to_screen(_2d_);
		return _2d_;
	}

	Vector3 BonePosition(int boneIndex)
	{
		Bone bone = {};
		Vector3 vec_bone = Vector3();
		Vector3 pos = Position();

		ULONG64 bone_array = *(ULONG64*)(Entity + Offsets::Classes::CRopeKeyframe::m_bConstrainBetweenEndpoints);
		ULONG64 bone_location = (boneIndex * 0x30);

		bone = *(Bone*)(bone_array + bone_location);
		vec_bone.x = bone.x + pos.x;
		vec_bone.y = bone.y + pos.y;
		vec_bone.z = bone.z + pos.z;
		return vec_bone;
	}

	Vector2 BonePosition2D(int boneIndex)
	{
		Bone bone = {};
		Vector3 vec_bone = Vector3();
		Vector3 pos = Position();

		ULONG64 bone_array = *(ULONG64*)(Entity + Offsets::Classes::CRopeKeyframe::m_bConstrainBetweenEndpoints);
		ULONG64 bone_location = (boneIndex * 0x30);

		bone = *(Bone*)(bone_array + bone_location);
		vec_bone.x = bone.x + pos.x;
		vec_bone.y = bone.y + pos.y;
		vec_bone.z = bone.z + pos.z;

		Vector2 _2d_;
		vec_bone.world_to_screen(_2d_);
		return _2d_;
	}

	int iHealth() {
		return *(int*)(Entity + Offsets::Classes::CPlayer::m_iHealth);
	}

	int iShield() {
		return *(int*)(Entity + Offsets::Classes::CBaseEntity::m_shieldHealth);
	}

	int iTeam() {
		return *(int*)(Entity + Offsets::Classes::CBaseEntity::m_iTeamNum);
	}

	int isKnocked() {
		return *(int*)(Entity + Offsets::Classes::CPlayer::m_bleedoutState);
	}

	CWeapon ActiveWeapon()
	{
		uintptr_t pWeap = *(uintptr_t*)(Entity + Offsets::Classes::CPlayer::m_latestPrimaryWeapons);
		pWeap &= 0xffff;
		uintptr_t weap_handle = *(uintptr_t*)(dwbase + Offsets::cl_entitylist + (pWeap << 5));

		return CWeapon(weap_handle);
	}

	DWORD ModelNamePTR()
	{
		if (!Entity)
			return -1;

		return *(DWORD64*)(Entity + 0x40);
	}

	char ModelName()
	{
		char szModelName;
		szModelName = *(char*)(this->ModelNamePTR() + 0x4);

		return szModelName;
	}

	int ArmorType()
	{
		if (!Entity)
			return -1;
		return *(int*)(Entity + Offsets::Classes::CPlayer::m_armorType);
	}

	int HelmetType()
	{
		if (!Entity)
			return -1;
		return *(int*)(Entity + Offsets::Classes::CPlayer::m_helmetType);
	}

	std::string PlayerName()
	{
		if (!Entity)
			return "";
		int INDEX = *(int*)(*(unsigned char*)(Entity + 0x8) - 0x10000);
		//DWORD64 NameIndex = Driver::Read<DWORD64>(Entity + 0x0694);
		DWORD64 namelist = *(DWORD64*)(dwbase + Offsets::name_list);
		DWORD64 nameent = *(DWORD64*)(namelist + ((INDEX - 1) << 4));
		std::string name = *(std::string*)(nameent);
		return name;
	}

	uintptr_t Observer()
	{
		unsigned long ot = *(unsigned long*)(Entity + Offsets::Classes::CPlayer::m_hObserverTarget);
		int index = ot & ENT_ENTRY_MASK;
		int ObserverID = index;
		if (ObserverID > 0) {
			uint64_t centity2 = *(uint64_t*)(Offsets::cl_entitylist + ((uint64_t)ObserverID << 5));
			return centity2;
		}
		return 0;
	}

	void Glow(float r, float g, float b, float a)
	{
		float time = 5000.f;

		colorRGB color = { r * a / 255, g * a / 255, b * a / 255 };
		*(glowMode*)(Entity + Offsets::glow_type) = glowStyle;
		*(colorRGB*)(Entity + Offsets::glow_color) = color;
		*(float*)(Entity + Offsets::glow_distance) = 40000.f;
		*(float*)(Entity + Offsets::glow_lifetime) = time;
		time -= 1.f;
		*(int*)(Entity + Offsets::glow_enable) = 1;
		*(int*)(Entity + Offsets::glow_visible_type) = 1;
		*(glowFade*)(Entity + Offsets::glow_fade) = { 872415232, 872415232, time, time, time, time };
	}

	void GlowOff()
	{
		colorRGB color = { 255, 255, 255 };

		*(int*)(Entity + Offsets::glow_enable) = 0;
		*(int*)(Entity + Offsets::glow_visible_type) = 0;
		//Driver::write<colorRGB>(Entity + Offsets::glow_color, color);
	}

	void setGlowStyle(int GeneralGlowMode, int BorderGlowMode, int BorderSize, int TransparentLevel) //Should only be used for python userscripts
	{
		glowStyle = { (unsigned char)GeneralGlowMode, (unsigned char)BorderGlowMode, (unsigned char)BorderSize, (unsigned char)TransparentLevel };
	}


	bool IsVisible(float curtime)
	{
		float last_visible_time = *(float*)(Entity + Offsets::Classes::CPlayer::m_lastVisibleTime);
		return last_visible_time > 0.0f && fabsf(last_visible_time - curtime) < 0.1f;
	}
};
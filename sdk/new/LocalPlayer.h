#pragma once
#pragma once
#include "Engine.h"
#include "Weapon.h"

class LocalPlayer {
public:

	uintptr_t Entity;

	explicit LocalPlayer()
	{
		Entity = *(uintptr_t*)(dwbase + Offsets::local_player);
	}

	Vector3 Position() {
		return *(Vector3*)(Entity + Offsets::Classes::CPlayer::m_vecAbsOrigin);
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

	Vector3 getViewAngles() {
		return *(Vector3*)(Entity + Offsets::Classes::CPlayer::m_view_angles);
	}

	void setViewAngles(Vector3 angles) {
		*(Vector3*)(Entity + Offsets::Classes::CPlayer::m_view_angles) = angles;
	}

	Vector3 getAimPunch() {
		return *(Vector3*)(Entity + Offsets::Classes::CPlayer::m_vecPunchWeapon_Angle);
	}

	Vector3 getBreathAngles() {
		return *(Vector3*)(Entity + Offsets::Classes::CPlayer::m_breath_angles);
	}

	Vector3 getRecoil() {
		return *(Vector3*)(Entity + Offsets::Classes::CPlayer::m_vecPunchWeapon_Angle);
	}

	Vector3 getCamPosition() {
		return *(Vector3*)(Entity + 0x1f20);
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

	CWeapon ActiveWeapon()
	{
		uintptr_t pWeap = *(uintptr_t*)(Entity + Offsets::Classes::CPlayer::m_latestPrimaryWeapons);
		pWeap &= 0xffff;
		uintptr_t weap_handle = *(uintptr_t*)(dwbase + Offsets::cl_entitylist + (pWeap << 5));

		return CWeapon(weap_handle);
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

	DWORD ModelNamePTR()
	{
		if (!Entity)
			return -1;

		return *(DWORD64*)(Entity + 0x30);
	}

	char ModelName()
	{
		char szModelName;
		szModelName = *(char*)(this->ModelNamePTR());

		return szModelName;
	}

	int ModelIndex()
	{
		return *(int*)(Entity + Offsets::Classes::CPropSurvival::m_nModelIndex);
	}


};
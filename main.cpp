#include <vector>
//#include "global.h"
#include "sdk/new/Entity.h"

// test
Present oPresent;
WNDPROC oWndProc;
HWND window = NULL;
ID3D11Device* pDevice = NULL;
ID3D11DeviceContext* pContext = NULL;
ID3D11RenderTargetView* mainRenderTargetView;

uintptr_t LP;
uintptr_t activeweap;

bool show_main = false;

bool Thirdperson = false;
bool skinscroller = false;
bool test = false;
bool NoRecoil = false;
float rcs_amount = 1.f;


bool weaponskinchanger = false;
int weaponskin_id = 0;


bool enable_aimbot = false;
bool aimbot_drawfov = false;
float aimbot_fov = false;
bool aimbot_norecoil = false;
bool aimbot_onlyvisible = true;
bool aimbot_onlyenemys = true;
float aimbot_smoothness = 1.f;

bool Radar = false;
bool Radar_onlyenemys = false;
bool Radar_viewcheck = false;
bool Radar_visibleonly = false;
int Radar_type = 4;

bool chams = false;
bool lineesp = false;
bool boxesp = false;
float esp_range = 250.f;

bool debugitemids = false;
bool itemesp = false;

SignonState signon_state;
SignonState old_signon_state;
bool connected;
char level_buffer;

bool Unload = false;

HWND g_GameHWND = NULL;


void DrawLine(const ImVec2& aPoint1, const ImVec2 aPoint2, ImU32 aColor, const float aLineWidth)
{
	auto vList = ImGui::GetBackgroundDrawList();
	vList->AddLine(aPoint1, aPoint2, aColor, aLineWidth);
}

void DrawCorneredBox(float X, float Y, float W, float H, const ImU32& color, float thickness)
{
	auto vList = ImGui::GetBackgroundDrawList();

	float lineW = (W / 3);
	float lineH = (H / 3);
	//black outlines
	auto col = ImGui::GetColorU32(color);

	//corners
	vList->AddLine(ImVec2(X, Y - thickness / 2), ImVec2(X, Y + lineH), col, thickness);//top left
	vList->AddLine(ImVec2(X - thickness / 2, Y), ImVec2(X + lineW, Y), col, thickness);

	vList->AddLine(ImVec2(X + W - lineW, Y), ImVec2(X + W + thickness / 2, Y), col, thickness);//top right horizontal
	vList->AddLine(ImVec2(X + W, Y - thickness / 2), ImVec2(X + W, Y + lineH), col, thickness);

	vList->AddLine(ImVec2(X, Y + H - lineH), ImVec2(X, Y + H + (thickness / 2)), col, thickness);//bot left
	vList->AddLine(ImVec2(X - thickness / 2, Y + H), ImVec2(X + lineW, Y + H), col, thickness);

	vList->AddLine(ImVec2(X + W - lineW, Y + H), ImVec2(X + W + thickness / 2, Y + H), col, thickness);//bot right
	vList->AddLine(ImVec2(X + W, Y + H - lineH), ImVec2(X + W, Y + H + (thickness / 2)), col, thickness);

}
int ScreenWidth = GetSystemMetrics(SM_CXSCREEN); int screenX = ScreenWidth / 2;
int ScreenHeight = GetSystemMetrics(SM_CYSCREEN); int screenY = ScreenHeight / 2;

std::vector<std::string> hwids;
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

/*
inline bool w2s(c_vec source, c_vec& destination) {
	return o_worldtoscreen(source, destination);
}

uintptr_t ActiveWeapon(uintptr_t entity)
{
	uintptr_t pWeap = *(uintptr_t*)(entity + Offsets::Classes::CPlayer::m_latestPrimaryWeapons);
	pWeap &= 0xffff;
	return *(uintptr_t*)(dwbase + Offsets::cl_entitylist + (pWeap << 5));;
}
*/

DWORD GetModuleSize(DWORD64 base)
{
	IMAGE_DOS_HEADER dos_header = { 0 };
	IMAGE_NT_HEADERS nt_headers = { 0 };
	if (!base)return -1;
	dos_header = *(IMAGE_DOS_HEADER*)base;
	nt_headers = *(IMAGE_NT_HEADERS*)(base + dos_header.e_lfanew);
	return nt_headers.OptionalHeader.SizeOfImage;
}



typedef struct
{
	DWORD64 dwEP;
	void* pParam;
}CALL_MYFUNCTION, * PCALL_MYFUNCTION;
typedef DWORD(*_Function)(VOID* p);

void InitImGui()
{
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags = ImGuiConfigFlags_NoMouseCursorChange;

	//io.IniFilename = "../gaypex.ini";
	io.IniFilename = NULL;

	//ImGui::StyleColorsDark();

	ImGui_ImplWin32_Init(window);
	ImGui_ImplDX11_Init(pDevice, pContext);
}

LRESULT __stdcall WndProc(const HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

	if (true && ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam))
		return true;

	return CallWindowProc(oWndProc, hWnd, uMsg, wParam, lParam);
}

void WINAPI MyFunctionThread(PCALL_MYFUNCTION pCMF)
{
	if (pCMF != NULL && pCMF->dwEP != NULL)
	{
		_Function Function = (_Function)pCMF->dwEP;
		Function(pCMF->pParam);
	}
}

HANDLE MyCreateThread(LPTHREAD_START_ROUTINE lpStartAddress, LPVOID lpParameter, LPDWORD lpThreadId)
{
	HMODULE hNtDll = GetModuleHandleA("ntdll.dll");
	if (hNtDll != NULL)
	{
		DWORD dwImageSize = GetModuleSize((DWORD64)hNtDll);
		BYTE* pMemoryData = (BYTE*)hNtDll + dwImageSize - 0x400;

		if (pMemoryData != NULL)
		{
			DWORD dwProtect;
			VirtualProtect(pMemoryData, 0x100, PAGE_EXECUTE_READWRITE, &dwProtect);
			CALL_MYFUNCTION* pCMF = (CALL_MYFUNCTION*)VirtualAlloc(NULL, 0x100, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
			pCMF->dwEP = (DWORD64)(lpStartAddress);
			pCMF->pParam = lpParameter;
			memcpy((LPVOID)pMemoryData, (LPVOID)MyFunctionThread, 0x100);
			HANDLE hHandle = CreateRemoteThread(GetCurrentProcess(), NULL, 0, (LPTHREAD_START_ROUTINE)pMemoryData, pCMF, NULL, lpThreadId);
			return hHandle;
		}
	}
	return 0;
}

inline bool is_in_game() {
	return signon_state == SignonState::Full && level_buffer != '\0' && !!strcmp((const char*)level_buffer, "mp_lobby");
}

inline const char* level_name() {
	return &level_buffer;
}

int AimbotKey = 0x02;
const char* const KeyNames[] = {
	"Unknown",
	"VK_LBUTTON",
	"VK_RBUTTON",
	"VK_CANCEL",
	"VK_MBUTTON",
	"VK_XBUTTON1",
	"VK_XBUTTON2",
	"Unknown",
	"VK_BACK",
	"VK_TAB",
	"Unknown",
	"Unknown",
	"VK_CLEAR",
	"VK_RETURN",
	"Unknown",
	"Unknown",
	"VK_SHIFT",
	"VK_CONTROL",
	"VK_MENU",
	"VK_PAUSE",
	"VK_CAPITAL",
	"VK_KANA",
	"Unknown",
	"VK_JUNJA",
	"VK_FINAL",
	"VK_KANJI",
	"Unknown",
	"VK_ESCAPE",
	"VK_CONVERT",
	"VK_NONCONVERT",
	"VK_ACCEPT",
	"VK_MODECHANGE",
	"VK_SPACE",
	"VK_PRIOR",
	"VK_NEXT",
	"VK_END",
	"VK_HOME",
	"VK_LEFT",
	"VK_UP",
	"VK_RIGHT",
	"VK_DOWN",
	"VK_SELECT",
	"VK_PRINT",
	"VK_EXECUTE",
	"VK_SNAPSHOT",
	"VK_INSERT",
	"VK_DELETE",
	"VK_HELP",
	"0",
	"1",
	"2",
	"3",
	"4",
	"5",
	"6",
	"7",
	"8",
	"9",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"A",
	"B",
	"C",
	"D",
	"E",
	"F",
	"G",
	"H",
	"I",
	"J",
	"K",
	"L",
	"M",
	"N",
	"O",
	"P",
	"Q",
	"R",
	"S",
	"T",
	"U",
	"V",
	"W",
	"X",
	"Y",
	"Z",
	"VK_LWIN",
	"VK_RWIN",
	"VK_APPS",
	"Unknown",
	"VK_SLEEP",
	"VK_NUMPAD0",
	"VK_NUMPAD1",
	"VK_NUMPAD2",
	"VK_NUMPAD3",
	"VK_NUMPAD4",
	"VK_NUMPAD5",
	"VK_NUMPAD6",
	"VK_NUMPAD7",
	"VK_NUMPAD8",
	"VK_NUMPAD9",
	"VK_MULTIPLY",
	"VK_ADD",
	"VK_SEPARATOR",
	"VK_SUBTRACT",
	"VK_DECIMAL",
	"VK_DIVIDE",
	"VK_F1",
	"VK_F2",
	"VK_F3",
	"VK_F4",
	"VK_F5",
	"VK_F6",
	"VK_F7",
	"VK_F8",
	"VK_F9",
	"VK_F10",
	"VK_F11",
	"VK_F12",
	"VK_F13",
	"VK_F14",
	"VK_F15",
	"VK_F16",
	"VK_F17",
	"VK_F18",
	"VK_F19",
	"VK_F20",
	"VK_F21",
	"VK_F22",
	"VK_F23",
	"VK_F24",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"VK_NUMLOCK",
	"VK_SCROLL",
	"VK_OEM_NEC_EQUAL",
	"VK_OEM_FJ_MASSHOU",
	"VK_OEM_FJ_TOUROKU",
	"VK_OEM_FJ_LOYA",
	"VK_OEM_FJ_ROYA",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"VK_LSHIFT",
	"VK_RSHIFT",
	"VK_LCONTROL",
	"VK_RCONTROL",
	"VK_LMENU",
	"VK_RMENU"
};

inline ImVec2 Add_ImVec2(ImVec2 a, ImVec2 b)
{
	return ImVec2(a.x + b.x, a.y + b.y);
}

inline ImVec2 Minus_ImVec2(ImVec2 a, ImVec2 b)
{
	return ImVec2(a.x - b.x, a.y - b.y);
}

bool Hotkey(const char* label, int* k, const ImVec2& size_arg)
{
	ImGuiWindow* window = ImGui::GetCurrentWindow();
	if (window->SkipItems)
		return false;

	ImGuiContext& g = *GImGui;
	ImGuiIO& io = g.IO;
	const ImGuiStyle& style = g.Style;

	const ImGuiID id = window->GetID(label);
	const ImVec2 label_size = ImGui::CalcTextSize(label, NULL, true);
	ImVec2 size = ImGui::CalcItemSize(size_arg, ImGui::CalcItemWidth(), label_size.y + style.FramePadding.y * 2.0f);
	ImVec2 fbbgfkgjg = Add_ImVec2(window->DC.CursorPos, ImVec2(label_size.x + style.ItemInnerSpacing.x, 0.0f));
	const ImRect frame_bb(fbbgfkgjg, Add_ImVec2(window->DC.CursorPos, size));
	const ImRect total_bb(window->DC.CursorPos, frame_bb.Max);

	ImGui::ItemSize(total_bb, style.FramePadding.y);
	if (!ImGui::ItemAdd(total_bb, id))
		return false;

	const bool hovered = ImGui::ItemHoverable(frame_bb, id);

	if (hovered) {
		ImGui::SetHoveredID(id);
		g.MouseCursor = ImGuiMouseCursor_TextInput;
	}

	const bool user_clicked = hovered && io.MouseClicked[0];

	if (user_clicked) {
		if (g.ActiveId != id) {
			// Start edition
			memset(io.MouseDown, 0, sizeof(io.MouseDown));
			memset(io.KeysDown, 0, sizeof(io.KeysDown));
			*k = 0;
		}
		ImGui::SetActiveID(id, window);
		ImGui::FocusWindow(window);
	}
	else if (io.MouseClicked[0]) {
		// Release focus when we click outside
		if (g.ActiveId == id)
			ImGui::ClearActiveID();
	}

	bool value_changed = false;
	int key = *k;

	if (g.ActiveId == id) {
		for (auto i = 0; i < 5; i++) {
			if (io.MouseDown[i]) {
				switch (i) {
				case 0:
					key = VK_LBUTTON;
					break;
				case 1:
					key = VK_RBUTTON;
					break;
				case 2:
					key = VK_MBUTTON;
					break;
				case 3:
					key = VK_XBUTTON1;
					break;
				case 4:
					key = VK_XBUTTON2;
					break;
				}
				value_changed = true;
				ImGui::ClearActiveID();
			}
		}
		if (!value_changed) {
			for (auto i = VK_BACK; i <= VK_RMENU; i++) {
				if (io.KeysDown[i]) {
					key = i;
					value_changed = true;
					ImGui::ClearActiveID();
				}
			}
		}

		if (ImGui::IsKeyPressedMap(ImGuiKey_Escape)) {
			*k = 0;
			ImGui::ClearActiveID();
		}
		else {
			*k = key;
		}
	}

	// Render
	// Select which buffer we are going to display. When ImGuiInputTextFlags_NoLiveEdit is Set 'buf' might still be the old value. We Set buf to NULL to prevent accidental usage from now on.

	char buf_display[64] = "None";

	ImGui::RenderFrame(frame_bb.Min, frame_bb.Max, ImGui::GetColorU32(ImVec4(0.20f, 0.25f, 0.30f, 1.0f)), true, style.FrameRounding);

	if (*k != 0 && g.ActiveId != id) {
		strcpy_s(buf_display, KeyNames[*k]);
	}
	else if (g.ActiveId == id) {
		strcpy_s(buf_display, "<Press a key>");
	}

	const ImRect clip_rect(frame_bb.Min.x, frame_bb.Min.y, frame_bb.Min.x + size.x, frame_bb.Min.y + size.y); // Not using frame_bb.Max because we have adjusted size
	ImVec2 render_pos = Add_ImVec2(frame_bb.Min, style.FramePadding);
	ImGui::RenderTextClipped(Add_ImVec2(frame_bb.Min, style.FramePadding), Minus_ImVec2(frame_bb.Max, style.FramePadding), buf_display, NULL, NULL, style.ButtonTextAlign, &clip_rect);
	//RenderTextClipped(frame_bb.Min + style.FramePadding, frame_bb.Max - style.FramePadding, buf_display, NULL, NULL, GetColorU32(ImGuiCol_Text), style.ButtonTextAlign, &clip_rect);
	//draw_window->DrawList->AddText(g.Font, g.FontSize, render_pos, GetColorU32(ImGuiCol_Text), buf_display, NULL, 0.0f, &clip_rect);

	if (label_size.x > 0)
		ImGui::RenderText(ImVec2(total_bb.Min.x, frame_bb.Min.y + style.FramePadding.y), label);

	return value_changed;
}

bool IsInGame(void)
{
	//typedef bool(__thiscall* OriginalFn)(void*);
	//return vfunc<OriginalFn>(this, 45)(this);
}

DWORD ImGuiMain_threadID;
DWORD MainThread_threadID;
DWORD GlowThread_threadID;
DWORD weapon_threadID;

int skin_loop_index = 0;
int currentSkinID = 0;

bool thirdperson_onit = true;
void WINAPI thread_Main()
{
	while (!Unload)
	{
		LP = *(uintptr_t*)(dwbase + Offsets::local_player);

		LocalPlayer LE = LocalPlayer();

		activeweap = LE.ActiveWeapon().Handle;

		old_signon_state = signon_state;

	

		signon_state = *(SignonState*)(dwbase + Offsets::client_state + Offsets::sign_on_state);
		level_buffer = *(char*)(dwbase + Offsets::client_state + Offsets::level_name);

		connected = old_signon_state != SignonState::Full && signon_state == SignonState::Full;

		if (LP)
		{
			//if (connected)
			{

				if (test) {
				//	*(bool*)(dwbase + Offsets::Offset_SendPacket) = false;
				}

				if (Thirdperson)
				{
					*(int*)(dwbase + Offsets::thirdperson_override) = 1;
					*(int*)(LP + Offsets::m_thirdPersonShoulderView) = 1;
					thirdperson_onit = true;
				}
				else
				{
					if (thirdperson_onit)
					{
						*(int*)(dwbase + Offsets::thirdperson_override) = 0;
						*(int*)(LP + Offsets::m_thirdPersonShoulderView) = 0;
						thirdperson_onit = false;
					}
				}

				if (weaponskinchanger)
				{
					LE.ActiveWeapon().ChangeSkin(weaponskin_id);
				}

				if (skinscroller)
				{
					*(int*)(LP + Offsets::Classes::CPropSurvival::m_nSkin) = currentSkinID;
					LE.ActiveWeapon().ChangeSkin(currentSkinID);	

					if (skin_loop_index > (480000 * 8)) {
						skin_loop_index = 0;
						currentSkinID++;
						if (currentSkinID > 12) {
							currentSkinID = 0;
						}
					}
					skin_loop_index++;
				}
			}
		}
	}
}

Vector3 oldAngles = Vector3();
Vector3 oPunch = { 0, 0, 0 };
uintptr_t bestEntity = NULL;
int closestEnt = 99999;
Vector3 OldPunch{ 0, 0, 0 };
void WINAPI thread_weapon()
{
	while (!Unload)
	{
		if (LP)
		{
			//if (connected)
			{
				LocalPlayer Localplayer = LocalPlayer();

				if (Localplayer.Entity)
				{
					if (NoRecoil)
					{

						Vector3 punch = Localplayer.getAimPunch() * (0.500f * 2);
						if (GetAsyncKeyState(VK_LBUTTON))
						{
							Vector3 view_angles = Localplayer.getViewAngles();

							Vector3 newAngle = view_angles + oPunch - punch;

							//newAngle.normalize();

							Localplayer.setViewAngles(newAngle);
						}
						oPunch = punch;
					}

					if (enable_aimbot)
					{
						for (int i = 0; i < 100; i++)
						{
							BaseEntity entity = BaseEntity(i);
							if (!entity.Entity || entity.Entity == LP || !LP)
								continue;

							float distance = entity.Position().Distance(Localplayer.Position());

							//if (distance > esp_range)
								//continue;

							if (!entity.IsAlive())
								continue;

							//if (!entity.isKnocked() == 0)
								//continue;

							if (aimbot_onlyenemys && entity.iTeam() == Localplayer.iTeam())
								continue;

							CGlobalVars globalvars; globalvars = *(CGlobalVars*)(dwbase + Offsets::global_vars);

							if (aimbot_onlyvisible && !entity.IsVisible(globalvars.curtime))
								continue;

							Vector2 Screen = entity.Position2D();

							Vector3 pos = entity.BonePosition(5);

							entity.PredictPosition(Localplayer, &pos);

							//pos.z = pos.z - 0.75f;
							//if (strcmp(entity.identifier().c_str(), "npc_dummie") ||*/ strcmp(entity.identifier().c_str(), "player"))
							{
								float Dist = sqrt(powf(Screen.x - screenX / 2, 2) + powf(Screen.y - screenY / 2, 2));
								if ((bestEntity == entity.Entity || Dist < closestEnt) && entity.IsAlive())
								{
									if (aimbot_drawfov)
									{
										if (abs(ScreenWidth / 2 - Screen.x) < aimbot_fov && abs(ScreenHeight / 2 - Screen.y) < aimbot_fov)
										{

											bestEntity = entity.Entity;
											closestEnt = Dist;

											//int modelindex = Driver::Read<int>(entity.Entity + Offsets::Classes::CBaseEntity::m_nModelIndex);
											//std::cout << modelindex << std::endl;

											Vector3 tempangles = Localplayer.BonePosition(8).CalcAngle(pos, 0);

											if (GetAsyncKeyState(AimbotKey))
											{
												Vector3 CurrAngles = Localplayer.getViewAngles();//Entity::GetViewAngles(localPlayer);
												Vector3 AimRecoilPunch = Localplayer.getAimPunch();
												Vector3 punchangle = AimRecoilPunch * (0.500f * 2);
												Vector3 Delta = tempangles - CurrAngles;
												if (aimbot_norecoil)
												{
													Vector3 aimangles = tempangles + OldPunch - punchangle;
													Localplayer.setViewAngles(aimangles);
												}
												else
												{
													Vector3 aimangles = tempangles + OldPunch - punchangle;
													Vector3 Delta = aimangles - CurrAngles;

													//Delta.normalize();

													//Localplayer.setViewAngles(CurrAngles + Delta / (aimbot_smoothness * 20));
													Localplayer.setViewAngles(tempangles);
												}

											}
										}
									}
									else
									{
										bestEntity = entity.Entity;
										closestEnt = Dist;

										Vector3 tempangles = Localplayer.BonePosition(8).CalcAngle(pos, 0);

										if (GetAsyncKeyState(AimbotKey))
										{
											Vector3 CurrAngles = Localplayer.getViewAngles();//Entity::GetViewAngles(localPlayer);
											Vector3 AimRecoilPunch = Localplayer.getAimPunch();
											Vector3 punchangle = AimRecoilPunch * (0.500f * 2);

											if (aimbot_norecoil)
											{
												Vector3 aimangles = tempangles + OldPunch - punchangle;
												Localplayer.setViewAngles(aimangles);
											}
											else
											{
												Vector3 aimangles = tempangles + OldPunch - punchangle;
												Vector3 Delta = aimangles - CurrAngles;

												//Delta.normalize();

												Localplayer.setViewAngles(tempangles);
												//Localplayer.setViewAngles(CurrAngles + Delta / (aimbot_smoothness * 20));
											}
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}
}

bool IsInScreen(Vector2 aPos)
{
	return !(aPos.x > ScreenWidth || aPos.x<0 || aPos.y>ScreenHeight || aPos.y < 0);
}



float Distance(Vector3 EntityPos, Vector3 PlayerPos)
{
	float Dist = sqrt((PlayerPos.x - EntityPos.x) * (PlayerPos.x - EntityPos.x)
		+ (PlayerPos.y - EntityPos.y) * (PlayerPos.y - EntityPos.y)
		+ (PlayerPos.z - EntityPos.z) * (PlayerPos.z - EntityPos.z));
	return Dist * 0.01905f;
}

Vector3 RotatePoint(Vector3 EntityPos, Vector3 LocalPlayerPos, int posX, int posY, int sizeX, int sizeY, float angle, float zoom, bool* viewCheck, bool angleInRadians = false)
{
	float r_1, r_2;
	float x_1, y_1;

	r_1 = -(EntityPos.y - LocalPlayerPos.y);
	r_2 = EntityPos.x - LocalPlayerPos.x;

	float Yaw = angle - 90.0f;
	float yawToRadian = Yaw * (float)(M_PI / 180.0F);
	x_1 = (float)(r_2 * (float)cos((double)(yawToRadian)) - r_1 * sin((double)(yawToRadian))) / 20;
	y_1 = (float)(r_2 * (float)sin((double)(yawToRadian)) + r_1 * cos((double)(yawToRadian))) / 20;
	*viewCheck = y_1 < 0;
	x_1 *= zoom;
	y_1 *= zoom;
	int sizX = sizeX / 2;
	int sizY = sizeY / 2;
	x_1 += sizX;

	y_1 += sizY;
	if (x_1 < 5)
		x_1 = 5;
	if (x_1 > sizeX - 5)
		x_1 = sizeX - 5;
	if (y_1 < 5)
		y_1 = 5;
	if (y_1 > sizeY - 5)
		y_1 = sizeY - 5;
	x_1 += posX;
	y_1 += posY;
	return Vector3(x_1, y_1, 0);
}
ImU32 GetU32(int r, int b, int g, int a)
{
	return ((a & 0xff) << 24) + ((g & 0xff) << 16) + ((b & 0xff) << 8)
		+ (r & 0xff);
}
void DrawRadar()
{
	ImGuiStyle& style = ImGui::GetStyle();
	ImVec2 oldPadding = style.WindowPadding;
	float oldAlpha = style.Colors[ImGuiCol_WindowBg].w;
	style.WindowPadding = ImVec2(0, 0);
	ImGui::GetStyle().Alpha = 1.f;
	ImGui::SetWindowSize(ImVec2(200, 200));
	if (ImGui::Begin("Radar", 0, ImGuiWindowFlags_NoTitleBar))
	{
		ImVec2 siz = ImGui::GetWindowSize();
		ImVec2 pos = ImGui::GetWindowPos();

		ImDrawList* windowDrawList = ImGui::GetWindowDrawList();
		windowDrawList->AddLine(ImVec2(pos.x + (siz.x / 2), pos.y + 0), ImVec2(pos.x + (siz.x / 2), pos.y + siz.y), GetU32(255, 255, 255, 255), 1.5f);
		windowDrawList->AddLine(ImVec2(pos.x + 0, pos.y + (siz.y / 2)), ImVec2(pos.x + siz.x, pos.y + (siz.y / 2)), GetU32(255, 255, 255, 255), 1.5f);
		if (LP)
		{
			//if (connected)
			{
				LocalPlayer LE = LocalPlayer();
				if (LE.Entity)
				{
					Vector3 LocalPos = LE.BonePosition(8);
					Vector3 shit = LE.getViewAngles();
					Vector3 ang = Vector3(shit.x, shit.y, shit.z);

					for (int i = 1; i < 100; i++)
					{
						BaseEntity pBaseEntity = BaseEntity(i);

						if (!pBaseEntity.Entity || !LP || pBaseEntity.Entity == LP)
							continue;
						//std::string m_iSignifierName = pBaseEntity.identifier();

						//if (strcmp(m_iSignifierName.c_str(), "player"))
						{
							if (!pBaseEntity.Entity || pBaseEntity.Entity == LP || !pBaseEntity.IsAlive())
								continue;


							float distance = Distance(pBaseEntity.Position(), LocalPos);
							if (distance > 250.f)
								continue;

							CGlobalVars globalvars; globalvars = *(CGlobalVars*)(dwbase + Offsets::global_vars);

							bool bIsEnemy = LE.iTeam() != pBaseEntity.iTeam();
							bool isVisibled = pBaseEntity.IsVisible(globalvars.curtime);
							if (Radar_onlyenemys && !bIsEnemy)
								continue;
							bool viewCheck = false;
							Vector3 EntityPos = RotatePoint(pBaseEntity.Position(), LocalPos, pos.x, pos.y, siz.x, siz.y, ang.y, 1.5f, &viewCheck);
							if (!viewCheck && Radar_viewcheck)
								isVisibled = false;
							ImU32 clr = bIsEnemy ? (isVisibled ? GetU32(0, 255, 0, 255) : GetU32(255, 255, 255, 255)) : GetU32(0, 0, 255, 255);
							if (Radar_visibleonly && !isVisibled)
								continue;
							int s = 4;
							windowDrawList->AddCircleFilled(ImVec2(EntityPos.x, EntityPos.y), s, clr);
						}
					}
				}
			}
		}
	}
	ImGui::End();
	style.WindowPadding = oldPadding;
	ImGui::GetStyle().Alpha = oldAlpha;
}

void Line(int newId)
{
	std::string id = ("imguipp_line_" + std::to_string(newId));
	//ImGui::PushStyleColor(ImGuiCol_ChildBg, ImColor_ImVec4(col));
	{
		ImGui::BeginChild(id.c_str(), ImVec2(ImGui::GetContentRegionAvail().x, 1), false);
		ImGui::Separator();
		ImGui::EndChild();
	}
	//ImGui::PopStyleColor();
}

void InitMenuColors()
{
	ImGuiStyle* style = &ImGui::GetStyle();

	style->WindowPadding = ImVec2(15, 15);
	style->WindowRounding = 5.0f;
	style->FramePadding = ImVec2(5, 5);
	style->FrameRounding = 0.0f;
	style->ItemSpacing = ImVec2(12, 0);
	style->ItemInnerSpacing = ImVec2(8, 6);
	style->IndentSpacing = 25.0f;
	style->ScrollbarSize = 15.0f;
	style->ScrollbarRounding = 9.0f;
	style->GrabMinSize = 5.0f;
	style->GrabRounding = 3.0f;

	style->Colors[ImGuiCol_Text] = ImVec4(0.9f, 0.9f, 0.9f, 0.9f);
	style->Colors[ImGuiCol_TextDisabled] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
	style->Colors[ImGuiCol_WindowBg] = ImVec4(0.06f, 0.05f, 0.07f, 0.85f);
	style->Colors[ImGuiCol_PopupBg] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
	style->Colors[ImGuiCol_Border] = ImVec4(0.1f, 0.1f, 0.1f, 0.9f);
	style->Colors[ImGuiCol_BorderShadow] = ImVec4(0.1f, 0.1f, 0.1f, 0.9f);
	style->Colors[ImGuiCol_FrameBg] = ImVec4(0.1f, 0.1f, 0.1f, 0.9f);
	style->Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
	style->Colors[ImGuiCol_FrameBgActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	style->Colors[ImGuiCol_TitleBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	style->Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(1.00f, 0.98f, 0.95f, 0.75f);
	style->Colors[ImGuiCol_TitleBgActive] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
	style->Colors[ImGuiCol_MenuBarBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	style->Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	style->Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
	style->Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	style->Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	style->Colors[ImGuiCol_CheckMark] = ImVec4(0.0f, 0.5f, 0.5f, 0.9f);
	style->Colors[ImGuiCol_SliderGrab] = ImVec4(0.0f, 0.5f, 0.5f, 0.9f);
	style->Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.0f, 0.5f, 0.5f, 0.9f);
	style->Colors[ImGuiCol_Button] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	style->Colors[ImGuiCol_ButtonHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
	style->Colors[ImGuiCol_ButtonActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	style->Colors[ImGuiCol_Header] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	style->Colors[ImGuiCol_HeaderHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	style->Colors[ImGuiCol_HeaderActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	style->Colors[ImGuiCol_ResizeGrip] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	style->Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	style->Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	style->Colors[ImGuiCol_PlotLines] = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
	style->Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
	style->Colors[ImGuiCol_PlotHistogram] = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
	style->Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
	style->Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.25f, 1.00f, 0.00f, 0.43f);
	style->Colors[ImGuiCol_TabActive] = ImVec4(0.0f, 0.5f, 0.5f, 0.9f);
	style->Colors[ImGuiCol_Tab] = ImVec4(0.14f, 0.14f, 0.14f, 0.95f);
}

int in_tab_index = 0;
bool init = false;
HRESULT __stdcall hkPresent(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags)
{
	if (!init)
	{
		if (SUCCEEDED(pSwapChain->GetDevice(__uuidof(ID3D11Device), (void**)&pDevice)))
		{
			pDevice->GetImmediateContext(&pContext);
			DXGI_SWAP_CHAIN_DESC sd;
			pSwapChain->GetDesc(&sd);
			window = sd.OutputWindow;
			ID3D11Texture2D* pBackBuffer;
			pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
			pDevice->CreateRenderTargetView(pBackBuffer, NULL, &mainRenderTargetView);
			pBackBuffer->Release();
			oWndProc = (WNDPROC)SetWindowLongPtr(window, GWLP_WNDPROC, (LONG_PTR)WndProc);
			InitImGui();
			init = true;
		}

		else
			return oPresent(pSwapChain, SyncInterval, Flags);
	}

	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	if (GetKeyState(VK_INSERT) & 0x80)
	{
		show_main = !show_main;
	}

	if (GetKeyState(VK_END) & 1)
	{
		settings::chams = false;
		settings::lineesp = false;
		NoRecoil = false;
		rcs_amount = 0;
		Thirdperson = false;
		skinscroller = false;
		weaponskinchanger = false;
		weaponskin_id = 0;

		TerminateThread((HANDLE)ImGuiMain_threadID, 0);
		TerminateThread((HANDLE)MainThread_threadID, 0);
		TerminateThread((HANDLE)weapon_threadID, 0);

		Unload = true;
		FreeConsole();
		kiero::unbind(8);
		kiero::shutdown();
	}

	LocalPlayer LE = LocalPlayer();

	//if (connected)
	{
		if (LE.Entity)
		{
			for (int i = 0; i < 100; i++)
			{
				BaseEntity ent = BaseEntity(i);


				if (!ent.Entity || ent.Entity == LE.Entity || !LE.Entity)
					continue;

				//float dist = Distance(ent.Position(), LE.Position());

			//	if (dist < settings::esp_range)
				//{
				//	ent.GlowOff();
				//	continue;
				//}

				/*
				if (strcmp(ent.identifier().c_str(), "npc_dummie"))
				{

					ent.GlowOff();
				}
				if (lineesp)
				{
					ImGui::GetBackgroundDrawList()->AddLine(ImVec2(ScreenWidth / 2, ScreenHeight), ImVec2(_2d_pos.x, _2d_pos.y), ImColor(255, 255, 255, 255));
				}
				if (boxesp)
				{
					DrawCorneredBox(x, y, width, height, ImColor(255, 255, 255, 255), 1.f);
				}*/

				//if (!ent.isKnocked() == 0)
					//continue;

				if (!ent.IsAlive())
					continue;

				//if (strcmp(ent.identifier().c_str(), "player"))
				//{
					//if (ent.iTeam() == LE.iTeam())
						//return;

				Vector3 pos = ent.Position();
				Vector2 _2d_pos;
				pos.world_to_screen(_2d_pos);

				Vector3 headpos = ent.BonePosition(8);
				Vector2 _2d_headpos;
				headpos.world_to_screen(_2d_headpos);

				const float height = _2d_pos.y - _2d_headpos.y;
				const float width = height / 2.f;
				const float y = _2d_headpos.y;
				const float x = _2d_pos.x - (width / 2.f);

				if (chams)
				{
					ent.Glow(255, 255, 255, 50);
				}
				else
				{
					ent.GlowOff();
				}
				if (lineesp)
				{
					ImGui::GetBackgroundDrawList()->AddLine(ImVec2(ScreenWidth / 2, ScreenHeight), ImVec2(_2d_pos.x, _2d_pos.y), ImColor(255, 255, 255, 255));
				}
				if (boxesp)
				{
					DrawCorneredBox(x, y, width, height, ImColor(255, 255, 255, 255), 1.f);
				}
			}

			if (aimbot_drawfov)
			{
				ImGui::GetBackgroundDrawList()->AddCircle(ImVec2(ScreenWidth / 2, ScreenHeight / 2), aimbot_fov, ImColor(255, 255, 255, 255), 100, 1.f);
			}
		}
	}
	if (show_main)
	{
		InitMenuColors();

		if (ImGui::Begin("Gaypex"))
		{
			int h = 40;
			int w = 100;
			ImGui::Columns(2);
			ImGui::SetColumnOffset(1, w + 30);

			{
				static ImVec4 inactive = ImVec4(0.09f, 0.09, 0.09f, 0.88f);
				static ImVec4 active = ImVec4(0.0f, 0.5f, 0.5f, 0.9f);
				ImGui::Spacing();
				ImGui::Spacing();
				ImGui::Spacing();
				ImGui::PushStyleColor(ImGuiCol_Button, in_tab_index == 0 ? active : inactive);
				if (ImGui::Button("Weapon", ImVec2(w, h)))
					in_tab_index = 0;

				ImGui::PushStyleColor(ImGuiCol_Button, in_tab_index == 1 ? active : inactive);
				if (ImGui::Button("Visuals", ImVec2(w, h)))
					in_tab_index = 1;

				ImGui::PushStyleColor(ImGuiCol_Button, in_tab_index == 2 ? active : inactive);
				if (ImGui::Button("Misc", ImVec2(w, h)))
					in_tab_index = 2;

				ImGui::PushStyleColor(ImGuiCol_Button, in_tab_index == 3 ? active : inactive);
				if (ImGui::Button("Skins", ImVec2(w, h)))
					in_tab_index = 3;

				ImGui::PushStyleColor(ImGuiCol_Button, in_tab_index == 4 ? active : inactive);
				if (ImGui::Button("About", ImVec2(w, h)))
					in_tab_index = 4;

				ImGui::PopStyleColor(5);

			}

			ImGui::NextColumn();

			{
				ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(12, 20));
				ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 4.f);
				if (in_tab_index == 1)
				{
					ImGui::SliderFloat("ESP Range", &settings::esp_range, 1.f, 500.f);
					ImGui::Checkbox("Chams", &settings::chams);
					ImGui::Checkbox("Line ESP", &settings::lineesp);
					ImGui::Checkbox("Box ESP", &settings::boxesp);

					if (ImGui::TreeNode("Radar"))
					{
						ImGui::Checkbox("Enable", &Radar);
						ImGui::Checkbox("Only Enemys", &Radar_onlyenemys);
						ImGui::Checkbox("Visible Only", &Radar_visibleonly);
						ImGui::Checkbox("View Check", &Radar_viewcheck);
						//ImGui::SliderInt("Radar Type", &Radar_type, 0, 3);
						ImGui::TreePop();
					}

					/*
					Line(454121548511);

					ImGui::Text("Item ESP");
					ImGui::Checkbox("Debug item ids", &debugitemids);
					ImGui::Checkbox("Item ESP", &itemesp);
					*/
				}
				if (in_tab_index == 0)
				{
					ImGui::Checkbox("No Recoil", &NoRecoil);
					//Line(8794237845);
					ImGui::Checkbox("Aimbot", &enable_aimbot);
					if (enable_aimbot)
					{
						Hotkey("Aimbot Hotkey", &AimbotKey, ImVec2(350, 0));
						ImGui::Checkbox("Aimbot Onlyvisible", &aimbot_onlyvisible);
						ImGui::Checkbox("Aimbot Onlyenemys", &aimbot_onlyenemys);
						ImGui::Checkbox("Aimbot No Recoil", &aimbot_norecoil);
						ImGui::SliderFloat("Aimbot Smooth", &aimbot_smoothness, 0.500f, 2.f);
						ImGui::Checkbox("Draw FOV", &aimbot_drawfov);
						if (aimbot_drawfov)
						{
							ImGui::SliderFloat("Aimbot FOV", &aimbot_fov, 1, GetSystemMetrics(SM_CXSCREEN));
						}
					}
				}
				if (in_tab_index == 2)
				{
					ImGui::Checkbox("Thirdperson", &Thirdperson);
					ImGui::Checkbox("Test", &test);
					if (ImGui::Button("Unhook"))
					{
						settings::chams = false;
						settings::lineesp = false;
						NoRecoil = false;
						rcs_amount = 0;
						Thirdperson = false;
						skinscroller = false;
						weaponskinchanger = false;
						weaponskin_id = 0;

						TerminateThread((HANDLE)ImGuiMain_threadID, 0);
						TerminateThread((HANDLE)MainThread_threadID, 0);
						TerminateThread((HANDLE)weapon_threadID, 0);

						Unload = true;
						FreeConsole();
						kiero::unbind(8);
						kiero::shutdown();
					}
				}
				if (in_tab_index == 3)
				{
					ImGui::Checkbox("Skinchanger loop", &skinscroller);
					ImGui::Checkbox("Weapon Skinchanger", &weaponskinchanger);
					ImGui::InputInt("Weapon Skin", &weaponskin_id);
				}
				if (in_tab_index == 4)
				{
					ImGui::Text("Credits:");
					ImGui::Text("Rudolf#2773");
					ImGui::Text("Leon.#0001");
					ImGui::Text("HERE DC SERVER");
				}
				ImGui::PopStyleVar(2);
			}
		}
		ImGui::End();
	}

	ImGui::Render();

	pContext->OMSetRenderTargets(1, &mainRenderTargetView, NULL);
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	return oPresent(pSwapChain, SyncInterval, Flags);
}


DWORD WINAPI thread_imguimain(LPVOID lpReserved)
{
	bool init_hook = false;
	do
	{
		if (kiero::init(kiero::RenderType::D3D11) == kiero::Status::Success)
		{
			kiero::bind(8, (void**)&oPresent, hkPresent);
			init_hook = true;
		}
	} while (!init_hook);
	return TRUE;
}

std::string get_hwid() {
	HW_PROFILE_INFO hwProfileInfo;
	GetCurrentHwProfile(&hwProfileInfo);
	char* HWID = hwProfileInfo.szHwProfileGuid;
	std::string HWIDSTR(HWID);
	return HWIDSTR;
}
void __stdcall _thr() {
	AllocConsole();
	freopen(xorstr_("con"), xorstr_("w"), stdout);


	//hwids.push_back(xorstr_("{26bfd063-6fc6-11ec-a124-806e6f6e6963}"));

	while (!dwbase) {
		dwbase = (uintptr_t)iat(GetModuleHandleA).get()(xorstr_("r5apex.exe"));
	}

	std::cout << xorstr_("r5apex.exe ") << std::hex << dwbase << std::endl;

	//std::cout << xorstr_("HWID: ") << get_hwid() << std::endl;

	//std::cout << xorstr_("Checking HWID!") << std::endl;
	/*
	for (auto hwid : hwids)
	{
		if (hwid != get_hwid())
		{
			exit(0);
		}
	}
	*/
	//std::cout << xorstr_("Init MinHook") << std::endl;

	//std::cout << xorstr_("loading game classes") << std::endl;
/*


	uint64_t uFindPos = (uint64_t)memory::PatternScan((void*)dwbase, "48 89 AC 24 ?? ?? ?? ?? 48 89 BC 24 ?? ?? ?? ?? 40 B7 01");
	Offsets::Offset_SendPacket = uFindPos + 0x12 - dwbase;
	std::cout << "SendPackets: " << std::hex << Offsets::Offset_SendPacket << std::endl;
	DWORD dwAddr;
	VirtualProtect((void*)(Offsets::Offset_SendPacket + dwbase), 0x1, PAGE_EXECUTE_READWRITE, &dwAddr);




*/


	//g_pinput = (c_input*)memory::dereference(memory::occurence(xorstr_("r5apex.exe"), xorstr_("48 8D 0D ? ? ? ? 41 FF 90 ? ? ? ? EB E9")), 3);
	//g_pglobals = (c_globalvars*)memory::dereference(memory::occurence(xorstr_("r5apex.exe"), xorstr_("48 8B 05 ? ? ? ? F3 0F 10 50 ? 74 38")), 4);
	//createinterface = (createinterface_fn)memory::dereference(memory::occurence(xorstr_("r5apex.exe"), xorstr_("E8 ? ? ? ? 48 89 05 ? ? ? ? 48 83 3D ? ? ? ? ? 0F 84 ? ? ? ? 33 D2")), 1);
	//o_worldtoscreen = (worldtoscreen_fn)(dwbase + 0x642B10);

	//std::cout << xorstr_("g_pinput ") << std::hex << g_pinput << std::endl;
	//std::cout << xorstr_("g_pglobals ") << std::hex << g_pglobals << std::endl;
	//std::cout << xorstr_("createinterface ") << std::hex << createinterface << std::endl;
	//std::cout << xorstr_("o_worldtoscreen ") << std::hex << o_worldtoscreen << std::endl;

	//LP = *(uintptr_t*)(dwbase + Offsets::local_player);

	//std::cout << xorstr_("LocalPlayer: ") << std::hex << LP << std::endl;

	//activeweap = ActiveWeapon(LP);

	//std::cout << xorstr_("Active Weapon: ") << std::hex << activeweap << std::endl;

	MyCreateThread((LPTHREAD_START_ROUTINE)thread_imguimain, NULL, &ImGuiMain_threadID);
	MyCreateThread((LPTHREAD_START_ROUTINE)thread_Main, NULL, &MainThread_threadID);
	MyCreateThread((LPTHREAD_START_ROUTINE)thread_weapon, NULL, &weapon_threadID);

	std::cout << xorstr_("Injected!") << std::endl;

	//MyCreateThread((LPTHREAD_START_ROUTINE)thread_Overlay, g_overlay, &Overlay_threadID);
}

bool __stdcall DllMain(void* module, unsigned long reason, void* buffer) {//inject with hack thread option
	if (reason == 1) {
		_thr();
	}
	return true;
}


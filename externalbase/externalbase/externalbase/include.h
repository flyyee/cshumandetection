#pragma once

#pragma warning( disable : 4244 )
#pragma warning( disable : 4309 )
#pragma warning( disable : 4838 )
#pragma warning( disable : 4800 )
#pragma warning( disable : 4334 )
#pragma warning( disable : 4305 )
#pragma warning( disable : 4805 )
#pragma warning( disable : 4018 )
#pragma warning( disable : 4172 )
#pragma warning( disable : 4042 )

#pragma comment(lib,"wininet.lib")

#undef CreateFont

#include <windows.h>
#include <vector>
#include <string>
#include <math.h>
#include <algorithm>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <tlhelp32.h>
#include <psapi.h>
#include <thread>
#include <functional>
#include <future>
#include <array>
#include <wininet.h>


#include "enums.h"
#include "overlay\d3d.h"
#include "utils\math.h"
#include "utils\utils.h"
#include "utils\WebShit.h"

#include "entity.h"

// init
#include "init\dynamic.h"
#include "init\interface.h"
#include "init\static.h"
// memory
#include "memory\module.h"
#include "memory\process.h"

#include "utils\consolefuncs.h"
#include "utils\inputkeys.h"
#include "utils\files.h"
#include "utils\config.h"

// overlay
#include "overlay\Sprite.h"
#include "overlay\font.h"
#include "overlay\renderer.h"
#include "overlay\color.h"
#include "overlay\overlay.h"

// valve
#include "valve\dt_recv.h"

//
#include "BSPMap.h"
#include "engine.h"
#include "globals.h"
#include "hitbox.h"
#include "ray.h"

// features
#include "features\esp.h"
#include "features\aimbot.h"
#include "features\triggerbot.h"
#include "features\misc.h"

// menu
#include "menu\menu_elements.h"
#include "menu\d3d_menu.h"
#include "menu\aimbot_menu.h"
#include "menu\trigger_menu.h"
#include "menu\visuals_menu.h"
#include "menu\misc_menu.h"
#include "menu\config_menu.h"


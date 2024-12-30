#ifndef INCLUDES_HPP
#define INCLUDES_HPP

static inline int m_target_pid;

#include "hash.hpp"
#include "sdk/sdk.hpp"
#include "imports.hpp"
#include "../resources/offsets.h"


// debug
#include "../resources/debug/debug.hpp"

// dependencies
#include "../dependencies/communication/ia32/ia32.hpp"
#include "../dependencies/communication/edk.hpp"
#include "../dependencies/communication/scanner/scanner.hpp"

// sdk
#include "sdk/math/Vector.hpp"
#include "sdk/utilities/utilities.hpp"

// framework
#include "../dependencies/framework/framework.hpp"
// resources
#include "../resources/ipc/ipc.hpp"
// source
#include "engine/engine.hpp"
#include "aimbot/aimbot.hpp"
#include "entity/entity.hpp"
#include "interface/interface.hpp"
#include "render/render.hpp"
#endif
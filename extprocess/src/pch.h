#ifndef DIFFUSER_PCH
#define DIFFUSER_PCH
#include <chrono>
#include <iostream>
#include <memory>
#include <ostream>
#include <string>
#include <thread>
#include <random>
#include <mutex>
#include <cinttypes>

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#else
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <cstring>
#include <dlfcn.h>
#endif

#include "libs/stb_image.h"
#include "libs/stb_image_write.h"
#include "libs/stb_image_resize.h"

#include "libs/json.hpp"

#include "ver.hpp"
#include "config.hpp"

#include "helpers/formatbytes.hpp"


#include <wx/event.h>
#include <wx/string.h>
#include <wx/translation.h>
#include <wx/filename.h>
#include <wx/image.h>
#include <wx/log.h>

#include "libs/CustomLogFormatter.h"
#include "helpers/sd.hpp"
#include "helpers/sslUtils.hpp"
#include "helpers/base64.h"
#include "helpers/QueueEvents.h"
#include "helpers/QueueStatus.h"
#include "helpers/QueueItemStats.h"
#include "network/RemoteQueueItem.h"
#include "helpers/QueueItem.h"

#include "network/packets.h"

#include "libs/SharedLibrary.h"
#include "libs/SharedMemoryManager.h"
#include "ui/QueueManager.h"


#include "ApplicationLogic.h"

#endif // DIFFUSER_PCH
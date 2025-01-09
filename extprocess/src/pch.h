#include <chrono>
#include <iostream>
#include <memory>
#include <ostream>
#include <string>
#include <thread>
#include <random>

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#else
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <cstring>
#endif

#include "libs/json.hpp"

#include "ver.hpp"
#include "config.hpp"

#include <wx/event.h>
#include <wx/string.h>
#include <wx/translation.h>
#include <wx/filename.h>
#include "helpers/sd.hpp"
#include "helpers/QueueItem.h"
#include "network/RemoteQueueJobItem.h"
#include "network/packets.h"
#include "helpers/sd.hpp"
#include "helpers/sslUtils.hpp"
#include "libs/SharedLibrary.h"
#include "libs/SharedMemoryManager.h"
#include "ui/QueueManager.h"


#include "ApplicationLogic.h"
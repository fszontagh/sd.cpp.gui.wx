#include <map>
#include <string>
#include <vector>
#include <cstddef>
#include <cstdint>

#include "libs/SharedLibrary.h"


#include <wx/app.h>
#include <wx/evtloop.h>
#include <wx/filename.h>
#include <wx/log.h>
#include <wx/stdpaths.h>
#include <wx/textfile.h>
#include <wx/time.h>
#include <wx/timer.h>
#include <wx/event.h>

#include "sockets-cpp/TcpServer.h"
#include "EventQueue.h"
#include "libs/SharedLibrary.h"
#include "libs/SharedMemoryManager.h"
#include "ver.hpp"
#include "extprocess/config.hpp"
#include "libs/json.hpp"
#include "helpers/sslUtils.hpp"
#include "network/packets.h"
#include "network/RemoteModelInfo.h"

#include "ServerConfig.h"
#include "SocketApp.h"
#include "TerminalApp.h"
#include "EventQueue.h"

#include "ServerConfig.h"


#include "libs/json.hpp"


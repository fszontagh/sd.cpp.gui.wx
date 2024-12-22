#ifndef _LIBS_TCPCLIENT_H_
#define _LIBS_TCPCLIENT_H_
namespace sd_gui_utils {
    enum class ThreadEvents;
}
#include "../network/sdServer.h"

namespace sd_gui_utils::networks {

    class TcpClient {
        sd_gui_utils::sdServer* server = nullptr;
        wxEvtHandler* evt              = nullptr;
        sockets::TcpClient<TcpClient> m_client;
        std::atomic_int conn_counter = 0;
        void sendMsg(const sd_gui_utils::networks::Packet& packet);
        std::unordered_map<size_t, sd_gui_utils::networks::Packet> receivedPackets;
        std::vector<char> buffer;
        size_t expected_size = 0;

        template <typename T>
        void SendThreadEvent(sd_gui_utils::ThreadEvents eventType, const T& payload, std::string text = "") {
            wxThreadEvent* e = new wxThreadEvent();
            e->SetString(wxString::Format("%d:%s", (int)eventType, text));
            e->SetPayload(payload);
            wxQueueEvent(this->evt, e);
        };

        template <typename T>
        void SendThreadEvent(sd_gui_utils::ThreadEvents eventType, const T& payload, size_t packet_id, std::string text = "") {
            wxThreadEvent* e = new wxThreadEvent();
            e->SetString(wxString::Format("%d:%s", (int)eventType, text));
            e->SetInt(packet_id);
            e->SetPayload(payload);
            wxQueueEvent(this->evt, e);
        };

        void HandlePackets(sd_gui_utils::networks::Packet& packet);

    public:
        TcpClient(sd_gui_utils::sdServer* server, wxEvtHandler* evt);
        virtual ~TcpClient() = default;
        void onReceiveData(const char* data, size_t size);
        void onDisconnect(const sockets::SocketRet& ret);
        void sendMsg(const char* data, size_t len);
        inline sd_gui_utils::networks::Packet getPacket(size_t Id) {
            if (receivedPackets.find(Id) != receivedPackets.end()) {
                return receivedPackets[Id];
            }
            return sd_gui_utils::networks::Packet();
        }
        inline void stop() {
            this->server->needToRun = false;
        }
    };
}
#endif;  // _LIBS_TCPCLIENT_H_
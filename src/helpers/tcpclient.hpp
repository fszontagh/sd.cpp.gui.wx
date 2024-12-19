#ifndef _TCPCLIENT_H_
#define _TCPCLIENT_H_

class TcpClient {
    sd_gui_utils::sdServer server;
public:
    TcpClient(const sd_gui_utils::sdServer& server): server(server) {};
    ~TcpClient();
};

#endif;
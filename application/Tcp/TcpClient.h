#include <string>
#include <sys/socket.h>

#define TAG "TCP"

namespace Tcp
{
class TcpClient
{
    private:
        std::string _host_ip;
        int _port;
        int _socket;
        struct sockaddr_in _dest_addr;

    public:
        TcpClient(std::string, int);
        esp_err_t socket_connect();
        void socket_disconnect();
        esp_err_t socket_send(std::string);
        esp_err_t socket_receive(std::string &, int=512);


};

};
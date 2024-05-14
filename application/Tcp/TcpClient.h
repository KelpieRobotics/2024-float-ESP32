#include <string>
#include <sys/socket.h>

#define HOST_IP_ADDR static_cast<std::string>(CONFIG_HOST_IP_ADDR)
#define PORT CONFIG_PORT

#define TAG "TCP"

namespace Tcp
{
class TcpClient
{
    private:
        std::string _host_ip;
        int _port;
        int sock;
        struct sockaddr_in _dest_addr;

    public:
        TcpClient(std::string, int);
        esp_err_t socket_connect();
        //esp_err_t socket_disconnect();
        esp_err_t socket_send(std::string);
        esp_err_t socket_receive(std::string &, int);


};

};
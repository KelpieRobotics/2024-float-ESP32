#include "TcpClient.h"
#include <cstring>
#include "esp_log.h"

namespace Tcp
{

TcpClient::TcpClient(std::string host_ip, int port) :
    _host_ip{host_ip},
    _port{port},
    sock{-1},
    _dest_addr{
        .sin_family = AF_INET,
        .sin_port = htons(PORT)
    }
{
    inet_pton(AF_INET, _host_ip.c_str(), &_dest_addr.sin_addr);
};

esp_err_t TcpClient::socket_connect()
{
    sock = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
    if (sock < 0) 
    {
        ESP_LOGE(TAG, "Unable to create socket: errno %d", errno);
        return ESP_FAIL;
    }

    int err = connect(sock, (struct sockaddr *)&_dest_addr, sizeof(_dest_addr));
    if (err != 0) 
    {
        ESP_LOGE(TAG, "Socket unable to connect: errno %d", errno);
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t TcpClient::socket_send(std::string payload)
{
    int bytes = send(sock, payload.c_str(), strlen(payload.c_str()), 0);
    if (bytes < 0)
    {
        ESP_LOGE(TAG, "Error occurred during sending: errno %d", errno);
        return ESP_FAIL;
    }
    
    ESP_LOGD(TAG, "Bytes sent: %d", bytes);
    return ESP_OK;
}

esp_err_t TcpClient::socket_receive(std::string &message, int size=512)
{
    char buffer[size];
    int bytes = recv(sock, buffer, sizeof(buffer), 0);
    if (bytes < 0)
    {
        ESP_LOGE(TAG, "Error occurred during receiving: errno %d", errno);
        return ESP_FAIL;
    }
    
    ESP_LOGD(TAG, "Bytes received: %d", bytes);
    message = buffer;
    return ESP_OK;
}

}
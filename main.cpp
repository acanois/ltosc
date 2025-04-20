#include <iostream>
#include <span>

#include <asio.hpp>

using asio::ip::udp;

class UdpClient {
public:
    explicit UdpClient(int port)
        : mPort(port),
          mIoSocket(mIoContext, udp::endpoint(udp::v4(), mPort))
    {

    }

    void receiveMessages() {
        try {
            while (true) {
                std::vector<u_char> receive_buffer(BUFFER_SIZE);
                asio::error_code error;

                auto bytesReceived = mIoSocket.receive_from(
                    asio::buffer(receive_buffer, BUFFER_SIZE),
                    mRemoteEndpoint,
                    0,
                    error
                );

                receive_buffer.resize(bytesReceived);

//                for (auto i = 0; i < receive_buffer.size(); ++i) {
//                    std::cout << i << ": " << receive_buffer[i] << std::endl;
//                }

                auto endMessageIdx = 0;
                for (auto i = 0; i < receive_buffer.size(); ++i) {
                    if (receive_buffer[i] == '\0') {
                        endMessageIdx = i;
                        break;
                    }
                }

                std::string messageTag(receive_buffer.begin(), receive_buffer.begin() + endMessageIdx);
                std::string dataSlice(receive_buffer.begin() + endMessageIdx, receive_buffer.end());

                auto typeTagIdx = 0;
                if (receive_buffer.at(endMessageIdx + 2) == ',') {
                    typeTagIdx = endMessageIdx + 3;
                }

                auto typeTag = receive_buffer.at(typeTagIdx);

                switch(typeTag) {
                    case 'i': {
                        int32_t value = 0;
                        std::memcpy(&value, &receive_buffer.at(typeTagIdx + 2 + sizeof(int32_t)), sizeof(int32_t));
                        std::cout << value << std::endl;
                        break;
                    }
                    default: {
                        std::cerr << "Unsupported type tag: " << typeTag << std::endl;
                        break;
                    }
                }
//                std::cout << "messageTag: " << messageTag << std::endl;
//                std::cout << "dataSlice: " << dataSlice.length() << std::endl;
//
//                auto dataIndex = receive_buffer.size() - sizeof(int32_t);
//
//                int32_t receivedInt = 0;
//                if (dataSlice.at(2) == ',') {
//                    std::memcpy(&receivedInt, &receive_buffer.at(16), sizeof(int32_t));
//                }
//
//                std::cout << receivedInt << std::endl;

//                parseMessage(receive_buffer);

//                std::string message(receive_buffer.begin(), receive_buffer.begin() + receive_buffer.size());
//                std::cout << message << std::endl;
//
//                receive_buffer.assign(BUFFER_SIZE, 0);
//
//                if (error && error != asio::error::message_size) {
//                    throw asio::system_error(error);
//                } else {
//                    std::cout << "Received: " << message << std::endl;
//                }
            }
        } catch (std::exception& e) {
            std::cerr << "Exception: " << e.what() << std::endl;
        }
    }

    ~UdpClient() = default;
private:
    int mPort { 7000 };

    static constexpr std::size_t BUFFER_SIZE { 1024 };
    asio::io_context mIoContext;
    asio::ip::udp::socket mIoSocket;
    asio::ip::udp::endpoint mRemoteEndpoint;
};

int main() {
    std::unique_ptr<UdpClient> client = std::make_unique<UdpClient>(7000);
    client->receiveMessages();
    return 0;
}
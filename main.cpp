#include <iostream>
#include <asio.hpp>

using asio::ip::udp;

class UdpClient {
public:
    explicit UdpClient(int port)
        : mPort(port),
          mIoSocket(mIoContext, udp::endpoint(udp::v4(), mPort))
    {

    }

    static void parseMessage(const std::vector<u_char>& data) {
        if (data.empty() || data[0] != '/') {
            std::cerr << "Invalid OSC message format." << std::endl;
            return;
        }

        auto addressEnd = 0;
        for (auto i = 1; i < data.size(); ++i) {
            if (data[i] == '\0') {
                addressEnd = i;
                break;
            }
        }
        std::cout << "Address end: " << addressEnd << std::endl;
        if (addressEnd == 0) {
            std::cerr << "Invalid OSC message: missing address terminator." << std::endl;
            return;
        }

        // Extract the address pattern
        std::string address(data.begin(), data.begin() + addressEnd);
        std::cout << "Address: " << address << std::endl;

        // Check for type tags
        if (addressEnd + 1 < data.size() && data[addressEnd + 1] == ',') {
            size_t type_tags_end = 0;
            for (size_t i = addressEnd + 2; i < data.size(); ++i) {
                if (data[i] == '\0') {
                    type_tags_end = i;
                    break;
                }
            }

            if (type_tags_end == 0) {
                std::cerr << "Invalid OSC message: missing type tags terminator." << std::endl;
                return;
            }

            std::string type_tags(data.begin() + addressEnd + 2, data.begin() + type_tags_end);
            std::cout << "Type Tags: " << type_tags << std::endl;
            std::cout << "Type Tags End: " << type_tags_end << std::endl;


            size_t data_start = type_tags_end + 1;
            for (char type_tag : type_tags) {
                if (data_start >= data.size()) break;

                switch (type_tag) {
                    case 'i': {
                        if (data_start + sizeof(int32_t) > data.size()) {
                            std::cerr << "int32_t overflow" << std::endl;
                            return;
                        }

                        int32_t value = 0;
                        std::memcpy(&value, &data[data_start + sizeof(int32_t)], sizeof(int32_t));
                        data_start += sizeof(int32_t);
                        std::cout << "  Int: " << value << std::endl;
                        break;
                    }
                    case 'f': {
                        // Handle Float
                        break;
                    }
                    case 's': { // string
                        // handle stringd
                        break;
                    }
                        // Add handlers for other data types as needed
                    default:
                        std::cerr << "Unsupported type tag: " << type_tag << std::endl;
                        return;
                }
            }
        }
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
                parseMessage(receive_buffer);

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
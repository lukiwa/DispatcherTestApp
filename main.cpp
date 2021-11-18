#include "Module.h"
#include <iostream>
#include <memory>

using namespace WPEFramework;

enum class ApplicationType {
    CLIENT,
    SERVER,
    UNKNOWN
};

ApplicationType SetApplicationType(int argc, char** argv)
{
    ApplicationType type = ApplicationType::UNKNOWN;

    if (argc >= 2) {
        std::string client = argv[1];

        if (client == "client") {
            type = ApplicationType::CLIENT;
        } else if (client == "server") {
            type = ApplicationType::SERVER;

        } else {
            std::cerr << "Unknown application type: " << client << std::endl;
        }
    } else {
        std::cerr << "You must specify application type (client/server)\n";
        exit(-1);
    }
    return type;
}
void PrintData(uint8_t type, uint16_t length, const uint8_t* value)
{
    std::cout << "Received data:\n";
    std::cout << "type: " << (int)type << "\n"
              << "length: " << length << "\n";

    for (int i = 0; i < length; ++i) {
        std::cout << "value: " << (int)value[i] << "\n";
    }
}

std::string SetPath(int argc, char** argv)
{
    std::string result;
    if (argc >= 3) {
        result = argv[2];
    } else {
        std::cerr << "You must specify path for sockets and stuff\n";
        exit(-1);
    }
    return result;
}

constexpr uint16_t METADATA_SIZE = 1024;
constexpr uint16_t DATA_SIZE = 2048;
using MessageDispatcherType = Core::MessageDispatcher<METADATA_SIZE, DATA_SIZE>;

int main(int argc, char** argv)
{
    auto applicationType = SetApplicationType(argc, argv);
    auto path = SetPath(argc, argv);
    char option;

    std::unique_ptr<MessageDispatcherType> dispatcher;

    do {
        printf("\n>");
        option = toupper(getchar());

        switch (option) {
        case 'C': {
            if (applicationType == ApplicationType::SERVER) {
                dispatcher.reset(new MessageDispatcherType(path, 0, true));
            }
            if (applicationType == ApplicationType::CLIENT) {
                dispatcher.reset(new MessageDispatcherType(path, 0, false));
            }

            if (dispatcher != nullptr) {
                std::cout << "Created dispatcher\n";
            } else {
                std::cerr << "Failed to create dispatcher\n";
            }

            break;
        }

        case 'D': {
            if (applicationType == ApplicationType::SERVER) {
                uint8_t testData[2] = { 13, 37 };
                if (dispatcher->PushData(0, sizeof(testData), testData) != Core::ERROR_NONE) {
                    std::cerr << "Failed writing data\n";
                } else {
                    dispatcher->Ring();
                    std::cout << "Data written and triggered doorbell\n";
                }
            }

            if (applicationType == ApplicationType::CLIENT) {
                uint8_t readType;
                uint16_t readLength;
                uint8_t readData[1024];

                std::cout << "Waiting for a doorbell ring\n";
                if (dispatcher->Wait(Core::infinite) == Core::ERROR_NONE) {
                    std::cout << "Somebody ringed\n";

                    if (dispatcher->PopData(readType, readLength, readData) == Core::ERROR_NONE) {
                        PrintData(readType, readLength, readData);
                    } else {
                        std::cerr << "Client unable to read data\n";
                    }
                }
            }

            break;
        }

        case 'M': {
            if (applicationType == ApplicationType::CLIENT) {
                uint8_t testData[2] = { 13, 37 };
                if (dispatcher->PushMetadata(0, sizeof(testData), testData) != Core::ERROR_NONE) {
                    std::cerr << "Failed writing meta data\n";
                } else {
                    std::cout << "Metadata written\n";
                }
            }
            break;
        }
        case 'R': {
            if (applicationType == ApplicationType::SERVER) {
                std::cout << "Registering metadata callback" << std::endl;
                dispatcher->RegisterDataAvailable([](const uint8_t type, const uint16_t length, const uint8_t* value) {
                    PrintData(type, length, value);
                });
            }
            break;
        }

        case 'U': {
            if (applicationType == ApplicationType::SERVER) {
                std::cout << "Unregistering metadata callback";
                dispatcher->UnregisterDataAvailable();
            }
            break;
        }

        case 'Q':
            break;
        }
    } while (option != 'Q');

    dispatcher.reset(nullptr);
    if (applicationType == ApplicationType::SERVER) {
        Core::Singleton::Dispose();
    }

    return 0;
}

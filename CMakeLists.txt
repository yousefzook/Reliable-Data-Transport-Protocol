cmake_minimum_required(VERSION 3.8)
project(TCP_Network)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pthread")

set(SOURCE_FILES Client/Client.cpp Server/Server.cpp Server/Server.h Client/Client.cpp Client/Client.h Packet/AckPacket.cpp Packet/AckPacket.h Packet/DataPacket.cpp Packet/DataPacket.h Packet/Packet.cpp Packet/Packet.h rdt/RDT.cpp rdt/RDT.h rdt/SR.cpp rdt/SR.h rdt/StopNWait.cpp rdt/StopNWait.h rdt/GBN.cpp rdt/GBN.h)

include_directories(Client)
include_directories(Packet)
include_directories(rdt)
include_directories(Server)


add_executable(Client
        Client/Client.cpp
        Client/Client.h
        Packet/AckPacket.cpp
        Packet/AckPacket.h
        Packet/DataPacket.cpp
        Packet/DataPacket.h
        Packet/Packet.cpp
        Packet/Packet.h
        rdt/RDT.cpp
        rdt/RDT.h
        rdt/SR.cpp
        rdt/SR.h
        rdt/StopNWait.cpp
        rdt/StopNWait.h
        rdt/GBN.cpp
        rdt/GBN.h
        )

add_executable(Server
        Packet/AckPacket.cpp
        Packet/AckPacket.h
        Packet/DataPacket.cpp
        Packet/DataPacket.h
        Packet/Packet.cpp
        Packet/Packet.h
        rdt/RDT.cpp
        rdt/RDT.h
        rdt/SR.cpp
        rdt/SR.h
        rdt/StopNWait.cpp
        rdt/StopNWait.h
        Server/Server.cpp
        Server/Server.h
        rdt/GBN.cpp
        rdt/GBN.h
        )

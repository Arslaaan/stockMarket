CMAKE_MINIMUM_REQUIRED(VERSION 3.21)
PROJECT(stockMarket)

SET(CMAKE_CXX_STANDARD 17)

FIND_PACKAGE(Boost 1.40 COMPONENTS system REQUIRED)
FIND_PACKAGE(Threads REQUIRED)

set(BACKEND_SERVER server)
set(CLIENT client)
set(COMMON common)

INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIR})
INCLUDE_DIRECTORIES(${COMMON})
INCLUDE_DIRECTORIES(${BACKEND_SERVER})
INCLUDE_DIRECTORIES(${BACKEND_SERVER}/include)
INCLUDE_DIRECTORIES(${BACKEND_SERVER}/src)
INCLUDE_DIRECTORIES(${CLIENT})
INCLUDE_DIRECTORIES(${CLIENT}/include)
INCLUDE_DIRECTORIES(${CLIENT}/src)


ADD_EXECUTABLE(
    ServerRun

    ${BACKEND_SERVER}/include/Server.h
    ${BACKEND_SERVER}/include/Core.h
    ${BACKEND_SERVER}/include/Session.h
    ${BACKEND_SERVER}/include/Order.h
    ${BACKEND_SERVER}/include/OrderKeeper.h
    ${BACKEND_SERVER}/include/EqualCostOrders.h
    ${BACKEND_SERVER}/include/NotificationService.h

    ${BACKEND_SERVER}/src/Server.cpp 
    ${BACKEND_SERVER}/src/Core.cpp 
    ${BACKEND_SERVER}/src/Session.cpp
    ${BACKEND_SERVER}/src/Order.cpp
    ${BACKEND_SERVER}/src/OrderKeeper.cpp
    ${BACKEND_SERVER}/src/EqualCostOrders.cpp
    ${BACKEND_SERVER}/src/NotificationService.cpp
    
    ${BACKEND_SERVER}/serverRun.cpp

    ${COMMON}/Common.hpp
    ${COMMON}/json.hpp
)

ADD_EXECUTABLE(
    ClientRun

    ${CLIENT}/include/Client.h
    ${CLIENT}/src/Client.cpp
    
    ${CLIENT}/clientRun.cpp

    ${COMMON}/Common.hpp
    ${COMMON}/json.hpp
)

TARGET_LINK_LIBRARIES(ServerRun PRIVATE Threads::Threads ${Boost_LIBRARIES})
TARGET_LINK_LIBRARIES(ClientRun PRIVATE Threads::Threads ${Boost_LIBRARIES})
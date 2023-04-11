CMAKE_MINIMUM_REQUIRED(VERSION 3.21)
PROJECT(stockMarket)

SET(CMAKE_CXX_STANDARD 17)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
FetchContent_MakeAvailable(googletest)

FIND_PACKAGE(Boost 1.40 COMPONENTS system REQUIRED)
FIND_PACKAGE(Threads REQUIRED)

set(BACKEND_SERVER server)
set(CLIENT client)
set(COMMON common)
set(TESTS_DIR tests)

INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIR})
INCLUDE_DIRECTORIES(${COMMON})
INCLUDE_DIRECTORIES(${BACKEND_SERVER})
INCLUDE_DIRECTORIES(${BACKEND_SERVER}/include)
INCLUDE_DIRECTORIES(${BACKEND_SERVER}/src)
INCLUDE_DIRECTORIES(${CLIENT})
INCLUDE_DIRECTORIES(${CLIENT}/include)
INCLUDE_DIRECTORIES(${CLIENT}/src)
# INCLUDE_DIRECTORIES(${BACKEND_SERVER}/${TESTS_DIR})


ADD_EXECUTABLE(
    ServerRun

    ${BACKEND_SERVER}/include/Server.h
    ${BACKEND_SERVER}/include/Core.h
    ${BACKEND_SERVER}/include/Session.h
    ${BACKEND_SERVER}/include/Order.h
    ${BACKEND_SERVER}/include/OrderKeeper.h
    ${BACKEND_SERVER}/include/EqualCostOrders.h
    ${BACKEND_SERVER}/include/NotificationService.h
    ${BACKEND_SERVER}/include/Trade.h
    ${BACKEND_SERVER}/include/TradeHistory.h
    ${BACKEND_SERVER}/include/ClientInfo.h

    ${BACKEND_SERVER}/src/Server.cpp 
    ${BACKEND_SERVER}/src/Core.cpp 
    ${BACKEND_SERVER}/src/Session.cpp
    ${BACKEND_SERVER}/src/Order.cpp
    ${BACKEND_SERVER}/src/OrderKeeper.cpp
    ${BACKEND_SERVER}/src/EqualCostOrders.cpp
    ${BACKEND_SERVER}/src/NotificationService.cpp
    ${BACKEND_SERVER}/src/Trade.cpp
    ${BACKEND_SERVER}/src/TradeHistory.cpp
    ${BACKEND_SERVER}/src/ClientInfo.cpp
    
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

enable_testing()

add_executable(
    allTests

    ${BACKEND_SERVER}/${TESTS_DIR}/CoreTest.cc
    ${BACKEND_SERVER}/${TESTS_DIR}/ClientInfo.cc
    ${BACKEND_SERVER}/${TESTS_DIR}/MainTest.cc

    ${BACKEND_SERVER}/include/Server.h
    ${BACKEND_SERVER}/include/Core.h
    ${BACKEND_SERVER}/include/Session.h
    ${BACKEND_SERVER}/include/Order.h
    ${BACKEND_SERVER}/include/OrderKeeper.h
    ${BACKEND_SERVER}/include/EqualCostOrders.h
    ${BACKEND_SERVER}/include/NotificationService.h
    ${BACKEND_SERVER}/include/Trade.h
    ${BACKEND_SERVER}/include/TradeHistory.h
    ${BACKEND_SERVER}/include/ClientInfo.h

    ${BACKEND_SERVER}/src/Server.cpp 
    ${BACKEND_SERVER}/src/Core.cpp 
    ${BACKEND_SERVER}/src/Session.cpp
    ${BACKEND_SERVER}/src/Order.cpp
    ${BACKEND_SERVER}/src/OrderKeeper.cpp
    ${BACKEND_SERVER}/src/EqualCostOrders.cpp
    ${BACKEND_SERVER}/src/NotificationService.cpp
    ${BACKEND_SERVER}/src/Trade.cpp
    ${BACKEND_SERVER}/src/TradeHistory.cpp
    ${BACKEND_SERVER}/src/ClientInfo.cpp

    ${COMMON}/Common.hpp
    ${COMMON}/json.hpp
)
target_link_libraries(
    allTests
    GTest::gtest_main
    GTest::gmock
)

include(GoogleTest)
gtest_discover_tests(allTests)

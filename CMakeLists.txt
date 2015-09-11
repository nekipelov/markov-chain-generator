PROJECT(markov-chain-generator)
CMAKE_MINIMUM_REQUIRED (VERSION 2.6)

FIND_PACKAGE(Boost COMPONENTS system log locale
                              serialization program_options
                              unit_test_framework thread REQUIRED)

SET(CMAKE_CXX_FLAGS "-W -Wall -Wextra -Wshadow -fPIC -pthread -fno-omit-frame-pointer -std=c++11")
SET(CMAKE_CXX_FLAGS_DEBUG "-O0 -g3 -DDEBUG ")
SET(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")

ADD_DEFINITIONS(-DBOOST_LOG_DYN_LINK)

SET(HEADERS
    textgenerator.h
    utils.h
)

SET(SOURCES
    utils.cpp
    textgenerator.cpp
)

INCLUDE_DIRECTORIES(
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
)

ADD_EXECUTABLE(learn learn.cpp ${SOURCES} ${HEADERS})
ADD_EXECUTABLE(generator generator.cpp ${SOURCES} ${HEADERS})
ADD_EXECUTABLE(test_text_generator test.cpp ${SOURCES} ${HEADERS})

TARGET_LINK_LIBRARIES(learn
    ${Boost_LIBRARIES}
)

TARGET_LINK_LIBRARIES(generator
    ${Boost_LIBRARIES}
)

TARGET_LINK_LIBRARIES(test_text_generator
    ${Boost_LIBRARIES}
)

INCLUDE(CTest)
ADD_TEST(TextGenerator test_text_generator)

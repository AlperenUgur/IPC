cmake_minimum_required(VERSION 3.0)
project(tictactoe_game)

###############################################################################
## file globbing ##############################################################
###############################################################################

# these instructions search the directory tree when CMake is
# invoked and put all files that match the pattern in the variables
# `sources` and `data`
file(GLOB_RECURSE client_sources      
    src/client/*
    src/lib/*
)

file(GLOB_RECURSE server_sources
    src/server/*
    src/game/*
    src/lib/*
)
# you can use set(sources src/main.cpp) etc if you don't want to
# use globbing to find files automatically

###############################################################################
## target definitions #########################################################
###############################################################################

# add the data to the target, so it becomes visible in some IDE
add_executable(client ${client_sources})
add_executable(server ${server_sources})

# just for example add some compiler flags
target_compile_options(client PUBLIC -std=c++1y -Wall -Wfloat-conversion)
target_compile_options(server PUBLIC -std=c++1y -Wall -Wfloat-conversion)

# this lets me include files relative to the root source directory with a <> pair
target_include_directories(client PUBLIC 
    src/client
    src/lib
)

target_include_directories(server PUBLIC 
    src/game
    src/server
    src/lib
)

###############################################################################
## dependencies ###############################################################
###############################################################################

# this defines the variables Boost_LIBRARIES that contain all library names
# that we need to link to
# find_package(Boost 1.36.0 COMPONENTS filesystem system REQUIRED)

# target_link_libraries(client PUBLIC
#  ${Boost_LIBRARIES}
  # here you can add any library dependencies
# )
# target_link_libraries(server PUBLIC
#  ${Boost_LIBRARIES}
  # here you can add any library dependencies
# )

###############################################################################
## packaging ##################################################################
###############################################################################

# all install commands get the same destination. this allows us to use paths
# relative to the executable.
install(TARGETS client DESTINATION build/bin)
install(TARGETS server DESTINATION build/bin)

# now comes everything we need, to create a package
# there are a lot more variables you can set, and some
# you need to set for some package types, but we want to
# be minimal here
set(CPACK_PACKAGE_NAME "TicTacToe Game")
set(CPACK_PACKAGE_VERSION "1.0.0")

# we don't want to split our program up into several things
set(CPACK_MONOLITHIC_INSTALL 1)

# This must be last
include(CPack)
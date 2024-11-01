#pragma once

namespace socket_message_ns {

enum SocketMessage
{
    BOARD_STATE ,
    TURN_WAIT ,
    GAME_OVER_WIN ,
    GAME_OVER_DRAW ,
    INVALID_MOVE,
    DISCONNECT
};

}
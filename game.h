#ifndef GAME_H
#define GAME_H

namespace Game {
    static const int FrameWidth = 750;
    static const int FrameHeight = 860;
    static const int StartTick = 0;
    static const unsigned int BossTick1 = 6604;
    static const unsigned int BossTick2 = 11502;
    static const unsigned int BossTick3 = 9479;
}
namespace GameState {
    static const int Menu = 0;
    static const int Playing = 1;
    static const int Paused = 2;
    static const int Failed = 3;
    static const int Won = 4;
}
namespace GamePage {
    static const int Menu = 0;
    static const int Playing = 1;
    static const int LevelSelecting = 2;
}
namespace EndListPage {
    static const int Won = 0;
    static const int Failed = 1;
    static const int Paused = 2;
}

#endif // GAME_H

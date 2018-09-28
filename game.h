#ifndef GAME_H
#define GAME_H

namespace Game {
    static constexpr int FrameWidth = 750;
    static constexpr int FrameHeight = 860;
    static constexpr int StartTick = 0;
    static constexpr unsigned int BossTick1 = 6604;
    static constexpr unsigned int BossTick2 = 11502;
    static constexpr unsigned int BossTick3 = 9479;
    enum GameEnum {
        GamePageMenu=0,
        GamePagePlaying,
        GamePageLevelSelecting,

        GameStateMenu=0,
        GameStatePlaying,
        GameStatePaused,
        GameStateFailed,
        GameStateWon,

        EndListPageWon=0,
        EndListPageFailed,
        EndListPagePaused
    };
}

#endif // GAME_H

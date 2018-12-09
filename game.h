#ifndef GAME_H
#define GAME_H

namespace Game {

    static constexpr int FrameWidth = 750;
    static constexpr int FrameHeight = 860;
    static constexpr unsigned int BossTick1 = 3302;
    static constexpr unsigned int BossTick2 = 5751;
    static constexpr unsigned int BossTick3 = 9479;
    static constexpr unsigned int BossTick4 = 12802;

    enum GamePage {
        GamePageMenu,
        GamePagePlaying,
        GamePageLevelSelecting
    };

    enum GameState {
        GameStateMenu,
        GameStatePlaying,
        GameStatePaused,
        GameStateFailed,
        GameStateWon
    };

    enum EndListPage {
        EndListPageWon,
        EndListPageFailed,
        EndListPagePaused
    };

}

#endif // GAME_H

#ifndef GAME_H
#define GAME_H

namespace Game {

    static constexpr int FrameWidth = 750;
    static constexpr int FrameHeight = 860;
    static constexpr unsigned int BossTick1 = 3302;
    static constexpr unsigned int BossTick2 = 5752;
    static constexpr unsigned int BossTick3 = 4740;
    static constexpr unsigned int BossTick4 = 7202;

    enum GamePage {
        GamePageMenu = 0,
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

    enum Sound {
        SoundCymbal = 0,
        SoundFire,
        SoundShoot01,
        SoundHit,
        SoundBd,
        SoundWarning,
        SoundWarning02,
        SoundSnare
    };

    enum MagicType {
        MagicTypeWhite,
        MagicTypeBlue
    };

}

#endif // GAME_H

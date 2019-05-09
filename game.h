#ifndef GAME_H
#define GAME_H

namespace Game {

    static constexpr int FrameWidth = 750;
    static constexpr int FrameHeight = 860;
    static constexpr unsigned int BossTick1 = 3302;
    static constexpr unsigned int BossTick2 = 5752;
    static constexpr unsigned int BossTick3 = 4740;
    static constexpr unsigned int BossTick4 = 7202;
    static constexpr unsigned int BossTick5 = 7202;

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
        SoundSnare,
        SoundShake,
        SoundMagicShield,
        SoundMagicSmite,
        SoundLaser
    };

    enum MagicType {
        MagicTypeWhite,
        MagicTypeBlue
    };

    enum UIBaseGeometry {
        UIBaseGeometryDialogue,
        UIBaseGeometryHP,
        UIBaseGeometryHPShort,
        UIBaseGeometryEndList,
        UIBaseGeometryWarning,
        UIBaseGeometryIntro,
        UIBaseGeometrySkill,
        UIBaseGeometryLife,
        UIBaseGeometryWinWidget,
        UIBaseGeometryRestartButton_3,
        UIBaseGeometryMenuButton_3,
        UIBaseGeometryLoseLabel,
        UIBaseGeometryRestartButton_2,
        UIBaseGeometryMenuButton_2,
        UIBaseGeometryPauseLabel,
        UIBaseGeometryRestartButton,
        UIBaseGeometryMenuButton,
        UIBaseGeometryContinueButton
    };

    enum ZValue{
        ZValueUI = 5,
        ZValueFlash = 4,
        ZValueDot = 3,
        ZValueForeEffect = 2,
        ZValueInFrontOfStandard = 1,
        ZValueStandard = 0,
        ZValueBehindStandard = -1,
        ZValueLaser = -2,
        ZValueImportantBackEffect = -3,
        ZValueTeammate = -4,
        ZValueBackEffect = -5,
        ZValueBackground = -6,
    };
}

#endif // GAME_H

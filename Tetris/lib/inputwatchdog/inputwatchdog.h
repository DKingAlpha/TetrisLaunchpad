#pragma once
#include "../console/gameui.h"


class GameUI;
class InputWatchDog
{
public:
    InputWatchDog(GameUI* game);
    void startWatch();
    void stopWatch();
    void reloadHandle();


private:
    void proc_console();
    void proc_launchpad();

    void LaunchpadKeyEventProc(int key);
    void KeyEventProc(KEY_EVENT_RECORD ker);
    void MouseEventProc(MOUSE_EVENT_RECORD mer);

    HANDLE hConsoleIn = NULL;
    GameUI* gameui = NULL;
    DWORD fdwSaveOldMode, fdwNewMode;
    bool jobstarted = false;

};

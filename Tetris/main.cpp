#include "lib/launchpad/launchpad.h"
#include "lib/console/gameui.h"
#include <iostream>

int main()
{
    GameUI* game = new GameUI();

    while (true)
    {
       Sleep(100);
    }

    free(game);

    return 0;

}
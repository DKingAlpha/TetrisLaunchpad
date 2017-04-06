#include "inputwatchdog.h"
#include <vector>

void InputWatchDog::proc_console()
{
    INPUT_RECORD irInBuf[128];
    while (true)
    {
        if (gameui->cast2console)
        {
            DWORD cNumRead;
            if (ReadConsoleInput(hConsoleIn, irInBuf, 128, &cNumRead))
            {
                for (int i = 0; i < cNumRead; i++)
                {
                    switch (irInBuf[i].EventType)
                    {
                    case KEY_EVENT:
                        KeyEventProc(irInBuf[i].Event.KeyEvent);
                        break;

                    case MOUSE_EVENT:
                        MouseEventProc(irInBuf[i].Event.MouseEvent);
                        break;

                    case FOCUS_EVENT:
                        irInBuf[i].Event.FocusEvent.bSetFocus ?
                            gameui->resumeGame() :
                            gameui->pauseGame();
                        break;

                    default:
                        break;
                    }
                }
            }
        }
        Sleep(20);
      }
}

void InputWatchDog::proc_launchpad()
{
    while (true)
    {
        if (gameui->cast2launchpad)
        {
            vector<BYTE> lpdmsg;
            gameui->lpd->getMessage(&lpdmsg);
            if (lpdmsg.size() > 0)
            {
                int  key = 0;
                if ((lpdmsg[0] == (BYTE)0x90 || lpdmsg[0] == (BYTE)0xB0) && lpdmsg[2] != (BYTE)0)
                {
                    key = lpdmsg[1];
                    std::thread(&InputWatchDog::LaunchpadKeyEventProc, this, key).detach();
                }
            }
        }
        Sleep(20);
    }
}

void InputWatchDog::LaunchpadKeyEventProc(int key)
{
    if (gameui->gamerunning == false)
    {
        if (key == 44 || key == 45 || key == 54 || key == 55)
        {
            gameui->startGame();
            return;
        }
    }
    else
    {
        if (key % 10 >= 0 && key % 10 <= 2 && key / 10 > 0 && key / 10 < 9)
        {
            gameui->pressLeft();
        }
        if (key % 10 >= 7 && key % 10 <= 9 && key / 10 > 0 && key / 10 < 9)
        {
            gameui->pressRight();

        }
        if (key % 10 >= 3 && key % 10 <= 6 && key / 10 >= 3 && key / 10 <= 8)
        {
            gameui->pressRotate();

        }
        if (key >= 13 && key <= 16)
        {
            gameui->pressDown();
        }

        if (key >= 91 && key <= 95)
        {
            gameui->speed = key - 90;
            gameui->drawLayout();

        }
        if (key == 98)
        {
            gameui->stopGame();
            gameui->startGame();
        }

        if (key == 96)
        {
            gameui->gamepaused = !gameui->gamepaused;
        }
    }
}

void InputWatchDog::KeyEventProc(KEY_EVENT_RECORD ker)
{
    if (ker.bKeyDown)
    {
        if (ker.wVirtualKeyCode == VK_LEFT)
        {
            gameui->pressLeft();
        }
        if (ker.wVirtualKeyCode == VK_RIGHT)
        {
            gameui->pressRight();
        }
        if (ker.wVirtualKeyCode == VK_DOWN)
        {
            gameui->pressDown();
        }
        if (ker.wVirtualKeyCode == VK_UP || ker.wVirtualKeyCode == VK_SPACE)
        {
            gameui->pressRotate();
        }
        if (ker.wVirtualKeyCode == VK_ESCAPE)
        {
            gameui->exitGame();
        }

    }
}

void InputWatchDog::MouseEventProc(MOUSE_EVENT_RECORD mer)
{
    switch (mer.dwEventFlags)
    {
    case 0:
        if (mer.dwButtonState == FROM_LEFT_1ST_BUTTON_PRESSED)
        {
            gameui->buttonMapper(mer.dwMousePosition.X, mer.dwMousePosition.Y);
        }
    default:
        break;
    }
}


InputWatchDog::InputWatchDog(GameUI* game)
{
    gameui = game;
    hConsoleIn = GetStdHandle(STD_INPUT_HANDLE);
}

void InputWatchDog::startWatch()
{
    GetConsoleMode(hConsoleIn, &fdwSaveOldMode);
    fdwNewMode = fdwSaveOldMode | ENABLE_MOUSE_INPUT;
    SetConsoleMode(hConsoleIn, fdwNewMode);

    jobstarted = true;
    std::thread(&InputWatchDog::proc_console, this).detach();
    std::thread(&InputWatchDog::proc_launchpad, this).detach();

}

void InputWatchDog::stopWatch()
{
    jobstarted = false;
    SetConsoleMode(hConsoleIn, fdwSaveOldMode);
}

void InputWatchDog::reloadHandle()
{
    hConsoleIn = GetStdHandle(STD_INPUT_HANDLE);
}

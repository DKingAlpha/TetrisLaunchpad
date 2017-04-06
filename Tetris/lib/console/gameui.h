#pragma once
#include <Windows.h>
#include <stdlib.h>
#include <thread>
#include <iostream>
#include <time.h>
#include "../launchpad/launchpad.h"
#include "../inputwatchdog/inputwatchdog.h"

#define FG_BLACK        0
#define FG_BLUE         1
#define FG_GREEN        2
#define FG_RED          4
#define FG_CYAN         3
#define FG_PINK         5
#define FG_YELLOW       6
#define FG_WHITE        7

#define BG_BLACK        15
#define BG_BLUE         16
#define BG_GREEN        32
#define BG_RED          64
#define BG_CYAN         48
#define BG_PINK         80
#define BG_YELLOW       96
#define BG_WHITE        112


#define FG_INTENSITY    8
#define BG_INTENSITY    128

// https://msdn.microsoft.com/en-us/library/windows/desktop/ms685035(v=vs.85).aspx
// https://msdn.microsoft.com/en-us/library/windows/desktop/ms682662(v=vs.85).aspx

// FillConsoleOutputAttribute
// FillConsoleOutputCharacter 
// SetConsoleTextAttribute
// SetConsoleCursorPosition 

class InputWatchDog;
class Tetris;

class GameUI
{
    public:
        GameUI();
        ~GameUI();

        void initUI();
        void loadLaunchpad();
        void incWidth();
        void decWidth();
        void incHeight();
        void decHeight();
        void incSpeed();
        void decSpeed();
        void resizeScene(int x, int y);
        void setColor(int color);
        void cleanScreen();
        void initAllScenes();


        void startGame();
        void stopGame();
        void exitGame();
        void pauseGame();
        void resumeGame();
        void cast2Device();
        void drawLayout();
        void drawNextBlock();
        void drawScore();
        void gameOver();
        void prepareDebug();

        void pressUp();
        void pressDown();
        void pressLeft();
        void pressRight();
        void pressRotate();

        void buttonMapper(int x,int y);
        int  findLpdColor(int con_color);

        bool gamerunning = false , gamepaused = false;
        bool cast2console = false, cast2launchpad = false;
        LaunchpadPro * lpd = NULL;
        int* bg_scene = NULL, *lastrenderedscene = NULL,* renderedscene = NULL;
        int width = 8, height = 8, speed = 3;

        int ** canvas = NULL; int canvas_height = 0, canvas_width = 0;      // for collision detection

    private:
        void gameloop();
        
        HANDLE hConsoleOut = NULL;
        Tetris* tetris = NULL;
        InputWatchDog* inputWD = NULL;
        int score = 0;
        COORD layout_offset = {};
        COORD scene_offset = {2, 1};
        int status_left_pressed = false, status_right_pressed = false, status_rotate_pressed = false, status_down_pressed = false;
};

class Tetris
{
public:
    Tetris(GameUI* GUI);
    void initBlocks();
    void callNextBlock();
    void prepareBlock();
    bool isLegalMove(int direction);
    bool isSet();
    bool isFull();
    int matchAction();
    void saveScene();
    void composeFrame();
    bool move_Left();
    bool move_Right();
    bool move_Down();
    bool move_Rotate();
    long gametime = 0;
    bool frameChanged = false;
    int* currentblock = new int[16]{}, *nextblock = new int[16]{};

private:

    void randomizeNextBlock();
    void rotateBlock(int* blk, int times);
    int current_height = 0;
    clock_t starttime = 0;
    vector<int*> blocksLib;
    int currentblockid = 0,  nextblockid = 0;
    GameUI* gui = NULL;
    COORD block_offset = {};

};
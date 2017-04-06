#include "gameui.h"

GameUI::GameUI()
{
    loadLaunchpad();
    initUI();
    inputWD = new InputWatchDog(this);
    inputWD->startWatch();
}

GameUI::~GameUI()
{
    free(bg_scene);
    free(renderedscene);
    free(lastrenderedscene);
    free(lpd);
    free(tetris);
}


void GameUI::drawLayout()
{
    if (cast2console)
    {
        cleanScreen();
        setColor(FG_WHITE|BG_BLACK);
        COORD pos = { 0, 0 };
        DWORD written = 0;
        for (int y = 1; y < height + 1; y++)
        {
            pos.Y = y;
            FillConsoleOutputCharacter(hConsoleOut, L'▕', 2, pos, &written);
        }
        pos.X = 2 * (1 + width);
        for (int y = 1; y < height + 1; y++)
        {
            pos.Y = y;
            FillConsoleOutputCharacter(hConsoleOut, L'▏', 2, pos, &written);
        }

        pos.X = 2; pos.Y = 0;
        FillConsoleOutputCharacter(hConsoleOut, L'▁', 2 * width, pos, &written);
        pos.X = 2; pos.Y = height + 1;
        FillConsoleOutputCharacter(hConsoleOut, L'▔', 2 * width, pos, &written);

        pos.X = layout_offset.X + 1, pos.Y = 0;
        FillConsoleOutputCharacter(hConsoleOut, L'╔', 2, pos, &written);
        pos.X += 2 * 5;
        FillConsoleOutputCharacter(hConsoleOut, L'╗', 2, pos, &written);
        pos.X = layout_offset.X + 1; pos.Y = 5;
        FillConsoleOutputCharacter(hConsoleOut, L'╚', 2, pos, &written);
        pos.X += 2 * 5;
        FillConsoleOutputCharacter(hConsoleOut, L'╝', 2, pos, &written);

        setColor(BG_WHITE | FG_BLACK);
        pos.X = scene_offset.X ; pos.Y = scene_offset.Y  + height + 1;
        SetConsoleCursorPosition(hConsoleOut, pos);
        std::cout << "开始";
        pos.X = scene_offset.X + 2 * width - 4; pos.Y = scene_offset.Y + height + 1;
        SetConsoleCursorPosition(hConsoleOut, pos);
        std::cout << "结束";
        pos.X = scene_offset.X + 2 * width + 3 ; pos.Y = scene_offset.Y + height + 1;
        SetConsoleCursorPosition(hConsoleOut, pos);
        setColor(FG_WHITE | BG_BLACK);
        std::cout << "得分:";
        setColor(BG_INTENSITY | FG_RED);
        printf("%5d", score);


        setColor(FG_WHITE | BG_BLACK);
        pos.X = layout_offset.X; pos.Y = 6;
        SetConsoleCursorPosition(hConsoleOut, pos);
        std::cout << " 宽  << ";
        printf("%2d", width);
        std::cout << " >>";
        pos.Y += 1;
        SetConsoleCursorPosition(hConsoleOut, pos);
        std::cout << " 高  << ";
        printf("%2d", height);
        std::cout << " >>";
        pos.Y += 1;
        SetConsoleCursorPosition(hConsoleOut, pos);
        std::cout << "速度 << ";
        printf("%2d", speed);
        std::cout << " >>";
        setColor(FG_WHITE | BG_BLACK);
    }
    if (cast2launchpad)
    {
        for (int i = 1; i <= 5; i++)
            i <= speed ? lpd->setPulse(i + 90, 28 + (i - 1) * 8) : lpd->setColor(i + 90, 0);
    }
}

void GameUI::drawNextBlock()
{
    COORD pos = { 0, 0 };
    for (int row = 0; row < 4; row++)
    {
        pos.Y = row + layout_offset.Y + 1;
        for (int col = 0; col < 4; col++)
        {
            pos.X = 2 * col + layout_offset.X + 3;
            SetConsoleCursorPosition(hConsoleOut, pos);
            int color = tetris->nextblock[row*4 + col];
            setColor(tetris->nextblock[row*4 + col]);
            color ? printf("█") : printf("  ");
        }
    }
    setColor(FG_WHITE | BG_BLACK);
}


void GameUI::loadLaunchpad()
{
    lpd = new LaunchpadPro();
    lpd->connect();
    if (lpd->isConnected())
    {
        cast2launchpad = true;
        lpd->clearPad();
        lpd->setColor(98, 0);
        lpd->displayText(45, 0, 7, "Tetris");
    }
}

void GameUI::initUI()
{
    // Open stdout handle , Monitor input events to console
    hConsoleOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hConsoleOut)
    {
        cast2console = true;
        system("title Launchpad Tetris");
        setColor(FG_WHITE | BG_BLACK);
        cleanScreen();

        layout_offset.X = scene_offset.X + 2 * (width + 1);
        layout_offset.Y = 0;

        // Adjust font size
        CONSOLE_FONT_INFOEX fontinfo = { sizeof(CONSOLE_FONT_INFOEX) };
        GetCurrentConsoleFontEx(hConsoleOut, 0, &fontinfo);
        fontinfo.dwFontSize.X = 12;
        fontinfo.dwFontSize.Y = 24;
        SetCurrentConsoleFontEx(hConsoleOut, 0, &fontinfo);

        // Resize console size
        COORD bigbuffer = { 200, 300 };
        COORD screesize = { 2 * (width + 10),height + 4 };
        SMALL_RECT conrect = { 0, 0 ,  2 * (width + 10) - 1,height + 4 - 1 };
        CONSOLE_CURSOR_INFO concurinfo = { 1,false };
        SetConsoleScreenBufferSize(hConsoleOut, bigbuffer);
        SetConsoleWindowInfo(hConsoleOut, true, &conrect);
        SetConsoleScreenBufferSize(hConsoleOut, screesize);

        SetConsoleCursorInfo(hConsoleOut, &concurinfo);

        drawLayout();
    }
    initAllScenes();

}

void GameUI::resizeScene(int x, int y)
{
    // delete old canvas
    for (int i = 0; i<height; i++)
    {
        delete canvas[i];
    }
    delete canvas;


    width = x; height = y;

    layout_offset.X = scene_offset.X + 2 * (width + 1);
    layout_offset.Y = 0;

    if (cast2console)
    {
        COORD bigbuffer = { 200, 300 };
        COORD screesize = { 2 * (width +10),height + 4 };
        SMALL_RECT conrect = { 0, 0 ,  2 * (width + 10) - 1,height + 4 - 1 };
        SetConsoleScreenBufferSize(hConsoleOut, bigbuffer);
        SetConsoleWindowInfo(hConsoleOut, true, &conrect);
        SetConsoleScreenBufferSize(hConsoleOut, screesize);
    }

    // cleanup and reallocate 
    free(bg_scene); free(lastrenderedscene); free(renderedscene);
    initAllScenes();
    drawLayout();
    inputWD->reloadHandle();

}


void GameUI::setColor(int color)
{
    // this only works for cout/printf.  Not working on FillConsoleOutputCharactor, which needs FillConsoleOutputAttributes
    SetConsoleTextAttribute(hConsoleOut, color);
}

void GameUI::cleanScreen()
{
    COORD pos = { 0, 0 };
    SetConsoleCursorPosition(hConsoleOut, pos);
    setColor(FG_BLACK|BG_BLACK);
    string buf = "";
    for (int i = 0; i < 2 * (width + 12)*(height+4); i++)
    {
        buf += " ";
    }
    cout << buf;
    setColor(FG_WHITE | BG_BLACK);
}

void GameUI::initAllScenes()
{

    // Allocate memory for scenes
    bg_scene = (int*)malloc(width * height * sizeof(int));
    memset(bg_scene, 0, width * height * sizeof(int));
    lastrenderedscene = (int*)malloc(width * height * sizeof(int));
    memset(lastrenderedscene, 0, width * height * sizeof(int));
    renderedscene = (int*)malloc(width * height * sizeof(int));
    memset(renderedscene, 0, width * height * sizeof(int));

    // create a canvas to do collision detection
    canvas_height = height + 8;
    canvas_width = width + 8;
    canvas = new int*[canvas_height];
    for (int i = 0; i<canvas_height; i++)
    {
        canvas[i] = new int[canvas_width];
    }
    // memset for self-defined [][] structure
    for (int row = 0; row < canvas_height; row++)
    {
        for (int col = 0; col < canvas_width; col++)
        {
            canvas[row][col] = 0;
        }
    }
}

void GameUI::drawScore()
{
    COORD pos = { scene_offset.X + 2 * width + 8  , scene_offset.Y + height + 1 };
    SetConsoleCursorPosition(hConsoleOut, pos);
    DWORD written = 0;
    FillConsoleOutputCharacter(hConsoleOut, L' ', 5 , pos, &written);
    setColor(BG_INTENSITY | FG_RED);
    printf("%5d", score);
    setColor(FG_WHITE | BG_BLACK);
}

void GameUI::gameOver()
{
    if (cast2console)
    {
        COORD pos = {};
        setColor(BG_WHITE | FG_BLACK);
        pos.X = scene_offset.X + width - 4 ; pos.Y = scene_offset.Y + height /2 ;
        SetConsoleCursorPosition(hConsoleOut, pos);
        std::cout << "游戏结束";
        setColor(FG_WHITE | BG_BLACK);
    }
    if (cast2launchpad)
    {
        lpd->playMidiFile_speed("midires/gameover.mid",8);
    }
}

void GameUI::prepareDebug()
{
    COORD pos = {0, height + 2};
    SetConsoleCursorPosition(hConsoleOut,pos);
    DWORD written = 0;
    FillConsoleOutputCharacter(hConsoleOut, L' ', 40, pos, &written);

}

void GameUI::buttonMapper(int x, int y)
{
    if (((x >= scene_offset.X) || (x < scene_offset.X + 4)) && (y == scene_offset.Y + height + 1))startGame();
    if (((x >= scene_offset.X + 2 * width - 4) || (x < scene_offset.X + 2 * width)) && (y == scene_offset.Y + height + 1)) stopGame();
    if (((x == layout_offset.X + 5) || (x == layout_offset.X + 6)) && y == 6) decWidth();
    if (((x == layout_offset.X + 11) || (x == layout_offset.X + 12)) && y == 6) incWidth();
    if (((x == layout_offset.X + 5) || (x == layout_offset.X + 6)) && y == 7) decHeight();
    if (((x == layout_offset.X + 11) || (x == layout_offset.X + 12)) && y == 7) incHeight();
    if (((x == layout_offset.X + 5) || (x == layout_offset.X + 6)) && y == 8) decSpeed();
    if (((x == layout_offset.X + 11) || (x == layout_offset.X + 12)) && y == 8) incSpeed();
}

int GameUI::findLpdColor(int con_color)
{
    switch (con_color)
    {
    case FG_BLUE:
        return 45;
    case FG_GREEN:
        return 64;
    case FG_RED:
        return 5;
    case FG_CYAN:
        return 46;
    case FG_PINK:
        return 52;
    case FG_YELLOW:
        return 12;
    case FG_WHITE:
        return 3;

    default:
        return 3;
    }
    return 3;
}

void GameUI::gameloop()
{
    int delta_frame_time = 20;
    while (gamerunning)
    {
        while (gamepaused)
        {
            Sleep(delta_frame_time);
        }

        tetris->gametime += delta_frame_time;

        if (status_left_pressed)
        {
            tetris->move_Left();
            status_left_pressed = false;
        }
        if (status_right_pressed)
        {
            tetris->move_Right();
            status_right_pressed = false;
        }
        if (status_down_pressed)
        {
            tetris->move_Down();
            status_down_pressed = false;
        }
        if (status_rotate_pressed)
        {
            tetris->move_Rotate();
            status_rotate_pressed = false;
        }

        if (tetris->isSet())
        {
            for (int i = 0; i < height*width; i++)
            {
                if (renderedscene[i])score++;
            }
            int hitrow = -1;
            while ((hitrow = tetris->matchAction()) >= 0)
            {
                if (cast2launchpad)
                {
                    for (int i = 1; i <= 8; i++)
                    {
                        lpd->setPulse(8 - hitrow, i, 3);
                    }
                    Sleep(1000);
                    lpd->setRowColor(hitrow, 0);
                }
                score += (height- hitrow)*width;
                if (tetris->frameChanged)
                {
                    cast2Device();
                    tetris->frameChanged = false;
                }
            }
            if (tetris->isFull())
            {
                gameOver();
                stopGame();
                return;
            }
            else
                tetris->callNextBlock();
        }




        if (tetris->frameChanged)
        {
            cast2Device();
            tetris->frameChanged = false;
        }

        Sleep(delta_frame_time);
    }


}

void GameUI::cast2Device()
{
    drawNextBlock();
    drawScore();

    for (int row = 0; row < height; row++)
    {
        for (int col = 0; col < width ; col++)
        {
            int offset = col + width*row;
            // this optimization will generate ghost block. Discarded.
            //if (renderedscene[offset] != lastrenderedscene[offset])
            {
                int color = renderedscene[offset];
                if (cast2console)
                {
                    COORD shiftedpos = { 2*col + scene_offset.X , row + scene_offset.Y };
                    DWORD written;
                    SetConsoleCursorPosition(hConsoleOut, shiftedpos);
                    setColor(color);
                    color ?  printf("█") :  printf("  ");
                }
                if (cast2launchpad)
                {
                    color ? lpd->setColor(8 - row, col + 1, findLpdColor(color)) : lpd->setColor(8 - row, col + 1, 0);
                }
            }
        }
    }


}


void GameUI::incWidth()
{
    stopGame();
    if (width > 30)return;
    width++;
    resizeScene(width, height);
}

void GameUI::decWidth()
{
    stopGame();
    if (width <= 8)return;
    width--;
    resizeScene(width, height);

}

void GameUI::incHeight()
{
    stopGame();
    if (height > 100)return;
    height++;
    resizeScene(width, height);
}

void GameUI::decHeight()
{
    stopGame();
    if (height <= 8)return;
    height--;
    resizeScene(width, height);
}

void GameUI::incSpeed()
{
    if (speed >= 5)return;
    speed++;
    drawLayout();
}

void GameUI::decSpeed()
{
    if (speed <= 1)return;
    speed--;
    drawLayout();
}

void GameUI::pressUp()
{
    pressRotate();
}

void GameUI::pressDown()
{
    status_down_pressed = true;
    std::thread(&LaunchpadPro::playMidiFile_speed, lpd, "midires/down.mid", 8).detach();
}

void GameUI::pressLeft()
{
    status_left_pressed = true;
    std::thread(&LaunchpadPro::playMidiFile_speed, lpd, "midires/left.mid", 8).detach();
}

void GameUI::pressRight()
{
    status_right_pressed = true;
    std::thread(&LaunchpadPro::playMidiFile_speed, lpd, "midires/right.mid", 8).detach();
}

void GameUI::pressRotate()
{
    status_rotate_pressed = true;
    std::thread(&LaunchpadPro::playMidiFile_speed, lpd, "midires/rotate.mid" ,8).detach();
}



void GameUI::startGame()
{
    if (!gamerunning)
    {
        if (lpd->isConnected() && width == 8 && height == 8) 
            cast2launchpad = true;
        lpd->playMidiFile_speed("midires/start.mid",8);
        tetris = new Tetris(this);
        inputWD->reloadHandle();
        gamerunning = true;
        gamepaused = false;
        score = 0;
        initAllScenes();
        tetris->initBlocks();
        if (cast2console)cleanScreen();
        if (cast2launchpad)lpd->clearPad();
        drawLayout();

        std::thread(&GameUI::gameloop,this).detach();
        //gameloop();
    }
}

void GameUI::stopGame()
{
    if (gamerunning)
    {
        gamerunning = false;
        if (cast2console)cleanScreen();
        if (cast2launchpad)lpd->clearPad();
        delete tetris;
        drawLayout();
    }
}

void GameUI::exitGame()
{
    gamerunning = false;
    inputWD->stopWatch();
    lpd->clearPad();
    lpd->disconnect();
    stopGame();
    system("exit");
}

void GameUI::pauseGame()
{
    gamepaused = true;
}

void GameUI::resumeGame()
{
    gamepaused = false;
}


Tetris::Tetris(GameUI* GUI)
{
    int* block_1 = new int[16] {
        0,0,0,0,
        1,1,1,1,
        0,0,0,0,
        0,0,0,0 };
    blocksLib.push_back(block_1);

    int* block_L = new int[16]{
        0,1,0,0,
        0,1,0,0,
        0,1,1,0,
        0,0,0,0 };
    blocksLib.push_back(block_L);
    
    int* block_L_rev = new int[16]{
        0,0,1,0,
        0,0,1,0,
        0,1,1,0,
        0,0,0,0 };
    blocksLib.push_back(block_L_rev);

    int* block_2 = new int[16]{
        0,0,0,0,
        0,0,0,0,
        0,1,1,0,
        0,0,0,0 };
    blocksLib.push_back(block_2);

    int* block_0 = new int[16]{
        0,0,0,0,
        0,1,1,0,
        0,1,1,0,
        0,0,0,0 };
    blocksLib.push_back(block_0);

    int* block_Z = new int[16]{
        0,0,0,0,
        1,1,0,0,
        0,1,1,0,
        0,0,0,0 };
    blocksLib.push_back(block_Z);

    int* block_Z_rev = new int[16]{
        0,0,0,0,
        0,1,1,0,
        1,1,0,0,
        0,0,0,0 };
    blocksLib.push_back(block_Z_rev);

    int* block_7 = new int[16]{
        0,0,0,0,
        0,1,0,0,
        0,1,1,0,
        0,0,0,0 };
    blocksLib.push_back(block_7);

    int* block_7_rev = new int[16]{
        0,0,0,0,
        0,0,1,0,
        0,1,1,0,
        0,0,0,0 };
    blocksLib.push_back(block_7_rev);
    

    gui = GUI;

    initBlocks();

}

void Tetris::initBlocks()
{

    srand((unsigned)time(NULL));
    currentblockid = (rand() % (blocksLib.size()));
    memcpy(currentblock, blocksLib[currentblockid], 16 * sizeof(int));
    rotateBlock(currentblock, rand() % 4);
    int color = rand() % 6 + 2;
    for (int i = 0; i < 16; i++)
    {
        currentblock[i] *= color;
    }

    prepareBlock();
    randomizeNextBlock();
}

void Tetris::callNextBlock()
{
    // swap pointer with next block
    currentblockid = nextblockid;
    int* tmp = currentblock;
    currentblock = nextblock;
    nextblock = tmp;
    // gen a new block in var next
    randomizeNextBlock();
    prepareBlock();

    // save rendered scene as background scene
    for (int i = 0; i < gui->width * gui->height; i++)
    {
        (gui->bg_scene)[i] = (gui->renderedscene)[i];
    }

}

void Tetris::prepareBlock()
{
    // adjust Y
    block_offset.Y = - 4;
    for (int i = 3; i >= 0; i--)
    {
        int line_zero = true;
        for (int j = 0; j < 4; j++)
        {
            if (currentblock[4 * i + j] > 0)
            {
                line_zero = false;
                break;
            }
        }
        if (line_zero)
            block_offset.Y++;
        else
            break;
    }
    // random X
    block_offset.X = rand() % (gui->width-4) ;
}

bool Tetris::isLegalMove(int direction)
{
    COORD post_offset = {};
    int* tmp_block = currentblock;

    if (direction == 1)
    {
        post_offset.X = block_offset.X;
        post_offset.Y = block_offset.Y + 1;

    }
    if (direction == 2)
    {
        post_offset.X = block_offset.X - 1;
        post_offset.Y = block_offset.Y;
    }
    if (direction == 3)
    {
        post_offset.X = block_offset.X + 1;
        post_offset.Y = block_offset.Y;

    }
    if (direction == 4)
    {
        tmp_block = (int*)malloc(gui->width*gui->height * sizeof(int));
        rotateBlock(tmp_block, 1);
        post_offset.X = block_offset.X;
        post_offset.Y = block_offset.Y;
    }

    for (int row = 0; row < gui->canvas_height; row++)
    {
        for (int col = 0; col < gui->canvas_width; col++)
        {
            gui->canvas[row][col] = 0;
        }
    }

    for (int row = 0; row < gui->height; row++)
    {
        for (int col = 0; col < gui->width; col++)
        {
            gui->canvas[row + 4][col + 4] = gui->bg_scene[row*gui->width + col];
        }
    }

    for (int row = 3; row >= 0; row--)
    {
        for (int col = 0; col < 4; col++)
        {
            int c_row = row + 4 + post_offset.Y, c_col = col + 4 + post_offset.X;
            int var1 = gui->canvas[c_row][c_col];
            int var2 = tmp_block[row * 4 + col];
            if (var2) // block_sign > 0
            {
                // var2 out of scene
                if (c_row >= 4 + gui->height || c_col < 4 || c_col >= 4 + gui->width)
                {
                    if (direction == 4)free(tmp_block);
                    return false;
                }
                else
                {
                    // In range, Conflict with scene
                    if (var1)
                    {
                        if (direction == 4)free(tmp_block);
                        return false;
                    }
               }
                // not conflict. check next one
            }
        }
    }

    return true;

}

bool Tetris::isSet()
{
    int period = 200 * (6 - gui->speed);
    if (gametime > period)
    {
        gametime %= period;
        return !move_Down();
    }
    else
    {
        return false;
    }
}

bool Tetris::isFull()
{
    for (int i = 0; i < gui->width; i++)
    {
        if (gui->renderedscene[i])return true;
    }
    return false;
}

int Tetris::matchAction()
{
    int hit = -1;
    int row, col;
    for (row = gui->height - 1; row >= 0; row--)
    {
        int found0 = false;
        for (col = 0; col < gui->width; col++)
        {
            if (gui->renderedscene[row*gui->width + col] == 0)
            {
                found0 = true;
                break;
            }
        }
        if (found0 == false)
        {
            hit = row;
            for (; row >= 0; row--)
            {
                for (col = 0; col < gui->width; col++)
                {
                    if (row)
                        gui->renderedscene[row*gui->width + col] = gui->renderedscene[(row - 1)*gui->width + col];
                    else
                        gui->renderedscene[row*gui->width + col] = 0;
                }
            }
            frameChanged = true;
            return hit;
        }
    }

    return hit;
}

void Tetris::saveScene()
{
    for (int iter = 0; iter < gui->width * gui->height; iter++)
    {
        (gui->lastrenderedscene)[iter] = (gui->renderedscene)[iter];
    }
}

bool Tetris::move_Down()
{
    bool legal = isLegalMove(1);
    if (legal)
    {
        block_offset.Y++;
        composeFrame();
    };
    return legal;
}

bool Tetris::move_Left()
{
    bool legal = isLegalMove(2);
    if (legal)
    {
        block_offset.X--;
        composeFrame();
    };
    return legal;
}

bool Tetris::move_Right()
{
    bool legal = isLegalMove(3);
    if (legal)
    {
        block_offset.X++;
        composeFrame();
    };
    return legal;
}

bool Tetris::move_Rotate()
{
    bool legal = isLegalMove(4);
    if (legal)
    {
        rotateBlock(currentblock, 1);
        composeFrame();
    };
    return legal;
}

void Tetris::composeFrame()
{
    saveScene();
    frameChanged = true;

    for (int i = 0; i < gui->width*gui->height; i++)
        gui->renderedscene[i] = gui->bg_scene[i];

    for (int row = 0; row < 4; row++)
    {
        for (int col = 0; col < 4; col++)
        {
            int s_row = row + block_offset.Y, s_col = col + block_offset.X;
            if( s_row >= 0 && s_row < gui->height && s_col >=0 && s_col < gui->width &&
                ! gui->renderedscene[s_row * gui->width + s_col]
                )
                 gui->renderedscene[s_row * gui->width + s_col] = currentblock[row * 4 + col];
        }
    }
}

void Tetris::rotateBlock(int* blk, int times)
{
    for (int i = 0; i < times; i++)
    {
        int layer;
        for (layer = 0; layer< 2; layer++)
        {
            int first = layer;
            int last = 3 - layer;
            int i;
            for (i = layer; i<last; i++)
            {
                int offset = i - layer;
                char top = blk[first * 4 + i];
                blk[first * 4 + i] = blk[(last - offset) * 4 +first];
                blk[(last - offset) * 4 +first] = blk[last * 4 + (last - offset)];
                blk[last * 4 + (last - offset)] = blk[i * 4 +last];
                blk[i * 4 +last] = top;
            }
        }
    }
}

void Tetris::randomizeNextBlock()
{
    nextblockid = (rand() % (blocksLib.size()));
    memcpy(nextblock, blocksLib[nextblockid], 16 * sizeof(int));
    rotateBlock(nextblock, rand() % 4);
    int color = rand() % 7 + 1;
    for (int i = 0; i < 16; i++)
    {
        nextblock[i] *= color;
    }
}
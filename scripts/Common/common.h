#ifndef PROJECT_NAME_COMMON_H
#define PROJECT_NAME_COMMON_H

#include <bits/stdc++.h>
#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_image.h>
#include <SDL_mouse.h>

using namespace std;

extern map<string, SDL_Color> defualtTheme;


struct Mouse {
    int x, y, scrolloffsetCodeMenu, scrolloffsetBlocksPlayGround;
    bool isMouseDown = false;
    bool isClicked = false;
};

struct Spirit {
    string spiritName = "spirit-1";
    int spiritX = 0, spiritY = 0, direction = 0
            /* varies from 0 to 360, 0 -> faced to the right, 90 -> faced upward and so on ... */ , size = 100;
    bool isShow = true;
    SDL_Texture* texture = nullptr;
};

struct App;

enum BlockType {
    START,
    MOTION_SETX,
    MOTION_SETY
};

struct Block {
    string blockMode, blockName;
    SDL_Color bgColor;
    BlockType type;
};

struct App {
    int DW, DH;
    bool isRunning = true , isInDebug = false , isExecuting = false;
    map<string, SDL_Color> theme = defualtTheme;
    string selectedMode = "Code", selectedBlockTypeInCodeMenu = "Motion";
    Mouse mouse;
    Spirit spirit;
    vector<Block> executeList;
    // int executeIndex = 0;
};

extern App scratchApp;

struct Button {
    SDL_Rect rect;
    char *text;
    SDL_Color normalColor, hoverColor, focusColor, textColor, focusTextColor;
    SDL_Color currentColor = normalColor, currentTextColor = textColor;
};

extern SDL_Window *m_window;
extern SDL_Renderer *m_renderer;
extern TTF_Font *font , *fontSmall;

bool prepPhase(int fontSize, int fontSizeSmall);

void endPhase();

SDL_Color changeBrightness(SDL_Color color, int amount);

bool isPointerInElement(SDL_Rect rect, int mx, int my);

void updateMouse(bool isMouseDown);

void updateScrolloffsetCodeMenu(int viewportHeight, int contentHeight, SDL_Event &event);

void isClicked(const SDL_Event &event);

void updateScrolloffsetBlockPlayGround(int viewportHeight, int contentHeight, SDL_Event &event);

void removeLastBlockFromExecuteList();

bool loadSpiritTexture(Spirit spirit);

#endif //PROJECT_NAME_COMMON_H

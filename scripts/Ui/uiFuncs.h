#ifndef PROJECT_NAME_FUNCS_H
#define PROJECT_NAME_FUNCS_H

#include "../Common/common.h"

void drawReact(int x, int y, int w, int h, SDL_Color bgColor);

void drawReactWithBorder(int x, int y, int w, int h, SDL_Color bgColor, SDL_Color borderColor, int borderWidth);


void renderFilledButton(const Button &btn);

void updateButton(Button &btn, bool mouseDown);

void renderText(const string &text, int x, int y, SDL_Color color, TTF_Font *f = font);

void drawCircle(int centerX, int centerY, int radius, SDL_Color color);

void renderBlockType(string blockType, int x, int y, int r, SDL_Color color);

void drawFilledCircleWithBorder(int centerX,
                                int centerY,
                                int radius,
                                SDL_Color fillColor,
                                SDL_Color borderColor,
                                int borderSize);

void renderSimpleBlocks(Block block,int x , int y, bool isInMenu = true);

void updateBlocks(Block &block, bool isInside);

#endif //PROJECT_NAME_FUNCS_H

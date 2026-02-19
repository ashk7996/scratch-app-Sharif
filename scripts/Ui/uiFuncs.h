#ifndef PROJECT_NAME_FUNCS_H
#define PROJECT_NAME_FUNCS_H

#include "../Common/common.h"

void drawReact(int x , int y , int w , int h, SDL_Color bgColor);

void drawReactWithBorder(int x , int y , int w , int h, SDL_Color bgColor, SDL_Color borderColor, int borderWidth);


void renderFilledButton(const Button &btn);

void updateButton(Button &btn, bool mouseDown);

void renderText(const string& text, int x, int y, SDL_Color color);



#endif //PROJECT_NAME_FUNCS_H
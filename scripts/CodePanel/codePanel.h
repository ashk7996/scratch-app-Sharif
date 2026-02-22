#ifndef PROJECT_NAME_CODEPANEL_H
#define PROJECT_NAME_CODEPANEL_H

#include "../Common/common.h"

vector<pair<string , vector<Block>>>& getMyVector();

void renderScrollableMenu(int x, int y, int width, int height);
void updateBlockType(SDL_Rect rect , Block block);
void renderCodePanel();
void renderScrollablePlayGround();


#endif //PROJECT_NAME_CODEPANEL_H
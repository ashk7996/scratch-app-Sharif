#ifndef PROJECT_NAME_SPIRITPREVIEW_H
#define PROJECT_NAME_SPIRITPREVIEW_H
#include "../Common/common.h"
#include "spiritPreview.h"
#include "../Ui/uiFuncs.h"

void renderGamePreview();

void renderSpiritStats();

void updateShowButton(Button & btn);

void updateDebugButton(Button & btn);

void renderRunAndDebugButtons();

bool loadSpiritTexture(SDL_Texture * spiritTexture);

#endif //PROJECT_NAME_SPIRITPREVIEW_H
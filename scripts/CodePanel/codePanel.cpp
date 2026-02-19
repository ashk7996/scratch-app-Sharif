#include "codePanel.h"
#include "../Ui/uiFuncs.h"

void renderCodePanel() {
    drawReactWithBorder(20 , 190 , 100 , 840 , scratchApp.theme["bgWhite"] , scratchApp.theme["bgSecondary"] , 2);
    drawReactWithBorder(110 , 190 , 300 , 840 , scratchApp.theme["bgWhite"] , scratchApp.theme["bgSecondary"] , 2);
    drawReactWithBorder(400 , 190 , 900 , 840 , scratchApp.theme["bgWhite"] , scratchApp.theme["bgSecondary"] , 2);


}

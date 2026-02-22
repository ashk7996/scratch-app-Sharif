#include "codePanel.h"
#include "../Ui/uiFuncs.h"
#include "../SpiritFunctions/spiritFunctions.h"

vector<pair<string, vector<Block> > > &getMyVector() {
    static vector<pair<string, vector<Block> > > myVector = {
        {"Motion",
            {{"Motion", "changeX", scratchApp.theme["bgMotion"], MOTION_SETX} ,
                {"Motion", "changeY", scratchApp.theme["bgMotion"], MOTION_SETY}
        }},
        {"Looks", {{"Looks", "Say", scratchApp.theme["bgLooks"],}}},
        {"Sound", {{"Sound", "Play", scratchApp.theme["bgSound"]}}},
        {"Event", {{"Event", "Start", scratchApp.theme["bgEvent"], START}}},
        {"Sensing", {{"Sensing", "isTouching", scratchApp.theme["bgSensing"]}}},
        {"Control", {{"Control", "wait", scratchApp.theme["bgControl"]}}},
        {"Variable", {{"Variable", "variable", scratchApp.theme["bgVariable"]}}},
        {"Operators", {{"Operators", "isEqual", scratchApp.theme["bgOperators"]}}},
        {"MyBlocks", {{"MyBlocks", "Mine", scratchApp.theme["bgMyBlocks"]}}},
    };

    return myVector;
}


void renderScrollableMenu(int x, int y, int width, int height) {
    SDL_Rect viewport = {x, y, width, height};

    // Draw the background box
    drawReactWithBorder(x, y, width, height,
                        scratchApp.theme["bgWhite"],
                        scratchApp.theme["bgSecondary"],
                        2);

    // Enable clipping
    SDL_RenderSetClipRect(m_renderer, &viewport);

    // Render content shifted by scrollOffset
    int currentY = y - scratchApp.mouse.scrolloffsetCodeMenu;


    // deciding to render which type of blocks to render
    vector<Block> types;
    for (auto pairs: getMyVector()) {
        if (pairs.first == scratchApp.selectedBlockTypeInCodeMenu) {
            for (auto blocks: pairs.second) {
                types.emplace_back(blocks);
                renderSimpleBlocks(blocks, x, currentY);
                currentY += 100;
            }
        }
    }

    // Disable clipping after drawing
    SDL_RenderSetClipRect(m_renderer, NULL);
}

void renderScrollablePlayGround(int x, int y, int width, int height) {
    SDL_Rect viewport = {x, y, width, height};
    // Draw the background box
    drawReactWithBorder(x, y, width, height,
                        scratchApp.theme["bgWhite"],
                        scratchApp.theme["bgSecondary"],
                        2);

    // Enable clipping
    SDL_RenderSetClipRect(m_renderer, &viewport);

    // Render content shifted by scrollOffset
    int currentY = y - scratchApp.mouse.scrolloffsetBlocksPlayGround;

    for (auto block: scratchApp.executeList) {
        renderSimpleBlocks(block, x + width / 2 - 120, currentY, false);
        currentY += 80;
    }

    // Disable clipping after drawing
    SDL_RenderSetClipRect(m_renderer, NULL);
}


void updateBlockType(SDL_Rect rect, string blockMode) {
    bool isInside = isPointerInElement(rect, scratchApp.mouse.x, scratchApp.mouse.y);
    if (isInside && scratchApp.mouse.isClicked) {
        scratchApp.selectedBlockTypeInCodeMenu = blockMode;
        scratchApp.mouse.scrolloffsetCodeMenu = 0;
    }
}


void renderCodePanel() {
    drawReactWithBorder(20, 190, 100, 840, scratchApp.theme["bgWhite"], scratchApp.theme["bgSecondary"], 2);
    renderScrollableMenu(110, 190, 300, 840);
    renderScrollablePlayGround(400, 190, 900, 840);


    int spacer = 0;
    for (auto actionBlock: getMyVector()) {
        renderBlockType(actionBlock.first, 65, 230 + spacer, 25, actionBlock.second[0].bgColor);
        updateBlockType({65 - 25, 230 + spacer - 25, 100, 50}, actionBlock.second[0].blockMode);
        spacer += 90;
    }
}

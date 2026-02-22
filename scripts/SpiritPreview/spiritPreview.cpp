#include "spiritPreview.h"

void updateShowButton(Button &btn) {
    bool isInside = isPointerInElement(btn.rect, scratchApp.mouse.x, scratchApp.mouse.y);

    if (isInside && scratchApp.mouse.isClicked) {
        scratchApp.mouse.isClicked = false;
        scratchApp.spirit.isShow = !scratchApp.spirit.isShow;
    }

    if (scratchApp.spirit.isShow) {
        btn.currentColor = btn.focusColor;
        btn.currentTextColor = btn.focusTextColor;
    } else {
        btn.currentColor = btn.normalColor;
        btn.currentTextColor = btn.textColor;
    }
}



void renderGamePreview() {
    drawReactWithBorder(1320, 190, 580, 420, scratchApp.theme["bgWhite"], scratchApp.theme["bgSecondary"], 2);

    // showing spirit in the window
    if (scratchApp.spirit.isShow && scratchApp.spirit.texture) {
        SDL_FRect character = {
            1320.0f + (scratchApp.spirit.spiritX / 1920.0f) * 580.0f,
            190.0f + (scratchApp.spirit.spiritY / 1080.0f) * 420.0f,
            48.0f,
            48.0f
        };

        SDL_RenderCopyF(m_renderer,
                        scratchApp.spirit.texture,
                        nullptr,
                        &character);
    }
}

void renderSpiritStats() {
    drawReactWithBorder(1320, 630, 580, 400, scratchApp.theme["bgWhite"], scratchApp.theme["bgSecondary"], 2);

    string nameText = "Name: " + scratchApp.spirit.spiritName, xText =
                    "X: " + to_string(scratchApp.spirit.spiritX), yText = "Y: " + to_string(scratchApp.spirit.spiritY),
            directionText = "Direction: " + to_string(scratchApp.spirit.direction),
            sizeText = "Size: " + to_string(scratchApp.spirit.size);

    renderText(nameText, 1350, 660, scratchApp.theme["textColor"]);
    renderText(xText, 1350, 750, scratchApp.theme["textColor"]);
    renderText(yText, 1580, 750, scratchApp.theme["textColor"]);
    renderText(sizeText, 1350, 830, scratchApp.theme["textColor"]);
    renderText(directionText, 1580, 830, scratchApp.theme["textColor"]);


    char *btnText;

    if (scratchApp.spirit.isShow)
        btnText = "Shown";
    else
        btnText = "Hidden";

    Button btn = {
        {
            1350, 900, 520, 100
        },
        btnText,
        scratchApp.theme["bgSecondary"],
        scratchApp.theme["white"],
        scratchApp.theme["primary"],
        scratchApp.theme["textSecondary"],
        scratchApp.theme["white"]
    };


    updateShowButton(btn);
    renderFilledButton(btn);
};

void updateDebugButton(Button &btn) {
    bool isInside = isPointerInElement(btn.rect, scratchApp.mouse.x, scratchApp.mouse.y);
    if (isInside && scratchApp.mouse.isClicked) {
        scratchApp.mouse.isClicked = false;
        scratchApp.isInDebug = !scratchApp.isInDebug;
    }

    if (scratchApp.isInDebug) {
        btn.currentColor = btn.focusColor;
        btn.currentTextColor = btn.focusTextColor;
    } else {
        btn.currentColor = btn.normalColor;
        btn.currentTextColor = btn.textColor;
    }
}

void updateRunButton(SDL_Rect rect) {
    bool isInside = isPointerInElement(rect, scratchApp.mouse.x, scratchApp.mouse.y);
    if (isInside && scratchApp.mouse.isClicked) {
        if (scratchApp.isExecuting && scratchApp.isInDebug) {
            scratchApp.mouse.isClicked = false;
            SDL_Event event;
            event.type = SDL_KEYDOWN;
            event.key.type = SDL_KEYDOWN;
            event.key.state = SDL_PRESSED;
            event.key.keysym.sym = SDLK_SPACE;
            event.key.repeat = 0;
            SDL_PushEvent(&event);
            SDL_Event eventUp;
            eventUp.type = SDL_KEYUP;
            eventUp.key.type = SDL_KEYUP;
            eventUp.key.state = SDL_RELEASED;
            eventUp.key.keysym.sym = SDLK_SPACE;
            SDL_PushEvent(&eventUp);
        } else {
            scratchApp.mouse.isClicked = false;
            scratchApp.isExecuting = true;
        }
    }
}

void renderRunAndDebugButtons() {
    drawFilledCircleWithBorder(1320 + 25, 150, 25, scratchApp.theme["bgOperators"],
                               changeBrightness(scratchApp.theme["bgOperators"], 90), 2);
    renderText("Go!", 1332, 140, scratchApp.theme["textPrimary"], fontSmall);
    updateRunButton({1320, 125, 50, 50});

    char *btnText;

    if (scratchApp.isInDebug)
        btnText = "Debug";
    else
        btnText = "Rush";

    Button btn = {
        {1920 - 220, 115, 200, 60},
        btnText,
        scratchApp.theme["bgSecondary"],
        scratchApp.theme["white"],
        scratchApp.theme["primary"],
        scratchApp.theme["textSecondary"],
        scratchApp.theme["white"]
    };

    updateDebugButton(btn);
    renderFilledButton(btn);
}

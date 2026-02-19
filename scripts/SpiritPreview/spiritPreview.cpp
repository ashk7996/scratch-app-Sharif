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

    // for now we imagine spirit as a square
    if (scratchApp.spirit.isShow) {
        SDL_FRect charecter = {
            (float) 1320 + (float) (scratchApp.spirit.spiritX) / 1920 * 580,
            (float) 190 + (float) (scratchApp.spirit.spiritY) / 1080 * 420, 10, 10
        };

        drawReact(charecter.x, charecter.y, charecter.w, charecter.h, scratchApp.theme["primary"]);
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

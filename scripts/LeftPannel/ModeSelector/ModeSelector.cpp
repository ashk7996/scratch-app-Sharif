#include <string>
#include "ModeSelector.h"
#include "../../Common/common.h"
#include "../../Ui/uiFuncs.h"

void updateButton(Button &btn) {
    bool isInside = isPointerInElement(btn.rect, scratchApp.mouse.x, scratchApp.mouse.y);
    if (isInside && scratchApp.mouse.isMouseDown) {
        scratchApp.selectedMode = btn.text;
        btn.currentColor = btn.focusColor;
        btn.currentTextColor = btn.focusTextColor;
    } else if (isInside) {
        // cout << "It's indside the button !" << endl;
        btn.currentColor = btn.hoverColor;
        btn.currentTextColor = btn.textColor;
        // cout << btn.currentColor.r << " " << btn.currentColor.g << " " << btn.currentColor.b << endl;
    } else {
        btn.currentColor = btn.normalColor;
        btn.currentTextColor = btn.textColor;
    }
}

void renderModeSelector(vector<Button> &modesList) {
    int spacer = 20;
    for (Button btn: modesList) {
        if (btn.text == scratchApp.selectedMode) {
            btn.currentColor = btn.focusColor;
            btn.currentTextColor = btn.focusTextColor;
        }

        btn.rect.x += spacer;
        renderFilledButton(btn);
        updateButton(btn);
        spacer += btn.rect.w + 10;
    }
}

#include "CodePanel/codePanel.h"
#include "Common/common.h"
#include "CostumesPanel/CostumesPanel.h"
#include "LeftPannel/ModeSelector/ModeSelector.h"
#include "Ui/uiFuncs.h"
#include "Menu/menu.h"
#include "SpiritFunctions/spiritFunctions.h"
#include "SpiritPreview/spiritPreview.h"


using namespace std;


vector<Button> modesList = {
    {
        {0, 120, 150, 50},
        "Code",
        scratchApp.theme["bgSecondary"],
        scratchApp.theme["white"],
        scratchApp.theme["white"],
        scratchApp.theme["textSecondary"],
        scratchApp.theme["primary"]
    },
    {
        {0, 120, 150, 50},
        "Costumes",
        scratchApp.theme["bgSecondary"],
        changeBrightness(scratchApp.theme["bgSecondary"], 0),
        scratchApp.theme["white"],
        scratchApp.theme["textSecondary"],
        scratchApp.theme["primary"]
    },
    {
        {0, 120, 150, 50},
        "Sounds",
        scratchApp.theme["bgSecondary"],
        changeBrightness(scratchApp.theme["bgSecondary"], 0),
        scratchApp.theme["white"],
        scratchApp.theme["textSecondary"],
        scratchApp.theme["primary"]
    }
};


int main(int argc, char *args[]) {
    cout << "HELLO WORLD!" << endl;

    // <<--------------------------------------------- initializing phase --------------------------------------------->>

    if (!prepPhase(24)) {
        cout << "initialization failed" << endl;
        return 1; // initialization failed
    }

    // <<--------------------------------------------- program loop --------------------------------------------->>

    // testing part
    setSpiritX(1500);

    bool isMouseDown = false;
    while (scratchApp.isRunning) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT || event.key.keysym.sym == SDLK_ESCAPE) {
                scratchApp.isRunning = false;
                break;
            }

            // Handles clicking
            if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT)
                isMouseDown = true;

            if (event.type == SDL_MOUSEBUTTONUP && event.button.button == SDL_BUTTON_LEFT)
                isMouseDown = false;

            isClicked(event);

            // Handles scrolling
            if (event.type == SDL_MOUSEWHEEL) {
                updateScrolloffset(840, 950, event);
            }

            updateMouse(isMouseDown);
        }
        // clear the frame
        SDL_SetRenderDrawColor(m_renderer, scratchApp.theme["secondary"].r, scratchApp.theme["secondary"].g,
                               scratchApp.theme["secondary"].b, scratchApp.theme["secondary"].a);
        SDL_RenderClear(m_renderer);

        // <<--------------------------------------------- draw everything per frame --------------------------------------------->>

        // top menu
        renderMenu();

        // mode selectors under the top menu
        renderModeSelector(modesList);

        // deciding to render which panel
        if (scratchApp.selectedMode == "Code") {
            renderCodePanel();
        } else if (scratchApp.selectedMode == "Costumes") {
            renderScrollableMenu(20 , 190 , 1300 , 1030);
        }

        // Spirit preview and stats
        renderGamePreview();
        renderSpiritStats();

        // present the drawings
        SDL_RenderPresent(m_renderer);
    }


    // testing part
    cout << "DW" << scratchApp.DW << "-DH" << scratchApp.DH << endl;

    // <<--------------------------------------------- destroys window and handles autosave (coming soon...) --------------------------------------------->>
    endPhase();

    return 0;
}

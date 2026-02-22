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
    // <<--------------------------------------------- initializing phase --------------------------------------------->>

    if (!prepPhase(24, 14)) {
        cout << "initialization failed" << endl;
        return 1; // initialization failed
    }

    // <<--------------------------------------------- program loop --------------------------------------------->>

    // testing part
    changeSpiritX(1920);

    bool isMouseDown = false;
    int executeIndex = 0;
    while (scratchApp.isRunning) {
        SDL_Event event;
        // normal executing mode ---------------->
        if (!scratchApp.isInDebug && scratchApp.isExecuting) {
            if (executeIndex < scratchApp.executeList.size()) {
                cout << "mode : " << scratchApp.executeList[executeIndex].blockMode << "-- name : " << scratchApp.
                        executeList[executeIndex].blockName << endl;
                executeIndex++;
            } else {
                cout << "<-------- no execution -------->" << endl;
                executeIndex = 0;
                scratchApp.isExecuting = false;
            }
        }

        while (SDL_PollEvent(&event)) {
            // closing app
            if (event.type == SDL_QUIT || event.key.keysym.sym == SDLK_ESCAPE) {
                scratchApp.isRunning = false;
                break;
            }

            // Handles clicking
            isClicked(event);


            if (scratchApp.selectedMode == "Code") {
                if (event.type == SDL_MOUSEWHEEL) {
                    // Handles scrolling for left side menu
                    if (isPointerInElement({110, 190, 300, 840}, scratchApp.mouse.x, scratchApp.mouse.y))
                        updateScrolloffsetCodeMenu(840, 1500, event);
                    // Handle scrolling for playground
                    if (isPointerInElement({400, 190, 900, 840}, scratchApp.mouse.x, scratchApp.mouse.y))
                        updateScrolloffsetBlockPlayGround(900, 1500, event);
                }

                // removes last added block to the execute list
                if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_BACKSPACE && event.key.repeat == 0 &&
                    scratchApp.executeList.size() > 0) {
                    removeLastBlockFromExecuteList();
                }
            }

            // debug executing mode ---------------->
            if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_SPACE && event.key.repeat == 0) {
                if (scratchApp.isExecuting && scratchApp.isInDebug) {
                    if (executeIndex < scratchApp.executeList.size()) {
                        cout << "mode : " << scratchApp.executeList[executeIndex].blockMode << "-- name : " <<
                                scratchApp.executeList[executeIndex].blockName << endl;
                        executeIndex++;
                    } else {
                        executeIndex = 0;
                        scratchApp.isExecuting = false;
                        cout << "<-------- no execution -------->" << endl;
                    }
                }
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
            // renderScrollableMenu(20 , 190 , 1300 , 1030);
        }

        // Spirit preview and stats
        renderRunAndDebugButtons();
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

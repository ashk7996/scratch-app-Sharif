#include "CostumesPanel.h"
#include "../Ui/uiFuncs.h"

// void renderScrollableMenu(int x, int y, int width, int height) {
//     SDL_Rect viewport = {x, y, width, height};
//
//     // Draw the background box
//     drawReactWithBorder(x, y, width, height,
//                         scratchApp.theme["bgWhite"],
//                         scratchApp.theme["bgSecondary"],
//                         2);
//
//     // Enable clipping
//     SDL_RenderSetClipRect(m_renderer, &viewport);
//
//     // Render content shifted by scrollOffset
//     int currentY = y - scratchApp.mouse.scrolloffset;
//
//     string items[10] = {
//         "item1","item2","item3","item4","item5","item6","item7","item8","item9","item10"
//     };
//
//     for (int i = 0; i < 10; i++) {
//         renderText(items[i], x + 10, currentY, scratchApp.theme["primary"]);
//         currentY += 40; // item spacing
//     }
//
//     // Disable clipping after drawing
//     SDL_RenderSetClipRect(m_renderer, NULL);
// }

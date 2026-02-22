#include "common.h"

SDL_Window *m_window = nullptr;
SDL_Renderer *m_renderer = nullptr;
TTF_Font *font = nullptr, *fontSmall = nullptr;

map<string, SDL_Color> defualtTheme = {
    {"primary", {137, 207, 240, 255}},
    {"secondary", {230, 240, 255, 255}},
    {"bgSecondary", {217, 227, 242, 255}},
    {"white", {255, 255, 255, 255}},
    {"bgWhite", {249, 249, 249, 255}},
    {"textPrimary", {255, 255, 255, 255}},
    {"textSecondary", {110, 110, 110, 255}},
    {"bgMotion", {76, 151, 255, 255}},
    {"bgLooks", {153, 102, 255, 255}},
    {"bgSound", {207, 99, 207, 255}},
    {"bgEvent", {255, 191, 0, 255}},
    {"bgSensing", {92, 177, 214, 255}},
    {"bgControl", {255, 171, 25, 255}},
    {"bgOperators", {89, 192, 89, 255}},
    {"bgVariable", {255, 140, 26, 255}},
    {"bgMyBlocks", {255, 102, 128, 255}}
};

App scratchApp;

bool loadSpiritTexture() {
    SDL_Surface *surface = IMG_Load("../assets/north.png");
    if (!surface) {
        cout << "IMG_Load Error: " << IMG_GetError() << endl;
        return false;
    }

    scratchApp.spirit.texture = SDL_CreateTextureFromSurface(m_renderer, surface);
    SDL_FreeSurface(surface);

    if (!scratchApp.spirit.texture) {
        cout << "CreateTexture Error: " << SDL_GetError() << endl;
        return false;
    }

    return true;
}

bool prepPhase(int fontSize, int fontSizeSmall) {
    Uint32 SDL_flags = SDL_INIT_VIDEO | SDL_INIT_TIMER;
    Uint32 WND_flags = SDL_WINDOW_SHOWN | /* SDL_WINDOW_FULLSCREEN_DESKTOP */ SDL_WINDOW_MAXIMIZED;

    if (SDL_Init(SDL_flags) < 0)return false;
    if (SDL_CreateWindowAndRenderer(1920, 480, WND_flags, &m_window, &m_renderer) < 0)return false;

    if (TTF_Init() < 0)return false;
    font = TTF_OpenFont("../assets/NotoSans-Medium.ttf", fontSize);
    fontSmall = TTF_OpenFont("../assets/NotoSans-Medium.ttf", fontSizeSmall);
    if (!font || !fontSmall) return false;

    IMG_Init(IMG_INIT_PNG);

    SDL_RaiseWindow(m_window);

    SDL_DisplayMode DM;
    SDL_GetCurrentDisplayMode(0, &DM);
    scratchApp.DW = DM.w, scratchApp.DH = DM.h;

    SDL_SetRenderDrawColor(m_renderer, scratchApp.theme["secondary"].r, scratchApp.theme["secondary"].g,
                           scratchApp.theme["secondary"].b, scratchApp.theme["secondary"].a);
    SDL_RenderClear(m_renderer);
    SDL_RenderPresent(m_renderer);
    SDL_SetRenderDrawBlendMode(m_renderer, SDL_BLENDMODE_BLEND);

    loadSpiritTexture();

    return true;
}

void endPhase() {
    if (m_renderer) SDL_DestroyRenderer(m_renderer);
    if (m_window) SDL_DestroyWindow(m_window);

    if (scratchApp.spirit.texture)
        SDL_DestroyTexture(scratchApp.spirit.texture);


    IMG_Quit();
    SDL_Quit();
}

SDL_Color changeBrightness(SDL_Color color, int amount) {
    color.r = color.r * amount / 100;
    color.g = color.g * amount / 100;
    color.b = color.b * amount / 100;
    return color;
}

bool isPointerInElement(SDL_Rect rect, int mx, int my) {
    if (mx > rect.x && mx < rect.x + rect.w && my > rect.y && my < rect.y + rect.h)
        return true;

    return false;
}

void updateMouse(bool isMouseDown) {
    SDL_GetMouseState(&scratchApp.mouse.x, &scratchApp.mouse.y);
    scratchApp.mouse.isMouseDown = isMouseDown;
}

void isClicked(const SDL_Event &event) {
    scratchApp.mouse.isClicked = (event.type == SDL_MOUSEBUTTONDOWN &&
                                  event.button.button == SDL_BUTTON_LEFT);
}

void updateScrolloffsetCodeMenu(int viewportHeight, int contentHeight, SDL_Event &event) {
    scratchApp.mouse.scrolloffsetCodeMenu -= event.wheel.y * 20; // scroll speed

    if (scratchApp.mouse.scrolloffsetCodeMenu < 0)
        scratchApp.mouse.scrolloffsetCodeMenu = 0;

    if (scratchApp.mouse.scrolloffsetCodeMenu > contentHeight - viewportHeight)
        scratchApp.mouse.scrolloffsetCodeMenu = contentHeight - viewportHeight;
}

void updateScrolloffsetBlockPlayGround(int viewportHeight, int contentHeight, SDL_Event &event) {
    scratchApp.mouse.scrolloffsetBlocksPlayGround -= event.wheel.y * 20; // scroll speed

    if (scratchApp.mouse.scrolloffsetBlocksPlayGround < 0)
        scratchApp.mouse.scrolloffsetBlocksPlayGround = 0;

    if (scratchApp.mouse.scrolloffsetBlocksPlayGround > contentHeight - viewportHeight)
        scratchApp.mouse.scrolloffsetBlocksPlayGround = contentHeight - viewportHeight;
}

void removeLastBlockFromExecuteList() {
    scratchApp.executeList.pop_back();
}

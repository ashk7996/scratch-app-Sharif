#include "uiFuncs.h"
#include "../Common/common.h"

void drawReact(int x, int y, int w, int h, SDL_Color bgColor) {
    SDL_Rect rect = {x, y, w, h};
    SDL_SetRenderDrawColor(m_renderer, bgColor.r, bgColor.g, bgColor.b, bgColor.a);
    SDL_RenderFillRect(m_renderer, &rect);
}

void drawReactWithBorder(int x, int y, int w, int h, SDL_Color bgColor, SDL_Color borderColor, int borderWidth) {
    SDL_Rect biggerRect = {x, y, w, h}, smallerReact = {
        x + borderWidth, y + borderWidth, w - 2 * borderWidth, h - 2 * borderWidth
    };
    // bigger rect for border
    SDL_SetRenderDrawColor(m_renderer, borderColor.r, borderColor.g, borderColor.b, borderColor.a);
    SDL_RenderFillRect(m_renderer, &biggerRect);

    // smaller react for background
    SDL_SetRenderDrawColor(m_renderer, bgColor.r, bgColor.g, bgColor.b, bgColor.a);
    SDL_RenderFillRect(m_renderer, &smallerReact);
}

void renderFilledButton(const Button &btn) {
    SDL_SetRenderDrawColor(m_renderer, btn.currentColor.r, btn.currentColor.g, btn.currentColor.b, btn.currentColor.a);
    SDL_RenderFillRect(m_renderer, &btn.rect);

    SDL_Surface *surf = TTF_RenderUTF8_Blended(font, btn.text, btn.currentTextColor);
    SDL_Texture *tex = SDL_CreateTextureFromSurface(m_renderer, surf);

    SDL_Rect textRect;
    textRect.w = surf->w;
    textRect.h = surf->h;
    textRect.x = btn.rect.x + (btn.rect.w - textRect.w) / 2;
    textRect.y = btn.rect.y + (btn.rect.h - textRect.h) / 2;

    SDL_FreeSurface(surf);
    SDL_RenderCopy(m_renderer, tex, NULL, &textRect);
    SDL_DestroyTexture(tex);
}

void renderText(const string &text, int x, int y, SDL_Color color, TTF_Font *f) {
    SDL_Surface *surface = TTF_RenderText_Blended(f, text.c_str(), color);
    SDL_Texture *texture = SDL_CreateTextureFromSurface(m_renderer, surface);

    SDL_Rect dst = {x, y, surface->w, surface->h};

    SDL_RenderCopy(m_renderer, texture, NULL, &dst);

    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}

void drawCircle(int centerX, int centerY, int radius, SDL_Color color) {
    SDL_SetRenderDrawColor(m_renderer, color.r, color.g, color.b, color.a);

    for (int y = -radius; y <= radius; y++) {
        int xLimit = (int) sqrt(radius * radius - y * y);

        SDL_RenderDrawLine(m_renderer,
                           centerX - xLimit,
                           centerY + y,
                           centerX + xLimit,
                           centerY + y);
    }
}

void drawFilledCircleWithBorder(int centerX, int centerY, int radius, SDL_Color fillColor, SDL_Color borderColor,
                                int borderSize) {
    if (borderSize < 0) borderSize = 0;
    if (borderSize > radius) borderSize = radius;

    // ---- Draw outer border circle ----
    SDL_SetRenderDrawColor(m_renderer,
                           borderColor.r,
                           borderColor.g,
                           borderColor.b,
                           borderColor.a);

    for (int y = -radius; y <= radius; y++) {
        int xLimit = (int) sqrt(radius * radius - y * y);

        SDL_RenderDrawLine(m_renderer,
                           centerX - xLimit,
                           centerY + y,
                           centerX + xLimit,
                           centerY + y);
    }

    // ---- Draw inner fill circle ----
    int innerRadius = radius - borderSize;

    SDL_SetRenderDrawColor(m_renderer,
                           fillColor.r,
                           fillColor.g,
                           fillColor.b,
                           fillColor.a);

    for (int y = -innerRadius; y <= innerRadius; y++) {
        int xLimit = (int) sqrt(innerRadius * innerRadius - y * y);

        SDL_RenderDrawLine(m_renderer,
                           centerX - xLimit,
                           centerY + y,
                           centerX + xLimit,
                           centerY + y);
    }
}

void renderBlockType(string blockType, int x, int y, int r, SDL_Color color) {
    drawFilledCircleWithBorder(x, y, r, color, changeBrightness(color, 90), 3);
    renderText(blockType, x - r - 2 * (blockType.size() - 6), y + r + 10, color, fontSmall);
}

void updateBlocks(Block &block, bool isInside) {
    if (isInside && scratchApp.mouse.isClicked) {
        scratchApp.executeList.emplace_back(block);
        scratchApp.mouse.isClicked = false;
    }
}

void renderSimpleBlocks(Block block, int x, int y, bool isInMenu) {
    int textW, textH;
    TTF_SizeUTF8(font, block.blockName.c_str(), &textW, &textH);
    drawReactWithBorder(x + 25, y + 30, 240, 80, block.bgColor, changeBrightness(block.bgColor, 90), 2);
    renderText(block.blockName, x + 25 + (240 - textW) / 2, y + 30 + (80 - textH) / 2, scratchApp.theme["textColor"]);
    if (isInMenu) {
        bool isInside = isPointerInElement({x + 25, y + 30, 240, 80}, scratchApp.mouse.x, scratchApp.mouse.y);
        updateBlocks(block, isInside);
    }
}


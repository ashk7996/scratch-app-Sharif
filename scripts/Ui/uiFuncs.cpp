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

void renderText(const string& text, int x, int y, SDL_Color color) {
    SDL_Surface* surface = TTF_RenderText_Blended(font, text.c_str(), color);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(m_renderer, surface);

    SDL_Rect dst = { x, y, surface->w, surface->h };

    SDL_RenderCopy(m_renderer, texture, NULL, &dst);

    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}

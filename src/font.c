#include "font.h"
#include "la.h"
#include "utils.h"
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_surface.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUF_SIZE 1024

font_t *font_init(SDL_Renderer *renderer, const char *filename) {

    char *_filename = malloc(BUF_SIZE * sizeof(char));
    assert(_filename && "Can't alloc filename");
    memset(_filename, 0, BUF_SIZE * sizeof(char));

    strcat(_filename, filename);
    strcat(_filename, ".fnt");

    font_t *font = malloc(sizeof(font_t));
    assert(font && "Can't alloc font");
    memset(font, 0, sizeof(font_t));

    FILE *fhandle = fopen(_filename, "r");
    assert(fhandle && "Can't not load file");

    char *line = malloc(BUF_SIZE);
    assert(line && "Can't alloc line");
    memset(line, 0, BUF_SIZE);

    int ch;
    int i = 0;
    int skiplines = 1;
    int count = 0;
    int lineheight = 0;
    int page = 0;

    while ((ch = fgetc(fhandle)) != EOF) {
        if (i + 1 == BUF_SIZE) {
            fprintf(stderr, "Buffer overfflow\n");
            i = 0;
            continue;
        }

        if ((ch == '\n') || (ch == '\r')) {
            if (skiplines > 0) {
                skiplines--;
            } else {
                int rc = 0;
                if (lineheight < 1) {
                    rc = sscanf(line, "common lineHeight=%i", &lineheight);
                    if (rc < 1) {
                        fprintf(stderr, "Font parse error %s\n", line);
                    }
                    font->lineheight = lineheight;
                } else if (page < 1) {
                    page = 1; // skip line 2
                } else if (count < 1) {
                    rc = sscanf(line, "chars count=%i", &count);
                    if (rc < 1) {
                        fprintf(stderr, "Font parse error %s\n", line);
                    }
                    font->count = count;
                } else {
                    int id, x, y, w, h, xo, yo, xa, p, chl = 0;
                    rc = sscanf(line,
                                "char id=%i   x=%i    y=%i     width=%i     height=%i    xoffset=%i     yoffset=%i     "
                                "xadvance=%i     page=%i  chnl=%i",
                                &id, &x, &y, &w, &h, &xo, &yo, &xa, &p, &chl);
                    if (rc < 10) {
                        fprintf(stderr, "font parse error %s\n", line);
                    }

                    ch_t ch = {id, x, y, w, h, xo, yo, xa};
                    font->chars[id] = ch;
                }
            }

            // Reset line after read.
            memset(line, 0, BUF_SIZE);
            i = 0;
        } else {
            line[i] = ch;
            i++;
        }
    }

    free(line);
    fclose(fhandle);

    memset(_filename, 0, BUF_SIZE);
    strcat(_filename, filename);
    strcat(_filename, ".png");

    SDL_Surface *tmp = icp(IMG_Load(_filename));
    font->tex = SDL_CreateTextureFromSurface(renderer, tmp);
    SDL_FreeSurface(tmp);

    free(_filename);

    return font;
}
void font_render(SDL_Renderer *renderer, font_t *font, Vec2i pos, const char *str) {

    int cx = pos.x;
    int cy = pos.y;
    int ch;

    SDL_Rect src = {0};
    SDL_Rect dest = {0};

    int len = (int)strlen(str);
    for (int i = 0; i < len; i++) {
        ch = str[i];

        if (ch == ' ') {
            cx += font->chars['j'].xadv;
            continue;
        } else if (ch == '\t') {
            cx += font->chars['j'].xadv * 4;
        } else if (ch == '\n') {
            cx = pos.x;
            cy += font->chars['j'].h + 2;
            continue;
        }

        src.x = font->chars[ch].x;
        src.y = font->chars[ch].y;
        src.w = font->chars[ch].w;
        src.h = font->chars[ch].h;

        dest.x = cx + font->chars[ch].xoff;
        dest.y = cy + font->chars[ch].yoff;
        dest.w = font->chars[ch].w;
        dest.h = font->chars[ch].h;
        cx += font->chars[ch].xadv;

        scc(SDL_RenderCopy(renderer, font->tex, &src, &dest));
    }
}

void render_text_center(SDL_Renderer *renderer, font_t *font, Vec2i pos, const char *str) {
    int ch;
    SDL_Rect src = {0};
    SDL_Rect dest = {0};

    int len = (int)strlen(str);
    int width = 0;
    int height = font->lineheight; // 1 line.
    for (int i = 0; i < len; i++) {
        ch = str[i];

        if (ch == ' ') {
            src.w += font->chars['j'].xadv;
            width += src.w;
            continue;
        } else if (ch == '\t') {
            src.w += font->chars['j'].xadv * 4;
            width += src.w;
            continue;
        } else if (ch == '\n') {
            assert(0 && "Unimplemented");
        }

        src.w = font->chars[ch].w;
        src.h = font->chars[ch].h + font->chars[ch].yoff;

        width += src.w;
    }

    pos.x = pos.x - width / 2;
    pos.y = pos.y - height / 2;

    font_render(renderer, font, pos, str);
}
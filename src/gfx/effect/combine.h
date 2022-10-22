#ifndef COMBINE_H
#define COMBINE_H

#include "effect.h"

class Combine : public Effect {
    public:
        Combine(int width, int height);
        Framebuffer& render(Framebuffer& a, Framebuffer& b);
};

#endif
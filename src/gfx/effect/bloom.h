#ifndef BLOOM_H
#define BLOOM_H

#include "simple_blur.h"
#include "effect.h"
#include "combine.h"

#include <vector>
#include <algorithm>

class Bloom : public Effect {

    private:    
        std::vector<std::pair<SimpleBlur*, Combine*>> stages;
        Shader transparentShader;

    public:
        /**
         * @brief Construct a new Bloom object
         * 
         * @param width 
         * @param height 
         * @param mipCount how many mip levels the bloom uses
         * @param alpha
         * @param threshold
         */
        Bloom(int width, int height, int mipCount, float alpha=0.4, float threshold=0.6);
        virtual ~Bloom();
        virtual Framebuffer& render(unsigned int textureId) override;
        void renderFramebuffer();
};

#endif
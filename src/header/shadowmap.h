#ifndef SHADOWMAP_H
#define SHADOWMAP_H

#include "renderer_fbo.h"
#include "shader.h"
#include "framebuffer.h"

class ShadowMap : RendererFBO {
	private:
		static constexpr int N_SHADOWMAPS = 2;
		Framebuffer shadowMaps[N_SHADOWMAPS];
		Shader combine;
		int index;
	public:
		ShadowMap(int width, int height);
		virtual ~ShadowMap() {};

		virtual void render(const std::vector<std::pair<Chunk*, float>>& chunks, Skybox* skybox=NULL);
		int getShadowMap() const;
};

#endif
#include "header/shadowmap.h"
#include <memory>

ShadowMap::ShadowMap(int width, int height) : RendererFBO(width, height, false, true,
	std::make_shared<Shader>("depth.vs", "depth.fs"),
	{},
	std::make_shared<Shader>("depthLeaf.vs", "depth.fs")),
	shadowMaps({Framebuffer(width, height, false, true), Framebuffer(width, height, false, true)}),
	combine("quad.vs" , "interpolate.fs"),
	index(0) 
{
	combine.use();
	combine.setInt("current", 0);
	combine.setInt("prev", 1);
}

void ShadowMap::render(const std::vector<std::pair<Chunk*, float>>& chunks, Skybox* skybox) {
	RendererFBO::render(chunks, skybox);
	glActiveTexture(GL_TEXTURE0);
	
	glActiveTexture(GL_TEXTURE1);
}
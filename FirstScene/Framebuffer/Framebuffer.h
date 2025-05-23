#pragma once
#include <vector>
#include <memory>
#include "Texture/Texture.h"
#include "Model/Model.h"
#include "Shader/Shader.h"

enum class DepthStencilType
{
	NONE,
	DEPTH,
	STENCIL,
	DEPTH_STENCIL,
};

class Framebuffer
{
public:
	Framebuffer(unsigned int width, unsigned int height, bool msaa = false);
	~Framebuffer();
	Framebuffer(const Framebuffer& other) = delete;
	Framebuffer& operator=(const Framebuffer& other) = delete;
	Framebuffer(Framebuffer&& other) noexcept;
	Framebuffer& operator=(Framebuffer&& other) noexcept;

	std::shared_ptr<RenderTexture2D> attachColorTexture(unsigned int internalFormat, unsigned int format, unsigned dataType);
	void attachColorTexture(const std::shared_ptr<RenderTexture2D>& texture);

	std::shared_ptr<RenderTexture2D> attachDepthTexture(DepthStencilType type);
	void attachDepthTexture(const std::shared_ptr<RenderTexture2D>& texture);

	void configureColorAttachments(const std::vector<unsigned int>& indices) const;
	void configureColorAttachments(std::vector<unsigned int>&& indices) const;

	void blitColorTexture(unsigned int selfIndex, const Framebuffer& other, unsigned int otherIndex) const;
	void blitDepthTexture(const Framebuffer& other) const;

	void bindTexture(int texIndex, unsigned int targetIndex) const;	// (texIndex = -1) refers to depth texture
	void bind() const;

	void draw(const Model& model, const Shader& shader, glm::mat4 transform) const;
	void drawInstanced(const Model& model, const Shader& shader, glm::mat4 transform, const std::vector<glm::vec3>& translations) const;
	void drawInstanced(const Model& model, const Shader& shader, glm::mat4 transform, std::vector<glm::vec3>&& translations) const;
private:
	unsigned int FBO;
	unsigned int width, height;
	bool msaa;
	std::vector<std::shared_ptr<RenderTexture2D>> colorAttachments;
	unsigned int colorAttachmentCount = 0;
	std::shared_ptr<RenderTexture2D> depthStencilAttachment;
};

#include "vulkan_renderpass_geometry.hpp"
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_float4x4.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "glm/ext/vector_float3.hpp"
#include "glm/trigonometric.hpp"
#include "real/core/core.hpp"
#include "real/core/game.hpp"
#include "real/core/logging.hpp"
#include "real/core/types.hpp"
#include "real/graphics/render_pass.hpp"
#include "real/graphics/render_pass_geometry.hpp"
#include "real/graphics/renderer.hpp"
#include "real/resource/resource_shader.hpp"
#include "vulkan_renderer.hpp"
#include "vulkan_resource_image.hpp"
#include "vulkan_resource_mesh.hpp"
#include "vulkan_resource_shader.hpp"
#include <GLFW/glfw3.h>
#include <cstdlib>
#include <cstring>
#include <optional>
#include <vector>
#include <vulkan/vulkan_core.h>

namespace real {

struct GPUDrawPushConstants {
    glm::mat4 worldMatrix;
    VkDeviceAddress vertexBuffer;
};

VulkanRenderPassGeometry::VulkanRenderPassGeometry(
		Game *_game, RenderPassGeometryInfo info,
		std::vector<ResourceHandle<ResourceShader>> shaders,
		std::vector<RenderPassResource> resources)
		: RenderPassGeometry(_game, shaders[0].get()->get_layout(), resources) {

	VulkanRenderer *renderer = (VulkanRenderer*)game->renderer.get();
	RL_LOG_TRACE("creating renderpass geometry");

	renderImage = info.renderImage;
	// depthImage = info.depthImage;

	if(info.depthImage.has_value()) {
		depthImage = std::make_optional<ResourceHandle<VulkanResourceImage>>(info.depthImage.value());
	} else {
		depthImage = std::nullopt;
	}

	VkPushConstantRange pushConstant{};
	pushConstant.offset = 0;
	pushConstant.size = 128;
	pushConstant.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

	VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {};
	pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutCreateInfo.pNext = nullptr;
	pipelineLayoutCreateInfo.setLayoutCount = 0;
	pipelineLayoutCreateInfo.pushConstantRangeCount = 1;
	pipelineLayoutCreateInfo.pPushConstantRanges = &pushConstant;
	VK_CHECK(vkCreatePipelineLayout(renderer->device, &pipelineLayoutCreateInfo, nullptr, &layout));

	VkPipelineInputAssemblyStateCreateInfo inputAssembly =  { .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO };
    inputAssembly.primitiveRestartEnable = VK_FALSE;
	inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	
	VkPipelineRasterizationStateCreateInfo rasterizer = { .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO };;
    rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizer.lineWidth = 1.f;
	rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
	
	VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
	colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachment.blendEnable = VK_FALSE;

    VkPipelineMultisampleStateCreateInfo multisampling = { .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO };
    multisampling.sampleShadingEnable = VK_FALSE;
    multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    multisampling.minSampleShading = 1.0f;
    multisampling.pSampleMask = nullptr;
    multisampling.alphaToCoverageEnable = VK_FALSE;
    multisampling.alphaToOneEnable = VK_FALSE;
	
    VkPipelineDepthStencilStateCreateInfo depthStencil = { .sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO };
    depthStencil.depthTestEnable = VK_FALSE;
    depthStencil.depthWriteEnable = VK_FALSE;
    depthStencil.depthCompareOp = VK_COMPARE_OP_NEVER;
    depthStencil.depthBoundsTestEnable = VK_FALSE;
    depthStencil.stencilTestEnable = VK_FALSE;
    depthStencil.front = {};
    depthStencil.back = {};
    depthStencil.minDepthBounds = 0.f;
    depthStencil.maxDepthBounds = 1.f;

	if(info.depthImage.has_value()) enable_depth(depthStencil);

	RL_LOG_TRACE("creating render info color formats");
	VkPipelineRenderingCreateInfo renderInfo = { .sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO };
	std::vector<VkFormat> colorFormats;
	/*
	size_t i = 0;
	for (auto &img : resources) {
		if(img.format == ImageFormat::DEPTH)
			continue;

		if(img.format == ImageFormat::COLOR) {
			color_image_index = i;
		}
		i++;
		VulkanResourceImage *vimg = (VulkanResourceImage*)img.texture.get();
		colorFormats.push_back(vimg->imageFormat); 
	}
	*/
	colorFormats.push_back(renderImage.get()->imageFormat);

	renderInfo.colorAttachmentCount = colorFormats.size();
	renderInfo.pColorAttachmentFormats = colorFormats.data();
    renderInfo.depthAttachmentFormat = VK_FORMAT_D32_SFLOAT;

    VkPipelineViewportStateCreateInfo viewportState = {};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.pNext = nullptr;
    viewportState.viewportCount = 1;
    viewportState.scissorCount = 1;

    VkPipelineColorBlendStateCreateInfo colorBlending = {};
    colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending.pNext = nullptr;
    colorBlending.logicOpEnable = VK_FALSE;
    colorBlending.logicOp = VK_LOGIC_OP_COPY;
    colorBlending.attachmentCount = 1;
    colorBlending.pAttachments = &colorBlendAttachment;

    VkPipelineVertexInputStateCreateInfo vertexInputInfo = { .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO };

	RL_LOG_TRACE("doing shader stages now");
	std::vector<VkPipelineShaderStageCreateInfo> shaderStages;
	for (auto &s : shaders) {
		for(int i = 1; i < 8; i++) {
			VkPipelineShaderStageCreateInfo stage = {};
			stage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
			stage.pNext = nullptr;
			stage.module = dynamic_cast<VulkanResourceShader*>(s.get())->module;
			if(CHECK_FLAG(s.get()->get_type(), BIT(i)) && BIT(i) == ShaderType_VERTEX) {
				RL_LOG_TRACE("vertex");
				stage.pName = "vertex_main";
				stage.stage = VK_SHADER_STAGE_VERTEX_BIT;
				shaderStages.push_back(stage);
			} else if(CHECK_FLAG(s.get()->get_type(), BIT(i)) && BIT(i) == ShaderType_FRAGMENT) {
				RL_LOG_TRACE("fragment");
				stage.pName = "fragment_main";
				stage.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
				shaderStages.push_back(stage);
			} 
		}
	}

    VkGraphicsPipelineCreateInfo pipelineInfo = { .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO };
    pipelineInfo.pNext = &renderInfo;
    pipelineInfo.stageCount = (uint32_t)shaderStages.size();
    pipelineInfo.pStages = shaderStages.data();
    pipelineInfo.pVertexInputState = &vertexInputInfo;
    pipelineInfo.pInputAssemblyState = &inputAssembly;
    pipelineInfo.pViewportState = &viewportState;
    pipelineInfo.pRasterizationState = &rasterizer;
    pipelineInfo.pMultisampleState = &multisampling;
    pipelineInfo.pColorBlendState = &colorBlending;
    pipelineInfo.pDepthStencilState = &depthStencil;
    pipelineInfo.layout = layout;

	VkDynamicState state[] = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
    VkPipelineDynamicStateCreateInfo dynamicInfo = { .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO };
    dynamicInfo.pDynamicStates = &state[0];
    dynamicInfo.dynamicStateCount = 2;
    pipelineInfo.pDynamicState = &dynamicInfo;

	RL_LOG_TRACE("creating the pipeline");
	if (vkCreateGraphicsPipelines(renderer->device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &pipeline) != VK_SUCCESS) {
        fmt::println("failed to create pipeline");
    }

	push_constant_buffer = (char*)malloc(128);
	addr_loc = shader_layout.get_field("_vertex_buffer");
}

void VulkanRenderPassGeometry::enable_depth(
		VkPipelineDepthStencilStateCreateInfo &depth) {
	
	depth.depthTestEnable = VK_TRUE;
    depth.depthWriteEnable = true;
    depth.depthCompareOp = VK_COMPARE_OP_GREATER_OR_EQUAL;
    depth.depthBoundsTestEnable = VK_FALSE;
    depth.stencilTestEnable = VK_FALSE;
    depth.front = {};
    depth.back = {};
    depth.minDepthBounds = 0.f;
    depth.maxDepthBounds = 1.f;
}

VulkanRenderPassGeometry::~VulkanRenderPassGeometry() {
	VulkanRenderer *renderer = (VulkanRenderer*)game->renderer.get();
	free(push_constant_buffer);
    vkDeviceWaitIdle(renderer->device);
	vkDestroyPipelineLayout(renderer->device, layout, nullptr);
    vkDestroyPipeline(renderer->device, pipeline, nullptr);
}

void VulkanRenderPassGeometry::begin_pass(FrameContext context) {
	FrameDataVulkan *frame = (FrameDataVulkan*)context;
	VulkanResourceImage *vimg = renderImage.get();
	VulkanResourceImage *dimg = depthImage.value().get();

	VkRenderingAttachmentInfo depthAttachment = vkutil::depth_attachment_info(dimg->imageView, VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL);
	VkRenderingAttachmentInfo colorAttachment = vkutil::attachment_info(vimg->imageView, nullptr, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
	VkRenderingInfo renderInfo = vkutil::rendering_info(frame->draw_extent, &colorAttachment, &depthAttachment);
	
	vkutil::transition_image(frame->main_command_buffer, vimg->image, VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
	vkutil::transition_image(frame->main_command_buffer, dimg->image, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL);

	vkCmdBeginRendering(frame->main_command_buffer, &renderInfo);
	vkCmdBindPipeline(frame->main_command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);

	//set dynamic viewport and scissor
	VkViewport viewport = {};
	viewport.x = 0;
	viewport.y = 0;
	viewport.width = vimg->imageExtent.width;
	viewport.height = vimg->imageExtent.height;
	viewport.minDepth = 0.f;
	viewport.maxDepth = 1.f;

	vkCmdSetViewport(frame->main_command_buffer, 0, 1, &viewport);

	VkRect2D scissor = {};
	scissor.offset.x = 0;
	scissor.offset.y = 0;
	scissor.extent.width = vimg->imageExtent.width;
	scissor.extent.height = vimg->imageExtent.height;

	vkCmdSetScissor(frame->main_command_buffer, 0, 1, &scissor);
}

void VulkanRenderPassGeometry::draw_mesh(
		FrameContext context, 
		ResourceHandle<ResourceMesh> mesh) {
	
	FrameDataVulkan *frame = (FrameDataVulkan*)context;
	VulkanResourceMesh *da_mesh = (VulkanResourceMesh*)mesh.get();

	GPUDrawPushConstants push_constants;
	glm::mat4 a{1.f};
	glm::mat4 view = glm::mat4(1.0f);
	view = glm::translate(view, glm::vec3(0.0f, 0.0f, -7.0f));
	view = glm::rotate(view, glm::radians((float)glfwGetTime() * 20.0f + 180), glm::vec3(1.0f, 0.0f, 0.0f));

	glm::mat4 projection;
	float aspect = (float)renderImage.get()->imageExtent.width / renderImage.get()->imageExtent.height;
	projection = glm::perspective(glm::radians(70.0f), aspect, 0.1f, 100.0f);
	projection[1][1] *= -1;
	push_constants.worldMatrix = projection * view;
	push_constants.vertexBuffer = da_mesh->address;


	vkCmdPushConstants(frame->main_command_buffer, layout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(GPUDrawPushConstants), &push_constants);

	// set_variable(addr_loc, (char*)da_mesh->address, sizeof(VkDeviceAddress));
	// vkCmdPushConstants(frame->main_command_buffer, layout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(GPUDrawPushConstants), push_constant_buffer);
	vkCmdBindIndexBuffer(frame->main_command_buffer, da_mesh->indexBuffer.buffer, 0, VK_INDEX_TYPE_UINT32);
	vkCmdDrawIndexed(frame->main_command_buffer, da_mesh->indices_count, 1, 0, 0, 0);
}

void VulkanRenderPassGeometry::end_pass(FrameContext context) {
	FrameDataVulkan *frame = (FrameDataVulkan*)context;

	// GPUDrawPushConstants push_constants;
	// push_constants.worldMatrix = glm::mat4{ 1.f };
	// push_constants.vertexBuffer = mesh->address;

	// vkCmdPushConstants(frame->main_command_buffer, layout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(GPUDrawPushConstants), &push_constants);
	// vkCmdBindIndexBuffer(frame->main_command_buffer, mesh->indexBuffer.buffer, 0, VK_INDEX_TYPE_UINT32);
	// vkCmdDrawIndexed(frame->main_command_buffer, 6, 1, 0, 0, 0);
	
	vkCmdEndRendering(frame->main_command_buffer);
}

void VulkanRenderPassGeometry::set_variable(
		ShaderField field, char *data, size_t size) {

	switch (field.type) {
	case(ShaderFieldType::PUSH_CONSTANT): {
		char *write_pointer = (char*)(push_constant_buffer+field.offset);
		memcpy(write_pointer, data, size);
		return; 
	}

	default:
		break;
	}
}

}
 

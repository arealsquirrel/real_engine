
#include "vulkan_renderpass_geometry.hpp"
#include "glm/ext/matrix_float4x4.hpp"
#include "glm/ext/vector_float3.hpp"
#include "glm/ext/vector_float4.hpp"
#include "real/core/core.hpp"
#include "real/core/logging.hpp"
#include "real/core/types.hpp"
#include "real/graphics/render_pass.hpp"
#include "real/graphics/render_pass_geometry.hpp"
#include "real/resource/resource_shader.hpp"
#include "vulkan_buffer.hpp"
#include "vulkan_renderer.hpp"
#include "vulkan_resource_image.hpp"
#include "vulkan_resource_shader.hpp"
#include <vector>
#include <vulkan/vulkan_core.h>

namespace real {

struct Vertex {
	glm::vec3 position;
	float uv_x;
	glm::vec3 normal;
	float uv_y;
	glm::vec4 color;
}; 

struct GPUDrawPushConstants {
    glm::mat4 worldMatrix;
    VkDeviceAddress vertexBuffer;
};

VulkanRenderPassGeometry::VulkanRenderPassGeometry(
		Instance *_instance, RenderPassGeometryInfo info,
		std::vector<ResourceHandle<ResourceShader>> shaders,
		std::vector<RenderPassResource> resources)
		: RenderPassGeometry(_instance, {}, resources) {

	VulkanRenderer *renderer = (VulkanRenderer*)_instance->renderer.get();
	RL_LOG_TRACE("creating renderpass geometry");

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

	RL_LOG_TRACE("creating render info color formats");
	VkPipelineRenderingCreateInfo renderInfo = { .sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO };
	std::vector<VkFormat> colorFormats;
	size_t i = 0;
	for (auto &img : resources) {
		if(img.format == ImageFormat::COLOR) {
			color_image_index = i;
		}
		i++;
		VulkanResourceImage *vimg = (VulkanResourceImage*)img.texture.get();
		colorFormats.push_back(vimg->imageFormat); 
	}
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
	
	std::vector<Vertex> rect_vertices(4);
	rect_vertices[0].position = {0.5,-0.5, 0};
	rect_vertices[1].position = {0.5,0.5, 0};
	rect_vertices[2].position = {-0.5,-0.5, 0};
	rect_vertices[3].position = {-0.5,0.5, 0};
	rect_vertices[0].color = {0,0, 0, 1};
	rect_vertices[1].color = {0.5, 0.5, 0.5, 1};
	rect_vertices[2].color = {1, 0, 0, 1};
	rect_vertices[3].color = {0, 1, 0, 1};

	std::vector<uint32_t> rect_indices(6);
	rect_indices[0] = 0;
	rect_indices[1] = 1;
	rect_indices[2] = 2;
	rect_indices[3] = 2;
	rect_indices[4] = 1;
	rect_indices[5] = 3;
	mesh = new VulkanMeshBuffer(renderer,
			rect_indices, (char*)rect_vertices.data(),
			rect_vertices.size()*sizeof(Vertex));
}

VulkanRenderPassGeometry::~VulkanRenderPassGeometry() {
	VulkanRenderer *renderer = (VulkanRenderer*)instance->renderer.get();
    vkDeviceWaitIdle(renderer->device);
	vkDestroyPipelineLayout(renderer->device, layout, nullptr);
    vkDestroyPipeline(renderer->device, pipeline, nullptr);
	delete mesh;
}

void VulkanRenderPassGeometry::begin_pass(FrameContext context) {
	FrameDataVulkan *frame = (FrameDataVulkan*)context;
	VulkanResourceImage *vimg = (VulkanResourceImage*)resources[color_image_index].texture.get();

	VkRenderingAttachmentInfo colorAttachment = vkutil::attachment_info(vimg->imageView, nullptr, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
	VkRenderingInfo renderInfo = vkutil::rendering_info(frame->draw_extent, &colorAttachment, nullptr);
	vkutil::transition_image(frame->main_command_buffer, vimg->image, VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
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

void VulkanRenderPassGeometry::end_pass(FrameContext context) {
	FrameDataVulkan *frame = (FrameDataVulkan*)context;

	GPUDrawPushConstants push_constants;
	push_constants.worldMatrix = glm::mat4{ 1.f };
	push_constants.vertexBuffer = mesh->address;

	vkCmdPushConstants(frame->main_command_buffer, layout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(GPUDrawPushConstants), &push_constants);
	vkCmdBindIndexBuffer(frame->main_command_buffer, mesh->indexBuffer.buffer, 0, VK_INDEX_TYPE_UINT32);

	vkCmdDrawIndexed(frame->main_command_buffer, 6, 1, 0, 0, 0);
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
 

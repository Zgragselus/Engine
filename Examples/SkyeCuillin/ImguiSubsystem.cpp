#include "ImguiSubsystem.h"
#include "../Dependencies/Imgui/imgui.h"

using namespace SkyeCuillin;

ImguiSubsystem::ImguiSubsystem(Engine::D3DRenderer* renderer, int width, int height)
{
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.UserData = this;
	io.DisplaySize.x = (float)width;
	io.DisplaySize.y = (float)height;
	io.Fonts->AddFontDefault();

	io.KeyMap[ImGuiKey_Tab] = Engine::Keyboard::KEY_TAB;
	io.KeyMap[ImGuiKey_LeftArrow] = Engine::Keyboard::KEY_LEFT_ARROW;
	io.KeyMap[ImGuiKey_RightArrow] = Engine::Keyboard::KEY_RIGHT_ARROW;
	io.KeyMap[ImGuiKey_UpArrow] = Engine::Keyboard::KEY_UP_ARROW;
	io.KeyMap[ImGuiKey_DownArrow] = Engine::Keyboard::KEY_DOWN_ARROW;
	io.KeyMap[ImGuiKey_PageUp] = Engine::Keyboard::KEY_PAGE_UP;
	io.KeyMap[ImGuiKey_PageDown] = Engine::Keyboard::KEY_PAGE_DOWN;
	io.KeyMap[ImGuiKey_Home] = Engine::Keyboard::KEY_HOME;
	io.KeyMap[ImGuiKey_End] = Engine::Keyboard::KEY_END;
	io.KeyMap[ImGuiKey_Delete] = Engine::Keyboard::KEY_DELETE;
	io.KeyMap[ImGuiKey_Backspace] = Engine::Keyboard::KEY_BACKSPACE;
	io.KeyMap[ImGuiKey_Enter] = Engine::Keyboard::KEY_RETURN;
	io.KeyMap[ImGuiKey_Escape] = Engine::Keyboard::KEY_ESCAPE;
	io.KeyMap[ImGuiKey_A] = 'A';
	io.KeyMap[ImGuiKey_C] = 'C';
	io.KeyMap[ImGuiKey_V] = 'V';
	io.KeyMap[ImGuiKey_X] = 'X';
	io.KeyMap[ImGuiKey_Y] = 'Y';
	io.KeyMap[ImGuiKey_Z] = 'Z';

	unsigned char* pixels;
	int textureWidth, textureHeight;
	io.Fonts->GetTexDataAsRGBA32(&pixels, &textureWidth, &textureHeight);

	mGuiFont = new Engine::Texture();
	mGuiFont->Init(renderer, textureWidth, textureHeight, 1, Engine::Graphics::Format::RGBA8, pixels);

	io.Fonts->TexID = mGuiFont;
	
	mGuiRS = new Engine::RootSignature(renderer->GetDevice(), 2, 1);
	(*mGuiRS)[0].InitAsConstants(0, 2);
	(*mGuiRS)[1].InitAsDescriptorTable(1);
	(*mGuiRS)[1].SetTableRange(0, D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 0, 1);
	mGuiRS->InitStaticSampler(0, Engine::Sampler::CreateSamplerD3D(Engine::Sampler::Filter::MIN_LINEAR_MAG_LINEAR_MIP_LINEAR,
		Engine::Sampler::Address::WRAP,
		Engine::Sampler::Address::WRAP,
		Engine::Sampler::Address::WRAP,
		0.0f,
		1,
		Engine::Sampler::ComparisonFunc::NEVER,
		1.0f, 1.0f, 1.0f, 1.0f,
		0.0f,
		32.0f));
	mGuiRS->Finalize(D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	mGuiShader = new Engine::D3DShader("../Data/SkyeCuillin/Gui.hlsl",
		std::vector<Engine::D3DShader::ShaderEntryPoint>
	{
		Engine::D3DShader::ShaderEntryPoint(Engine::D3DShader::VERTEX_SHADER, "VS"),
			Engine::D3DShader::ShaderEntryPoint(Engine::D3DShader::PIXEL_SHADER, "PS")
	});

	Engine::InputLayout guiLayout = Engine::InputLayout();
	guiLayout.AddAttribute(Engine::InputLayout::Attribute("POSITION", 0, Engine::Graphics::Format::RGBA32F, 0, (size_t)(&((ImDrawVert*)0)->pos), Engine::InputLayout::Classification::PER_VERTEX, 0));
	guiLayout.AddAttribute(Engine::InputLayout::Attribute("TEXCOORD", 0, Engine::Graphics::Format::RG32F, 0, (size_t)(&((ImDrawVert*)0)->uv), Engine::InputLayout::Classification::PER_VERTEX, 0));
	guiLayout.AddAttribute(Engine::InputLayout::Attribute("COLOR", 0, Engine::Graphics::Format::RGBA8, 0, (size_t)(&((ImDrawVert*)0)->col), Engine::InputLayout::Classification::PER_VERTEX, 0));

	Engine::DepthStencilState guiDsState = Engine::DepthStencilState(false);

	std::vector<Engine::Graphics::Format> guiRtvFormats;
	guiRtvFormats.push_back(Engine::Graphics::Format::RGBA8);

	Engine::BlendState::BlendDesc guiBlend = Engine::BlendState::BlendDesc(true, false, Engine::BlendState::SRC_ALPHA, Engine::BlendState::INV_SRC_ALPHA, Engine::BlendState::OP_ADD,
		Engine::BlendState::INV_SRC_ALPHA, Engine::BlendState::ZERO, Engine::BlendState::OP_ADD);

	mGuiPS = new Engine::PipelineState(renderer->GetDevice(),
		mGuiRS,
		mGuiShader,
		Engine::BlendState::CreateStateD3D(false, true, guiBlend),
		Engine::RasterizerState::CreateStateD3D(Engine::RasterizerState::SOLID, Engine::RasterizerState::NONE, true),
		&guiDsState,
		&guiLayout,
		Engine::PipelineState::PrimitiveType::TRIANGLE,
		guiRtvFormats,
		Engine::Graphics::Format::D32F,
		1,
		0);

	mGuiVBO = new Engine::TypedBuffer(Engine::Graphics::R32F);
	mGuiVBO->Init(renderer, 65536, sizeof(float), nullptr);

	mGuiIBO = new Engine::TypedBuffer(Engine::Graphics::R32F);
	mGuiIBO->Init(renderer, 65536, sizeof(unsigned int), nullptr);

}

ImguiSubsystem::~ImguiSubsystem()
{
	delete mGuiIBO;
	delete mGuiVBO;
	delete mGuiPS;
	delete mGuiShader;
	delete mGuiRS;
	delete mGuiFont;
	ImGui::DestroyContext();
}

void ImguiSubsystem::RenderGui(ImDrawData* draw_data)
{
	ImGuiIO& io = ImGui::GetIO();

	this->mContext->SetPipelineState(this->mGuiPS);
	this->mContext->SetRootSignature(this->mGuiRS);
	this->mContext->SetPrimitiveTopology(Engine::Graphics::TRIANGLELIST);
	this->mContext->SetConstants(0, Engine::DWParam(io.DisplaySize.x), Engine::DWParam(io.DisplaySize.y));
	this->mContext->GetCommandList()->Get()->SetGraphicsRootDescriptorTable(1, this->mGuiFont->GetSRV().mGpuHandle);

	// TODO: Grow index and vertex buffer if and when necessary

	/*this->mContext->TransitionResource(this->mGuiVBO, D3D12_RESOURCE_STATE_COPY_DEST, false);
	this->mContext->TransitionResource(this->mGuiIBO, D3D12_RESOURCE_STATE_COPY_DEST, true);

	size_t vtxOffset = 0;
	size_t idxOffset = 0;

	for (int n = 0; n < draw_data->CmdListsCount; n++)
	{
		const ImDrawList* cmd_list = draw_data->CmdLists[n];
		this->mContext->WriteBuffer(this->mGuiVBO, vtxOffset, cmd_list->VtxBuffer.Data, cmd_list->VtxBuffer.Size * sizeof(ImDrawVert));
		this->mContext->WriteBuffer(this->mGuiIBO, vtxOffset, cmd_list->IdxBuffer.Data, cmd_list->IdxBuffer.Size * sizeof(ImDrawIdx));
		vtxOffset += cmd_list->VtxBuffer.Size * sizeof(ImDrawVert);
		idxOffset += cmd_list->IdxBuffer.Size * sizeof(ImDrawIdx);
	}

	this->mContext->TransitionResource(this->mGuiVBO, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, false);
	this->mContext->TransitionResource(this->mGuiIBO, D3D12_RESOURCE_STATE_INDEX_BUFFER, true);

	this->mContext->SetIndexBuffer(this->mGuiIBO->IndexBufferView(0, (unsigned int)draw_data->TotalIdxCount));
	this->mContext->SetVertexBuffer(0, this->mGuiVBO->VertexBufferView(0, (unsigned int)draw_data->TotalVtxCount, sizeof(ImDrawVert)));*/

	for (int n = 0; n < draw_data->CmdListsCount; n++)
	{
		int idx_offset = 0;
		int vtx_offset = 0;

		const ImDrawList* cmd_list = draw_data->CmdLists[n];
		size_t verticesCount = cmd_list->VtxBuffer.size();
		size_t indicesCount = cmd_list->IdxBuffer.size();
		size_t verticesSize = verticesCount * sizeof(ImDrawVert);
		size_t indicesSize = indicesCount * sizeof(ImDrawIdx);

		this->mContext->TransitionResource(this->mGuiVBO, D3D12_RESOURCE_STATE_COPY_DEST, false);
		this->mContext->TransitionResource(this->mGuiIBO, D3D12_RESOURCE_STATE_COPY_DEST, true);
		this->mContext->WriteBuffer(this->mGuiVBO, 0, &cmd_list->VtxBuffer[0], verticesSize);
		this->mContext->WriteBuffer(this->mGuiIBO, 0, &cmd_list->IdxBuffer[0], indicesSize);
		this->mContext->TransitionResource(this->mGuiVBO, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, false);
		this->mContext->TransitionResource(this->mGuiIBO, D3D12_RESOURCE_STATE_INDEX_BUFFER, true);

		this->mContext->SetIndexBuffer(this->mGuiIBO->IndexBufferView(0, (unsigned int)indicesSize));
		this->mContext->SetVertexBuffer(0, this->mGuiVBO->VertexBufferView(0, (unsigned int)verticesSize, sizeof(ImDrawVert)));

		for (int cmd_i = 0; cmd_i < draw_data->CmdLists[n]->CmdBuffer.Size; cmd_i++)
		{
			const ImDrawCmd* pcmd = &draw_data->CmdLists[n]->CmdBuffer[cmd_i];
			if (pcmd->TextureId != io.Fonts->TexID)
			{
				D3D12_GPU_DESCRIPTOR_HANDLE handle;
				handle.ptr = (uint64_t)pcmd->TextureId;
				this->mContext->GetCommandList()->Get()->SetGraphicsRootDescriptorTable(1, handle);
				if (pcmd->UserCallback)
				{
					pcmd->UserCallback(draw_data->CmdLists[n], pcmd);
				}
				else
				{
					this->mContext->SetScissorRect(pcmd->ClipRect.x, pcmd->ClipRect.y, pcmd->ClipRect.z, pcmd->ClipRect.w);
					this->mContext->DrawIndexed(pcmd->ElemCount, idx_offset, vtx_offset);
				}
				this->mContext->GetCommandList()->Get()->SetGraphicsRootDescriptorTable(1, this->mGuiFont->GetSRV().mGpuHandle);
			}
			else
			{
				if (pcmd->UserCallback)
				{
					pcmd->UserCallback(draw_data->CmdLists[n], pcmd);
				}
				else
				{
					this->mContext->SetScissorRect(pcmd->ClipRect.x, pcmd->ClipRect.y, pcmd->ClipRect.z, pcmd->ClipRect.w);
					this->mContext->DrawIndexed(pcmd->ElemCount, idx_offset, vtx_offset);
				}
			}
			idx_offset += pcmd->ElemCount;
		}
		vtx_offset += draw_data->CmdLists[n]->VtxBuffer.size();
	}
}
#ifndef __IMGUI__H__
#define __IMGUI__H__

#include "Main.h"
#include "../Dependencies/Imgui/imgui.h"
#include "../Dependencies/Imgui/imgui_internal.h"
#include "Graphics/Graphics.h"

namespace SkyeCuillin
{
	class ImguiSubsystem
	{
	private:
		Engine::D3DShader* mGuiShader;
		Engine::PipelineState* mGuiPS;
		Engine::RootSignature* mGuiRS;
		Engine::GpuBuffer* mGuiVBO;
		void* mGuiVBOPtr;
		Engine::GpuBuffer* mGuiIBO;
		void* mGuiIBOPtr;
		Engine::GraphicsContext* mContext;
		Engine::Texture* mGuiFont;

		void RenderGui(ImDrawData* draw_data);

	public:
		ImguiSubsystem(Engine::D3DRenderer* renderer, int width, int height);
		~ImguiSubsystem();
		
		inline void Update(Engine::GraphicsContext* context)
		{
			ImGuiIO& io = ImGui::GetIO();
			io.DeltaTime = 1.0f / 60.0f;

			mContext = context;
		}

		inline void Resize(int width, int height)
		{
			ImGuiIO& io = ImGui::GetIO();
			io.DisplaySize.x = (float)width;
			io.DisplaySize.y = (float)height;
		}

		inline void NewFrame()
		{
			ImGui::NewFrame();
		}

		inline void Render()
		{
			ImGui::Render();
			RenderGui(ImGui::GetDrawData());
		}

		//static void RenderGui(ImguiSubsystem* imgui, ImDrawData* draw_data);
	};
}

#endif
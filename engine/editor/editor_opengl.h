#pragma once
#include "engine/global/base.h"
#include "engine/runtime/rhi/opengl/rhi_window_opengl.h"
#include "engine/runtime/rhi/rhi_window.h"
#include "editor.h"
#include "imgui.h"
#include <memory>

namespace rtr {

class Editor_OpenGL : public Editor {

public:
    using Ptr = std::shared_ptr<Editor_OpenGL>;

    static Ptr create(const RHI_window::Ptr& window) {
        return std::make_shared<Editor_OpenGL>(window);
    }
    
    Editor_OpenGL(const RHI_window::Ptr& window) : Editor(window) {
        if (auto gl_window = std::dynamic_pointer_cast<RHI_window_OpenGL>(m_window)) {
            IMGUI_CHECKVERSION();
            ImGui::CreateContext();
            ImGui::StyleColorsDark();
            ImGui_ImplGlfw_InitForOpenGL(gl_window->window(), true);
            ImGui_ImplOpenGL3_Init("#version 460");
        }
    }
   
    ~Editor_OpenGL() {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }

    void begin_frame() override {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
    }

    void end_frame() override {
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    void begin_render(const std::string& title) override {
        ImGui::Begin(title.c_str());
    }

    void end_render() override {
        ImGui::End();
    }

    void color_edit(const std::string& title, float* color) override {
        ImGui::ColorEdit3(title.c_str(), color);
    }
    
    bool button(const std::string& title, float width, float height) override {
        return ImGui::Button(title.c_str(), ImVec2{width, height});
    }
    
};

}
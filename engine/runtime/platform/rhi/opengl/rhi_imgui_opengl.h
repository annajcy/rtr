#pragma once


#include "../rhi_imgui.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <memory>

class GLFWwindow;

namespace rtr {

class RHI_imgui_OpenGL : public RHI_imgui {

public:

    static std::shared_ptr<RHI_imgui_OpenGL> create(GLFWwindow* window, float dpi_scale = 1.0f) {
        return std::make_shared<RHI_imgui_OpenGL>(window, dpi_scale);
    }
    
    RHI_imgui_OpenGL(GLFWwindow* window, float dpi_scale) : RHI_imgui() {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();

        ImGuiIO& io = ImGui::GetIO();
        io.FontGlobalScale = dpi_scale;

        ImGui::StyleColorsDark();
        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init("#version 460");
    }
   
    ~RHI_imgui_OpenGL() {
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

    bool checkbox(const std::string& title, bool* value) override {
        return ImGui::Checkbox(title.c_str(), value);
    }
    void text(const std::string& title, const std::string& text) override {
        ImGui::Text("%s", text.c_str());
    }

    void text_edit(const std::string& title, std::string* text) override {
        ImGui::InputText(title.c_str(), text->data(), text->size());
    }

    bool slider_float(const std::string& title, float* value, float min, float max) override {
        return ImGui::SliderFloat(title.c_str(), value, min, max);
    }

    bool is_io_captured() override {
        return ImGui::GetIO().WantCaptureMouse || ImGui::GetIO().WantCaptureKeyboard;
    }
    
    float frame_rate() override {
        return ImGui::GetIO().Framerate;
    }
    
};

}
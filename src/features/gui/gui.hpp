#ifndef JACI_GUI_HPP
#define JACI_GUI_HPP

class CGui
{
public:
	static void Initialize();
	static void Render();
	static void Reset();
	
	static void DrawGui();
	static bool IsOpen();
	static void ToggleGui();

private:
	static bool IsInitialized();
	static void SetInitialized(bool value);
	static bool HasVulkan();
	// defined in gui_dxvk.cpp
	static void InitDXVK();
	static void RenderDXVK();
	static void ResetDXVK();

	// defined in gui_opengl.cpp
	static void InitOpenGL();
	static void RenderOpenGL();
	static void ResetOpenGL();
};

#endif
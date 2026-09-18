//
// Created by tevin on 29/06/2026.
//

#ifndef GUARACI_IGUI_HPP
#define GUARACI_IGUI_HPP

#include <SDL2/SDL.h>

class IGui
{
public:
	virtual ~IGui() = default;

	// starts ImGui and the actual gui
	virtual void init() = 0;
	virtual void render() = 0;
	virtual void reset() = 0;
};

IGui* f_gui();
void draw_gui();
bool is_gui_open();

#endif //GUARACI_IGUI_HPP

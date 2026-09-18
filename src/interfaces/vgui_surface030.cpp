//
// Created by tevin on 29/06/2026.
//

#include "vgui_surface030.hpp"

#include "../classes/createinterface.hpp"
#include "../utils/mem.hpp"

void ISurface::SetCursorAlwaysVisible(bool visible)
{

}

ISurface* v_surface()
{
	return GetInterface<ISurface>("vguimatsurface.so", "VGUI_Surface030");
}

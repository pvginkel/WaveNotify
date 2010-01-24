/*
 * This file is part of Google Wave Notifier.
 *
 * Google Wave Notifier is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Google Wave Notifier is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Google Wave Notifier.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _INC_LAYOUT
#define _INC_LAYOUT

#pragma once

//
// Popup layout information
//

#define PL_BORDER_WIDTH		2	// Outer border width
#define PL_WIDTH		264	// Width of all popups
#define PL_MSG_HEIGHT		65	// Height of the message popup
#define PL_WAVE_HEIGHT		109	// Height of the Wave popup
#define PL_PADDING		6	// Inner padding for popups within the border
#define PL_LINE_SPACING		3	// Spacing between different text blocks

// For 32x32 icons
//#define PL_ICON_SIZE		32	// Size the MainIcon is loaded with
//#define PL_TEXT_OFFSET	44	// Left offset of the text counting from padding
//#define PL_ICON_DX		0	// Full X adjustment of the icon
//#define PL_ICON_DY		0	// Full Y adjustment of the icon

// For 48x48 icons
#define PL_ICON_SIZE		48	// Size the MainIcon is loaded with
#define PL_TEXT_OFFSET		54	// Left offset of the text counting from padding
#define PL_ICON_DX		0	// Full X adjustment of the icon
#define PL_ICON_DY		-12	// Full Y adjustment of the icon

//
// Contact online popup information
//

#define PL_CO_HEIGHT		86	// Full height of the popup
#define PL_CO_WIDTH		263	// Full width of the popup
#define PL_CO_ICON_SIZE		64	// Avatar size
#define PL_CO_ICON_DX		9	// Avatar left offset
#define PL_CO_ICON_DY		9	// Avatar top offset
#define PL_CO_LABEL_DX		7	// Label left offset
#define PL_CO_LABEL_DY		4	// Label top offset

//
// Flyout layout information
//

#define FL_WIDTH		240	// Flyout width
#define FL_HEIGHT		300	// Flyout height
#define FL_BOTTOM_HEIGHT	30	// Height of the bottom panel
#define FL_SPACING		6	// Spacing between the different waves
#define FL_MAXIMUM_WAVES	5	// Maximum number of waves to show

#endif // _INC_LAYOUT

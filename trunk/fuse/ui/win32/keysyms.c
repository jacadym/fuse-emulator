/* keysyms.c: keysym to Spectrum key mappings for Win32
   Copyright (c) 2004 Marek Januszewski

   $Id$

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 49 Temple Place, Suite 330, Boston, MA 02111-1307 USA

   Author contact information:

   E-mail: pak21-fuse@srcf.ucam.org
   Postal address: 15 Crescent Road, Wokingham, Berks, RG40 2DB, England

*/

/* This file is autogenerated from keysyms.dat by keysyms.pl.
   Do not edit unless you know what you're doing! */

#include <config.h>

#ifdef UI_WIN32			/* Use this iff we're using UI_WIN32 */

#include "keyboard.h"
#include "ui/ui.h"

#include <windows.h>

/* http://msdn.microsoft.com/library/default.asp?url=/library/en-us/winui/WinUI/WindowsUserInterface/UserInput/VirtualKeyCodes.asp */

keysyms_map_t keysyms_map[] = {

  { VK_ESCAPE,      INPUT_KEY_Escape      },
  { VK_F1,          INPUT_KEY_F1          },
  { VK_F2,          INPUT_KEY_F2          },
  { VK_F3,          INPUT_KEY_F3          },
  { VK_F4,          INPUT_KEY_F4          },
  { VK_F5,          INPUT_KEY_F5          },
  { VK_F6,          INPUT_KEY_F6          },
  { VK_F7,          INPUT_KEY_F7          },
  { VK_F8,          INPUT_KEY_F8          },
  { VK_F9,          INPUT_KEY_F9          },
  { VK_F10,         INPUT_KEY_F10         },
  { VK_F11,         INPUT_KEY_F11         },
  { VK_F12,         INPUT_KEY_F12         },
  { '1',           INPUT_KEY_1           },
  { '2',           INPUT_KEY_2           },
  { '3',           INPUT_KEY_3           },
  { '4',           INPUT_KEY_4           },
  { '5',           INPUT_KEY_5           },
  { '6',           INPUT_KEY_6           },
  { '7',           INPUT_KEY_7           },
  { '8',           INPUT_KEY_8           },
  { '9',           INPUT_KEY_9           },
  { '0',           INPUT_KEY_0           },
  { 189,       INPUT_KEY_minus       },
  { 187,       INPUT_KEY_equal       },
  { VK_BACK,   INPUT_KEY_BackSpace   },
  { 'Q',           INPUT_KEY_q           },
  { 'W',           INPUT_KEY_w           },
  { 'E',           INPUT_KEY_e           },
  { 'R',           INPUT_KEY_r           },
  { 'T',           INPUT_KEY_t           },
  { 'Y',           INPUT_KEY_y           },
  { 'U',           INPUT_KEY_u           },
  { 'I',           INPUT_KEY_i           },
  { 'O',           INPUT_KEY_o           },
  { 'P',           INPUT_KEY_p           },
  { VK_CAPITAL,   INPUT_KEY_Caps_Lock   },
  { 'A',           INPUT_KEY_a           },
  { 'S',           INPUT_KEY_s           },
  { 'D',           INPUT_KEY_d           },
  { 'F',           INPUT_KEY_f           },
  { 'G',           INPUT_KEY_g           },
  { 'H',           INPUT_KEY_h           },
  { 'J',           INPUT_KEY_j           },
  { 'K',           INPUT_KEY_k           },
  { 'L',           INPUT_KEY_l           },
  { 186,   INPUT_KEY_semicolon   },
  { 222,  INPUT_KEY_apostrophe  },
/*
  TODO:
  { GDK_numbersign,  INPUT_KEY_numbersign  }, 
*/
  { VK_RETURN,      INPUT_KEY_Return      },
/*
  TODO:
  { GDK_Shift_L,     INPUT_KEY_Shift_L     },
*/
  { 'Z',           INPUT_KEY_z           },
  { 'X',           INPUT_KEY_x           },
  { 'C',           INPUT_KEY_c           },
  { 'V',           INPUT_KEY_v           },
  { 'B',           INPUT_KEY_b           },
  { 'N',           INPUT_KEY_n           },
  { 'M',           INPUT_KEY_m           },
  { 188,       INPUT_KEY_comma       },
  { 190,      INPUT_KEY_period      },
  { 191,       INPUT_KEY_slash       },
/*
  TODO:
  { GDK_Shift_R,     INPUT_KEY_Shift_R     },
  TODO:
*/
 { VK_CONTROL,   INPUT_KEY_Control_L   },
/*
  TODO:
  { GDK_Alt_L,       INPUT_KEY_Alt_L       },
  { GDK_Meta_L,      INPUT_KEY_Meta_L      },
  { GDK_Super_L,     INPUT_KEY_Super_L     },
  { GDK_Hyper_L,     INPUT_KEY_Hyper_L     },
*/
  { VK_SPACE,       INPUT_KEY_space       },
/*
  TODO:
  { GDK_Hyper_R,     INPUT_KEY_Hyper_R     },
  { GDK_Super_R,     INPUT_KEY_Super_R     },
  { GDK_Meta_R,      INPUT_KEY_Meta_R      },
  { GDK_Alt_R,       INPUT_KEY_Alt_R       },
  TODO:
*/
  { VK_CONTROL,   INPUT_KEY_Control_R   },
/*
  TODO:
  { GDK_Mode_switch, INPUT_KEY_Mode_switch },
*/
  { VK_LEFT,        INPUT_KEY_Left        },
  { VK_DOWN,        INPUT_KEY_Down        },
  { VK_UP,          INPUT_KEY_Up          },
  { VK_RIGHT,       INPUT_KEY_Right       },
  { VK_PRIOR,     INPUT_KEY_Page_Up     },
  { VK_NEXT,   INPUT_KEY_Page_Down   },
  { VK_HOME,        INPUT_KEY_Home        },
  { VK_END,         INPUT_KEY_End         },

  { 0, 0 }			/* End marker: DO NOT MOVE! */

};

#endif			/* #ifdef UI_WIN32 */

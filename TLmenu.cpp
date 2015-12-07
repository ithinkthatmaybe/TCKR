/*
 * TLmenu.cpp
 *
 * Created: 12/5/2015 8:18:05 PM
 *  Author: Sam
 */ 

#include "TLmenu.h"


menu_page* TLmenu_home_page_link;
menu_page* TLmenu_shoot_page_link;
menu_page* TLmenu_param_edit_page_link;

//extern int index; //debugging

menu_page TLmenu_home_page = {
	4,	// Number of page items
	{	// Items
		{
			MENU_ITEM_TEXT,
			{
				.text_item = {"-- TCKR --"}
			}
		},
		{
			MENU_ITEM_CALLBACK,
			{
				.callback_item =
				{
					"Start shoot",
					&TLmenu_start_shoot
				}
			}
		},
		{
			MENU_ITEM_PAGE_LINK,
			{
				.link_item =
				{
					"Edit params",
					&TLmenu_param_edit_page_link
				}
			}
		},
		{
			MENU_ITEM_CALLBACK,
			{
				.callback_item =
				{
					"Load preset A ",
					NULL
				}
			}
		}
	}
};

menu_page TLmenu_shoot_page = 
{
	1,
	{
		{
			MENU_ITEM_CALLBACK,
			{
				.callback_item =
				{
					"Stop shoot",
					&TLmenu_stop_shoot
				}
			}
		}
	}
};

menu_page TLmenu_param_edit_page = 
{
	6,
	{
		{
			MENU_ITEM_PAGE_LINK,
			{
				.link_item = 
				{
					"back",
					&TLmenu_home_page_link
				}
			}
		},
		{
			MENU_ITEM_PARAM_INT,
			{
				.param_item =
				{
					"Shoot time: %3d min",
					&shot_time_m,		// TODO add param
					1,
					300,
					1,
					0
				}
			}		
		},
		{
			MENU_ITEM_PARAM_INT,
			{
				.param_item =
				{
					"Clip  time: %3d sec",
					&clip_time_s,		// TODO add param
					1,
					300,
					1,
					0
				}
			}
		},
		{
			MENU_ITEM_PARAM_INT,
			{
				.param_item =
				{
					"Lin dist:   %3d mm",
					&rail_dist,		// TODO add param
					1,
					300,
					5,
					0
				}
			}
		},
		{
			MENU_ITEM_PARAM_INT,
			{
				.param_item =
				{
					"RotA dist:  %3d deg",
					&rotA_arc,		// TODO add param
					1,
					300,
					5,
					0
				}
			}
		},
		{
			MENU_ITEM_CALLBACK,
			{
				.callback_item =
				{
					"Start shoot",
					&TLmenu_start_shoot
				}
			}
		}
	}
};

void TLmenu_init()
{
	// Connect page links
	TLmenu_home_page_link = &TLmenu_home_page;
	TLmenu_shoot_page_link = &TLmenu_shoot_page;
	TLmenu_param_edit_page_link = &TLmenu_param_edit_page;
	
	menu_init(&TLmenu_home_page);
}



void TLmenu_start_shoot()
{
	shot_control_shot_begin();
	load_page(&TLmenu_shoot_page_link);
}


void TLmenu_stop_shoot()
{
	shot_control_shot_end();
	load_page(&TLmenu_param_edit_page_link);
}
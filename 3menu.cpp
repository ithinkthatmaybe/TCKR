/*
 * _3menu.cpp
 *
 * Created: 11/18/2015 5:39:22 PM
 *  Author: Sam
 */ 

#include "3menu.h"

#include "U8glib.h"


int actions_registered = 0;
menu_action_t registered_actions[MENU_MAX_NUM_REGISTERED_ACTIONS];

int index = 0;
int scroll_loc = 0;
int selected = 0;

menu_page_t* pcurrent_page;

char text_buffer[MENU_MAX_TEXT_LEN];

U8GLIB_SH1106_128X64 u8g(U8G_I2C_OPT_DEV_0|U8G_I2C_OPT_FAST);


menu_item_t get_current_item()
{
	return pcurrent_page->menu_items[index];
}



inline bool allowed_index()
{
	return get_current_item().type != MENU_ITEM_TEXT;
}

// Index cannot fall on text items
inline void index_inc()
{
	int init_val = index;
	do {
		if (index < (pcurrent_page->num_items-1))
		{
			index++;
			if (index-scroll_loc >= PAGE_NUM_ITEMS)
				scroll_loc++;
		}
		if (index > (pcurrent_page->num_items))
			index = init_val;
	} while (!allowed_index());
}

inline void index_dec()
{
	int init_val = index;
	do {
		if (index >= 0)
		{			
			index--;
			if (index < scroll_loc && scroll_loc > 0)
				scroll_loc--;
		}
		if (index < 0) // could not find an allowed index
			index = init_val;	
	} while (!allowed_index());
}



void menu_init(menu_page_t* pfirst_page)
{
	load_page(&pfirst_page);

	u8g.setColorIndex(1);         // B/W
	u8g.setFont(u8g_font_courR08);// use 8 pixel courier font
}


inline void menu_draw_cursor(int line_num)
{
	if (selected)
		u8g.drawStr(0, (line_num+1)*MENU_LINE_SPACING, "=");
	else	
		u8g.drawStr(0, (line_num+1)*MENU_LINE_SPACING, ">");
}

inline void menu_draw_str(char str[], int line_num)
{
	u8g.drawStr(MENU_ITEM_INDENT, (line_num+1)*MENU_LINE_SPACING, str);
}


void menu_draw_item_text(menu_item_t current_item, int line_num)
{
	menu_draw_str(current_item.item.text_item.text, line_num);
}

void menu_draw_item_param(menu_item_t current_item, int line_num)
{
	sprintf(text_buffer, current_item.item.param_item.text, 
		*current_item.item.param_item.param);
	menu_draw_str(text_buffer, line_num);
}

void menu_draw_item_param_getset(menu_item_t current_item, int line_num)
{
	sprintf(text_buffer, current_item.item.param_getset_item.text,
		current_item.item.param_getset_item.get());
	menu_draw_str(text_buffer, line_num);
}

void menu_draw_item_link(menu_item_t current_item, int line_num)
{
	menu_draw_str(current_item.item.link_item.text, line_num);
}

void menu_draw_item_callback(menu_item_t current_item, int line_num)
{
	menu_draw_str(current_item.item.callback_item.text, line_num);
}


void menu_update(void)
{
	while(actions_registered > 0)
		menu_action_do(registered_actions[--actions_registered]);
	
	u8g.firstPage();
	do {
		menu_draw_cursor(index-scroll_loc);
		// Draw items
		int i;
		for (i = 0; i < PAGE_NUM_ITEMS ; i++)
		{
			menu_item_t* current_item = &(pcurrent_page->menu_items[i + scroll_loc]);
			switch (current_item->type)
			{
				case MENU_ITEM_TEXT :
					menu_draw_item_text(*current_item, i);
					break;
				case MENU_ITEM_PARAM_INT :
					menu_draw_item_param(*current_item, i);
					break;
				case MENU_ITEM_PARAM_GETSET_INT :
					menu_draw_item_param_getset(*current_item, i);
					break;
				case MENU_ITEM_PAGE_LINK :
					menu_draw_item_link(*current_item, i);
					break;
				case MENU_ITEM_CALLBACK :
					menu_draw_item_callback(*current_item, i);
					break;
			}
		}
	} while(u8g.nextPage());
}


void menu_action_register(menu_action_t action)
{
	registered_actions[actions_registered] = action;
	actions_registered++;
}

void menu_action_register_decrement()
{
	menu_action_register(DECREMENT);	
}

void menu_action_register_increment()
{
	menu_action_register(INCREMENT);
}

void menu_action_register_select()
{
	menu_action_register(SELECT);
}




void load_page(menu_page_t** new_page)
{	
	assert(MENU_MAX_NUM_ITEMS >= (*new_page)->num_items);
	
	pcurrent_page = *(new_page);
	index = 0;
	scroll_loc = 0;
	if (!allowed_index())
		index_inc();
}


void menu_action_do(menu_action_t action)
{
	menu_item_t current_item = get_current_item();
	switch (current_item.type)
	{
		case MENU_ITEM_PARAM_INT :
			switch (action)
			{
				case INCREMENT:	
					if (selected)
					{
						int val = *(current_item.item.param_item.param);
						if (val + current_item.item.param_item.inc <= current_item.item.param_item.max)
							*(current_item.item.param_item.param) = val + current_item.item.param_item.inc;
					}
					else
						index_inc();
					break;
				case DECREMENT:
					if (selected)
					{
						int val = *(current_item.item.param_item.param);
						if (val + current_item.item.param_item.inc >= current_item.item.param_item.min)
						*(current_item.item.param_item.param) = val - current_item.item.param_item.inc;
					}
					else
						index_dec();
					break;
				case SELECT :
					selected = !selected;
					break;
			}
			break;			
		
		case MENU_ITEM_PARAM_GETSET_INT :
			switch (action)
			{
				case INCREMENT:
					if (selected)
					{
						if (current_item.item.param_getset_item.get() < current_item.item.param_getset_item.max)
							current_item.item.param_getset_item.set(current_item.item.param_getset_item.get() + 1);
					}
					else
						index_inc();
					break;
					
				case DECREMENT:
					if (selected)
					{
						if (current_item.item.param_getset_item.get() > current_item.item.param_getset_item.min)
							current_item.item.param_getset_item.set(current_item.item.param_getset_item.get() - 1);
					}
					else
						index_dec();
					break;
				
				case SELECT :
					selected = !selected;
					break;
			}
			break;
			
		case MENU_ITEM_PAGE_LINK :
			switch(action)
			{
				case INCREMENT :
					index_inc();
					break;
				case DECREMENT :
					index_dec();
					break;
				case SELECT :
					load_page(current_item.item.link_item.page_link);
					break;
			}
			break;
			
		case MENU_ITEM_CALLBACK :
			switch(action)
			{
				case INCREMENT:
					index_inc();
					break;
				case DECREMENT:
					index_dec();
					break;
				case SELECT:
					current_item.item.callback_item.func();
			}
			break;					
		
	}
}
	

/*
 * TLmenu.h
 *
 * Created: 12/5/2015 8:04:06 PM
 *  Author: Sam
 */ 


#ifndef TLMENU_H_
#define TLMENU_H_

#include "3menu.h"

#include "shot_control.h"



extern menu_page* TLmenu_home_page_link;
extern menu_page* TLmenu_shoot_page_link;
extern menu_page* TLmenu_param_edit_page_link;

extern menu_page TLmenu_home_page;
extern menu_page TL_menu_param_edit_page;
	

void TLmenu_init(void);

// Callback function to be used as a page link with added functionality
void TLmenu_start_shoot(void);
void TLmenu_stop_shoot(void);


#endif /* TLMENU_H_ */
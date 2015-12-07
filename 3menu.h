/*
 * _3menu.h
 *
 * Created: 11/18/2015 5:39:40 PM
 *  Author: Sam
 *
 * A menu system
 */ 


#ifndef THREE_MENU_H_
#define THREE_MENU_H_

#include <stdlib.h> 
#include <assert.h>

#include "U8glib.h"

#ifndef MENU_MAX_TEXT_LEN
#define MENU_MAX_TEXT_LEN 20
#endif
#ifndef MENU_MAX_NUM_ITEMS
#define MENU_MAX_NUM_ITEMS 12 
#endif
#ifndef MENU_LINE_SPACING
#define MENU_LINE_SPACING 10
#endif
#ifndef PAGE_NUM_ITEMS
#define PAGE_NUM_ITEMS 6
#endif
#ifndef MENU_ITEM_INDENT
#define MENU_ITEM_INDENT 8
#endif

// For buffereing actions that have been registered in the foreground
#define MENU_MAX_NUM_REGISTERED_ACTIONS 3


typedef enum actions{INCREMENT, DECREMENT, SELECT} menu_action_t;


enum {
	MENU_ITEM_TEXT, 
	MENU_ITEM_PARAM_INT,
	MENU_ITEM_PARAM_GETSET_INT, // Parameter with getter / setter functions
	MENU_ITEM_PAGE_LINK,
	MENU_ITEM_CALLBACK};


// bit fields for menu param item fields
/*enum {MENU_ITEM_FLAG_UNDERLINE = (1<<0),
	MENU_ITEM_FLAG_PARAM_ENABLE_ROLLOVER = (1<<1)
};*/


// TEXT ITEM
typedef struct menu_item_text{
	char text[MENU_MAX_TEXT_LEN];
} menu_item_text_t;

// PARAM ITEM
typedef struct menu_item_param_int{
	char text[MENU_MAX_TEXT_LEN];
	int* param;
	int min;
	int max;
	int inc;
	int flags;
} menu_item_param_int_t;


// PARAM GETSET ITEM
typedef struct menu_item_param_getset_int{
	char text[MENU_MAX_TEXT_LEN];
	int (*get)(void);
	void (*set)(int);
	int  max;
	int min;
	int flags;
} menu_item_param_getset_int_t;

// LINK ITEM
struct menu_page; // Resolve circular dependency
typedef struct menu_item_page_link{
	char text[MENU_MAX_TEXT_LEN];
	menu_page** page_link;
} menu_item_page_link_t;


// CALLBACK ITEM
typedef struct menu_item_callback{
	char text[MENU_MAX_TEXT_LEN];
	void (*func)(void);
} menu_item_callback_t;


typedef struct menu_item {
	int type;
	union {
		menu_item_text_t text_item;
		menu_item_param_int_t param_item;
		menu_item_param_getset_int_t param_getset_item;
		menu_item_page_link_t link_item;
		menu_item_callback_t callback_item;
	} item;
} menu_item_t;


typedef struct menu_page{
	int num_items;
	menu_item_t menu_items[MENU_MAX_NUM_ITEMS];
} menu_page_t;

	
void menu_init(menu_page_t *first_page);

void menu_update(void);

// Registers an application to be interpreted in the next update cycle
void menu_action_register(menu_action_t action);

void menu_action_register_increment(void);
void menu_action_register_decrement(void);
void menu_action_register_select(void);

void menu_action_do(menu_action_t action);

//void 3menu_action_update()


// Internal utility functions

void load_page(menu_page_t** new_page);	//navigation

/*
menu_item_t get_current_item(void); // Returns menu item at current index

// Cursor control & scrolling 
inline bool allowed_index(void);
inline void index_inc(void);
inline void index_dec(void);*/






#endif /* THREE_MENU_H_ */
#include <rtgui/widgets/list_view.h>

#include "play_list.h"
#include "douban_channel.h"
#include "player_ui.h"

static rtgui_list_view_t *_douban_channel_view = RT_NULL;

static void function_douban(struct rtgui_widget* widget, void *parameter);
static void function_return(struct rtgui_widget* widget, void *paramter);

static const struct rtgui_list_item channel_list[] =
{
	{"����-���ⲥ��",RT_NULL,function_douban, (void*)0}, 
    {"����׿ǳ� - ����Mhz", RT_NULL, function_douban, (void*)1},
    {"����ŷ��MHz", RT_NULL, function_douban, (void*)2},
    {"����ҡ��MHz", RT_NULL, function_douban, (void*)7},
    {"��������MHz", RT_NULL, function_douban, (void*)6},
    {"������ҥMHz", RT_NULL, function_douban, (void*)8},
    {"����������MHz", RT_NULL, function_douban, (void*)9},
    {"����70��MHz", RT_NULL, function_douban, (void*)3},
    {"����80��MHz", RT_NULL, function_douban, (void*)4},
    {"�����ϼ�", RT_NULL, function_return, RT_NULL },

    {"����90��MHz", RT_NULL, function_douban, (void*)5},

    {"����-��Ӱԭ��", RT_NULL, function_douban, (void*)10},
    {"����-��ʿ", RT_NULL, function_douban, (void*)13},
    {"����-����", RT_NULL, function_douban, (void*)14},
    {"����-˵��", RT_NULL, function_douban, (void*)15},
    {"����-R&B", RT_NULL, function_douban, (void*)16},
    {"����-����", RT_NULL, function_douban, (void*)17},
    {"����-����", RT_NULL, function_douban, (void*)18},
    {"����-Ů��", RT_NULL, function_douban, (void*)20},
    {"�����ϼ�", RT_NULL, function_return, RT_NULL },
	
    {"����-����", RT_NULL, function_douban, (void*)22},

    {"����23", RT_NULL, function_douban, (void*)23},
    {"����-������", RT_NULL, function_douban, (void*)26},
    {"����-�ŵ�", RT_NULL, function_douban, (void*)27},
    {"����-����", RT_NULL, function_douban, (void*)28},
    {"����29", RT_NULL, function_douban, (void*)29},
    {"����-����", RT_NULL, function_douban, (void*)32},
    {"����33", RT_NULL, function_douban, (void*)33},
    {"����35", RT_NULL, function_douban, (void*)35},
	{"�����ϼ�", RT_NULL, function_return, RT_NULL },
	   
    {"����42", RT_NULL, function_douban, (void*)42},
    {"����-��������", RT_NULL, function_douban, (void*)43},	
	
    {"�����ϼ�", RT_NULL, function_return, RT_NULL },
};

static void function_douban(struct rtgui_widget* widget, void *parameter)
{
	int channel;
	char channel_url[32];

	channel = (int)parameter;

	rt_kprintf("douban channel %d\n", channel);
	rt_snprintf(channel_url, sizeof(channel_url), "douban://%c", channel);

	play_list_clear();
	play_list_append_radio(channel_url, 
		channel_list[_douban_channel_view->current_item].name);

	player_play_item(play_list_start());
	player_update_list();

	rtgui_view_end_modal(RTGUI_VIEW(_douban_channel_view), RTGUI_MODAL_OK);
}

static void function_return(struct rtgui_widget* widget, void *paramter)
{
	rtgui_view_end_modal(RTGUI_VIEW(_douban_channel_view), RTGUI_MODAL_CANCEL);
}

void douban_channel_view(rtgui_workbench_t* workbench)
{
    rtgui_rect_t rect;

    /* add function view */
    rtgui_widget_get_rect(RTGUI_WIDGET(workbench), &rect);
	if (_douban_channel_view == RT_NULL)
	{
		_douban_channel_view = rtgui_list_view_create(channel_list,
											   sizeof(channel_list)/sizeof(struct rtgui_list_item),
											   &rect,
											   RTGUI_LIST_VIEW_LIST);
		rtgui_workbench_add_view(workbench, RTGUI_VIEW(_douban_channel_view));
	}

	rtgui_view_show(RTGUI_VIEW(_douban_channel_view), RT_TRUE);
	rtgui_view_destroy(RTGUI_VIEW(_douban_channel_view));
	_douban_channel_view = RT_NULL;
}


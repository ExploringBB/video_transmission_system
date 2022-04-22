#pragma once
#ifndef __SUB_SHOW_H
#define __SUB_SHOW_H
#pragma comment(lib, "User32.lib") //改为在静态库中使用，添加windows依赖库

void Image_show(void *p);
void gui_sub(void *st);
void time_show(void *time);
//extern struct s_return;
#endif
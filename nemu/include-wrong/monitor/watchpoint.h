#ifndef __WATCHPOINT_H__
#define __WATCHPOINT_H__

#include "common.h"

typedef struct watchpoint {
  int NO;
  struct watchpoint *next;

  /* TODO: Add more members if necessary */
 char str[32]; //表达式
 int value; //旧值
 int hitnum; //记录触发次数
} WP;

WP* new_wp(); //新建监视点
bool free_wp(int NO); //删除监视点
bool check_point(); 
void show_point();

#endif

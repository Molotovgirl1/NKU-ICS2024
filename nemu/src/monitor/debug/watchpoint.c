#include "monitor/watchpoint.h"
#include "monitor/expr.h"

#define NR_WP 32

static WP wp_pool[NR_WP];
static WP *head, *free_;
static int used_next;  //用于记录在head中下一个使用的wp的index
static WP *wptemp;  //辅助wp结构

void init_wp_pool() {
  int i;
  for (i = 0; i < NR_WP; i ++) {
    wp_pool[i].NO = i;
    wp_pool[i].next = &wp_pool[i + 1];
    wp_pool[i].old = 0; //初始化旧值
    wp_pool[i].hitNum = 0; //初始化命中次数
  }
  wp_pool[NR_WP - 1].next = NULL;

  head = NULL;
  free_ = wp_pool;
  used_next = 0;
}

/* TODO: Implement the functionality of watchpoint */
bool new_wp(char *args) { 
  if(free_ == NULL) { 
    assert(0); 
  } 
  WP* result = free_; 
  free_ = free_ -> next; 
  result -> NO = used_next; 
  used_next++; 
  result -> next = NULL;  
  strcpy(result -> e, args); 
  result -> hitNum = 0; 
  bool is_success; 
  result -> old = expr(result -> e, &is_success);  
  if(is_success == false) { 
    printf("error in new_wp; expression fault!\n"); 
    return false; 
  } 
  wptemp = head; 
  if(wptemp == NULL) { 
    head = result; 
  } 
  else { 
    while (wptemp -> next != NULL)
    { 
      wptemp = wptemp -> next;  
    } 
    wptemp -> next = result; 
  } 
  printf("Success: set watchpoint %d, oldvalue = %d\n", result -> NO, result -> old); 
  return true;
}

//删除监视点
bool free_wp(int num) {
  WP *chosen = NULL; 
  if(head == NULL) { 
    printf("no watch point now\n"); 
    return false; 
  } 
  if(head -> NO == num) { 
    chosen = head; 
    head = head -> next; 
  } 
  else { 
    wptemp = head; 
    while (wptemp != NULL && wptemp -> next != NULL) 
    { 
      /* code */ 
      if(wptemp -> next -> NO == num) { //找到要删除的结点
        chosen = wptemp -> next; 
        wptemp -> next = wptemp -> next -> next; 
        break;  
      } 
      wptemp = wptemp -> next; 
    }
  } 
  if(chosen != NULL) { 
    chosen -> next = free_; 
    free_ = chosen; 
    return true; 
  } 
  return false; 
}
//打印监视点信息
void print_wp() { 
  if(head == NULL) { 
    printf("no watchpoint now\n"); 
    return; 
  } 
  printf("watchpoint:\n"); 
  printf("NO.  expr    hitTimes\n"); 
  wptemp = head; 
  while (wptemp != NULL) 
  { 
    printf("%d  %s    %d\n", wptemp -> NO, wptemp -> e, wptemp -> hitNum); 
    wptemp = wptemp ->next; 
  } 
}

bool watch_wp() { 
  bool is_success; 
  int result; 
  if(head == NULL) { 
    return true; 
  }  
  wptemp = head;
  while (wptemp != NULL) 
  { 
    result = expr(wptemp -> e, &is_success); 
    if(result != wptemp -> old) 
    { 
      wptemp -> hitNum += 1; 
      printf("Hardware watchpoint %d:%s\n", wptemp -> NO, wptemp -> e); 
      printf("Old value:%d\nNew valus:%d\n\n", wptemp -> old, result); 
      wptemp -> old = result; 
      return false; 
    } 
    wptemp = wptemp -> next; 
  } 
  return true;
}

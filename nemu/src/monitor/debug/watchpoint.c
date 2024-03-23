#include "monitor/watchpoint.h"
#include "monitor/expr.h"

#define NR_WP 32

static WP wp_pool[NR_WP]; //监视点池
static WP *head, *free_; //head用于组织使用中的监视点结构，free_用于组织空闲的监视点结构
static int used_next;  //用于记录在head中下一个使用的wp的序号
static WP *wptemp;  //辅助wp结构
//初始化
void init_wp_pool() {
  int i;
  for (i = 0; i < NR_WP; i ++) {
    wp_pool[i].NO = i; //设置序号
    wp_pool[i].next = &wp_pool[i + 1]; //链接wp
    wp_pool[i].old = 0; //初始化旧值
    wp_pool[i].hitNum = 0; //初始化命中次数
  }
  wp_pool[NR_WP - 1].next = NULL; //设置最后一个节点

  head = NULL;
  free_ = wp_pool;
  used_next = 0;
}

/* TODO: Implement the functionality of watchpoint */
//分配监视点
bool new_wp(char *args) { 
  if(free_ == NULL) { //不存在空闲监视点
    assert(0); 
  } 
  WP* result = free_; //获取将要分配的监视点
  free_ = free_ -> next; //更新空闲监视点
  result -> NO = used_next; //分配监视点序号
  used_next++; //更新下一个监视点序号
  result -> next = NULL;  
  strcpy(result -> e, args);  //把表达式复制到监视点里
  result -> hitNum = 0; //初始化命中次数
  bool is_success; 
  result -> old = expr(result -> e, &is_success);   //求旧值
  if(is_success == false) { 
    printf("ERROR:error in new_wp; expression fault!\n"); 
    return false; 
  } 
  wptemp = head; 
  if(wptemp == NULL) { //头节点为空
    head = result; //将头节点设为该节点
  } 
  else { 
    while (wptemp -> next != NULL)
    { 
      wptemp = wptemp -> next;  
    } 
    wptemp -> next = result; //设为链表中最后一个节点
  } 
  printf("Success: set watchpoint %d, oldvalue = %d\n", result -> NO, result -> old); //分配成功
  return true;
}

//删除监视点
bool free_wp(int num) {
  WP *chosen = NULL; 
  if(head == NULL) { //如果没有使用的监视点
    printf("ERROR:no watch point now\n"); 
    return false; 
  } 
  if(head -> NO == num) { //链表中头节点为要删除的监视点
    chosen = head; 
    head = head -> next; 
  } 
  else { //遍历链表寻找要删除的节点
    wptemp = head; 
    while (wptemp != NULL && wptemp -> next != NULL) 
    { 
      if(wptemp -> next -> NO == num) { 
        chosen = wptemp -> next; 
        wptemp -> next = wptemp -> next -> next; 
        break;  
      } 
      wptemp = wptemp -> next; 
    }
  } 
  if(chosen != NULL) { //插入空闲链表
    chosen -> next = free_; 
    free_ = chosen; //更新空闲链表头节点
    return true; 
  } 
  return false; 
}

//打印监视点信息
void print_wp() { 
  if(head == NULL) { //没有使用中的监视点
    printf("ERROR:no watchpoint now\n"); 
    return; 
  } 
  printf("watchpoint:\n"); 
  printf("NO.  expr    hitTimes\n"); 
  wptemp = head; 
  while (wptemp != NULL) //遍历使用链表打印监视点信息
  { 
    printf("%d  %s    %d\n", wptemp -> NO, wptemp -> e, wptemp -> hitNum); 
    wptemp = wptemp ->next; 
  } 
}
//监视监视点表达式的值
bool watch_wp() { 
  bool is_success; 
  int result; 
  if(head == NULL) { //没有使用中的监视点
    return true; 
  }  
  wptemp = head;
  while (wptemp != NULL)  //遍历使用链表
  { 
    result = expr(wptemp -> e, &is_success); //监视点的表达式求值
    if(result != wptemp -> old) //判断是否发送改变
    { 
      wptemp -> hitNum += 1;  //命中次数加一
      printf("Hardware watchpoint %d:%s\n", wptemp -> NO, wptemp -> e); 
      printf("Old value:%d\nNew valus:%d\n\n", wptemp -> old, result); 
      wptemp -> old = result;  //更新旧值
      return false;  //触发一次就返回
    } 
    wptemp = wptemp -> next; 
  } 
  return true;
}

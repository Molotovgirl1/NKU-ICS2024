#include "monitor/monitor.h"
#include "monitor/expr.h"
#include "monitor/watchpoint.h"
#include "nemu.h"

#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>

void cpu_exec(uint64_t);

/* We use the `readline' library to provide more flexibility to read from stdin. */
char* rl_gets() {
  static char *line_read = NULL;

  if (line_read) {
    free(line_read);
    line_read = NULL;
  }

  line_read = readline("(nemu) ");

  if (line_read && *line_read) {
    add_history(line_read);
  }

  return line_read;
}

static int cmd_c(char *args) {
  cpu_exec(-1);
  return 0;
}

static int cmd_q(char *args) {
  return -1;
}
//函数声明
static int cmd_help(char *args);
static int cmd_si(char *args);
static int cmd_info(char *args);
static int cmd_x(char *args);
static int cmd_p(char *args);
static int cmd_w(char *args);
static int cmd_d(char *args);

static struct {
  char *name;
  char *description;
  int (*handler) (char *);
} cmd_table [] = {
  { "help", "Display informations about all supported commands", cmd_help },
  { "c", "Continue the execution of the program", cmd_c },
  { "q", "Exit NEMU", cmd_q },
  { "si", "args:[N]; exectue [N] instructions step by step", cmd_si},//单步执行 
  { "info", "args:r/w;print information about register or watch point ", cmd_info}, //打印寄存器状态
  { "x", "x [N] [EXPR];sacn the memory", cmd_x }, //内存扫描
  { "p", "expr", cmd_p}, //表达式
  { "w", "set the watchpoint", cmd_w}, //添加监视点
  { "d", "delete the watchpoint", cmd_d} //删除监视点
  /* TODO: Add more commands */

};

#define NR_CMD (sizeof(cmd_table) / sizeof(cmd_table[0]))

static int cmd_help(char *args) {
  /* extract the first argument */
  char *arg = strtok(NULL, " ");
  int i;

  if (arg == NULL) {
    /* no argument given */
    for (i = 0; i < NR_CMD; i ++) {
      printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
    }
  }
  else {
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(arg, cmd_table[i].name) == 0) {
        printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
        return 0;
      }
    }
    printf("Unknown command '%s'\n", arg);
  }
  return 0;
}
//单步调试
static int cmd_si(char *args) {
  uint64_t N = 0;
  if(args == NULL) { //没有参数时
    N = 1; //默认为1
  }
  else {
    int temp = sscanf(args, "%llu", &N); //读取参数
    if(temp <= 0) { //解析失败
      printf("ERROR:args error in cmd_si\n");
      return 0;
    }
  }
  cpu_exec(N);
  return 0;
}
//打印程序状态
static int cmd_info(char *args) { 
  char s;
  if(args == NULL) { //没有参数时
    printf("ERROR:args error in cmd_info (miss args)\n");
    return 0;
  }
  int temp = sscanf(args, "%c", &s);
  if(temp <= 0) { //解析失败
    printf("args error in cmd_info\n");
    return 0;
  }
  if(s == 'w') { //打印监视点信息
    print_wp();;
    return 0;
  }
  if(s == 'r') { //打印寄存器信息
 	//32bit
    for(int i = 0; i < 8; i++) {
      printf("%s  0x%x\n", regsl[i], reg_l(i));
    }
    printf("eip  0x%x\n", cpu.eip);
    //16bit
    for(int i = 0; i < 8; i++) {
      printf("%s  0x%x\n", regsw[i], reg_w(i));
    }
    //8bit
    for(int i = 0; i < 8; i++)
    {
      printf("%s  0x%x\n", regsb[i], reg_b(i));
    }
    return 0;
  }
  printf("ERROR:args error in cmd_info\n"); //如果产生错误报错
  return 0;
}
//内存扫描
static int cmd_x(char *args) {
  int nLen = 0; //长度
  vaddr_t addr; //起始地址
  int temp = sscanf(args, "%d 0x%x", &nLen, &addr);  //从字符串中读进与指定格式相符的数据
  if(temp <= 0) { //解析失败
    printf("ERROR:args error in cmd_si\n");
    return 0;
  } 
  printf("Memory:"); //打印内存
  for(int i = 0; i < nLen; i++) {  
    if(i % 4 == 0) { 
      printf("\n0x%x:  0x%02x", addr + i, vaddr_read(addr + i, 1));
    } 
    else {
      printf("  0x%02x", vaddr_read(addr + i, 1));
    }
  }
  printf("\n");
  return 0;
}
//计算表达式的值
static int cmd_p(char *args) {
  bool is_success;
  int temp = expr(args, &is_success); //计算表达式
  if(is_success == false) { //表达式错误
    printf("ERROR:error in expr()\n");
  }
  else {
    printf("the value of expr is:%d\n", temp); //打印表达式的值
  }
  return 0;
}
//添加监视点
static int cmd_w(char *args) { 
  new_wp(args); //分配监视点
  return 0;
}
//删除监视点
static int cmd_d(char* args) {
  int num = 0;
  int nRet = sscanf(args, "%d", &num); //解析参数
  if(nRet <= 0) { //解析失败
    printf("args error in cmd_si\n");
    return 0;
  }
  int r = free_wp(num); //删除监视点
  if(r == false) {//删除监视点失败
    printf("ERROR: no watchpoint %d\n", num);
  }
  else {
    printf("SUCCESS:Success delete watchpoint %d\n", num);
  }
  return 0;
}



void ui_mainloop(int is_batch_mode) {
  if (is_batch_mode) {
    cmd_c(NULL);
    return;
  }

  while (1) {
    char *str = rl_gets();
    char *str_end = str + strlen(str);

    /* extract the first token as the command */
    char *cmd = strtok(str, " ");
    if (cmd == NULL) { continue; }

    /* treat the remaining string as the arguments,
     * which may need further parsing
     */
    char *args = cmd + strlen(cmd) + 1;
    if (args >= str_end) {
      args = NULL;
    }

#ifdef HAS_IOE
    extern void sdl_clear_event_queue(void);
    sdl_clear_event_queue();
#endif

    int i;
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(cmd, cmd_table[i].name) == 0) {
        if (cmd_table[i].handler(args) < 0) { return; }
        break;
      }
    }

    if (i == NR_CMD) { printf("Unknown command '%s'\n", cmd); }
  }
}

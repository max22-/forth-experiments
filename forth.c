#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHAR_OUT(c) printf("%c", c)
#define CHAR_IN() getchar()
static void bye() { exit(0); }

typedef int cell;
static char tib[256];
static cell tin = 0, ntib = 0;

#define HEAP_SIZE (64*1024)
static cell heap[HEAP_SIZE];
static cell *here, *stack_start, *rstack_start, *stack_end, *rstack_end, *sp, *rp;

#define STACK_SIZE 32

typedef struct xt_t {
  struct xt_t *next;
  char *name;
  void (*prim)(void);
} xt_t;

static xt_t *dictionary = NULL;

static void print(const char *msg)
{
  while(*msg)
    CHAR_OUT(*msg++);
}

static void error(const char *msg)
{
  while(*msg)
    CHAR_OUT(*msg++);
  CHAR_OUT('\n');
  tin = 0;
  ntib = 0;
}

static void line()
{
  cell c;
  print(" ok\n");
  tin = 0;
  ntib = 0;
  while(1) {
    c = CHAR_IN();
    if(c == '\n')
      break;
    if(c == -1)
      bye();
    if(ntib < sizeof(tib))
      tib[ntib++] = (char)c;
    else error("line too long");
  }
}

static void skip_space()
{
  if(tin >= ntib) return;
  if(tib[tin] == ' ') tin++;
}

static const char *word()
{
  static char buffer[256];
  cell nb = 0;
  buffer[0] = 0;
  if(tin >= ntib) line();
  skip_space();
  while(tin < ntib && tib[tin] != ' ')
      buffer[nb++] = tib[tin++];
  buffer[nb] = 0;
  return buffer;
}

static xt_t *find(const char *w)
{
  xt_t *xt;
  for(xt = dictionary; xt; xt = xt->next)
    if(!strcmp(xt->name, w)) return xt;
  return NULL;
}

static void add_word(char *name, void (*prim)(void))
{
  #warning Avoid mallocs ?
  xt_t *xt = calloc(1, sizeof(xt_t));
  xt->next = dictionary;
  dictionary = xt;
  xt->name = malloc(sizeof(strlen(name)) + 1);
  strcpy(xt->name, name);
  xt->prim = prim; 
}

static void push(cell v)
{
  if(sp == stack_end) { error("stack overflow"); return; }
  *sp++ = v;
}

static cell pop()
{
  #warning is returning 0 correct ?
  if(sp <= stack_start) { error("stack underflow"); return 0; }
  return *--sp;
}

static void f_hello(void)
{
  print("hello, world!");
}

static void f_add()
{
  cell a = pop();
  *sp += a;
}

static void f_mul()
{
  cell a = pop();
  *sp *= a;
}

static void f_drop()
{
  pop();
}

static void f_words()
{
  xt_t *w;
  for(w = dictionary; w; w = w->next) {
    print(w->name); print(" ");
  }
  print("\n");
}

static void f_dot()
{
  printf("%d ", pop());
}

static void register_primitives(void)
{
  add_word("hello", f_hello);
  add_word("+", f_add);
  add_word("*", f_mul);
  add_word("drop", f_drop);
  add_word("words", f_words);
  add_word(".", f_dot);
}

static void interpret(const char *w)
{
  xt_t *xt;
  if((xt = find(w))) xt->prim();
  else {
    char *end;
    cell number = strtol(w, &end, 0);
    if(*end) error("undefined word");
    else push(number);
  }
}

cell *allot(cell n)
{
  cell *r;
  if(here - heap + n > sizeof(heap) / sizeof(cell)) {
    error("heap is full");
    return 0;
  }
  r = here;
  here += n;
  return r;
}

static void init()
{
  here = heap;
  stack_start = allot(STACK_SIZE);
  stack_end = here;
  sp = stack_start;
  rstack_start = allot(STACK_SIZE);
  rstack_end = here;
  rp = rstack_start;
  register_primitives();
}

int main(int argc, char *argv[])
{
  const char *w;
  init();
  while((w = word())) interpret(w);
  return 0;
}

#include "forth.h"

void plus(void)
{
    //printf(" adding ");
    push(pop() + pop());
}

void minus(void)
{
    cell n = pop();
    push(pop() - n);
}

void times(void)
{
    //printf(" timesing ");
    push(pop() * pop());
}

void divide(void)
{
    cell n = pop();
    push(pop() / n);
}

void print(void)
{
    cell n = pop();

    if (base == 10) {
        printf(" => %d ", n);
    } else {
        printf(" => %x ", n);
    }
}

void drop(void)
{
    (void)pop();
}

void bye(void)
{
    exit(0);
}

void unnest()
{
    ip = (cell *)rpop();
}

static cell unnester;

void semicolon(void)
{
    compile(unnester);
    state = 0;
}


void docolon(void)
{
    rpush((cell)ip);
    ip = w+1;
}

void colon(void)
{
    char *p;
    cell len;
    if ((len = safe_parse_word(&p)) == 0) {
	return;
    }
    header(p, len, docolon);
    state = 1;
}


void doconstant(void)
{
    push(*(w+1));
}

void constant(void)
{
    char *p;
    cell len;
    if ((len = safe_parse_word(&p)) == 0) {
	return;
    }
    header(p, len, doconstant);
    comma(pop());
}

void dovariable(void)
{
    push((cell)(w+1));
}

void variable(void)
{
    char *p;
    cell len;
    if ((len = safe_parse_word(&p)) == 0) {
	return;
    }
    header(p, len, dovariable);
    comma(0);
}


void doliteral(void)
{
    push(*ip++);
}

static cell dolit;

void literal(cell n)
{
    compile(dolit);
    compile(n);
}


void fetch(void)
{
    cell *adr = (cell *)pop();
    push(*adr);
}

void store(void)
{
    cell *adr = (cell *)pop();
    *adr = pop();
}

void init_dictionary(void)
{
    here = origin;

    cheader("exit", unnest);
    unnester = lastacf();
    cheader(";", semicolon); immediate();

    cheader(":", colon);
    cheader("variable", variable);
    cheader("constant", constant);

    cheader("(literal)", doliteral);
    dolit = lastacf();

    cheader("+", plus);
    cheader("-", minus);
    cheader("*", times);
    cheader("/", divide);
    cheader(".", print);

    cheader("drop", drop);
    cheader("dup", dup);
    cheader("depth", depth);

    cheader("@", fetch);
    cheader("!", store);

    cheader("bye", bye);
}

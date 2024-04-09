#include "mbed.h"

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "malisp.h"
#include "mbed_functions.h"

extern Serial pc;
extern DigitalOut led1;
extern DigitalOut led2;
extern DigitalOut led3;
extern DigitalOut led4;
extern char *lisplib;

int _stack = 5000;

LIST *TRU;
LIST *g_alist;      // 連想リスト
LIST *g_oblist;     // list of Lisp Functions

char progon;

//FILE *fd;     // input file descriptor
FILE_MINE   fd;

int32_t  getc_mine_buffer_pt = 0;
int      getc_mine_buffer[8];
uint32_t lisplib_counter = 0;

//----------------------------------------------------------------
//int getc_mine(FILE *fd)
int getc_mine(FILE_MINE fd)
{
    if (getc_mine_buffer_pt > 0) {
        int c = getc_mine_buffer[getc_mine_buffer_pt];
        getc_mine_buffer_pt--;
        if (getc_mine_buffer_pt < 0) {
            getc_mine_buffer_pt = 0;
        }
        return c;
    }

    //return getc(fd);
    
    int c = 0;
    if (fd == FILE_SERIAL) {
        c = pc.getc();
        //pc.putc(c);
 
    } else if (fd == FILE_STRING) {

        if (lisplib_counter > strlen(lisplib)) {
            c = EOF;     // EOF
        } else {
            c = *(lisplib + lisplib_counter);
            lisplib_counter++;
        }
    }
    
    return c;
}

//void ungetc_mine(int c, FILE *fd)
void ungetc_mine(int c, FILE_MINE fd)
{
    getc_mine_buffer_pt++;
    getc_mine_buffer[getc_mine_buffer_pt] = c;
}

//----------------------------------------------------------------
// main program
//----------------------------------------------------------------

void malisp_main()
{
    initialize();
    pc.printf("\nMarc Adler's LISP Interpreter. (mbed port and expansion by ohneta)\n");
    load_library();

    pc.printf("[FREE-MEM: %d bytes]\n", _getFreeMemorySize());

    {
        fd = FILE_SERIAL;
        getc_mine_buffer_pt = 0;
        interpret_malisp();
    }
}

void interpret_malisp()
{
    LIST *p = NULL;
    LIST *q = NULL;
    int c;

    while (EOF != (c = gettok())) {
        if (c == ERR) {
            continue;
        }

        switch (c) {
            case LPAREN:
                getc_mine(fd);   // span the paren
                q = makelist();
                p = eval(q, g_alist);
                break;

            case LETTER:
                p = cdr(car(getid()));
                break;
        }

        if (fd == FILE_SERIAL) {
            pc.printf("\n");
            pc.printf("value => ");
            if (p == NULL) {
                pc.printf("nil");
            } else {
                lisp_print(cons(p, NULL));
                //lisp_print(p);
            }
            pc.printf("\n");
        }
    }
}

//----------------------------------------------------------------
// initialization procedures
//----------------------------------------------------------------

void initialize()
{
    init("'", QUOTE);
    init("car", FCAR);
    init("cond", COND);
    init("cdr", FCDR);
    init("defun", DEFUN);
    init("cons",FCONS);
    init("nil", NILL);
    init("atom",FATOM);
    init("prog",PROG);
    init("eq",  FEQ);
    init("go",  GO);
    init("setq",FSETQ);
    init("return",RETRN);
    init("print",PRINT);
    init("read", FREAD);
    init("rplaca",FREPLACA);
    init("rplacd",FREPLACD);
    init("apply", FAPPLY);
    init("eval",  FEVAL);
    init("and", FAND);
    init("or", FOR);
    init("not", FNOT);
    init("plus",  PLUS);
    init("zerop", ZEROP);
    init("diff",  DIFF);
    init("greaterp", GREATERP);
    init("times", TIMES);
    init("lessp", LESSP);
    init("add1",  ADD1);
    init("sub1",  SUB1);
    init("quot",  QUOTIENT);
    TRU = cons(init("t", T), NULL);
    init("numberp", NUMBERP);
    rplact(TRU, SATOM);
    init("null",  NUL);
    init("funcall",FUNCALL);

    // for mbed functions
    init("info", FINFO);
    init("freemem", FFREEMEM);

    init("wait", FWAIT);
    init("dout", FDOUT);
    init("din",  FDIN);
    init("aout", FAOUT);
    init("ain",  FAIN);
    init("pwmout", PWMOUT);

    g_oblist = g_alist;
}

LIST *init(char *name, int t)
{
    LIST *p = install(name, false);
    rplact(p, t);

    return p;
}

//----------------------------------------------------------------
// create the executable list form of a LISP program
//----------------------------------------------------------------

LIST *makelist()
{
    LIST *p;

    switch (gettok()) {
        case LPAREN:
            getc_mine(fd);  // span the paren ?????
            p = makelist();
            p = cons(p, makelist());
            rplact(p, LST);
            return p;

        case LETTER:
            p = getid();
            return cons(p, makelist());

        case INQUOTE:
            p = getid();
            p = cons(p, makelist());
            rplaca(p, cons(car(p), cons(car(cdr(p)), NULL)));
            rplacd(p, cdr(cdr(p)));
            return p;

        case DIGIT:
            p = getnum();
            return cons(p, makelist());

        case RPAREN:
            getc_mine(fd);  // span rparen ??????
            return NULL;
    }

    return NULL;
}


// isp_print - walks along the list structure printing atoms
void lisp_print(LIST *p)
{
    if (p == NULL) {
        return;
    }

    if (type(p) == RATOM) {
        pc.printf("%f ", p->u.num);
    } else if (type(p) == IATOM) {
        pc.printf("%d ", (int) p->u.num);
    } else if (type(p) == SATOM) {
        pc.printf("%s ", getname(car(p)));
    } else if (type(car(p)) == LST) {
        pc.printf("%c", '(');
        lisp_print(car(p));
        pc.printf("%c", ')');
        lisp_print(cdr(p));
    } else if (type(p) == LST) {
        lisp_print(car(p));
        lisp_print(cdr(p));
    } else {
        pc.printf("******** can't print it out *******\n");
    }
}

//----------------------------------------------------------------
// evaluate a LISP function
//----------------------------------------------------------------

LIST *eval(LIST *x, LIST *alist)
{
    LIST *p, *q;
    int savt, t;

    if (x == NULL) {
        return NULL;
    }
    t = type(x);
    if (t == VARI) {
        return assoc(alist, getname(car(x)));
    }
    if (t == IATOM || t == RATOM) {
        return x;
    }
    if (t == LABL) {
        return NULL;
    }

    switch (type(car(x))) {
        case T:
            return TRU;

        case NILL:
            return NULL;

        case QUOTE:
            var_to_atom(car(cdr(x)));
            return car(cdr(x));

        case FCAR:
            return car(eval(cdr(x), alist));

        case FCDR:
            return cdr(eval(cdr(x), alist));

        case FATOM:
            return atom(eval(cdr(x), alist));

        case FEQ:
            return eq(eval(car(cdr(x)),alist), eval(cdr(cdr(x)),alist));

        case NUL:
            return eq(eval(car(cdr(x)), alist), NULL);

        case FCONS:
            return cons(eval(car(cdr(x)),alist), eval(cdr(cdr(x)), alist));

        case FLIST:
            return _list(x);

        case COND:
            return evalcond(cdr(x), alist);

        case FSETQ:
            p = eval(cdr(cdr(x)), alist);
            rplacd(getvar(alist, getname(car(car(cdr(x))))), p);
            return p;

        case DEFUN:
            rplact(car(car(cdr(x))), FUSER);
            rplacd(car(car(cdr(x))), cdr(cdr(x)));
            var_to_user(cdr(cdr(cdr(x))));
            if (fd == FILE_SERIAL) {
                pc.printf("%s\n", getname(car(car(cdr(x)))));
            }
            return NULL;

        case FUSER:
            p = cdr(car(car(x)));   // p is statement list
            return eval(car(cdr(p)), pairargs(car(p), evalargs(cdr(x),alist), alist, FALSE));

        case FAPPLY:
        case FUNCALL:
            p = eval(car(cdr(x)), alist);   // func name
            if (isfunc(savt = type(car(p)))) {
                p = cons(p, cdr(cdr(x)));
                if (savt == FUSER) {
                    rplact(car(p), FUSER);
                }
                q = eval(p, alist);
                rplact(car(p), savt);
                return q;
            } else
                return NULL;

        case FEVAL:
            p = eval(cdr(x), alist);
            if (type(p) == SATOM) {
                return assoc(alist, getname(car(p)));
            }
            return eval(p, alist);

        case PRINT:
            lisp_print(eval(car(cdr(x)), alist));
            pc.printf("\n");
            return NULL;

        case FREAD:
            return makelist();

        case FAND:
            return _and(x);
        case FOR:
            return _or(x);
        case FNOT:
            return _not(x);

        case PLUS:
        case DIFF:
        case TIMES:
        case QUOTIENT:
        case GREATERP:
        case LESSP:
            return arith(car(x), eval(car(cdr(x)), alist), eval(cdr(cdr(x)), alist));

        case ADD1:
        case SUB1:
            return arith(car(x), eval(car(cdr(x)), alist), NULL);

        case ZEROP:
            p = eval(car(cdr(x)), alist);
            return  (p->u.num == 0) ? TRU : NULL;

        case NUMBERP:
            savt = type(eval(car(cdr(x)), alist));
            return (savt==IATOM || savt==RATOM) ? TRU : NULL;

        case PROG:
            return evalprog(x, alist);

        case GO:
            return cdr(car(car(cdr(x))));

        case RETRN:
            progon = FALSE;
            return eval(cdr(x), alist);

        case LST:
            if (cdr(x) == NULL) {
                return eval(car(x), alist);
            }
            return cons(eval(car(x),alist),eval(cdr(x),alist));

        case VARI:
            return assoc(alist, getname(car(car(x))));

        case IATOM:
        case RATOM:
            return car(x);



        // mbed expand
        case FINFO:
        {
pc.printf("\noblist --\n");
debug(g_oblist);

            return NULL;
        }
        case FFREEMEM:
        {
            LIST * p = memfreesize();
            if (p != NULL) {
                p->gcbit = GARBAGE;
            }
            return p;
        }
        case FWAIT:
        {
            LIST * p = mbed_wait(eval(car(cdr(x)), alist));
/*
            if (p != NULL) {
                p->gcbit = GARBAGE;
            }
*/
            return p;
        }
        case FDOUT:
            return mbed_digitalout(eval(car(cdr(x)), alist), eval(cdr(cdr(x)), alist));
        case FDIN:
            return mbed_digitalin(eval(car(cdr(x)), alist), eval(cdr(cdr(x)), alist));
        case FAOUT:
            return mbed_analogout(eval(car(cdr(x)), alist), eval(cdr(cdr(x)), alist));
        case FAIN:
            return mbed_analogin(eval(car(cdr(x)), alist));
        case PWMOUT:
            return mbed_pwmout(eval(car(cdr(x)), alist), eval(car(cdr(cdr(x))), alist), eval(cdr(cdr(cdr(x))), alist));
    }
    
    return NULL;
}


LIST *evalcond(LIST *expr, LIST *alist)
{
    if (expr == NULL) {
        return NULL;
    }

    if (eval(car(car(expr)), alist) != NULL) {      // expr was true
        return eval(car(cdr(car(expr))), alist);    // return result
    }

    return evalcond(cdr(expr), alist);        // eval rest of args
}


LIST *evalprog(LIST *p, LIST *alist)
{
    LIST *x = NULL;

    // set up parameters as locals
    alist = pairargs(car(cdr(p)), cons(NULL, NULL), alist, TRUE);
    progon = TRUE;
    p = cdr(cdr(p));     /* p now points to the statement list */
    find_labels(p);  /* set up all labels in the prog */

    while (p != NULL && progon) {
        x = eval(car(p), alist);
        if (type(car(car(p))) == GO) {
            p = x;       /* GO returned the next statement to go to */
        } else {
            p = cdr(p);  /* just follow regular chain of statements */

        }
    }

    progon = TRUE;   /* in case of nested progs */
    return x;
}

// pairargs - installs parameters in the alist, and sets the value to be the value of the corresponding argument.
LIST *pairargs(LIST *params, LIST *args, LIST *alist, int prog)
{
    if (params == NULL) {  // no more args to be evaluated
        return alist;
    }

    LIST *p = cons(NULL, car(args));  // value of param is corresponding arg
    p->u.pname = getname(car(car(params)));
    rplact(p, VARI);
    if (prog) {
        return cons(p, pairargs(cdr(params), cons(NULL,NULL), alist, prog));
    }

    return cons(p, pairargs(cdr(params), cdr(args),   alist, prog));
}

LIST *evalargs(LIST *arglist, LIST *alist)
{
    if (arglist == NULL) {
        return NULL;
    }

    return cons(eval(car(arglist),alist), evalargs(cdr(arglist), alist));
}

LIST *assoc( LIST *alist, char *name)
{
    return cdr(getvar(alist, name));
}

LIST *getvar(LIST *alist, char *name)
{
    return lookup(alist, name);
}

// arith - performs arithmetic on numeric items
LIST *arith(LIST *op, LIST *x, LIST *y)
{
    LIST *p;
    float res = 0;
    int t = type(op);

    if (t == LESSP) {
        return (x->u.num < y->u.num) ? TRU : NULL;
    }
    if (t == GREATERP) {
        return (x->u.num > y->u.num) ? TRU : NULL;
    }

    switch (t) {
        case PLUS:
            res = x->u.num + y->u.num;
            break;
        case DIFF:
            res = x->u.num - y->u.num;
            break;
        case TIMES:
            res = x->u.num * y->u.num;
            break;
        case QUOTIENT:
            res = x->u.num / y->u.num;
            break;
        case ADD1:
            res = x->u.num + 1;
            break;
        case SUB1:
            res = x->u.num - 1;
            break;
    }

    p = cons(NULL, NULL);

// @TODO: tがADD1かSUB1の場合、yは必ずNULLなので、 type(y)を実行するとエラーだと思うんだが...
/*
    if (    (type(x) == IATOM) &&
            (type(y) == IATOM) ||
                (t == ADD1) || (t == SUB1)  )
        ) {
*/
    if ((type(x) == IATOM) && ((t == ADD1) || (t == SUB1))) {
        p->u.num = (int)res;
        rplact(p, IATOM);
    } else {
        p->u.num = res;
        rplact(p, RATOM);
    }
    
    return p;
}

//----------------------------------------------------------------
// input functions
//----------------------------------------------------------------

// advance - skips white space in input file
int advance()
{
    int c;
#if 0
    while (((c = getc_mine(fd)) != EOF) && (strchr(" \t\n", c) != NULL));
#else
    while (1) {
        c = getc_mine(fd);
        if (c == EOF) {
            break;
        }
        if (strchr(" \t\n\r", c) == NULL) {
            break;
        }
    }
#endif
    ungetc_mine(c, fd);
//pc.printf("%c", c);

    return c;
}

LIST *lookup(LIST *head, char *name)
{
    LIST *p;

#if 0
    for (p = head; p != NULL && strcmp(name, getname(car(p))); p = cdr(p)) {
        ;
    }
#else
    p = head;
    while (p != NULL) {
        if (strcmp(name, getname(car(p))) == 0) {
            break;
        }
        p = cdr(p);
    }
#endif

    return ((p == NULL) ? NULL : car(p));
}

/**
 * nameをalistに加える
 *
 * @param char *name alistに加える名前
 * @param bool nameCopyFlag nameをコピーするか否か。 true=コピーする、1=コピーしない(nameがconstな文字列)
 */
LIST *install(char *name, bool nameCopyFlag = true)
{
    LIST *p = cons(NULL, NULL);

    if (nameCopyFlag) {
        p->u.pname = (char *)emalloc(strlen(name) + 1);
        strcpy(p->u.pname, name);
    } else {
        p->u.pname = name;
    }

    rplact(p, VARI);
    g_alist = cons(p, g_alist);

    return p;
}

LIST *getnum()
{
    LIST *p;
    float sum, n;
    int c;

    sum = 0.0;
    p = cons(NULL, NULL);
    rplact(p, IATOM);

    while (isdigit(c = getc_mine(fd))) {
        sum = sum * 10 + c - '0';
    }

    if (c == '.') {  /* the number is real */
        n = 10;
        rplact(p, RATOM);
        //while (isdigit(c = getc(fd))) {
        while (isdigit(c = getc_mine(fd))) {
                sum += (c - '0')/n;
            n *= 10;
        }
    }

    ungetc_mine(c, fd);
    p->u.num = sum;

    return p;
}

LIST *getid()
{
    LIST *p;

    char inbuf[120];    // トークン　１つ分のバッファ
    char *s = inbuf;
    LIST *idptr;

    // トークンを取得する
    {
        int c = getc_mine(fd);
        *s = c;
        s++;
        if (c != '\'') {
            while(1) {
                c = getc_mine(fd);
                if (!isalnum(c)) {
                    ungetc_mine(c, fd);
                    break;
                }
                *s = c;
                s++;
            }
        }
        *s = '\0';
    }

    if ((idptr = lookup(g_oblist, inbuf)) == NULL) {    // not a LISP function
        if ((idptr = lookup(g_alist, inbuf)) == NULL) { // id not declared yet
            idptr = install(inbuf, true);               // install it in g_alist (alist)
        }
    }
    p = cons(idptr, NULL);
    rplact(p, type(idptr));

    return p;
}

int gettok()
{
    int c;
 
    while ((c = advance()) == ';') {    // saw a comment
        while (1) {
            c = getc_mine(fd);
            if ((c == EOF) || (c == '\n')) { // EOF or CR
                break;
            }
        }
    }

    if (isalpha(c)) {
        return LETTER;
    }
    if (isdigit(c)) {
        return DIGIT;
    }
    switch (c) {
        case '(':
            return LPAREN;
        case ')':
            return RPAREN;
        case '\'':
            return INQUOTE;
        case EOF:
            return EOF;
    }
    
    return ERR;
}

//----------------------------------------------------------------
// LISP primitive functions
//----------------------------------------------------------------

// new - gets a new node from the free storage
LIST *new_malisp()
{
    LIST *p = (struct LIST *)emalloc(sizeof(LIST));
    p->gcbit = RUNNING;

    return p;
}

int type(LIST *p)
{
    return p->htype;
}

char* getname(LIST *p)
{
    return (p == NULL) ? NULL : p->u.pname;
}

// pのcar部をqに置き換える
void rplaca(LIST *p, LIST *q)
{
    p->left = q;
}

// pのcdr部をqに置き換える
void rplacd(LIST *p, LIST *q)
{
    p->right = q;
}

// pのタイプ(htype)をtに置き換える
void rplact(LIST *p, int t)
{
    p->htype = t;
}

LIST *car(LIST *p)
{
    return (p == NULL) ? NULL : p->left;
}

LIST *cdr(LIST *p)
{
    return (p == NULL) ? NULL : p->right;
}

LIST *cons(LIST *p, LIST *q)
{
    LIST *x = new_malisp();

    rplaca(x, p);
    rplacd(x, q);
    rplact(x, LST);

    return x;
}

LIST *eq(LIST *x, LIST *y)
{
    if (x == NULL || y == NULL) {
        if (x == y) {
            return TRU;
        }
    } else if ( (type(x) == SATOM) &&
                (type(y) == SATOM) &&
                (car(x) == car(y))  ) {
        return TRU;
    }

    return NULL;
}

LIST *atom(LIST *x)
{
#if 0
    int typ;

    if (x == NULL || (typ = type(x)) == IATOM || typ == RATOM || typ == SATOM) {
        return TRU;
    }
#else
    if (x == NULL) {
        return TRU;
    }
    
    int typ = type(x);
    if (typ == IATOM) {
         return TRU;
    }
    if (typ == RATOM) {
         return TRU;
    }
    if (typ == SATOM) {
         return TRU;
    }
#endif
    return NULL;
}

//----------------------------------------------------------------
// logical connectives - and, or, not

LIST *_and(LIST *x)
{
    LIST *p;
    for (p = cdr(x); p != NULL; p = cdr(p)) {
        if (eval(car(p), NULL) == NULL) {
            return NULL;
        }
    }

    return TRU;
}

LIST *_or(LIST *x)
{
    LIST *p;
    for (p = cdr(x); p != NULL; p = cdr(p)) {
        if (eval(car(p), NULL) != NULL) {
            return TRU;
        }
    }

    return NULL;
}

LIST *_not(LIST *x)
{
    return (eval(cdr(x), NULL) == NULL) ? TRU : NULL;
}

// other primitives

LIST *_list(LIST *x)
{
    LIST *res, *p;

    for (res = NULL, p = cdr(x);  p != NULL;  p = cdr(p)) {
        res = cons(res, car(p));
    }

    return res;
}


void var_to_user(LIST *p)
{
    if (p == NULL) {
        return;
    }

    if (type(p) == VARI) {
        if (type(car(p)) == FUSER) {
            rplact(p, FUSER);
        }
    } else if (type(p) == LST) {
        var_to_user(car(p));
        var_to_user(cdr(p));
    }
}

void var_to_atom(LIST *p)
{
    int t;

    if (p != NULL) {
        if (((t = type(p)) != LST && !isfunc(t)) || t == FUSER) {
            rplact(p, SATOM);
        } else {
            var_to_atom(car(p));   var_to_atom(cdr(p));
        }
    }
}

// find_labels - change the type of all labels in a PROG to LABL
void find_labels(LIST *p)
{
    for ( ;  p != NULL;  p = cdr(p)) {
        if (type(car(p)) == VARI) {
            rplact(car(p), LABL);           // change the type to LABL
            rplacd(car(car(p)), cdr(p));    // label points to next statement
        }
    }
}

//----------------------------------------------------------------
// garbage collection
//----------------------------------------------------------------

void work_garbageCollect(LIST *p)
{
    int cnt = 0;
    while (p != NULL) {

        int t = type(p);

        pc.printf("[%d] ", cnt);
        pc.printf("(%d) ", t);
        if ((t == IATOM) || (t == RATOM)) {
            pc.printf("[%f ] : ", p->u.num);
        } else if (t == SATOM) {
            pc.printf("[%s ] : ", p->u.pname);
        } else {
            pc.printf(" : ");
        }

        //pc.printf("%d : ", (p->gcbit >> 16) & 0xff);    // num
        //pc.printf("%d \n", (p->gcbit & 0xff));          // bit (USED/RUNNING)
        
        p = cdr(p);
        cnt++;
    }
}

//  marktree - recursively marks all used items in a list
void marktree(LIST *p)
{
    if (p != NULL) {
        if (type(p) == LST) {
            marktree(car(p));
            marktree(cdr(p));
        }
        p->gcbit = USED;
    }
}

/*********************** storage allocator *****************/

void *emalloc(size_t size)
{
    void *s;

    if ((s = malloc(size)) == NULL) {
        pc.printf("OUT OF MEMORY !! : crashed !! \n");
        exit(0);
    }

    return s;
}

// routine to load the library of lisp functions in
void load_library(void)
{
#if 0
    char libpath[1024];
    strcpy(libpath, getenv("HOME"));
    strcat(libpath, "/lisplib");
    
    if ((fd = fopen(libpath, "r")) !=  NULL) {
        interpret_malisp();
        fclose(fd);
        
        pc.printf("loaded lisplib from %s\n", libpath);
    }
    interpret_malisp();
    
    fd = stdin;
#else

    fd = FILE_STRING;
    interpret_malisp();
    pc.printf("loaded lisplib from flash\n");

    fd = FILE_SERIAL;

#endif
}

// isfunc - returns YES if type t is a user-function or a lisp primitive
int isfunc(int t)
{
    return
        (   t==FUSER || t==ADD1 || t==SUB1 || t==PLUS || t==DIFF || t==TIMES ||
            t==QUOTIENT || t==LESSP || t==GREATERP || t==ZEROP || t==NUMBERP ||
            t==FCAR || t==FCDR || t==FCONS || t==FREAD || t==PRINT || t==FNOT||
            t==FAND || t==FOR  || t==FEVAL || t==FEQ || t==FATOM ||

            // mbed extends
            t == FFREEMEM || t == FWAIT || t == FDOUT || t == FDIN ||
            t == FAOUT || t == FAIN || t == PWMOUT
        );
}

void debug(LIST *p)
{
    pc.printf("DEBUG ---\n");
    debug2(p);
    pc.printf("\n");
}

void debug2(LIST *p)
{
  int t;

  if (p != NULL) {
    if ((t = type(p)) == LST) {
        pc.printf("(");
        debug2(car(p));
        debug2(cdr(p));
        pc.printf(")");
    } else if (t == RATOM) {
        pc.printf("RATOM %f ", p->u.num);
    } else if (t == IATOM) {
        pc.printf("IATOM %d ", (int) p->u.num);
    } else if (t == SATOM) {
        pc.printf("SATOM %s ", getname(car(p)));
    } else {
        pc.printf("FUNC %d ", type(p));
    }
  }
}

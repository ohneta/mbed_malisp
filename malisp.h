/**
 *
 */

#ifndef MALISP_H
#define MALISP_H


#define FALSE   0
#define TRUE    1

// input types
#define INQUOTE 1
#define LPAREN  2
#define RPAREN  3
#define LETTER  4
#define DIGIT   5


// token types
#define IATOM   6       // 整数ATOM
#define RATOM   7       // 実数ATOM (float)
#define SATOM   8       // 文字列ATOM
#define FUNC    9       // 関数
#define LST     10      // リスト
#define VARI    11      // 変数
#define QUOTE   12      // クオート
#define NILL    13      // nil
#define T       14      // t
#define COND    15
#define DEFUN   16
#define FCAR    17
#define FCDR    18
#define FCONS   19
#define FEQ     20
#define FATOM   21
#define FQUOTE  22
#define FSETQ   23
#define FUSER   24
#define PLUS    25
#define DIFF    26
#define TIMES   27
#define QUOTIENT    28
#define ADD1    29
#define SUB1    30
#define ZEROP   31
#define NUMBERP 32
#define GREATERP    33
#define LESSP   34
#define PRINT   35
#define NUL     36
#define FUNCALL 37
#define PROG    38
#define GO      39
#define RETRN   40
#define LABL    41
#define FREAD   42
#define FREPLACA    43
#define FREPLACD    44
#define FEVAL   45
#define FAPPLY  46

/*
// for garbage collection
#define GARBAGE 47
#define USED    48
#define RUNNING 49
*/
#define GARBAGE 0
#define USED    1
#define RUNNING 2


// more primitives
#define FAND    50
#define FOR     51
#define FNOT    52
#define FLIST   53

#define ERR     -2

// mbed extpand functions
#define FINFO       55
#define FFREEMEM    59
#define FWAIT       60
#define FDOUT       61
#define FDIN        62
#define FAOUT       63
#define FAIN        64
#define PWMOUT      65


typedef struct LIST {

    uint32_t  gcbit;

    int32_t   htype;
    union {
        float num;
        char  *pname;
    } u;
    struct LIST  *left;
    struct LIST  *right;
} LIST;



#define     FILE_MINE   int32_t
#define     FILE_SERIAL     2
#define     FILE_STRING     3



// Prototypes
int getc_mine(FILE_MINE fd);
void ungetc_mine(int c, FILE_MINE fd);

size_t _getFreeMemorySize();
LIST *memfreesize();


void malisp_main();
void interpret_malisp();

void initialize();
LIST *init(char *name, int t);

LIST *makelist();
void lisp_print(LIST *p);
LIST *eval(LIST *x, LIST *alist);
LIST *evalcond(LIST *expr, LIST *alist);
LIST *evalprog(LIST *p, LIST *alist);
LIST *pairargs(LIST *params, LIST *args, LIST *alist, int prog);
LIST *evalargs(LIST *arglist, LIST *alist);
LIST *assoc( LIST *alist, char *name);
LIST *getvar(LIST *alist, char *name);
LIST *arith(LIST *op, LIST *x, LIST *y);
int advance();
LIST *lookup(LIST *head, char *name);
//LIST *install(char *name);
//LIST *install(char *name, int nameConstKind);
LIST *install(char *name, bool nameCopyFlag);

LIST *getnum();
LIST *getid();
int gettok();
LIST *new_malisp();
int type(LIST *p);
char* getname(LIST *p);
void rplaca(LIST *p, LIST *q);
void rplacd(LIST *p, LIST *q);
void rplact( LIST *p, int t);
LIST *car(LIST *p);
LIST *cdr(LIST *p);
LIST *cons(LIST *p, LIST *q);
LIST *eq(LIST *x, LIST *y);
LIST *atom(LIST *x);
LIST *_and(LIST *x);
LIST *_or(LIST *x);
LIST *_not(LIST *x);
LIST *_list(LIST *x);
void var_to_user(LIST *p);
void var_to_atom(LIST *p);
void find_labels(LIST *p);

void work_garbageCollect(LIST *);   // for work
void marktree(LIST *p);
void *emalloc(size_t size);

void load_library();
int isfunc(int t);
void debug(LIST *p);
void debug2(LIST *p);

#endif 

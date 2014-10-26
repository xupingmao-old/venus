
#ifndef lex_h
#define lex_h

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define is_name( c ) (c >= 'a' && c <='z') || ( c >= 'A' && c <='Z' ) || c == '_' || c == '$' || ( c >= '0' && c <= '9')
#define is_number( c ) ( c >= '0' && c <= '9')

#define LEX_NAME 300
#define LEX_NUMBER 301
#define LEX_STRING 302

#define LEX_EQ 400
#define LEX_GE 401
#define LEX_LE 402

typedef struct lex_state
{
	int cur;
	int next;
	int len;
	int state;
	int start; // string开始符号
	int strlen; // string 的长度，主要是会出现\0
	char *fname;
	FILE* fis; // file input stream

	int skipline; //  是否跳过换行符
	int lineno; // 行号
	int token_num; // 单词数量
	char* error;
	int type;
	char name[1024];
	double number;
}lex_state;

#endif

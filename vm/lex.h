
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
	int start; // string��ʼ����
	int strlen; // string �ĳ��ȣ���Ҫ�ǻ����\0
	char *fname;
	FILE* fis; // file input stream

	int skipline; //  �Ƿ��������з�
	int lineno; // �к�
	int token_num; // ��������
	char* error;
	int type;
	char name[1024];
	double number;
}lex_state;

#endif

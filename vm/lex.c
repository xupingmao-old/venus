
#include "lex.h"

void lex_load_file( lex_state* s)
{
	s->fis = fopen(s->fname, "rb");
	if( s->fis == NULL )
	{
		s->state = -1;
		s->error = "can not open file";
	}
}

lex_state* lex_init( )
{
	lex_state* s = malloc( sizeof( lex_state ) );
	s->cur = 0;
	s->error = NULL;
	s->len = 0;
	s->state = 0;
	s->next = 1;
	s->token_num = 0;
	s->lineno = 1;
	return s;
}

void lex_free( lex_state * s)
{
	if( s->fis != NULL ){
		fclose(s->fis);
	}

	free(s);
}


void next_char(lex_state* s)
{
	if( s->next != EOF){
		s->next = getc(s->fis);
	}
}

void skip_whitechars(lex_state* s)
{
	while( s->next == '\t' || s->next == ' ' || s->next == '\r')
	{
		next_char(s);
	}
}

int do_whitechar_and_line( lex_state* s)
{
	while( s->next == '\t' || s->next == ' ' || s->next == '\r' || s->next == '\n')
	{
		if( s->next == '\n')
		{
			s->lineno++;
			if( !s->skipline )
			{
				s->type = '\n';
				next_char(s);
				skip_whitechars(s);
				return 1;
			}
		}
		next_char(s);
	}
	return 0;
}


void next_name(lex_state* s)
{
	int p = 0;
	while( is_name( s->next ) ){
		s->name[p] = s->next;
		next_char(s);
		p++;
	}
	s->name[p] = '\0';
	s->type = LEX_NAME;
}

void next_hex_number( lex_state* s)
{
	int state = 1;
	switch( s->next )
	{
		case '0': case '1': case '2': case '3': case '4':
		case '5': case '6': case '7': case '8': case '9':
			s->number = ( (long) s->number << 4) + (s->next - '0');
			break;
		case 'a': case 'b': case 'c': case 'd': case 'e': case 'f':
			s->number = ( (long) s->number << 4) + (s->next - 'a' + 10);
			break;
	}

}

void next_number(lex_state* s)
{
	int p = 0;
	while( is_number( s->next) ){
		s->name[p] = s->next;
		next_char(s);
		p++;
	}

	if( p == 2 && s->name[0] == '0' && s->name[1] == 'x')
	{
		next_hex_number(s);
		return;
	}

	if( s->next == '.' )
	{
		s->name[p] = '.';
		p++;
		next_char(s);
		while( is_number(s->next) )
		{
			s->name[p] = s->next;
			next_char(s);
			p++;
		}
	}

	s->name[p] = '\0';
	s->number = atof(s->name);
	s->type = LEX_NUMBER;
}


void next_string(lex_state* s)
{
	next_char(s);
	int p = 0;
	s->strlen = 0;
	while ( s->next != EOF && s->next != s->start)
	{
		if( s->next == '\\')
		{
			next_char(s);
			switch( s->next )
			{
			case '0':s->next = '\0';break;
			case 'b':s->next = '\b';break;
			case 'n':s->next = '\n';break;
			case 'r':s->next = '\r';break;
			case 't':s->next = '\t';break;
			case '"':s->next = '"';break;
			case '\'':s->next = '\'';break;
			}
		}
		s->name[p] = s->next;
		p++;
		s->strlen ++;
		next_char(s);
	}
	s->name[p] = '\0';
	s->type = LEX_STRING;

	if( s->next == s->start )
	{
		next_char(s);
	}
}

void next_symbol( lex_state* s)
{
	s->type = s->next;
	next_char(s);
}

void next_symbol2(lex_state* s)
{
	int cur = s->next;

	next_char(s);

	if( s->next == '=' ){
		switch( cur )
		{
		case '=': s->type = LEX_EQ;break;
		case '>': s->type = LEX_GE;break;
		case '<': s->type = LEX_LE;break;
		}
		next_char(s);
	}else {
		s->type = cur;
	}
}


void lex_next(lex_state* s)
{
	if( s->next == EOF ) return;
	s->token_num ++;
	if( do_whitechar_and_line(s) )
	{
		return;
	}

	switch(s->next){
	case 'a': case 'b': case 'c': case 'd': case 'e':
	case 'f': case 'g':	case 'h': case 'i':	case 'j':
	case 'k': case 'l': case 'm': case 'n': case 'o':
	case 'p': case 'q': case 'r': case 's': case 't':
	case 'u': case 'v': case 'w': case 'x': case 'y':
	case 'z':
	case 'A': case 'B': case 'C': case 'D': case 'E':
	case 'F': case 'G':	case 'H': case 'I':	case 'J':
	case 'K': case 'L': case 'M': case 'N': case 'O':
	case 'P': case 'Q': case 'R': case 'S': case 'T':
	case 'U': case 'V': case 'W': case 'X': case 'Y':
	case 'Z':
		next_name(s);
		break;
	case '0': case '1': case '2': case '3': case '4':
	case '5': case '6': case '7': case '8': case '9':
		next_number(s);
		break;
	case '"': case '\'':
		s->start = s->next;
		next_string(s);
		break;
	case '+': case '-': case '*': case '/': case '%':
	case ',': case ';': case ':':
	case '(': case ')': case '[': case ']': case '{': case '}':
		next_symbol(s);
		break;
	case '=': case '>': case '<':
		next_symbol2(s);
		break;
	default:
		next_char(s);
		break;

	}
}

void lex_print( lex_state * s )
{
	switch( s->type )
	{
	case LEX_STRING:
		printf("string:%s ", s->name);
		break;
	case LEX_NAME:
		printf("name:%s ", s->name);
		break;
	case LEX_NUMBER:
		printf("number:%g ", s->number);
		break;
	case ',': case ':': case ';':
	case '+': case '-': case '*': case '/': case '%':
	case '>': case '<': case '=':
	case '(': case ')': case '[': case ']': case '{': case '}':
		printf("symbol:\'%c\' ", s->type);
		break;
	case LEX_EQ:
		printf("symbol:\'==\' ");
		break;
	case LEX_GE:
		printf("symbol:\'>=\' ");
		break;
	case LEX_LE:
		printf("symbol:\'<=\' ");
		break;
	case '\n':
		printf("line ");
		break;
	default:
		printf("unknown ");
	}
	puts("");
}

void lex_start(lex_state* s)
{
	next_char(s);
}

void lex_info( lex_state* s)
{
	printf("tokens number = %d\n", s->token_num);
	printf("lines number = %d\n", s->lineno);
}

int main()
{
	lex_state* s = lex_init();
	s->fname = "test.txt";
	lex_load_file(s);
	lex_start(s);
	s->skipline = 1;
	while ( s->next != EOF ){
		lex_next(s);
		lex_print(s);
		Sleep(500);
	}
	lex_info(s);
	lex_free(s);
	return 0;
}

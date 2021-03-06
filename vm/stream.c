
#include "tm.h"

tm_obj stream_new(FILE* fp){
	tm_stream* st = tm_alloc(sizeof(tm_stream));
	st->fp = fp;
	st->name = obj_none;
	tm_obj o;
	o.type = TM_STREAM;
	o.value.stream = st;
	return gc_track(o);
}

void stream_free(tm_stream* st){
	if( st->fp != NULL ){
		fclose(st->fp);
	}
	tm_free( st, sizeof(st));
}

tm_obj stream_open(tm_obj params){
	int len = list_len( params );
	tm_obj fname, mode;
	char* fname_s, *mode_s;
	if( len == 1){
		fname = get_arg(params, 0, TM_STR);
		fname_s = get_str( fname );
		mode_s = "rb";
	}
	else if ( len == 2){
		fname = get_arg( params, 0, TM_STR);
		mode = get_arg( params, 1, TM_STR);
		fname_s = get_str( fname );
		mode_s = get_str( mode );
	}
	FILE *fp = fopen(fname_s, mode_s);
	if( fp == NULL ){
		tm_raise("stream_open: can not open stream");
	}
	return stream_new( fp);
}

tm_obj stream_close(tm_obj params){
	tm_obj fp_ = get_arg( params, 0, TM_STREAM);
	FILE* fp = get_stream(fp_);
	if( fp == NULL ){
		tm_raise("stream_close: file is not open");
	}
	fclose(fp);
	return obj_none;
}

long _get_file_rest_len( FILE* fp){
	if( fp == NULL ){
		tm_raise("stream_read: can not open stream");
	}
    long cur = ftell(fp);
	fseek(fp, 0, SEEK_END);
    long file_size = ftell(fp) - cur;
    fseek(fp, cur, SEEK_SET);
    if( file_size > MAX_FILE_SIZE ){
    	tm_raise("stream_read: file too large");
    }
    return file_size;
}

tm_obj stream_read( tm_vm* tm, tm_obj params){
	int len = list_len( params );
	tm_obj fp_ = get_arg( params, 0, TM_STREAM);
	int rest_len = 0;
	FILE* fp = get_file(fp_);
	if( len == 1){
		rest_len = _get_file_rest_len( fp );
		fseek(fp, 0, SEEK_END);
	}else if( len == 2){
		tm_obj size = get_arg( params, 1, TM_NUM);
		int v = (int) get_num( size );
		rest_len = _get_file_rest_len( fp );
		if( rest_len > v ){
			rest_len = v;
		}
	}else {
		tm_raise("stream_read: too many arguments");
	}
	tm_obj des = str_new(NULL, rest_len);
	char* s = get_str(des);
	fread(s, rest_len, 1, fp );
	return des;
}

tm_obj _load(char* fname){
	FILE* fp = fopen(fname, "rb");
	if( fp == NULL ){
		tm_raise("load: can not open file \"@\"", str_new(fname, strlen(fname)));
		return obj_none;
	}
	long len = _get_file_rest_len(fp);
	if(len > MAX_FILE_SIZE ){
		tm_raise("load: file too big to load, size = @", number_new(len));
		return obj_none;
	}
	tm_obj text = str_new(NULL, len);
	char* s = get_str(text);
	fread(s, len, 1, fp);
	fclose(fp);
	return text;
}


tm_obj tm_load( tm_obj p){
	tm_obj fname = get_arg( p, 0, TM_STR);
	return _load( get_str(fname) );
}

tm_obj _tm_save(char*fname, tm_obj content){
	FILE* fp = fopen(fname, "wb");
	if( fp == NULL ){
		tm_raise("tm_save : can not save to file @" , fname );
	}
	char* txt = get_str( content );
	int len = get_str_len( content );
	fwrite(txt, len, 1, fp);
    fclose(fp);
}

tm_obj tm_save( tm_obj p){
	tm_obj fname = get_arg(p, 0, TM_STR);
	return _tm_save(get_str(fname), get_arg(p,1, TM_STR));
}

tm_obj tm_exists(tm_obj p){
    tm_obj _fname = get_arg(p, 0, TM_STR);
    char* fname = get_str(_fname);
    FILE*fp = fopen(fname, "rb");
    if( fp == NULL ) return obj_false;
    fclose(fp);
    return obj_true;
}

tm_obj tm_mtime(tm_obj p){
    char const *s = get_str_arg(p, 0);
    struct stat stbuf;
    if (!stat(s,&stbuf)) { return number_new(stbuf.st_mtime); }
    tm_raise("tm_mtime(%s)",s);
}

#include "tm.h"

tm_obj stream_new(tm_vm* tm, FILE* fp){
	tm_stream* st = tm_alloc(tm, sizeof(tm_stream));
	st->fp = fp;
	st->name = tm->none;
	tm_obj o;
	o.type = TM_STREAM;
	o.value.stream = st;
	return gc_track(tm, o);
}

void stream_free(tm_vm* tm, tm_stream* st){
	if( st->fp != NULL ){
		fclose(st->fp);
	}
	free( st );
}

tm_obj stream_open(tm_vm* tm, tm_obj params){
	int len = list_len( params );
	tm_obj fname, mode;
	char* fname_s, *mode_s;
	if( len == 1){
		fname = get_arg(tm, params, 0, TM_STR);
		fname_s = get_str( fname );
		mode_s = "rb";
	}
	else if ( len == 2){
		fname = get_arg(tm, params, 0, TM_STR);
		mode = get_arg(tm, params, 1, TM_STR);
		fname_s = get_str( fname );
		mode_s = get_str( mode );
	}
	FILE *fp = fopen(fname_s, mode_s);
	if( fp == NULL ){
		tm->error = string_new(tm, "stream_open: can not open stream");
		tm_raise(tm);
	}
	return stream_new(tm, fp);
}

tm_obj stream_close(tm_vm* tm, tm_obj params){
	tm_obj fp_ = get_arg(tm, params, 0, TM_STREAM);
	FILE* fp = get_stream(fp_);
	if( fp == NULL ){
		_tm_raise("stream_close: file is not open");
	}
	fclose(fp);
	return tm->none;
}

int _get_file_rest_len(tm_vm* tm, FILE* fp){
	if( fp == NULL ){
		tm->error = string_new(tm, "stream_read: can not open stream");
		tm_raise( tm);
	}
    long cur = ftell(fp);
	fseek(fp, 0, SEEK_END);
    long file_size = ftell(fp) - cur;
    fseek(fp, cur, SEEK_SET);
    if( file_size > MAX_FILE_SIZE ){
    	_tm_raise("stream_read: file too large");
    }
    return file_size;
}

tm_obj stream_read( tm_vm* tm, tm_obj params){
	int len = list_len( params );
	tm_obj fp_ = get_arg(tm, params, 0, TM_STREAM);
	int rest_len = 0;
	FILE* fp = get_file(fp_);
	if( len == 1){
		rest_len = _get_file_rest_len( tm, fp );
		fseek(fp, 0, SEEK_END);
	}else if( len == 2){
		tm_obj size = get_arg(tm, params, 1, TM_NUM);
		int v = (int) get_num( size );
		rest_len = _get_file_rest_len( tm, fp );
		if( rest_len > v ){
			rest_len = v;
		}
	}else {
		_tm_raise("stream_read: too many arguments");
	}
	char* s = tm_alloc( tm, rest_len + 1);
	fread(s, rest_len, 1, fp );
	return string_new_(tm, s, rest_len);
}


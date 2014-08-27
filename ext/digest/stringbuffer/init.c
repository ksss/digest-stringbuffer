#include "ruby.h"

/*
 * digest module from https://github.com/ruby/ruby/blob/trunk/ext/digest/digest.c
 * Copyright (C) 1995-2001 Yukihiro Matsumoto
 * Copyright (C) 2001-2006 Akinori MUSHA
 */

#define BUFFER_INIT 64

typedef struct {
	char* buffer;
	char* p;
	size_t memsize;
} buffer_t;

static void
buffer_init(buffer_t* ptr)
{
	ptr->buffer = (char*) xmalloc(sizeof(char) * BUFFER_INIT);
	ptr->p = ptr->buffer;
	ptr->memsize = BUFFER_INIT;
}

static void
buffer_realloc(buffer_t* ptr, size_t size)
{
	char* buffer;

	buffer = (char*) xrealloc(ptr->buffer, size);
	if (buffer == NULL)
		rb_raise(rb_eNoMemError, "failed to realloc");

	ptr->buffer = buffer;
}

static void
buffer_free(buffer_t* ptr)
{
	if (ptr->buffer != NULL)
		xfree(ptr->buffer);
	if (ptr != NULL)
		xfree(ptr);
}

static size_t
buffer_buffer_length(buffer_t* ptr)
{
	return ptr->p - ptr->buffer;
}

static VALUE
buffer_alloc(VALUE self)
{
	buffer_t* ptr = ALLOC(buffer_t);
	buffer_init(ptr);
	return Data_Wrap_Struct(self, 0, buffer_free, ptr);
}

static VALUE
hexencode_str_new(VALUE str_digest)
{
	char *digest;
	size_t digest_len;
	size_t i;
	VALUE str;
	char *p;
	static const char hex[] = {
		'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
		'a', 'b', 'c', 'd', 'e', 'f'
	};

	StringValue(str_digest);
	digest = RSTRING_PTR(str_digest);
	digest_len = RSTRING_LEN(str_digest);

	if (LONG_MAX / 2 < digest_len) {
		rb_raise(rb_eRuntimeError, "digest string too long");
	}

	str = rb_usascii_str_new(0, digest_len * 2);

	for (i = 0, p = RSTRING_PTR(str); i < digest_len; i++) {
		unsigned char byte = digest[i];

		p[i + i]     = hex[byte >> 4];
		p[i + i + 1] = hex[byte & 0x0f];
	}

	return str;
}

static VALUE
buffer_initialize_copy(VALUE copy, VALUE origin)
{
	buffer_t *ptr_copy, *ptr_origin;
	size_t buffer_len;

	if (copy == origin) return copy;

	rb_check_frozen(copy);

	Data_Get_Struct(copy, buffer_t, ptr_copy);
	Data_Get_Struct(origin, buffer_t, ptr_origin);

	buffer_len = buffer_buffer_length(ptr_origin);

	if (ptr_copy->memsize < ptr_origin->memsize) {
		buffer_realloc(ptr_copy, sizeof(char) * ptr_origin->memsize);
		ptr_copy->memsize = ptr_origin->memsize;
	}

	memcpy(ptr_copy->buffer, ptr_origin->buffer, buffer_len);
	ptr_copy->p = ptr_copy->buffer + buffer_len;

	return copy;
}

static VALUE
buffer_reset(VALUE self)
{
	buffer_t *ptr;
	Data_Get_Struct(self, buffer_t, ptr);
	ptr->p = ptr->buffer;
	return self;
}

static VALUE
buffer_update(VALUE self, VALUE str)
{
	size_t buffer_len, str_len, require, newsize;
	const char* str_p;
	buffer_t *ptr;
	Data_Get_Struct(self, buffer_t, ptr);

	StringValue(str);
	str_p = RSTRING_PTR(str);
	str_len = RSTRING_LEN(str);
	buffer_len = buffer_buffer_length(ptr);
	require = buffer_len + str_len;
	if (ptr->memsize < require) {
		newsize = ptr->memsize;
		while (newsize < require) {
			newsize *= 2;
		}
		buffer_realloc(ptr, sizeof(char) * newsize);
		ptr->p = ptr->buffer + buffer_len;
		ptr->memsize = newsize;
	}
	memcpy(ptr->p, str_p, str_len);
	ptr->p += str_len;

	return self;
}

static VALUE
buffer_finish(VALUE self)
{
	rb_raise(rb_eRuntimeError, "%s does not implement finish()",
			rb_obj_classname(self));
}

static VALUE
buffer_digest(int argc, VALUE *argv, VALUE self)
{
	VALUE str, value;

	if (0 < rb_scan_args(argc, argv, "01", &str)) {
		buffer_reset(self);
		buffer_update(self, str);
		value = rb_funcall(self, rb_intern("finish"), 0);
		buffer_reset(self);
	} else {
		value = rb_funcall(self, rb_intern("finish"), 0);
	}

	return value;
}

static VALUE
buffer_digest_bang(VALUE self)
{
	VALUE value = rb_funcall(self, rb_intern("finish"), 0);
	buffer_reset(self);

	return value;
}

static VALUE
buffer_hexdigest(int argc, VALUE *argv, VALUE self)
{
	return hexencode_str_new(buffer_digest(argc, argv, self));
}

static VALUE
buffer_hexdigest_bang(VALUE self)
{
	return hexencode_str_new(buffer_digest_bang(self));
}

static VALUE
buffer_to_s(VALUE self)
{
	return hexencode_str_new(buffer_digest(0, 0, self));
}

static VALUE
buffer_digest_length(VALUE self)
{
	/* subclasses really should redefine this method */
	VALUE digest = buffer_digest(0, 0, self);

	/* never blindly assume that #digest() returns a string */
	StringValue(digest);
	return UINT2NUM(RSTRING_LEN(digest));
}

static VALUE
buffer_equal(VALUE self, VALUE other)
{
	VALUE str1, str2;
	str1 = buffer_digest(0, 0, self);
	str2 = buffer_digest(0, 0, other);

	StringValue(str1);
	StringValue(str2);

	if (RSTRING_LEN(str1) == RSTRING_LEN(str2) &&
		rb_str_cmp(str1, str2) == 0) {
		return Qtrue;
	} else {
		return Qfalse;
	}
}

static VALUE
buffer_inspect(VALUE self)
{
	VALUE str;
	size_t digest_len = 32;        /* about this size at least */
	const char *cname;

	cname = rb_obj_classname(self);

	/* #<Digest::ClassName: xxxxx...xxxx> */
	str = rb_str_buf_new(2 + strlen(cname) + 2 + digest_len * 2 + 1);
	rb_str_buf_cat2(str, "#<");
	rb_str_buf_cat2(str, cname);
	rb_str_buf_cat2(str, ": ");
	rb_str_buf_append(str, buffer_hexdigest(0, 0, self));
	rb_str_buf_cat2(str, ">");
	return str;
}

static VALUE
buffer_get(VALUE self)
{
	buffer_t *ptr;
	Data_Get_Struct(self, buffer_t, ptr);
	return rb_str_new(ptr->buffer, buffer_buffer_length(ptr));
}

static VALUE
buffer_s_digest(int argc, VALUE *argv, VALUE klass)
{
	VALUE str;
	volatile VALUE obj;

	if (argc < 1) {
		rb_raise(rb_eArgError, "no data given");
	}

	str = *argv++;
	argc--;

	StringValue(str);

	obj = rb_obj_alloc(klass);
	rb_obj_call_init(obj, argc, argv);

	return buffer_digest(1, &str, obj);
}

static VALUE
buffer_s_hexdigest(int argc, VALUE *argv, VALUE klass)
{
	VALUE str;
	volatile VALUE obj;

	if (argc < 1) {
		rb_raise(rb_eArgError, "no data given");
	}

	str = *argv++;
	argc--;

	StringValue(str);

	obj = rb_obj_alloc(klass);
	rb_obj_call_init(obj, argc, argv);

	return hexencode_str_new(buffer_digest(1, &str, obj));
}

void
Init_stringbuffer(void)
{
	VALUE mDigest, cDigest_Class, cDigest_StringBuffer;

	/* Digest::Buffer is require that Digest module and Digest::Class class of CRuby built-in */
	rb_require("digest");
	mDigest = rb_path2class("Digest");

	/* class Digest::Buffer < Digest::Class */
	cDigest_StringBuffer = rb_define_class_under(mDigest, "StringBuffer", rb_cObject);

	rb_define_alloc_func(cDigest_StringBuffer, buffer_alloc);

	rb_define_singleton_method(cDigest_StringBuffer, "digest", buffer_s_digest, -1);
	rb_define_singleton_method(cDigest_StringBuffer, "hexdigest", buffer_s_hexdigest, -1);

	rb_define_method(cDigest_StringBuffer, "update", buffer_update, 1);
	rb_define_alias(cDigest_StringBuffer, "<<", "update");
	rb_define_private_method(cDigest_StringBuffer, "finish", buffer_finish, 0);
	rb_define_method(cDigest_StringBuffer, "reset", buffer_reset, 0);
	rb_define_method(cDigest_StringBuffer, "digest_length", buffer_digest_length, 0);
	rb_define_alias(cDigest_StringBuffer, "length", "digest_length");
	rb_define_alias(cDigest_StringBuffer, "size", "digest_length");
	rb_define_method(cDigest_StringBuffer, "==", buffer_equal, 1);
	rb_define_method(cDigest_StringBuffer, "initialize_copy", buffer_initialize_copy, 1);

	rb_define_method(cDigest_StringBuffer, "digest", buffer_digest, -1);
	rb_define_method(cDigest_StringBuffer, "digest!", buffer_digest_bang, 0);
	rb_define_method(cDigest_StringBuffer, "hexdigest", buffer_hexdigest, -1);
	rb_define_method(cDigest_StringBuffer, "hexdigest!", buffer_hexdigest_bang, 0);
	rb_define_method(cDigest_StringBuffer, "to_s", buffer_to_s, 0);

	rb_define_method(cDigest_StringBuffer, "buffer", buffer_get, 0);
}

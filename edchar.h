#ifndef EDCHAR_H
#define EDCHAR_H

#include <stdlib.h>

#define CE_INIT_SIZE 1024
#define CE_MAX_INT_LEN 99
#define CE_MAX_STR_LEN 1024

#define CE_PROMPT_S '$'
#define CE_PROMPT_C '>'
#define CE_PROMPT_I '#'

#define CE_SLEEP_TIME 250000

typedef struct {
    char *buf_raw;
    size_t len;
    size_t alloc_len;    
    char *filename;
} ce_buf_t;

typedef enum {
    CE_OKAY,
    CE_NOFILE,
    CE_NOMEM,
    CE_OOB,
    CE_INVALID_INPUT,
} ce_status_t;

//buffer utilities
ce_status_t ce_buf_init(ce_buf_t *buf);
ce_status_t ce_file_load(const char *filename, ce_buf_t *buf);
ce_status_t ce_file_save_as(const char *filename, ce_buf_t *buf);
ce_status_t ce_file_save(ce_buf_t *buf);
ce_status_t ce_put(const size_t idx, const char c, ce_buf_t *buf);
ce_status_t ce_insert(const size_t idx, const char c, ce_buf_t *buf);
ce_status_t ce_delete(const size_t idx, ce_buf_t *buf);
ce_status_t ce_get(const size_t idx, char *c, const ce_buf_t *buf);
ce_status_t ce_print_to_screen(const size_t start, const size_t stop, 
    const ce_buf_t *buf);
ce_status_t ce_print_to_screen_all(const ce_buf_t *buf);
ce_status_t ce_buf_destroy(ce_buf_t *buf);

//ui
void ce_print(const char *str, size_t max_len);
void ce_printf(const char *fmt, ...);
ce_status_t ce_getc(char *c);
ce_status_t ce_geti(unsigned int *i, const char first);
ce_status_t ce_gets(char *s, size_t max_len);

//debug
void ce_dprintf(int row, const char *fmt, ...);

#endif

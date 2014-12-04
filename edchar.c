#include <stdio.h>
#include <string.h>
#include <ncurses.h>
#include "edchar.h"


ce_status_t ce_buf_init(ce_buf_t *buf) {
    buf->buf_raw = (char *)malloc(CE_INIT_SIZE * sizeof(*buf->buf_raw));
    buf->buf_raw[0] = '\0';
    if (!buf->buf_raw) {
        return CE_NOMEM;
    }
    buf->len = 0;
    buf->alloc_len = CE_INIT_SIZE;
    buf->filename = NULL;
    return CE_OKAY;    
}

ce_status_t ce_put(const size_t idx, const char c, ce_buf_t *buf) {
    if (idx >= buf->alloc_len - 1) {
        char *new_buf;
        if (!(new_buf = (char *)malloc(buf->alloc_len * 2 
            * sizeof(*buf->buf_raw)))) {
            return CE_NOMEM;
        }
        memcpy(new_buf, buf->buf_raw, buf->alloc_len * sizeof(*buf->buf_raw)); 
        free(buf->buf_raw);
        buf->buf_raw = new_buf;
        buf->alloc_len *= 2;
    }
    buf->buf_raw[idx] = c;
    buf->len = idx+1 > buf->len ? idx+1 : buf->len;
    buf->buf_raw[buf->len] = '\0';
    return CE_OKAY;
}

ce_status_t ce_put_end(const char c, ce_buf_t *buf) {
    return ce_put(buf->len, c, buf);
}

ce_status_t ce_insert(const size_t idx, const char c, ce_buf_t *buf) {
    int i;
    char c_tmp;
    if (buf->len > 0) {
        for (i = (buf->len)-1; i >= (int)idx; --i) {
            ce_get(i, &c_tmp, buf);
            ce_put(i+1, c_tmp, buf);
        }
    }
    ce_put(idx, c, buf);
    return CE_OKAY;
}

ce_status_t ce_delete(const size_t idx, ce_buf_t *buf) {
    size_t i;
    char c_tmp;
    for (i = idx; i < buf->len-1; ++i) {
        ce_get(i+1, &c_tmp, buf);
        ce_put(i, c_tmp, buf);
    }
    --buf->len;
    buf->buf_raw[buf->len] = '\0';
    return CE_OKAY;
}

ce_status_t ce_get(const size_t idx, char *c, const ce_buf_t *buf) {
    if (idx >= buf->len) {
        return CE_OOB;
    } 
    *c = buf->buf_raw[idx];
    return CE_OKAY;
}

ce_status_t ce_file_load(const char *filename, ce_buf_t *buf) { 
    ce_buf_destroy(buf);
    ce_buf_init(buf);
    FILE *fin; 
    if (!(fin = fopen(filename, "r"))) {
        return CE_NOFILE;
    }
    int c;
    while ((c = fgetc(fin)) != EOF) {
        ce_put_end((char)c, buf);
    }
    fclose(fin);
    free(buf->filename);
    if (!(buf->filename = strdup(filename))) {
        return CE_NOMEM;
    }
    return CE_OKAY;
}

ce_status_t ce_file_save_as(const char *filename, ce_buf_t *buf) {
    FILE *fout;
    if (!(fout = fopen(filename, "w"))) {
        return CE_NOFILE;
    }
    buf->buf_raw[buf->len] = '\0';
    fprintf(fout, "%s", buf->buf_raw);
    fclose(fout);
    free(buf->filename);
    if (!(buf->filename = strdup(filename))) {
        return CE_NOMEM;
    }
    return CE_OKAY;
}

ce_status_t ce_file_save(ce_buf_t *buf) {
    return ce_file_save_as(buf->filename, buf);
}

ce_status_t ce_print_to_screen(const size_t start, const size_t stop, 
    const ce_buf_t *buf) {
    char c = buf->buf_raw[stop+1];
    buf->buf_raw[stop+1] = '\0';
    ce_printf("%s", buf->buf_raw + start);
    buf->buf_raw[stop+1] = c;
    return CE_OKAY;
}

ce_status_t ce_print_to_screen_all(const ce_buf_t *buf) {
    if (buf->len > 0) {
        ce_printf("%s", buf->buf_raw);
    } 
    return CE_OKAY;
}

ce_status_t ce_buf_destroy(ce_buf_t *buf) {
    free(buf->filename);
    free(buf->buf_raw);
    return CE_OKAY;
}

void ce_printf(const char *fmt, ...) {
    char buf[CE_MAX_STR_LEN];
    va_list varargs;
    va_start(varargs, fmt);
    vsnprintf(buf, CE_MAX_STR_LEN, fmt, varargs);
    va_end(varargs);
    char c;
    int i;
    for (i = 0; i < CE_MAX_STR_LEN; ++i) {
        c = buf[i];
        if (c == 0) {
            break;
        }
        echochar(c);
        move(0, 0);
        getch();
        move(0, 0);
    }
}

// handles backslash escapes
ce_status_t __ce_getch(char *cout) {
    int c = getch();
    ce_dprintf(2, "%d", c);
    if (c == '\\') {
        move(0,0);
        c = getch();
        switch (c) {
        case 'n' :
            *cout = '\n';
            break;
        case 't' :
            *cout = '\t';
            break;
        default :
            *cout = 0;
            return CE_INVALID_INPUT;
        }
        return CE_OKAY;
    }
    if (c > 255 || c < 0) {
        *cout = 0;
        return CE_INVALID_INPUT;
    }
    *cout = (char)c;
    return CE_OKAY;
}

ce_status_t ce_getc(char *cout) {
    echochar(CE_PROMPT_C);
    move(0, 0);
    ce_status_t s = __ce_getch(cout);
    move(0, 0);
    return s;
}

ce_status_t __ce_gets(char *s, size_t max_len, char prompt) {
    int i;
    char c;
    ce_status_t status = CE_OKAY;
    echochar(prompt);
    move(0, 0);
    for (i = 0; i < max_len-1; ++i) {
        if (status = __ce_getch(&c) != CE_OKAY) {
            move(0, 0);
            break;
        }
        move(0, 0);
        if (c == '\n') {
            break;
        }
        echochar(c); 
        s[i] = c;
        move(0, 0);
    }
    s[i] = '\0';
    return status;
}

ce_status_t ce_geti(unsigned int *iout, const char first) {
    char buf[CE_MAX_INT_LEN];
    ce_status_t status = __ce_gets(buf, CE_MAX_INT_LEN, CE_PROMPT_I);
    *iout = (unsigned int)atoi(buf); //TODO handle invalid int
    return status;
}

ce_status_t ce_gets(char *s, size_t max_len) {
    return __ce_gets(s, max_len, CE_PROMPT_S);
}


void ce_dprintf(int row, const char *fmt, ...) {
    #ifdef DEBUG
    va_list varargs;
    move(row,0);
    clrtoeol();
    va_start(varargs, fmt);
    vwprintw(stdscr, fmt, varargs);
    va_end(varargs);
    refresh();
    move(0,0);
    #endif
}

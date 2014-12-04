#include "edchar.h"
#include <stdio.h>
#include <string.h>
#include <ncurses.h>

ce_status_t edit(char *fn, ce_buf_t *buf) {
    if (strlen(fn) > CE_MAX_STR_LEN) {
        //load failed, file name too long
        ce_printf("!01!"); 
    } else if (ce_file_load(fn, buf) != CE_OKAY) {
        buf->filename = strdup(fn);
        ce_printf("K");
    } else {
        ce_printf("K");
    }
}

ce_status_t main_loop(ce_buf_t *buf) {
    char command;
    char char_in;
    unsigned int cursor, str_idx, buf_idx, buf_len;
    int idx_off = 0;
    char s_buf[CE_MAX_STR_LEN];
    int looping = 1;
    cursor = 0;
    while (looping) {
        ce_getc(&command);
        switch (command) {
        case 'e' : // edit
            ce_gets(s_buf, CE_MAX_STR_LEN);
            edit(s_buf, buf);
            break;
        case 'f' : // current file name
            if (buf->filename) {
                ce_printf("%s", buf->filename);
            } else {
                ce_printf("?");
            }
            break;
        case 'w' : // write
            if (ce_file_save(buf) == CE_OKAY) {
                ce_printf("K");
                break;
            }
        case 'W' : // write as
            ce_printf("?");
            ce_gets(s_buf, CE_MAX_STR_LEN);
            ce_file_save_as(s_buf, buf);
            ce_printf("K");
            break;
        case 'q' : // quit
            looping = 0;
            break;
        case '0' :
        case '1' :
        case '2' :
        case '3' :
        case '4' :
        case '5' :
        case '6' :
        case '7' :
        case '8' :
        case '9' : // change cursor
            ce_geti(&cursor, command);
            break;
        case 'g' : // get 1
            ce_print_to_screen(cursor, cursor, buf);
            break;
        case 'G' : // multi-char get 
            ce_geti(&buf_len, command);
            ce_print_to_screen(cursor, cursor+buf_len, buf);
            break;
        case 'p' : // print dump
            ce_print_to_screen_all(buf);
            break;
        case 'r' : // replace
            ce_getc(&char_in);
            ce_put(cursor, char_in, buf);
            break;
        case 'R' : // multi-char replace
            ce_gets(s_buf, CE_MAX_STR_LEN);
            buf_idx = cursor;
            str_idx = 0;
            while ((char_in = s_buf[str_idx]) != '\0') {
                ce_put(buf_idx, char_in, buf);
                ++buf_idx;
                ++str_idx;
            }
            cursor = buf_idx;
            break;
        case 'a' : // append
            if (cursor < buf->len) {
                idx_off = 1;
            }
        case 'i' : // insert
            ce_getc(&char_in);
            ce_insert(cursor + idx_off, char_in, buf);
            cursor += 1 + idx_off;
            idx_off = 0;
            break;
        case 'A' : // append a whole string
            if (cursor < buf->len) {
                idx_off = 1;
            }
        case 'I' : // insert a whole string
            ce_gets(s_buf, CE_MAX_STR_LEN);
            buf_idx = cursor + idx_off;
            str_idx = 0;
            while ((char_in = s_buf[str_idx]) != '\0') {
                ce_insert(buf_idx, char_in, buf);
                ++str_idx;
                ++buf_idx;
            }
            cursor = buf_idx;
            idx_off = 0;
            break;
        case 'd' : // delete
            ce_delete(cursor, buf);
            break;
        case 'D' : // multichar delete
            ce_geti(&buf_len, command);
            for (; buf_len > 0; --buf_len) {
                ce_delete(cursor, buf);
            }
            break;
        case '?' : // help
            ce_printf("For help, please read the included README. ");
            ce_printf("Getting help through this interface would be very");
            ce_printf("tedious, yes?");
            break;
        case 'v' : // version
            ce_printf("EDCHAR 0.5");
            break;
        case 'h' : // left
            if (cursor > 0) {
                --cursor;
            }
            break;
        case 'l' : // right
            if (cursor < buf->len) {
                ++cursor;
            }
            break;
        case 'c' : // cursor pos
            ce_printf("%d", cursor);
            break;
        default:
            ce_printf("!");
            break;
        } 
        ce_dprintf(1, "%s", buf->buf_raw);
        ce_dprintf(3, "%d", cursor);
    }
    return CE_OKAY;
}

int main(int argc, char *argv[]) {
    initscr();
    clear();
    noecho();
    cbreak();
    move(0, 0);
    ce_buf_t buf;
    ce_buf_init(&buf);
    if (argc > 1) {
        edit(argv[1], &buf);
    }
    main_loop(&buf);
    ce_buf_destroy(&buf);
    endwin();
    return 0;
}

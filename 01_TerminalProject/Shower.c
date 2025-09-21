#include <curses.h>
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define max(x, y) ((x) > (y) ? (x) : (y))
#define min(x, y) ((x) < (y) ? (x) : (y))
#define TEXT_FRAME_LINES ((int)LINES - 5)
#define TEXT_FRAME_COLS ((int)COLS - 2)
#define KEY_ESCAPE 27

// Запущено с valgrind, ошибок не найдено
// Но после initscr() всегда происходит варнинг на неосвобожденную память
#define DEBUG_STRUCTIRE 0

#define assert(b, ...)           \
    do {                         \
        if (!(b)) {              \
            printf(__VA_ARGS__); \
            exit(1);             \
        }                        \
    } while (0)

struct ShowerInfo {
    char** lines;     // строки файла
    int* sizes;       // длины строк (массив той же длины)
    int lines_num;    // число строк
    int columns_num;  // число строк

    int str, col;  // текущая позиция ЛЕВОГО ВЕРХНЕГО УГЛА в файле.
                   // Всегда больше нуля.
                   // Изменяется при нажатии стрелок.

    int max_str, max_col;  // Максимально возможная ЛЕВОГО ВЕРХНЕГО УГЛА в файле.
                           // вычисляется единожды при чтении файла.
                           // зависит от размера экрана.
                           // состояние str==max_str считается нормальным.
};

static void free_shower(struct ShowerInfo* shower) {
    for (int i = 0; i < shower->lines_num; i++) {
        free(shower->lines[i]);
    }
    free(shower->lines);
    free(shower->sizes);
}

static void read_lines(const char* filename, struct ShowerInfo* shower) {
    // прочитать файл полностью
    FILE* f = fopen(filename, "r");
    assert(f != NULL, "ERROR: file %s is not opened!\n", filename);
    fseek(f, 0, SEEK_END);
    long fsize = ftell(f);
    fseek(f, 0, SEEK_SET);
    char* full_file = malloc(fsize + 1);
    fread(full_file, fsize, 1, f);
    fclose(f);
    // ЗАМЕНИТЬ /0, ВСТРЕЧЕННЫЙ В ФАЙЛЕ, на некоторый символ
    // тк /0  - это явно не текст
    for (long i = 0; i < fsize; i++) {
        if (full_file[i] == '\0') {
            full_file[i] = '?';
        }
    }
    // добавить незначащий /n, если его нет в конце
    if (full_file[fsize - 1] != '\n') {
        full_file[fsize] = '\n';
        fsize += 1;
    }

    // посчитать число строк
    shower->str = 0;
    shower->col = 0;
    shower->lines_num = 0;
    for (long i = 0; i < fsize; i++) {
        shower->lines_num += full_file[i] == '\n';
    }

    // перезаписать строки файла в вектор строк
    shower->lines = (char**)malloc(sizeof(char*) * (shower->lines_num + 1));
    shower->sizes = (int*)malloc(sizeof(int) * (shower->lines_num + 1));
    // обнулить последние номера, чтобы программа падала при некорректном обращении
    shower->lines[shower->lines_num] = NULL;
    shower->sizes[shower->lines_num] = -1;

    long index = 0;
    for (int i = 0; i < shower->lines_num; i++) {
        long index_start = index;
        while (full_file[index] != '\n') {
            index++;
        }
        // [a][b][c][/n]
        //  |        |
        //  |        index
        // index_start
        index++;

        // перекопировать строку
        shower->sizes[i] = (int)(index - index_start - 1);
        shower->lines[i] = (char*)malloc(shower->sizes[i] + 1);
        memcpy(shower->lines[i], full_file + index_start, shower->sizes[i]);
        shower->lines[i][shower->sizes[i]] = '\0';
    }

    free(full_file);

#if DEBUG_STRUCTIRE
    printf("-- %d --\n", shower->lines_num);
    for (int i = 0; i < shower->lines_num; i++) {
        printf("%d '%s'\n", shower->sizes[i], shower->lines[i]);
    }
#endif
}

static void init_borders(struct ShowerInfo* shower) {
    // обозначить пределы, в которых можно листать
    shower->max_str = max(0, shower->lines_num - TEXT_FRAME_LINES);
    int max_line_len = 0;
    for (int i = 0; i < shower->lines_num; i++) {
        max_line_len = max(max_line_len, shower->sizes[i]);
    }
    shower->columns_num = max_line_len;
    shower->max_col = max(0, max_line_len - TEXT_FRAME_COLS);

#if DEBUG_STRUCTIRE
    endwin();
    printf("-- %d %d --\n", shower->max_str, shower->max_col);
    free_shower(shower);
    printf("DEBUG finished\n");
    exit(0);
#endif
}

// передвинуть строки. Вернуть 1, если движение произошло
static int move_lines(struct ShowerInfo* shower, int n_raw, int n_col) {
    int old_str = shower->str, old_col = shower->col;
    shower->str += n_raw;
    shower->str = max(0, shower->str);
    shower->str = min(shower->max_str, shower->str);
    shower->col += n_col;
    shower->col = max(0, shower->col);
    shower->col = min(shower->max_col, shower->col);
    return shower->str != old_str || shower->col != old_col;
}

static void draw_it(WINDOW* text_window, struct ShowerInfo* shower) {
    for (int i = 0; i < TEXT_FRAME_LINES; i++) {
        if (i + shower->str >= shower->lines_num) {
            break;
        }
        char buffer[TEXT_FRAME_COLS + 1];
        memset(buffer, ' ', TEXT_FRAME_COLS);
        buffer[TEXT_FRAME_COLS] = '\0';
        if (shower->sizes[i + shower->str] - shower->col > 0) {
            memcpy(buffer, shower->lines[i + shower->str] + shower->col,
                   min(TEXT_FRAME_COLS, shower->sizes[i + shower->str] - shower->col));
        }
        mvwaddstr(text_window, i, 0, buffer);
    }
}

int main(int argc, char** argv) {
    setlocale(LC_ALL, "");

    assert(argv[1] != NULL, "ERROR: argv[1] must be a filename, not NULL!\n");

    struct ShowerInfo shower;
    read_lines(argv[1], &shower);

    initscr();
    assert(LINES >= 7 && COLS >= 50, "ERROR: use terminal window (expected at least 7*50, found %d*%d)!\n", LINES,
           COLS);
    init_borders(&shower);

    noecho();
    cbreak();

    // сообщение с информацией о файле
    char buffer[COLS];
    snprintf(buffer, COLS - 1, "File %s", argv[1]);
    mvaddstr(0, 0, buffer);
    snprintf(buffer, COLS - 1, "<HELP> use arrows / hjkl, SPACE, ESC, q, PGUP/PGDN");
    mvaddstr(2, 0, buffer);
    snprintf(buffer, COLS - 1, "lines: %d..%d/%d, columns: %d..%d/%d", shower.str + 1,
             min(shower.str + TEXT_FRAME_LINES, shower.lines_num), shower.lines_num, shower.col + 1,
             min(shower.col + TEXT_FRAME_COLS, shower.columns_num), shower.columns_num);
    mvaddstr(1, 0, buffer);
    refresh();

    // статичная рамка
    WINDOW* frame;
    frame = newwin(LINES - 3, COLS, 3, 0);
    box(frame, 0, 0);
    wrefresh(frame);

    WINDOW* text_window;
    text_window = newwin(TEXT_FRAME_LINES, TEXT_FRAME_COLS, 4, 1);
    draw_it(text_window, &shower);
    wrefresh(text_window);

    keypad(text_window, TRUE);

    while (1) {
        int c = wgetch(text_window);
        if (c == 'q' || c == KEY_ESCAPE) {
            break;
        }

        int n_raw = 0, n_col = 0;

        // определяем, куда переместится курсор
        if (c == KEY_UP || c == 'k') {
            n_raw -= 1;
        } else if (c == KEY_DOWN || c == 'j' || c == ' ') {
            n_raw += 1;
        } else if (c == KEY_LEFT || c == 'h') {
            n_col -= 1;
        } else if (c == KEY_RIGHT || c == 'l') {
            n_col += 1;
        } else if (c == KEY_NPAGE) {
            n_raw += TEXT_FRAME_LINES;
        } else if (c == KEY_PPAGE) {
            n_raw -= TEXT_FRAME_LINES;
        }

        // перемещаем курсор
        if (move_lines(&shower, n_raw, n_col) == 0) {
            continue;
        }

        draw_it(text_window, &shower);

        memset(buffer, ' ', COLS - 1);
        buffer[COLS - 1] = '\0';
        mvaddstr(1, 0, buffer);
        snprintf(buffer, COLS - 1, "lines: %d..%d/%d, columns: %d..%d/%d", shower.str + 1,
                 min(shower.str + TEXT_FRAME_LINES, shower.lines_num), shower.lines_num, shower.col + 1,
                 min(shower.col + TEXT_FRAME_COLS, shower.columns_num), shower.columns_num);
        mvaddstr(1, 0, buffer);

        refresh();
        // wrefresh(frame);
        wrefresh(text_window);
    }

    delwin(text_window);
    delwin(frame);
    endwin();
    free_shower(&shower);

    return 0;
}

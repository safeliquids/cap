#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define A_SIZE 16
#define LINE_BUFFER_SIZE 1024

// TODO: Implement topological sorting of user includes
//       Currently, it is not checked that the result is correct. If arguments
//       are given in the wrong order, the result is not correct.
//       (e.g. functions will be referenced before they are defined.)

const char * obtain_system_include(const char * line_buffer) {
    if (strncmp("#include", line_buffer, 8) != 0) {
        return NULL;
    }
    const char * opening = strchr(line_buffer, '<');
    if (!opening) {
        return (NULL);
    }
    const char * closing = strchr(opening, '>');
    if (!closing) {
        fprintf(stderr, "slicer: malformed include directive\n");
        exit(-1);
    }

    int len = closing - opening  - 1;
    char * i_name = (char *) malloc((len + 1) * sizeof(char));
    memcpy(i_name, opening + 1, len);
    i_name[len] = '\0';
    return i_name;
}

void insert_if_not_present(
        const char *** i_s, int * i_count, int * i_alloc,
        const char * i_name) {
    // try and find the same include
    int j = 0;
    int comp = 1;
    for ( ; j < *i_count; ++j) {
        comp = strcmp((*i_s)[j], i_name);
        if (comp >= 0) {
            break;
        }
    }
    if (comp == 0) { // this include is already present
        return;
    }

    // now we are adding something, so must check allocation size
    if (*i_count >= *i_alloc) {
        *i_alloc = (*i_alloc) * 2;
        *i_s = (const char **) realloc(*i_s, *i_alloc);
    }
    // insert new name into list
    const char * to_insert = i_name;
    for (int insert_here = j; insert_here < *i_count; ++insert_here) {
        const char * tmp = (*i_s)[insert_here];
        (*i_s)[insert_here] = to_insert;
        to_insert = tmp;
    }
    (*i_s)[(*i_count)++] = to_insert;
}

void extract_includes(
        const int file_count, const char ** files, int * include_count, 
        const char *** includes) {
    int i_alloc = A_SIZE;
    *includes = (const char **) malloc(A_SIZE * sizeof(const char *));

    for (int i = 0; i < file_count; ++i) {
        FILE * file = fopen(files[i], "r");
        if (!file) {
            fprintf(stderr, "slicer: could not open file %s\n", files[i]);
            exit(-1);
        }
        char line_buffer[LINE_BUFFER_SIZE];
        while (true) {
            // this is a dirty trick, but, since I am dealing with code written
            // by people the length of a line can be assumed less than
            // a constant. Thus, choosing a long enough buffer guarantees to
            // get the whole line in one fgets call.
            char * s = fgets(line_buffer, LINE_BUFFER_SIZE, file);
            int end = feof(file);
            if (!s && !end) {
                fprintf(stderr, "slicer: could not read from file %s\n", files[i]);
                exit(-1);
            }
            if (!s && end) {
                break;
            }

            const char * i_name = obtain_system_include(line_buffer);
            if (!i_name) {
                continue;
            }
            insert_if_not_present(includes, include_count, &i_alloc, i_name);
            if (end) {
                break;
            }
        }
        fclose(file);
    }
}

void dump_files_without_includes(
        FILE * dst, int file_count, const char ** files,
        bool * last_line_was_empty) {
    for (int i = 0; i < file_count; ++i) {
        if (!*last_line_was_empty) {
            fputs("\n", dst);
            *last_line_was_empty = true;
        }
        FILE * file = fopen(files[i], "r");
        if (!file) {
            fprintf(stderr, "slicer: cannot open file %s\n", files[i]);
            exit(-1);
        }
        char line_buffer[LINE_BUFFER_SIZE];
        bool skip_next = false;
        int preprocessor_if_depth = 0;
        while(true) {
            char * s = fgets(line_buffer, LINE_BUFFER_SIZE, file);
            int end = feof(file);
            if (!s && !end) {
                fprintf(stderr, "slicer: could not read from file %s\n", files[i]);
                exit(-1);
            }
            // empty line
            if (!s && end) {
                break;
            }

            if (skip_next) {
                skip_next = false;
                continue;
            }

            if (line_buffer[0] == '\n') {
                if (!*last_line_was_empty) {
                    fputs(line_buffer, dst);
                    *last_line_was_empty = true;
                }
                continue;
            }

            if (strncmp("#include", line_buffer, 8) == 0) {
                continue;
            }
            if (strncmp("#if", line_buffer, 3) == 0) {
                // if this is the outer-most #if, skip it together with the next line
                if (preprocessor_if_depth++ == 0) {
                    skip_next = true;
                    continue;
                }
            }
            if (strncmp("#endif", line_buffer, 6) == 0) {
                if (--preprocessor_if_depth == 0) {
                    continue;
                }
            }

            *last_line_was_empty = false;
            fputs(line_buffer, dst);
            if (end) {  // in this case, the last line of the file did not have a newline
                fputs("\n", dst);
                break;
            }
        }
        if (!*last_line_was_empty) {
            fputs("\n", dst);
            *last_line_was_empty = true;
        }
        fclose(file);
    }
}

int main(int argc, const char ** argv) {
    if (argc <= 1) {
        fprintf(stderr, "slicer: no files were provided\n");
        return -1;
    }

    const char ** files = (const char **) malloc((argc - 1) * sizeof(const char *));
    int file_count = 0;
    const char * output_file = NULL;
    for (int i = 1; i < argc; ++i) {
        const char * arg = argv[i];
        if (arg[0] != '-') {
            files[file_count++] = arg;
            continue;
        }
        if (arg[1] == 'o') {
            output_file = argv[i + 1];
            ++i;
            continue;
        }
        else if (arg[1] == 'h') {
            printf("usage:\n");
            printf("\tslicer.exe [-h] [-o output_file] file [file ...]\n");
            return 1;
        }
        else {
            fprintf(stderr, "slicer: unknown option %s\n", arg);
            return -1;

        }
    }

    int include_count = 0;
    const char ** includes;
    extract_includes(file_count, files, &include_count, &includes);

    FILE * output = stdout;
    if (output_file) {
        output = fopen(output_file, "w");
        if (!output) {
            fprintf(stderr, "slicer: could not create cap.h\n");
            return -1;
        }
    }

    fputs("#ifndef __CAP_H__\n#define __CAP_H__\n\n", output);
    for (int i = 0; i < include_count; ++i) {
        fprintf(output, "#include <%s>\n", includes[i]);
    }
    if (include_count > 0) {
        fprintf(output, "\n");
    }
    bool last_line_was_empty = true;

    dump_files_without_includes(output, file_count, files, &last_line_was_empty);

    if (!last_line_was_empty) {
        fputs("\n", output);
    }
    fputs("#endif\n", output);
    fflush(output);
    if (output_file) {
        fclose(output);
    }
    return 0;
}

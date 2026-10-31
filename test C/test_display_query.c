/* Nearest-line lookups across gaps larger than the fast search window. */
#include "synctex_parser.h"

#include <stdio.h>
#include <string.h>

static int check(synctex_scanner_p scanner, const char *source, int line,
                 int page_hint, int expected_line, int expected_page)
{
    synctex_node_p node;
    int count = synctex_display_query(scanner, source, line, 0, page_hint);
    node = synctex_scanner_next_result(scanner);
    if (expected_page == 0) {
        if (count == 0 && node == NULL) {
            return 0;
        }
    } else if (count > 0 && node && synctex_node_line(node) == expected_line
               && synctex_node_page(node) == expected_page) {
        do {
            const char *name = synctex_scanner_get_name(scanner, synctex_node_tag(node));
            if (!name || strcmp(name, source) || synctex_node_line(node) != expected_line) {
                fprintf(stderr, "Result belongs to the wrong source or line\n");
                return 1;
            }
        } while ((node = synctex_scanner_next_result(scanner)));
        return 0;
    }
    fprintf(stderr, "%s:%d (hint %d): expected line %d, page %d; got count %d, line %d, page %d\n",
            source, line, page_hint, expected_line, expected_page, count,
            node ? synctex_node_line(node) : -1, node ? synctex_node_page(node) : -1);
    return 1;
}

int main(int argc, char **argv)
{
    synctex_scanner_p scanner;
    int failed = 0;
    if (argc != 2) {
        fprintf(stderr, "Usage: %s path/to/display_query.pdf\n", argv[0]);
        return 1;
    }
    scanner = synctex_scanner_new_with_output_file(argv[1], NULL, 1);
    if (!scanner) {
        fprintf(stderr, "Could not parse the fixture\n");
        return 1;
    }
    failed |= check(scanner, "query.tex", 1, 0, 1, 1);       /* Exact. */
    failed |= check(scanner, "query.tex", 2, 0, 1, 1);       /* Nearby. */
    failed |= check(scanner, "query.tex", 99, 0, 1, 1);      /* Distant preceding line. */
    failed |= check(scanner, "query.tex", 101, 0, 201, 2);   /* Tie: following line. */
    failed |= check(scanner, "query.tex", 101, 4, 201, 4);   /* Preserve page hint. */
    failed |= check(scanner, "query.tex", 102, 0, 201, 2);   /* Distant following line. */
    failed |= check(scanner, "query.tex", 201, 0, 201, 2);   /* Exact, multiple pages. */
    failed |= check(scanner, "query.tex", 1000, 0, 201, 2);  /* Beyond last line. */
    failed |= check(scanner, "other.tex", 101, 0, 101, 3);   /* Source filtering. */
    failed |= check(scanner, "empty.tex", 50, 0, 0, 0);      /* No candidates. */
    synctex_scanner_free(scanner);
    if (!failed) {
        puts("10 display-query checks passed");
    }
    return failed;
}

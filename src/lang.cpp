#include <cstdio>
#include <string>
#include <cstring>
#include <list>
#include <queue>
#include <cstdlib>
using namespace std;

void usage() {
    fprintf(stderr, "usage: lang ALPHA...\n");
    fprintf(stderr, "where ALPHA is either:\n");
    fprintf(stderr, "\ta single character X\n");
    fprintf(stderr, "\ta range of characters X-Y\n");
}

int main(int argc, char *argv[]) {

    if (argc == 1) {
        usage();
        return 1;
    }

    list<char> symbols;
    for (int i = 1; i < argc; i++) {
        int len = strlen(argv[i]);
        if (len == 1) {
            symbols.push_back(argv[i][0]);
        } else if (len == 3 && argv[i][1] == '-') {
            char a = argv[i][0],
                 b = argv[i][2];

            int d = (b - a) / abs(b - a);

            while (true) {
                symbols.push_back(a);
                if (a == b) {
                    break;
                }

                a += d;
            }
        } else {
            fprintf(stderr, "error: unrecognized token '%s'\n", argv[i]);
            return 1;
        }
    }

    int cnt = symbols.size();
    char *syms = (char*)malloc(cnt * sizeof(char));
    int at = 0;
    for (list<char>::const_iterator it = symbols.begin(); it != symbols.end(); ++it) {
        syms[at++] = *it;
    }

    int curlen = 1024;
    int *st = (int*)calloc(curlen, sizeof(int));

    for (int len = 0; ; len++) {
        if (len > curlen) {
            curlen = len * 2;
            st = (int*)realloc(st, curlen * sizeof(int));
            memset(st, 0, curlen * sizeof(int));
        }

        while (true) {
            for (int i = len - 1; i >= 0; i--) {
                putchar(syms[st[i]]);
            }

            putchar('\n');

            bool add = true;
            for (int i = 0; i < len && add; i++) {
                st[i]++;
                if (st[i] == cnt) {
                    st[i] = 0;
                } else {
                    add = false;
                }
            }

            if (add) {
                break;
            }
        }
    }

    // queue<string> q;
    // q.push("");

    // while (!q.empty()) {
    //     string cur = q.front(); q.pop();
    //     printf("%s\n", cur.c_str());

    //     for (list<char>::const_iterator it = symbols.begin(); it != symbols.end(); ++it) {
    //         q.push(cur + *it);
    //     }
    // }

    return 0;
}


#include <cstdio>
#include <string>
#include <cstring>
#include <list>
#include <queue>
#include <cstdlib>
using namespace std;

int main(int argc, char *argv[]) {

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
        }
    }

    queue<string> q;
    q.push("");

    while (!q.empty()) {
        string cur = q.front(); q.pop();
        printf("%s\n", cur.c_str());

        for (list<char>::const_iterator it = symbols.begin(); it != symbols.end(); ++it) {
            q.push(cur + *it);
        }
    }

    return 0;
}


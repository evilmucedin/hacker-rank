#include <cstdio>

#include <vector>

using namespace std;

using IntVector = vector<int>;

int main() {
    int n;
    scanf("%d", &n);

    IntVector a(n + 1);
    a[0] = 1;
    for (int i = 0; i < n; ++i) {
        scanf("%d", &a[i + 1]);
    }

    int q;
    scanf("%d", &q);
    for (int i = 0; i < q; ++i) {
        int x;
        int y;
        scanf("%d%d", &x, &y);

        bool result = false;
        if (a[x] != 0) {
            if (x == y) {
                result = a[x] & 1;
            } else {
                if (a[x] & 1) {
                    result = true;
                } else {
                    result = (a[x + 1] == 0);
                }
            }
        } else {
            result = false;
        }

        if (result) {
            printf("Odd\n");
        } else {
            printf("Even\n");
        }
    }

    return 0;
}

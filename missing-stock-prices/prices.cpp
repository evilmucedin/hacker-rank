#include <cstdio>

#include <vector>
#include <iostream>

using namespace std;

using DoubleVector = vector<double>;

int main() {
    int n;
    scanf("%d", &n);
    DoubleVector prices(n);

    char dummy[1000];
    char buffer[1000];

    for (size_t i = 0; i < n; ++i) {
        scanf("%s%s%s", dummy, dummy, buffer);
        if (buffer[0] != 'M') {
            sscanf(buffer, "%lf", &prices[i]);
        } else {
            prices[i] = 0;
        }
    }
    auto oldPrices = prices;

    for (size_t i = 1; i < n; ++i) {
        if (prices[i] == 0) {
            prices[i] = prices[i - 1];
        }
    }

    for (int i = n - 2; i >= 0; --i) {
        if (prices[i] == 0) {
            prices[i] = prices[i + 1];
        }
    }

    for (size_t i = 0; i < n; ++i) {
        if (!oldPrices[i]) {
            cout << prices[i] << endl;
        }
    }

    return 0;
}

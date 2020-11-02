#include <cstdio>
#include <cstring>

#include <vector>
#include <iostream>

using namespace std;

using DoubleVector = vector<double>;
using DoubleMatrix = vector<DoubleVector>;

DoubleMatrix createMatrix(size_t n) {
    DoubleMatrix result(n, DoubleVector(n));
    return result;
}

DoubleVector createVector(size_t n) { return DoubleVector(n); }

ostream& operator<<(ostream& s, const DoubleVector& v) {
    s << "{";
    bool first = true;
    for (const auto& x : v) {
        if (!first) {
            s << ", ";
        }
        first = false;
        s << x;
    }
    s << "}";
    return s;
}

ostream& operator<<(ostream& s, const DoubleMatrix& m) {
    for (const auto& v : m) {
        s << v << endl;
    }
    s << endl;
    return s;
}

struct LinearRegressor {
    static DoubleVector mul(const DoubleMatrix& m, const DoubleVector& v) {
        DoubleVector result(v.size());
        for (size_t i = 0; i < v.size(); ++i) {
            for (size_t j = 0; j < v.size(); ++j) {
                result[i] += m[i][j] * v[j];
            }
        }
        return result;
    }

    static DoubleMatrix inverse(const DoubleMatrix& m) {
        const auto n = m.size();
        vector<DoubleVector> g(n, DoubleVector(2 * n));
        for (size_t i = 0; i < n; ++i) {
            for (size_t j = 0; j < n; ++j) {
                g[i][j] = m[i][j];
            }
        }
        for (size_t i = 0; i < n; ++i) {
            g[i][n + i] = 1.0;
        }

        auto swapLines = [&](size_t i, size_t j) {
            if (i != j) {
                for (size_t k = 0; k < 2 * n; ++k) {
                    swap(g[i][k], g[j][k]);
                }
            }
        };

        auto mulLine = [&](size_t i, double m) {
            for (size_t j = 0; j < 2 * n; ++j) {
                g[i][j] *= m;
            }
        };

        auto subLine = [&](size_t i, size_t j, double m) {
            for (size_t k = 0; k < 2 * n; ++k) {
                g[j][k] -= g[i][k] * m;
            }
        };

        for (size_t i = 0; i < n; ++i) {
            size_t j = i;
            while ((j < n) && (g[j][i] == 0)) {
                ++j;
            }
            if (j < n) {
                swapLines(i, j);
                mulLine(i, 1.0 / g[i][i]);
                for (size_t j = i + 1; j < n; ++j) {
                    subLine(i, j, g[j][i]);
                }
            }
        }

        for (int i = (int)n - 1; i >= 0; --i) {
            for (int j = 0; j < i; ++j) {
                subLine(i, j, g[j][i]);
            }
        }

        // cerr << g << endl;

        auto result = createMatrix(n);
        for (size_t i = 0; i < n; ++i) {
            for (size_t j = 0; j < n; ++j) {
                result[i][j] = g[i][n + j];
            }
        }
        return result;
    }

    LinearRegressor(size_t numFeatures)
        : numFeatures_(numFeatures), xtx_(createMatrix(numFeatures_)), xty_(createVector(numFeatures_)) {
    }

    void addSample(const DoubleVector& x, double y) {
        for (size_t j = 0; j < numFeatures_; ++j) {
            for (size_t k = 0; k < numFeatures_; ++k) {
                xtx_[j][k] += x[j] * x[k];
            }
            xty_[j] += x[j] * y;
        }
    }

    DoubleVector regress() const {
        auto ixtx = inverse(xtx_);
        auto beta = mul(ixtx, xty_);
        return beta;
    }

    double apply(const DoubleVector& beta, const DoubleVector& x) const {
        double result = 0;
        for (size_t i = 0; i < numFeatures_; ++i) {
            result += beta[i] * x[i];
        }
        return result;
    }

    size_t numFeatures_;
    DoubleMatrix xtx_;
    DoubleVector xty_;
};

int main() {
    int n;
    scanf("%d", &n);
    auto prices = createVector(n);

    char dummy[1000];
    char buffer[1000];

    size_t missing = 0;
    for (size_t i = 0; i < n; ++i) {
        scanf("%s%s%s", dummy, dummy, buffer);
        if (buffer[0] != 'M') {
            sscanf(buffer, "%lf", &prices[i]);
        } else {
            prices[i] = 0;
            ++missing;
            sprintf(dummy, "Missing_%zd", missing);
            if (strcmp(buffer, dummy)) {
                throw exception();
            }
        }
    }
    auto oldPrices = prices;

    auto good = [&](size_t i) {
        if (i >= 2 && i + 2 < n) {
            if (oldPrices[i - 2] && oldPrices[i - 1] && oldPrices[i + 1] && oldPrices[i + 2]) {
                return true;
            }
        }
        return false;
    };

    auto getFeatures = [&](size_t i) {
        DoubleVector x(4);
        x[0] = prices[i - 2];
        x[1] = prices[i - 1];
        x[2] = prices[i + 1];
        x[3] = prices[i + 2];
        return x;
    };

    LinearRegressor lr(4);
    for (size_t i = 0; i < n; ++i) {
        if (good(i) && prices[i]) {
            lr.addSample(getFeatures(i), prices[i]);
        }
    }

    auto beta = lr.regress();
    cerr << beta << endl;

    for (size_t i = 0; i < n; ++i) {
        if (good(i) && !prices[i]) {
            prices[i] = lr.apply(beta, getFeatures(i));
        }
    }

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

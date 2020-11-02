#include <iostream>
#include <vector>

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
    for (const auto& v: m) {
        s << v << endl;
    }
    s << endl;
    return s;
}

DoubleVector mul(const DoubleMatrix& m, const DoubleVector& v) {
    DoubleVector result(v.size());
    for (size_t i = 0; i < v.size(); ++i) {
        for (size_t j = 0; j < v.size(); ++j) {
            result[i] += m[i][j] * v[j];
        }
    }
    return result;
}

DoubleMatrix inverse(const DoubleMatrix& m) {
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
        for (size_t j = 0; j < 2*n; ++j) {
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

    cerr << g << endl;

    auto result = createMatrix(n);
    for (size_t i = 0; i < n; ++i) {
        for (size_t j = 0; j < n; ++j) {
            result[i][j] = g[i][n + j];
        }
    }
    return result;
}

DoubleVector readFeatures(size_t n) {
    DoubleVector x(n + 1);
    for (size_t i = 0; i < n; ++i) {
        cin >> x[i];
    }
    x[n] = 1.0;
    return x;
}

int main() {
    size_t numFeatures;
    cin >> numFeatures;

    size_t numTraining;
    cin >> numTraining;

    DoubleVector x(numFeatures);
    auto xtx = createMatrix(numFeatures + 1);
    auto xty = createVector(numFeatures + 1);
    for (size_t i = 0; i < numTraining; ++i) {
        x = readFeatures(numFeatures);
        double y;
        cin >> y;

        for (size_t j = 0; j < numFeatures + 1; ++j) {
            for (size_t k = 0; k < numFeatures + 1; ++k) {
                xtx[j][k] += x[j] * x[k];
            }
            xty[j] += x[j] * y;
        }
    }

    auto ixtx = inverse(xtx);
    auto beta = mul(ixtx, xty);

    cerr << beta << endl;

    size_t numTesting;
    cin >> numTesting;

    for (size_t i = 0; i < numTesting; ++i) {
        x = readFeatures(numFeatures);

        double result = 0;
        for (size_t j = 0; j < numFeatures + 1; ++j) {
            result += beta[j]*x[j];
        }
        cout << result << endl;
    }
    return 0;
}

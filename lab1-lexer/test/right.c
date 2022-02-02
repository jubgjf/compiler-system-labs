int my_function(int a, int b) {
    a = b + 1;
    printf("%d", a);
    return 1;
}

struct student {
    int    age    = 0;
    double height = 100.0;
    long   weight = 50;
}

int main() {
    unsigned a = 12;
    int      _a1, _a2 = -12, +12;
    int      b, c     = 07, 0x1f;
    float    d, e     = 1.5e-32, -0.3e+01;
    float    _d1, _d2 = 1., .1;

    _a1 = _a2 % _a1 * (-1) / (3 ^ 5);
    _a1++;
    _a2--;

    t, u, t = a > b, a <= b, a != b;
    t, u, t = t && u, t || u, !t;
    t = a | b;
    t = a & b;

    char a[3] = [ 0, 1, 2 ];

    if (a > b) {
        do {
            my_function(1, 2);
        } while (num > 2);
    } else {
        switch (a) {
        case 1:
            break;
        default:
            break;
        }
    }

    // 注释
    /*
     * 多行注释：abcde
     */
}
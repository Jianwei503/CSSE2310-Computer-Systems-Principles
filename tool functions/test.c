
char *f1(void) {
    return "f1";
}

char *f2(void) {
    return "f2";
}

int main() {
    char *(*foo[7])(void);
    char *(*p1)(void) = f1;
    char *(*p2)(void) = f2;

    foo[0] = p1;
    foo[1] = p2;
}
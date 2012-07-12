
typedef struct {
		char flags;
		int width;
		int precision;
		char length;
		char specifier;
} ftag_t;

char * itoa(int value, char * str, int base);
int atoi(const char *str);

int vsprintf(char * str, const char * format, va_list arg);
int sprintf(char * str, const char * format, ...);

int ftag_format(char *buf, int len, void *obj, ftag_t ftag);
const char * ftag_scan(ftag_t *ftag, const char *format);

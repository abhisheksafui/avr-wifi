#ifndef WEB_SERVER_H
#define WEB_SERVER_H
#define SPACE	' '
#define PLUS	'+'
//#define IS_SPACE(a)   ({__typeof__(a) b = (a); if(*b=='+')TRUE;else FALSE; 
#define ASCII_TO_NIBBLE(x)    ({char a = x; if(isdigit(a)){ a = a - '0'; } else{ if(a>=65 && a <= 70){ a = a - 55; }} a; })
void serve_static_html(const char *page, const char *handle, char * tmp, uint16_t len);

int urlDecode(char *src, char *tmp);

#endif

OBJ_SRC=api.c version.c protocol.c ip_acl.c
OBJ=$(OBJ_SRC:%.c=%.o)

all:httpd.a

httpd.a: $(OBJ)
	ar cr httpd.a $(OBJ)

$(OBJ): %.o: %.c
	gcc -c $<

clean:
	@rm $(OBJ) httpd.a

.PHONY: all clean

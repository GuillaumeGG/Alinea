
package=minicas
version=0.42.0

targets=(minicas)
type[minicas]=binary
sources[minicas]="$(echo *.c)"
cflags[minicas]="-O0 -g -Wall -Wextra"
ldflags[minicas]="-lreadline"


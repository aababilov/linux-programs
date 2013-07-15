#define _GNU_SOURCE

#include <dlfcn.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
/*#include <fcntl.h>*/

typedef int (*open_t)(const char *file, int oflag, ...);

static open_t open_f = NULL;

static void need_open()
{
	if (open_f == NULL) {
		*(void **) (&open_f) =
			dlsym(RTLD_NEXT, "open");
	}
}

static int open_log()
{
	need_open();
	int fid = (*open_f)(
		"/tmp/cspylog",
		0100|01/*O_CREAT|O_WRONLY*/,
		0644);
	lseek(fid, 0, SEEK_END);
	return fid;
}

int open(const char *file, int oflag, int mode)
{
	need_open();
	int fid = open_log();
	const char *s = "open: ";
	write(fid, s, strlen(s));
	write(fid, file, strlen(file));
	write(fid, "\n", 1);
	close(fid);
		       
	if (oflag | 0100) {/*O_CREAT*/
		return (*open_f)(file, oflag, mode);
	} else {
		return (*open_f)(file, oflag);
	}
}

#define LIBC_FUNC(name, ret, filename, args, ntype_args)		\
typedef ret (*name##_t)args;						\
static name##_t name##_f = NULL;					\
									\
static void need_##name()						\
{									\
	if (name##_f == NULL) {						\
		*(void **) (&name##_f) = dlsym(RTLD_NEXT, #name);	\
	}								\
}									\
									\
ret name args								\
{									\
	need_##name();							\
	int fid = open_log();						\
	const char *s = #name": ";					\
	write(fid, s, strlen(s));					\
	write(fid, filename, strlen(filename));				\
	write(fid, "\n", 1);						\
	close(fid);							\
									\
	(*name##_f)ntype_args;						\
}

LIBC_FUNC(execve, int, filename,
	  (const char *filename,
	   char *const argv[], char *const envp[]),
	  (filename, argv, envp))

LIBC_FUNC(execv, int, path,
	  (const char *path, char *const argv[]), (path, argv))

LIBC_FUNC(execvp, int, file,
	  (const char *file, char *const argv[]),
	  (file, argv))

#define O_RDONLY  0x00000000
#define O_WRONLY  0x00000001
#define O_RDWR    0x00000002
#define O_ACCMODE 0x00000003

#define O_TRUNC   0x00000400
#define O_APPEND  0x00000008
#define O_CREAT   0x00000200

#define AT_FDCWD  -100
#define CLOCKS_PER_SEC ((clock_t) 128)
#define RUSAGE_SELF 0

#define F_OK    0
#define X_OK    1
#define R_OK    4
#define W_OK    2

typedef int pid_t;

extern pid_t _getpid(void);
extern int _kill(pid_t, int);
extern int _sigaction(int, struct sigaction *, struct sigaction *);

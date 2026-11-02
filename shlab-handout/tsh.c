/* 
 * tsh - A tiny shell program with job control
 * 
 * <Put your name and login ID here>
 * 中文：tsh：带作业控制的微型 shell 程序。请在此填写姓名与登录 ID。
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <limits.h>

/* Misc manifest constants  中文：其他符号常量。 */
#define MAXLINE    1024   /* max line size  中文：输入行的最大长度。 */
#define MAXARGS     128   /* max args on a command line  中文：一条命令行的最大参数个数。 */
#define MAXJOBS      16   /* max jobs at any point in time  中文：任意时刻允许存在的最大作业数。 */
#define MAXJID    1<<16   /* max job ID  中文：作业 ID 的最大值。 */

/* Job states  中文：作业状态。 */
#define UNDEF 0 /* undefined  中文：未定义。 */
#define FG 1    /* running in foreground  中文：在前台运行。 */
#define BG 2    /* running in background  中文：在后台运行。 */
#define ST 3    /* stopped  中文：已停止。 */

/* 
 * Jobs states: FG (foreground), BG (background), ST (stopped)
 * Job state transitions and enabling actions:
 *     FG -> ST  : ctrl-z
 *     ST -> FG  : fg command
 *     ST -> BG  : bg command
 *     BG -> FG  : fg command
 * At most 1 job can be in the FG state.
 * 中文：作业状态：FG（前台）、BG（后台）、ST（停止）。状态转换与触发操作：FG → ST：
 * ctrl-z；ST → FG：fg 命令；ST → BG：bg 命令；BG → FG：fg 命令。任意时刻最多只有一个
 * FG 作业。
 */

/* Global variables  中文：全局变量。 */
extern char **environ;      /* defined in libc  中文：由 libc 定义。 */
char prompt[] = "tsh> ";    /* command line prompt (DO NOT CHANGE)  中文：命令行提示符；不要修改。 */
int verbose = 0;            /* if true, print additional output  中文：为真时输出更多信息。 */
int nextjid = 1;            /* next job ID to allocate  中文：下一个待分配的作业 ID。 */
char sbuf[MAXLINE];         /* for composing sprintf messages  中文：用于通过 sprintf 组装消息。 */

struct job_t {              /* The job struct  中文：作业结构体。 */
    pid_t pid;              /* job PID  中文：作业的进程 ID（PID）。 */
    int jid;                /* job ID [1, 2, ...]  中文：作业 ID（JID），从 1 开始编号。 */
    int state;              /* UNDEF, BG, FG, or ST  中文：状态为 UNDEF、BG、FG 或 ST。 */
    char cmdline[MAXLINE];  /* command line  中文：命令行文本。 */
};
struct job_t jobs[MAXJOBS]; /* The job list  中文：作业表。 */
/* End global variables  中文：全局变量部分结束。 */


/* Function prototypes  中文：函数原型。 */

/* Here are the functions that you will implement  中文：以下函数需要你实现。 */
void eval(char *cmdline);
int builtin_cmd(char **argv);
void do_bgfg(char **argv);
void waitfg(pid_t pid);

void sigchld_handler(int sig);
void sigtstp_handler(int sig);
void sigint_handler(int sig);

/* Here are helper routines that we've provided for you  中文：以下辅助函数已经提供。 */
int parseline(const char *cmdline, char **argv); 
void sigquit_handler(int sig);

void clearjob(struct job_t *job);
void initjobs(struct job_t *jobs);
int maxjid(struct job_t *jobs); 
int addjob(struct job_t *jobs, pid_t pid, int state, char *cmdline);
int deletejob(struct job_t *jobs, pid_t pid); 
pid_t fgpid(struct job_t *jobs);
struct job_t *getjobpid(struct job_t *jobs, pid_t pid);
struct job_t *getjobjid(struct job_t *jobs, int jid); 
int pid2jid(pid_t pid); 
void listjobs(struct job_t *jobs);

void usage(void);
void unix_error(char *msg);
void app_error(char *msg);
static void block_jobs(sigset_t *prev);
static void restore_mask(const sigset_t *prev);
static void safe_puts(const char *text);
static void safe_number(int value);
static void job_signal_message(int jid, pid_t pid, int stopped, int signum);
typedef void handler_t(int);
handler_t *Signal(int signum, handler_t *handler);

/*
 * main - The shell's main routine 
 * 中文：main：shell 的主函数。
 */
int main(int argc, char **argv) 
{
    int c;
    char cmdline[MAXLINE];
    int emit_prompt = 1; /* emit prompt (default)  中文：输出提示符；默认启用。 */

    /* Redirect stderr to stdout (so that driver will get all output
     * on the pipe connected to stdout)
     * 中文：将标准错误重定向到标准输出，让驱动通过连接 stdout 的管道接收到所有输出。
     */
    if (dup2(1, 2) < 0)
        unix_error("dup2");

    /* Parse the command line  中文：解析命令行选项。 */
    while ((c = getopt(argc, argv, "hvp")) != EOF) {
        switch (c) {
        case 'h':             /* print help message  中文：打印帮助信息。 */
            usage();
	    break;
        case 'v':             /* emit additional diagnostic info  中文：输出额外诊断信息。 */
            verbose = 1;
	    break;
        case 'p':             /* don't print a prompt  中文：不打印提示符。 */
            emit_prompt = 0;  /* handy for automatic testing  中文：便于自动化测试。 */
	    break;
	default:
            usage();
	}
    }

    /* Install the signal handlers  中文：安装信号处理函数。 */

    /* These are the ones you will need to implement  中文：以下信号处理函数需要你实现。 */
    Signal(SIGINT,  sigint_handler);   /* ctrl-c  中文：键盘 ctrl-c。 */
    Signal(SIGTSTP, sigtstp_handler);  /* ctrl-z  中文：键盘 ctrl-z。 */
    Signal(SIGCHLD, sigchld_handler);  /* Terminated or stopped child  中文：子进程终止或停止。 */

    /* This one provides a clean way to kill the shell  中文：此处理函数提供一种干净地终止 shell 的方式。 */
    Signal(SIGQUIT, sigquit_handler); 

    /* Initialize the job list  中文：初始化作业表。 */
    initjobs(jobs);

    /* Execute the shell's read/eval loop  中文：执行 shell 的读取与求值循环。 */
    while (1) {

	/* Read command line  中文：读取命令行。 */
	if (emit_prompt) {
	    printf("%s", prompt);
	    fflush(stdout);
	}
	if ((fgets(cmdline, MAXLINE, stdin) == NULL) && ferror(stdin))
	    app_error("fgets error");
	if (feof(stdin)) { /* End of file (ctrl-d)  中文：输入结束；终端中可由 ctrl-d 触发。 */
	    fflush(stdout);
	    exit(0);
	}

	/* Evaluate the command line  中文：执行命令行的求值处理。 */
	eval(cmdline);
	fflush(stdout);
	fflush(stdout);
    } 

    exit(0); /* control never reaches here  中文：程序控制流不会到达这里。 */
}
  
/* 
 * eval - Evaluate the command line that the user has just typed in
 * 
 * If the user has requested a built-in command (quit, jobs, bg or fg)
 * then execute it immediately. Otherwise, fork a child process and
 * run the job in the context of the child. If the job is running in
 * the foreground, wait for it to terminate and then return.  Note:
 * each child process must have a unique process group ID so that our
 * background children don't receive SIGINT (SIGTSTP) from the kernel
 * when we type ctrl-c (ctrl-z) at the keyboard.  
 * 中文：eval：处理用户刚输入的命令行。若是内建命令 quit、jobs、bg 或 fg，就立即执行；
 * 否则 fork 创建子进程，在子进程中运行作业。前台作业需要等待其终止后再返回。注意：每个
 * 子进程必须具有唯一的进程组 ID，以免键盘 ctrl-c / ctrl-z 导致内核把 SIGINT / SIGTSTP
 * 发给后台子进程。
*/
void eval(char *cmdline)
{
    char *argv[MAXARGS];
    int bg = parseline(cmdline, argv);
    sigset_t prev;
    pid_t pid;

    if (argv[0] == NULL || builtin_cmd(argv))
        return;

    /* 中文：先屏蔽作业相关信号，保证先登记，再处理子进程状态。 */
    block_jobs(&prev);
    pid = fork();
    if (pid < 0) {
        int error = errno;
        restore_mask(&prev);
        errno = error;
        perror("fork");
        return;
    }
    if (pid == 0) {
        if (setpgid(0, 0) < 0) {
            safe_puts("setpgid failed\n");
            _exit(1);
        }
        /* 中文：子进程恢复默认处理和原来的屏蔽集合，再执行外部程序。 */
        Signal(SIGINT, SIG_DFL);
        Signal(SIGTSTP, SIG_DFL);
        Signal(SIGQUIT, SIG_DFL);
        restore_mask(&prev);
        execve(argv[0], argv, environ);
        safe_puts(argv[0]);
        safe_puts(": Command not found\n");
        _exit(1);
    }

    /* 中文：父进程也设置一次，避免转发信号时子进程组还未建立。 */
    if (setpgid(pid, pid) < 0 && errno != EACCES && errno != ESRCH)
        perror("setpgid");
    if (!addjob(jobs, pid, bg ? BG : FG, cmdline)) {
        /* 中文：作业表已满，终止并回收这个无法登记的子进程。 */
        if (kill(pid, SIGKILL) < 0 && errno != ESRCH)
            perror("kill");
        while (waitpid(pid, NULL, 0) < 0) {
            if (errno == EINTR)
                continue;
            if (errno != ECHILD)
                perror("waitpid");
            break;
        }
        restore_mask(&prev);
        return;
    }
    if (bg) {
        printf("[%d] (%d) %s", pid2jid(pid), (int)pid, cmdline);
        fflush(stdout);
    }
    restore_mask(&prev);
    if (!bg)
        waitfg(pid);
}
  
/* 
 * parseline - Parse the command line and build the argv array.
 * 
 * Characters enclosed in single quotes are treated as a single
 * argument.  Return true if the user has requested a BG job, false if
 * the user has requested a FG job.  
 * 中文：parseline：解析命令行并构建 argv 数组。单引号包围的字符视为一个参数。用户请求
 * 后台（BG）作业时返回真，请求前台（FG）作业时返回假。
 */
int parseline(const char *cmdline, char **argv) 
{
    static char array[MAXLINE]; /* holds local copy of command line  中文：保存命令行的局部副本。 */
    char *buf = array;          /* ptr that traverses command line  中文：用于遍历命令行的指针。 */
    char *delim;                /* points to first space delimiter  中文：指向第一个空格分隔符。 */
    int argc;                   /* number of args  中文：参数个数。 */
    int bg;                     /* background job?  中文：是否为后台作业？ */

    strcpy(buf, cmdline);
    buf[strlen(buf)-1] = ' ';  /* replace trailing '\n' with space  中文：把末尾的换行符替换为空格。 */
    while (*buf && (*buf == ' ')) /* ignore leading spaces  中文：忽略开头的空格。 */
	buf++;

    /* Build the argv list  中文：构建 argv 参数列表。 */
    argc = 0;
    if (*buf == '\'') {
	buf++;
	delim = strchr(buf, '\'');
    }
    else {
	delim = strchr(buf, ' ');
    }

    while (delim) {
	argv[argc++] = buf;
	*delim = '\0';
	buf = delim + 1;
	while (*buf && (*buf == ' ')) /* ignore spaces  中文：忽略空格。 */
	       buf++;

	if (*buf == '\'') {
	    buf++;
	    delim = strchr(buf, '\'');
	}
	else {
	    delim = strchr(buf, ' ');
	}
    }
    argv[argc] = NULL;
    
    if (argc == 0)  /* ignore blank line  中文：忽略空行。 */
	return 1;

    /* should the job run in the background?  中文：该作业是否应在后台运行？ */
    if ((bg = (*argv[argc-1] == '&')) != 0) {
	argv[--argc] = NULL;
    }
    return bg;
}

/* 
 * builtin_cmd - If the user has typed a built-in command then execute
 *    it immediately.  
 * 中文：builtin_cmd：如果用户输入了内建命令，就立即执行。
 */
int builtin_cmd(char **argv)
{
    if (strcmp(argv[0], "quit") == 0)
        exit(0);
    if (strcmp(argv[0], "jobs") == 0) {
        sigset_t prev;
        block_jobs(&prev);
        listjobs(jobs);
        fflush(stdout);
        restore_mask(&prev);
        return 1;
    }
    if (strcmp(argv[0], "bg") == 0 || strcmp(argv[0], "fg") == 0) {
        do_bgfg(argv);
        return 1;
    }
    return 0;
}

/* 
 * do_bgfg - Execute the builtin bg and fg commands
 * 中文：do_bgfg：执行内建命令 bg 和 fg。
 */
void do_bgfg(char **argv)
{
    struct job_t *jobp;
    sigset_t prev;
    char *end;
    const char *number;
    long id;
    int is_jid;
    int bg = strcmp(argv[0], "bg") == 0;
    pid_t pid;

    if (argv[1] == NULL) {
        printf("%s command requires PID or %%jobid argument\n", argv[0]);
        return;
    }
    is_jid = argv[1][0] == '%';
    number = argv[1] + is_jid;
    errno = 0;
    id = strtol(number, &end, 10);
    if (!isdigit((unsigned char)number[0]) || *end != '\0' ||
        errno == ERANGE || id <= 0 || id > INT_MAX) {
        printf("%s: argument must be a PID or %%jobid\n", argv[0]);
        return;
    }

    /* 中文：查找、发信号、更新状态期间，禁止处理函数删除这条记录。 */
    block_jobs(&prev);
    jobp = is_jid ? getjobjid(jobs, (int)id) : getjobpid(jobs, (pid_t)id);
    if (jobp == NULL) {
        if (is_jid)
            printf("%%%ld: No such job\n", id);
        else
            printf("(%ld): No such process\n", id);
        restore_mask(&prev);
        return;
    }
    pid = jobp->pid;
    if (kill(-pid, SIGCONT) < 0) {
        int error = errno;
        restore_mask(&prev);
        errno = error;
        if (error != ESRCH)
            perror("kill");
        return;
    }
    jobp->state = bg ? BG : FG;
    if (bg) {
        printf("[%d] (%d) %s", jobp->jid, (int)pid, jobp->cmdline);
        fflush(stdout);
    }
    restore_mask(&prev);
    if (!bg)
        waitfg(pid);
}

/* 
 * waitfg - Block until process pid is no longer the foreground process
 * 中文：waitfg：阻塞等待，直到进程 pid 不再是前台进程。
 */
void waitfg(pid_t pid)
{
    sigset_t prev;
    block_jobs(&prev);
    /* 中文：检查时屏蔽信号；等待时原子地恢复屏蔽集合，避免漏掉通知。 */
    while (fgpid(jobs) == pid) {
        if (sigsuspend(&prev) < 0 && errno != EINTR)
            unix_error("sigsuspend");
    }
    restore_mask(&prev);
}

/*****************
 * Signal handlers
 * 中文：信号处理函数。
 *****************/

/* 
 * sigchld_handler - The kernel sends a SIGCHLD to the shell whenever
 *     a child job terminates (becomes a zombie), or stops because it
 *     received a SIGSTOP or SIGTSTP signal. The handler reaps all
 *     available zombie children, but doesn't wait for any other
 *     currently running children to terminate.  
 * 中文：sigchld_handler：子作业终止（成为僵尸进程），或因 SIGSTOP / SIGTSTP 停止时，内
 * 核会向 shell 发送 SIGCHLD。该处理函数回收所有可回收的僵尸子进程，但不等待仍在运行的
 * 其他子进程结束。
 */
void sigchld_handler(int sig)
{
    int olderrno = errno;
    int status;
    pid_t pid;
    (void)sig;

    for (;;) {
        pid = waitpid(-1, &status, WNOHANG | WUNTRACED);
        if (pid > 0) {
            struct job_t *jobp = getjobpid(jobs, pid);
            if (jobp == NULL)
                continue;
            if (WIFEXITED(status)) {
                deletejob(jobs, pid);
            } else if (WIFSIGNALED(status)) {
                job_signal_message(jobp->jid, pid, 0, WTERMSIG(status));
                deletejob(jobs, pid);
            } else if (WIFSTOPPED(status)) {
                jobp->state = ST;
                job_signal_message(jobp->jid, pid, 1, WSTOPSIG(status));
            }
        } else if (pid == 0) {
            break;
        } else {
            if (errno == EINTR)
                continue;
            if (errno != ECHILD)
                safe_puts("waitpid failed\n");
            break;
        }
    }
    errno = olderrno;
}

/* 
 * sigint_handler - The kernel sends a SIGINT to the shell whenver the
 *    user types ctrl-c at the keyboard.  Catch it and send it along
 *    to the foreground job.  
 * 中文：sigint_handler：用户按 ctrl-c 时，内核向 shell 发送 SIGINT。捕获该信号，并转发
 * 给前台作业。
 */
void sigint_handler(int sig)
{
    int olderrno = errno;
    pid_t pid = fgpid(jobs);
    if (pid > 0 && kill(-pid, sig) < 0 && errno != ESRCH)
        safe_puts("kill failed\n");
    errno = olderrno;
}

/*
 * sigtstp_handler - The kernel sends a SIGTSTP to the shell whenever
 *     the user types ctrl-z at the keyboard. Catch it and suspend the
 *     foreground job by sending it a SIGTSTP.  
 * 中文：sigtstp_handler：用户按 ctrl-z 时，内核向 shell 发送 SIGTSTP。捕获该信号，并通
 * 过转发 SIGTSTP 暂停前台作业。
 */
void sigtstp_handler(int sig)
{
    int olderrno = errno;
    pid_t pid = fgpid(jobs);
    if (pid > 0 && kill(-pid, sig) < 0 && errno != ESRCH)
        safe_puts("kill failed\n");
    errno = olderrno;
}

/*********************
 * End signal handlers
 * 中文：信号处理函数部分结束。
 *********************/

/***********************************************
 * Helper routines that manipulate the job list
 * 中文：操作作业表的辅助函数。
 **********************************************/

/* clearjob - Clear the entries in a job struct  中文：clearjob：清空作业结构体中的各字段。 */
void clearjob(struct job_t *job) {
    job->pid = 0;
    job->jid = 0;
    job->state = UNDEF;
    job->cmdline[0] = '\0';
}

/* initjobs - Initialize the job list  中文：initjobs：初始化作业表。 */
void initjobs(struct job_t *jobs) {
    int i;

    for (i = 0; i < MAXJOBS; i++)
	clearjob(&jobs[i]);
}

/* maxjid - Returns largest allocated job ID  中文：maxjid：返回已分配作业 ID 中的最大值。 */
int maxjid(struct job_t *jobs) 
{
    int i, max=0;

    for (i = 0; i < MAXJOBS; i++)
	if (jobs[i].jid > max)
	    max = jobs[i].jid;
    return max;
}

/* addjob - Add a job to the job list  中文：addjob：把一个作业加入作业表。 */
int addjob(struct job_t *jobs, pid_t pid, int state, char *cmdline) 
{
    int i;
    
    if (pid < 1)
	return 0;

    for (i = 0; i < MAXJOBS; i++) {
	if (jobs[i].pid == 0) {
	    jobs[i].pid = pid;
	    jobs[i].state = state;
	    jobs[i].jid = nextjid++;
	    if (nextjid > MAXJOBS)
		nextjid = 1;
	    strcpy(jobs[i].cmdline, cmdline);
  	    if(verbose){
	        printf("Added job [%d] %d %s\n", jobs[i].jid, jobs[i].pid, jobs[i].cmdline);
            }
            return 1;
	}
    }
    printf("Tried to create too many jobs\n");
    return 0;
}

/* deletejob - Delete a job whose PID=pid from the job list  中文：deletejob：从作业表删除 PID 等于 pid 的作业。 */
int deletejob(struct job_t *jobs, pid_t pid) 
{
    int i;

    if (pid < 1)
	return 0;

    for (i = 0; i < MAXJOBS; i++) {
	if (jobs[i].pid == pid) {
	    clearjob(&jobs[i]);
	    nextjid = maxjid(jobs)+1;
	    return 1;
	}
    }
    return 0;
}

/* fgpid - Return PID of current foreground job, 0 if no such job  中文：fgpid：返回当前前台作业的 PID；不存在时返回 0。 */
pid_t fgpid(struct job_t *jobs) {
    int i;

    for (i = 0; i < MAXJOBS; i++)
	if (jobs[i].state == FG)
	    return jobs[i].pid;
    return 0;
}

/* getjobpid  - Find a job (by PID) on the job list  中文：getjobpid：按进程 ID（PID）在作业表中查找作业。 */
struct job_t *getjobpid(struct job_t *jobs, pid_t pid) {
    int i;

    if (pid < 1)
	return NULL;
    for (i = 0; i < MAXJOBS; i++)
	if (jobs[i].pid == pid)
	    return &jobs[i];
    return NULL;
}

/* getjobjid  - Find a job (by JID) on the job list  中文：getjobjid：按作业 ID（JID）在作业表中查找作业。 */
struct job_t *getjobjid(struct job_t *jobs, int jid) 
{
    int i;

    if (jid < 1)
	return NULL;
    for (i = 0; i < MAXJOBS; i++)
	if (jobs[i].jid == jid)
	    return &jobs[i];
    return NULL;
}

/* pid2jid - Map process ID to job ID  中文：pid2jid：把进程 ID 映射为作业 ID。 */
int pid2jid(pid_t pid) 
{
    int i;

    if (pid < 1)
	return 0;
    for (i = 0; i < MAXJOBS; i++)
	if (jobs[i].pid == pid) {
            return jobs[i].jid;
        }
    return 0;
}

/* listjobs - Print the job list  中文：listjobs：打印作业表。 */
void listjobs(struct job_t *jobs) 
{
    int i;
    
    for (i = 0; i < MAXJOBS; i++) {
	if (jobs[i].pid != 0) {
	    printf("[%d] (%d) ", jobs[i].jid, jobs[i].pid);
	    switch (jobs[i].state) {
		case BG: 
		    printf("Running ");
		    break;
		case FG: 
		    printf("Foreground ");
		    break;
		case ST: 
		    printf("Stopped ");
		    break;
	    default:
		    printf("listjobs: Internal error: job[%d].state=%d ", 
			   i, jobs[i].state);
	    }
	    printf("%s", jobs[i].cmdline);
	}
    }
}
/******************************
 * end job list helper routines
 * 中文：作业表辅助函数部分结束。
 ******************************/


/***********************
 * Other helper routines
 * 中文：其他辅助函数。
 ***********************/

/*
 * usage - print a help message
 * 中文：usage：打印帮助信息。
 */
void usage(void) 
{
    printf("Usage: shell [-hvp]\n");
    printf("   -h   print this message\n");
    printf("   -v   print additional diagnostic information\n");
    printf("   -p   do not emit a command prompt\n");
    exit(1);
}

/*
 * unix_error - unix-style error routine
 * 中文：unix_error：Unix 风格的错误处理函数。
 */
void unix_error(char *msg)
{
    fprintf(stdout, "%s: %s\n", msg, strerror(errno));
    exit(1);
}

/*
 * app_error - application-style error routine
 * 中文：app_error：应用程序风格的错误处理函数。
 */
void app_error(char *msg)
{
    fprintf(stdout, "%s\n", msg);
    exit(1);
}

/*
 * Signal - wrapper for the sigaction function
 * 中文：Signal：sigaction 函数的包装函数。
 */
handler_t *Signal(int signum, handler_t *handler) 
{
    struct sigaction action, old_action;

    action.sa_handler = handler;  
    /* 中文：处理函数执行期间屏蔽其他信号，防止嵌套访问作业表。 */
    if (sigfillset(&action.sa_mask) < 0)
        unix_error("sigfillset");
    action.sa_flags = SA_RESTART; /* restart syscalls if possible  中文：尽可能自动重新启动被信号中断的系统调用。 */

    if (sigaction(signum, &action, &old_action) < 0)
	unix_error("Signal error");
    return (old_action.sa_handler);
}

/*
 * sigquit_handler - The driver program can gracefully terminate the
 *    child shell by sending it a SIGQUIT signal.
 * 中文：sigquit_handler：驱动程序可通过发送 SIGQUIT 信号，正常终止子 shell。
 */
void sigquit_handler(int sig) 
{
    (void)sig;
    safe_puts("Terminating after receipt of SIGQUIT signal\n");
    _exit(1);
}




/* 中文：普通代码访问作业表时，暂缓三个会访问作业表的信号。 */
static void block_jobs(sigset_t *prev)
{
    sigset_t mask;
    if (sigemptyset(&mask) < 0 || sigaddset(&mask, SIGCHLD) < 0 ||
        sigaddset(&mask, SIGINT) < 0 || sigaddset(&mask, SIGTSTP) < 0 ||
        sigprocmask(SIG_BLOCK, &mask, prev) < 0)
        unix_error("block job signals");
}

static void restore_mask(const sigset_t *prev)
{
    if (sigprocmask(SIG_SETMASK, prev, NULL) < 0)
        unix_error("sigprocmask");
}

/* 中文：信号处理函数使用 write 输出，避免调用不安全的 printf。 */
static void safe_puts(const char *text)
{
    size_t length = 0;
    while (text[length] != '\0')
        length++;
    while (length > 0) {
        ssize_t count = write(STDOUT_FILENO, text, length);
        if (count < 0 && errno == EINTR)
            continue;
        if (count <= 0)
            _exit(1);
        text += count;
        length -= (size_t)count;
    }
}

static void safe_number(int value)
{
    char buffer[3 * sizeof(int) + 1];
    char *p = buffer + sizeof(buffer) - 1;
    *p = '\0';
    do {
        *--p = '0' + value % 10;
        value /= 10;
    } while (value != 0);
    safe_puts(p);
}

static void job_signal_message(int jid, pid_t pid, int stopped, int signum)
{
    safe_puts("Job [");
    safe_number(jid);
    safe_puts("] (");
    safe_number((int)pid);
    safe_puts(stopped ? ") stopped by signal " : ") terminated by signal ");
    safe_number(signum);
    safe_puts("\n");
}

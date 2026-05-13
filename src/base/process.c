#include <ppr.h>
#include <ppr_int.h>

typedef struct process {
#if defined(PPR_IS_WIN32)
	HANDLE process;
	HANDLE thread;
	HANDLE h_stdin;
	HANDLE h_stdout;
	HANDLE h_stderr;
#elif defined(PPR_IS_UNIX)
	pid_t pid;
	int   fd_stdin;
	int   fd_stdout;
	int   fd_stderr;
#else
	void* reserved;
#endif
} process_t;

static ppr_bool is_allowed_env(const char* n) {
	char* p = ppr_strdup(n);
	char* s;

	if((s = strchr(p, '=')) != NULL) s[0] = 0;

	if(strcmp(p, "PATH") == 0) return ppr_true;

	return ppr_false;
}

void* ppr_process_create(const char* exec, char** env) {
#if defined(PPR_IS_WIN32)
	process_t*	    proc = malloc(sizeof(*proc));
	SECURITY_ATTRIBUTES attr;
	HANDLE		    pipe_stdin[2];
	HANDLE		    pipe_stdout[2];
	HANDLE		    pipe_stderr[2];
	char		    path[2048];
	char*		    envs;
	PROCESS_INFORMATION pi;
	STARTUPINFO	    si;
	char*		    d_envs = GetEnvironmentStrings();
	char*		    d_envs2;
	int		    envs_len = 0;
	int		    i;

	d_envs2 = d_envs;
	while(*d_envs2) {
		if(is_allowed_env(d_envs2)) envs_len += strlen(d_envs2) + 1;

		d_envs2 += strlen(d_envs2) + 1;
	}

	for(i = 0; env[i] != NULL; i++) envs_len += strlen(env[i]) + 1;
	envs_len++;

	envs	 = malloc(envs_len);
	envs_len = 0;

	d_envs2 = d_envs;
	while(*d_envs2) {
		if(is_allowed_env(d_envs2)) {
			strcpy(envs + envs_len, d_envs2);
			envs_len += strlen(d_envs2) + 1;
		}

		d_envs2 += strlen(d_envs2) + 1;
	}

	for(i = 0; env[i] != NULL; i++) {
		strcpy(envs + envs_len, env[i]);
		envs_len += strlen(env[i]) + 1;
	}
	FreeEnvironmentStrings(d_envs);

	envs[envs_len] = 0;

	memset(&pi, 0, sizeof(pi));

	memset(&si, 0, sizeof(si));
	si.cb	   = sizeof(si);
	si.dwFlags = STARTF_USESTDHANDLES;

	sprintf(path, "\"%s\"", exec);

	memset(&attr, 0, sizeof(attr));
	attr.nLength	    = sizeof(attr);
	attr.bInheritHandle = TRUE;

	CreatePipe(&pipe_stdin[0], &pipe_stdin[1], &attr, 0);
	SetHandleInformation(pipe_stdin[1], HANDLE_FLAG_INHERIT, 0);

	CreatePipe(&pipe_stdout[0], &pipe_stdout[1], &attr, 0);
	SetHandleInformation(pipe_stdout[0], HANDLE_FLAG_INHERIT, 0);

	CreatePipe(&pipe_stderr[0], &pipe_stderr[1], &attr, 0);
	SetHandleInformation(pipe_stderr[0], HANDLE_FLAG_INHERIT, 0);

	si.hStdInput  = pipe_stdin[0];
	si.hStdOutput = pipe_stdout[1];
	si.hStdError  = pipe_stderr[1];

	if(!CreateProcess(NULL, path, NULL, NULL, TRUE, 0, envs, NULL, &si, &pi)) {
		int i;

		free(proc);
		free(envs);

		for(i = 0; i < 2; i++) {
			CloseHandle(pipe_stdin[i]);
			CloseHandle(pipe_stdout[i]);
			CloseHandle(pipe_stderr[i]);
		}

		return NULL;
	} else {
		CloseHandle(pipe_stdin[0]);
		CloseHandle(pipe_stdout[1]);
		CloseHandle(pipe_stderr[1]);

		proc->process  = pi.hProcess;
		proc->thread   = pi.hThread;
		proc->h_stdin  = pipe_stdin[1];
		proc->h_stdout = pipe_stdout[0];
		proc->h_stderr = pipe_stderr[0];
	}

	free(envs);

	return proc;
#elif defined(PPR_IS_UNIX)
	process_t* proc = malloc(sizeof(*proc));
	int	   pipe_stdin[2];
	int	   pipe_stdout[2];
	int	   pipe_stderr[2];
	char**	   envs = NULL;
	int	   i;
	int	   c = 0;

	extern char** environ;

	for(i = 0; environ[i] != NULL; i++) {
		if(is_allowed_env(environ[i])) c++;
	}
	for(i = 0; env != NULL && env[i] != NULL; i++) c++;

	envs = malloc(sizeof(*envs) * (c + 1));

	c = 0;
	for(i = 0; environ[i] != NULL; i++) {
		if(is_allowed_env(environ[i])) envs[c++] = environ[i];
	}
	for(i = 0; env != NULL && env[i] != NULL; i++) envs[c++] = env[i];
	envs[c] = 0;

	pipe(pipe_stdin);
	pipe(pipe_stdout);
	pipe(pipe_stderr);

	if((proc->pid = fork()) == 0) {
		int basefd = pipe_stdout[1] > pipe_stderr[1] ? pipe_stdout[1] : pipe_stderr[1];
		int infd   = basefd + 3;
		int outfd  = basefd + 4;
		int errfd  = basefd + 5;

		free(proc);

		close(pipe_stdin[1]);
		close(pipe_stdout[0]);
		close(pipe_stderr[0]);

		dup2(pipe_stdin[0], infd);
		dup2(pipe_stdout[1], outfd);
		dup2(pipe_stderr[1], errfd);
		close(pipe_stdin[0]);
		close(pipe_stdout[1]);
		close(pipe_stderr[1]);
		dup2(infd, 0);
		dup2(outfd, 1);
		dup2(errfd, 2);
		close(infd);
		close(outfd);
		close(errfd);

		execle(exec, exec, NULL, envs);

		_exit(-1);
	} else {
		close(pipe_stdin[0]);
		close(pipe_stdout[1]);
		close(pipe_stderr[1]);

		proc->fd_stdin	= pipe_stdin[1];
		proc->fd_stdout = pipe_stdout[0];
		proc->fd_stderr = pipe_stderr[0];
	}

	free(envs);

	return proc;
#else
	return NULL;
#endif
}

void ppr_process_close(void* handle) {
	process_t* proc = handle;
#if defined(PPR_IS_WIN32)
	CloseHandle(proc->h_stdin);
	proc->h_stdin = NULL;
#elif defined(PPR_IS_UNIX)
	close(proc->fd_stdin);
	proc->fd_stdin = -1;
#endif
}

int ppr_process_write(void* handle, const void* data, int len) {
	process_t* proc = handle;
#if defined(PPR_IS_WIN32)
	DWORD r;

	if(!WriteFile(proc->h_stdin, data, len, &r, NULL)) return -1;

	return r;
#elif defined(PPR_IS_UNIX)
	return write(proc->fd_stdin, data, len);
#else
	return -1;
#endif
}

int ppr_process_read(void* handle, void* data, int len) {
	process_t* proc = handle;
#if defined(PPR_IS_WIN32)
	DWORD r;

	if(!ReadFile(proc->h_stdout, data, len, &r, NULL)) return -1;

	return r;
#elif defined(PPR_IS_UNIX)
	return read(proc->fd_stdout, data, len);
#else
	return -1;
#endif
}

int ppr_process_readable(void* handle) {
	process_t* proc = handle;
#if defined(PPR_IS_WIN32)
	DWORD avail;

	if(!PeekNamedPipe(proc->h_stdout, NULL, 0, NULL, &avail, NULL)) return 0;

	return (avail > 0) ? 1 : 0;
#elif defined(FPR_IS_UNIX)
	struct ppr_pollfd pfd;
	int		  n;

	pfd.fd	   = proc->fd_stdout;
	pfd.events = FPR_POLLIN | FPR_POLLPRI;

	if((n = ppr_poll(&pfd, 1, 0)) < 0) return 0;

	return (n > 0) ? 1 : 0;
#else
	return 0;
#endif
}

void ppr_process_destroy(void* handle) {
	process_t* proc = handle;
#if defined(PPR_IS_WIN32)
	if(proc->h_stdin != NULL) CloseHandle(proc->h_stdin);
	CloseHandle(proc->h_stdout);
	CloseHandle(proc->h_stderr);

	WaitForSingleObject(proc->process, INFINITE);

	CloseHandle(proc->process);
	CloseHandle(proc->thread);
#elif defined(PPR_IS_UNIX)
	int st;

	if(proc->fd_stdin >= 0) close(proc->fd_stdin);
	close(proc->fd_stdout);
	close(proc->fd_stderr);

	do {
		waitpid(proc->pid, &st, 0);
	} while(!WIFEXITED(st));
#endif
	free(proc);
}

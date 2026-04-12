#include <ppr.h>
#include <ppr_int.h>

int ppr_poll(struct ppr_pollfd* fds, int nfds, int timeout) {
#if defined(PPR_HAS_POLL)
	struct pollfd* pfds = malloc(sizeof(*pfds) * nfds);
	int	       i;
	int	       st;

	for(i = 0; i < nfds; i++) {
		pfds[i].fd     = fds[i].fd;
		pfds[i].events = 0;
		fds[i].revents = 0;

		if(fds[i].events & PPR_POLLIN) pfds[i].events |= POLLIN;
		if(fds[i].events & PPR_POLLPRI) pfds[i].events |= POLLPRI;
		if(fds[i].events & PPR_POLLOUT) pfds[i].events |= POLLOUT;
	}

	st = poll(pfds, nfds, timeout);

	if(st > 0) {
		for(i = 0; i < nfds; i++) {
			if(pfds[i].revents & POLLIN) fds[i].revents |= PPR_POLLIN;
			if(pfds[i].revents & POLLPRI) fds[i].revents |= PPR_POLLPRI;
			if(pfds[i].revents & POLLOUT) fds[i].revents |= PPR_POLLOUT;
		}
	}

	free(pfds);

	return st;
#else
	fd_set	       rfds;
	fd_set	       wfds;
	int	       i;
	struct timeval tv;
	int	       st;

	FD_ZERO(&rfds);
	FD_ZERO(&wfds);

	for(i = 0; i < nfds; i++) {
		if(fds[i].events & PPR_POLLIN) FD_SET(fds[i].fd, &rfds);
		if(fds[i].events & PPR_POLLOUT) FD_SET(fds[i].fd, &wfds);

		fds[i].revents = 0;
	}

	tv.tv_sec  = timeout / 1000;
	tv.tv_usec = (timeout % 1000) * 1000;

	st = select(FD_SETSIZE, &rfds, &wfds, NULL, &tv);

	for(i = 0; i < nfds; i++) {
		if(FD_ISSET(fds[i].fd, &rfds)) fds[i].revents |= PPR_POLLIN;
		if(FD_ISSET(fds[i].fd, &wfds)) fds[i].revents |= PPR_POLLOUT;
	}

	return st;
#endif
}

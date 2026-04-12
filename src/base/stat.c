#include <ppr.h>
#include <ppr_int.h>

int ppr_stat(const char* path, struct ppr_stat* s) {
#if defined(PPR_IS_WIN32)
	WIN32_FILE_ATTRIBUTE_DATA fad;
	ULARGE_INTEGER		  mtime;

	if(!GetFileAttributesEx(path, GetFileExInfoStandard, &fad)) return -1;

	memcpy(&mtime, &fad.ftLastWriteTime, sizeof(mtime));

	s->st_size    = 0;
	s->st_modtime = (mtime.QuadPart / 10000000) / 11644473600;
	s->st_mode    = 0;

	s->st_mode |= PPR_S_IFREG;
	if(fad.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
		s->st_mode &= ~PPR_S_IFREG;
		s->st_mode |= PPR_S_IFDIR;
	}

	if(PPR_S_ISREG(s->st_mode)) {
		PPR_FILE* f = ppr_fopen(path, "r"); /* :))))))) */
		s->st_size  = GetFileSize((HANDLE)f, NULL);
		ppr_fclose(f);
	}

	return 0;
#elif defined(PPR_IS_UNIX) || defined(PPR_IS_NETWARE)
	struct stat st;
	int	    sc;

	sc = stat(path, &st);
	if(sc != 0) return sc;

	s->st_size    = st.st_size;
	s->st_modtime = st.st_mtime;
	s->st_mode    = 0;

	if(S_ISREG(st.st_mode)) s->st_mode |= PPR_S_IFREG;
	if(S_ISDIR(st.st_mode)) s->st_mode |= PPR_S_IFDIR;

	return sc;
#else
	return -1;
#endif
}

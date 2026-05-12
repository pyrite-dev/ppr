#include <ppr.h>
#include <ppr_int.h>

#if defined(PPR_IS_WIN32)
typedef struct dir {
	ppr_bool	next;
	HANDLE		hFind;
	WIN32_FIND_DATA ffd;

	char*		  path;
	struct ppr_dirent dirent;
} dir_t;
#else
typedef struct dir {
	DIR* dir;

	char*		  path;
	struct ppr_dirent dirent;
} dir_t;
#endif

PPR_DIR* ppr_opendir(const char* path) {
	dir_t* dir = malloc(sizeof(*dir));
#if defined(PPR_IS_WIN32)
	char* p = ppr_strvacat(path, (path[strlen(path) - 1] == '/' || path[strlen(path) - 1] == '\\') ? "" : (strchr(path, '/') != NULL ? "/" : "\\"), "*", NULL);

	if((dir->hFind = FindFirstFile(p, &dir->ffd)) == INVALID_HANDLE_VALUE) {
		free(p);
		free(dir);

		return NULL;
	}
	free(p);

	dir->next = ppr_true;
#else
	if((dir->dir = opendir(path)) == NULL) {
		free(dir);

		return NULL;
	}
#endif

	dir->path = ppr_strdup(path);

	return (PPR_DIR*)dir;
}

struct ppr_dirent* ppr_readdir(PPR_DIR* handle) {
	dir_t* dir = handle;
#if defined(PPR_IS_WIN32)
	if(!dir->next) return NULL;

	strcpy(dir->dirent.d_name, dir->ffd.cFileName);

	dir->next = FindNextFile(dir->hFind, &dir->ffd);
#else
	struct dirent* d;

	if((d = readdir(dir->dir)) == NULL) return NULL;

	strcpy(dir->dirent.d_name, d->d_name);
#endif

	strcpy(dir->dirent.d_fullname, dir->path);
	if(dir->path[strlen(dir->path) - 1] != '/' && dir->path[strlen(dir->path) - 1] != '\\') {
#if defined(PPR_IS_WIN32)
		strcat(dir->dirent.d_fullname, strchr(dir->path, '/') != NULL ? "/" : "\\");
#else
		strcat(dir->dirent.d_fullname, "/");
#endif
	}
	strcat(dir->dirent.d_fullname, dir->dirent.d_name);

	memset(&dir->dirent.d_stat, 0, sizeof(dir->dirent.d_stat));
	ppr_stat(dir->dirent.d_fullname, &dir->dirent.d_stat);

	return &dir->dirent;
}

void ppr_closedir(PPR_DIR* handle) {
	dir_t* dir = handle;
#if defined(PPR_IS_WIN32)
	FindClose(dir->hFind);
#else
	closedir(dir->dir);
#endif

	free(dir->path);

	free(dir);
}

int ppr_scandir(const char* dirname, struct ppr_dirent*** namelist, int (*selectfn)(const struct ppr_dirent* d), int (*compar)(const struct ppr_dirent** d1, const struct ppr_dirent** d2)) {
	PPR_DIR*	   dir = ppr_opendir(dirname);
	struct ppr_dirent* d;
	int		   n  = 0;
	int		   n2 = 0;
	if(dir == NULL) return -1;

	while((d = ppr_readdir(dir)) != NULL) n++;

	ppr_closedir(dir);

	if((dir = ppr_opendir(dirname)) == NULL) return -1;

	*namelist = malloc(sizeof(**namelist) * n);

	while(n2 < n && (d = ppr_readdir(dir)) != NULL) {
		if(selectfn != NULL && !selectfn(d)) continue;

		(*namelist)[n2] = malloc(sizeof(***namelist));
		memcpy((*namelist)[n2], d, sizeof(*d));

		n2++;
	}

	ppr_closedir(dir);

	qsort(*namelist, n2, sizeof(**namelist), (int (*)(const void* d1, const void* d2))compar);

	return n2;
}

int ppr_alphasort(const struct ppr_dirent** d1, const struct ppr_dirent** d2) {
	return strcmp((*d1)->d_name, (*d2)->d_name);
}

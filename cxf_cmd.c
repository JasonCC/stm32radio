/**
 * File 			: cxf_cmd.c
 * This file is my test command.
 * file list.
 * Author			: Cai XiangFeng
 * 
 * Change Logs:
 * Date 		Author 			Notes
 * 2014-09-20 	Jason Cai 		first version
 */
/*#if defined(RTGUI_USING_DFS_FILERW) || defined(RTGUI_USING_STDIO_FILERW)*/
#include <dfs_posix.h>
#define PATH_SEPARATOR		'/'
#include <string.h>
#include <finsh.h>

/**
 * List files in given directory
 */
void cxf_list_dirs(const char *directory) 
{
	DIR *dir;
	rt_uint32_t count;
	struct dirent *dirent;

	if (directory == RT_NULL) {
		directory = "/";
	}
	
	dir = opendir(directory);
	if (dir == RT_NULL) {
		rt_kprintf("[CXF] error: opendir\r\n");
		return;
	}
	
	do {
		dirent = readdir(dir);
		if (dirent == RT_NULL) break;
		rt_kprintf("[CXF] d_name: %s\r\n", dirent->d_name);
		if (strcmp(dirent->d_name, ".") == 0) continue;
		if (strcmp(dirent->d_name, "..") == 0) continue;
		++count;
	} while (dirent != RT_NULL);
	
	rt_kprintf("[CXF] %d files in %s\r\n", count, directory);
	
	closedir(dir);
}
FINSH_FUNCTION_EXPORT(cxf_list_dirs, list files in given directory);
/*#endif*/

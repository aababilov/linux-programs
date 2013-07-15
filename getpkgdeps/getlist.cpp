#define _XOPEN_SOURCE

#include <cstdio>
#include <sys/select.h>
#include <stdio.h>      
#include <stdlib.h>	
#include <errno.h>	
#include <termios.h>	
#include <sys/types.h>
#include <sys/wait.h>	
#include <sys/ioctl.h> 
#include <string.h>	
#include <signal.h>	
#include <sys/stat.h>	
#include <ctype.h>	
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <list>

#include "getlist.hpp"

using namespace std;

namespace {
	char *line_in = NULL;
	size_t len_in = 0;
	list<string> pkglist;
	
	void clear_zombie(int no)
	{
		waitpid(-1, NULL, 0);		
	}

	bool set_clear_zombie()
	{
		struct sigaction actn = {0};
		actn.sa_handler = clear_zombie;
		sigaction(SIGCHLD, &actn, 0);
		return true;
	}

	bool zombie_clear_call = set_clear_zombie();

	void readlist(std::set<std::string> &pkgset, FILE *input, bool depends)
	{
		char *running, *name;

		getline(&line_in, &len_in, input);
		running = line_in;	
		while ((name = strsep(&running, ",|")) != NULL) {
			while (isspace(*name))
				++name;
			if (!isalpha(*name))
				continue;
			for (char *t = name; *t; ++t)
				if (isspace(*t)) {
					*t = '\0';
					break;
				}
			if (pkgset.count(name)) {
				printf("package `%s' in already pushed\n", name);
				continue;
			}
			if (depends) {
				pkgset.insert(name);
				pkglist.push_back(name);
				printf("package `%s' pushed\n", name);
			} else {
				pkgset.insert(name);
				pkglist.push_back(name);
				
				printf("package `%s' in recommended\n", name);
			}
		}
	}
}

void getlist(const string &pkgname, std::set<std::string> &pkgset)
{
	FILE *input;
	static char buf[1024];

	pkglist.clear();
	pkglist.push_back(pkgname);
	while (!pkglist.empty()) {
		printf("about %s\n", pkglist.front().c_str());

		snprintf(buf, sizeof(buf), "dpkg-query "
			 "-f '${Status}\n${Depends}\n${Suggests}, ${Recommends}\n' -W %s",
			 pkglist.front().c_str());

		pkglist.pop_front();
		input = popen(buf, "r");
		if (!input) {
			fprintf(stderr, "Could not run dpkg-query.\n");
			return;
		}
	
		getline(&line_in, &len_in, input);

		if (strncmp("install", line_in, 7) != 0) {
			printf("package `%s' in not installed\n", pkgname.c_str());
			goto cleanup;
		}
		readlist(pkgset, input, true);
		readlist(pkgset, input, false);
	
	cleanup:
		pclose(input);
	}
}

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
#include <set>
#include <deque>
#include <map>
#include <string>

#ifdef __GNUC__
#include <ext/hash_map>

using namespace __gnu_cxx;
namespace __gnu_cxx{

	template<>
	struct hash<std::string>
	{
		size_t
		operator()(const std::string &__s) const
			{ return hash<const char*>()(__s.c_str()); }
	};
}
#endif

#include "getlist2.hpp"

using namespace std;

#define DPKG_INFO_FILE "/var/lib/dpkg/status"

typedef deque<string> smpl_pkglist_t;
typedef deque<smpl_pkglist_t> cplx_pkglist_t;

#define ARRAYSIZE(a) (sizeof(a) / sizeof(a[0]))

class debpackage {
public:
	enum info_section { isProvides,
			    
			    isDepends,			    
			    isRecommends,
			    isSuggests,
			    
			    isPackage,
			    isStatus,
			    isPriority,
			    isOther,
			    isSpace};

	debpackage() : required_(false), taken_(false) {}
	string &name() { return name_; }
	const string &name() const { return name_; }

	cplx_pkglist_t &deps(info_section i) { return deps_[i]; }
	const cplx_pkglist_t &deps(info_section i) const { return deps_[i]; }

	bool &required() { return required_; }
	const bool &required() const { return required_; }

	bool &taken() const { return taken_; }
	
	void print(FILE *f) const;

	static void load_db();
	static const debpackage *byname(const string &name) {
		hash_map<string, debpackage*>::iterator i = pack_byname.find(name);
		return (i != pack_byname.end()) ? i->second : NULL;
	}		
	static const list<debpackage> &getlist()
		{ return packages; }
private:
	void readlist(info_section type);
	bool read(FILE *f);

	string name_;
	cplx_pkglist_t deps_[4];
	bool required_;
	mutable bool taken_;

	static info_section getsectname(char *s);
	static inline void readline();
	
	static FILE *pack_db;
	static list<debpackage> packages;
	static hash_map<string, debpackage*> pack_byname;
	static char *line_in;
	static size_t len_in;
};

FILE *debpackage::pack_db;
list<debpackage> debpackage::packages;
hash_map<string, debpackage*> debpackage::pack_byname;
char *debpackage::line_in;
size_t debpackage::len_in;

namespace {	
	bool isprefix(const char *s1, const char *s2)
	{
		--s1;
		--s2;
		while (*++s1 == *++s2)			
			if (*s2 == '\0')
				return true;
		return *s2 == '\0';
	}
	char *trim(char *line)
	{
		while (!isalpha(*line))
			++line;
		char *t = line;
		while (*t && !isspace(*t))
			++t;
		*t = '\0';
		return line;
	}
}

inline void debpackage::readline()
{
	getline(&line_in, &len_in, pack_db);
}

debpackage::info_section debpackage::getsectname(char *s)
{
#define DECL_SECT(name) { is##name, #name}
	static const struct {
		info_section id;
		const char *name;
	} sects[] = {
		DECL_SECT(Package),
		DECL_SECT(Status),
		DECL_SECT(Priority),
		DECL_SECT(Depends),
		DECL_SECT(Provides),
		DECL_SECT(Recommends),
		DECL_SECT(Suggests)
	};
	if (s[0] =='\n' || s[0] == '\0')
		return isSpace;
	if (isprefix(s, "Pre-Depends"))
	    return isDepends;      

	for (int i = 0; i < ARRAYSIZE(sects); ++i)
		if (isprefix(s, sects[i].name))
			return sects[i].id;      
	return isOther;
}

void debpackage::readlist(info_section type)
{
	char *running, *depnode;
	running = line_in;
	do {
		if (!*running)
			return;
		++running;
	} while (*running != ':');
	++running;
	while ((depnode = strsep(&running, ",")) != NULL) {
		char *running1 = depnode, *name;
		while (isspace(*depnode))
			++depnode;
		if (!isalpha(*depnode))
			continue;
		deps(type).push_back(smpl_pkglist_t());
		smpl_pkglist_t &node = deps(type).back();
		while ((name = strsep(&running1, ",")) != NULL) {
			while (isspace(*name))
				++name;
			
			if (!isalpha(*name))
				continue;
			for (char *t = name; *t; ++t)
				if (isspace(*t)) {
					*t = '\0';
					break;
				}
			node.push_back(name);
		}
	}
}

bool debpackage::read(FILE *f)
{
	do {
		readline();
		if (feof(f))
			return false;
	} while (getsectname(line_in) != isPackage);
	name() = trim(line_in + 7);
	
	do {
		readline();
		if (feof(f))
			return false;
	} while (getsectname(line_in) != isStatus);	
	if (!isprefix(trim(line_in + 6), "install")) {
		return false;
	}
	printf("%s\n", name().c_str());
	for (;;) {
		readline();
		switch (info_section sect = getsectname(line_in)) {
		case isSpace:
			return true;
		case isOther:
			break;
		case isPriority:
			required() = isprefix(line_in + strlen("Priority: "),
						  "required");
			break;
		default:
			readlist(sect);
		}
	}
}

void debpackage::print(FILE *f) const
{
	fprintf(f, "package %s\n", name().c_str());
	if (required())
		fprintf(f, "\treq\n");	
}

void debpackage::load_db()
{
	packages.clear();
	pack_db = fopen(DPKG_INFO_FILE, "rt");
	if (!pack_db) {		
		fprintf(stderr, "Could not open %s\n", DPKG_INFO_FILE);
		return;
	}

	for (;;) {
		debpackage pack;
		if (pack.read(pack_db)) {
			packages.push_back(pack);
			debpackage *p = &packages.back();
			pack_byname[pack.name()] = p;
			for (cplx_pkglist_t::const_iterator si = pack.deps(isProvides).begin();
			     si != pack.deps(isProvides).end();
			     ++si) {
				pack_byname[*si->begin()] = p;
			}
		}
		if (feof(pack_db))
			break;
	}
	free(line_in);
	exit(0);
}

void getlist2(std::vector<std::string> &need_pack) {
	debpackage::load_db();

	for (list<debpackage>::const_iterator i = debpackage::getlist().begin();
	     i != debpackage::getlist().end();
	     ++i) {
//		i->print(stdout);
	}

	for (int i = 0; i < need_pack.size(); ) {
		const debpackage *p = debpackage::byname(need_pack[i]);
		if (p == NULL) {
			fprintf(stderr, "package `%s' not installed!\n", need_pack[i].c_str());
			need_pack.erase(need_pack.begin() + i);
		} else {
			p->taken() = true;
			++i;
		}
	}
	for (list<debpackage>::const_iterator i = debpackage::getlist().begin();
	     i != debpackage::getlist().end();
	     ++i) {
		if (i->required() && !i->taken()) {
//			printf("req: %s\n", i->name().c_str());
			need_pack.push_back(i->name());
			i->taken() = true;
		}
	}

	int firstq = 0;
	while (firstq < need_pack.size()) {
		string curname = need_pack[firstq];
		const debpackage &curr = *debpackage::byname(need_pack[firstq]);
		++firstq;
		if (&curr == NULL)
			continue;

		const cplx_pkglist_t &deplist = curr.deps(debpackage::isDepends);
		for (cplx_pkglist_t::const_iterator si = deplist.begin();
		     si != deplist.end();
		     ++si) {
			for (smpl_pkglist_t::const_iterator ai = si->begin();
			     ai != si->end();
			     ++ai) {				
				const debpackage *p = debpackage::byname(*ai);
				if (p == NULL) {
					fprintf(stderr, "wow! not installed %s!\n", ai->c_str());
					continue;
				}
					
				if (!p->taken()) {
					need_pack.push_back(*ai);
					fprintf(stderr, "%s\n", (*ai + " because " + curname).c_str());
					p->taken() = true;
				}
			}				
		}
	}

	for (int i = 0; i < need_pack.size(); ++i)
		need_pack[i] = debpackage::byname(need_pack[i])->name();
}

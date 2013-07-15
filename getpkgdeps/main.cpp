#include "getlist.hpp"
#include "getlist2.hpp"

using namespace std;

int main(int argc, char *argv[])
{
	vector<string> deps;
    
	for (int i = 1; i < argc; ++i)
		deps.push_back(argv[i]);
	getlist2(deps);
	
	for (vector<string>::const_iterator i = deps.begin();
	     i != deps.end();
	     ++i) {
		printf("%s\n", i->c_str());
	}
}

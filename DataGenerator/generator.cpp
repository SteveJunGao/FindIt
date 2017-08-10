#include "generator.h"

static vector<FILE *> OpenAllFiles(const char *path)
{
	char pattern[260], *tail;
	WIN32_FIND_DATA file;
	HANDLE find;
	vector<FILE *> fins;

	strcpy(pattern, path);
	tail = pattern + strlen(pattern);
	if (tail[-1] != '\\')
		*tail++ = '\\';
	strcat(pattern, "*.txt");
	if ((find = FindFirstFile(pattern, &file)) == INVALID_HANDLE_VALUE)
		panic("Cannot find %s", pattern);
	do {
		char fullname[260];
		strcpy(fullname, path);
		tail = fullname + strlen(fullname);
		if (tail[-1] != '\\')
			*tail++ = '\\';
		strcat(fullname, file.cFileName);
		fins.push_back(fopen(fullname, "r"));
	} while (FindNextFile(find, &file));
	FindClose(find);
	return fins;
}

Generator::Generator(const char *path, int min_cnt, int max_cnt, double noise)
{
	uniform_int_distribution<int> char_gen(33, 126);
	vector<FILE*> fins;
	int total_len = 0;

	fins = OpenAllFiles(path);
	for (int i = 0; i < fins.size(); i++) {
		fseek(fins[i], 0, SEEK_END);
		total_len += ftell(fins[i]);
		fseek(fins[i], 0, SEEK_SET);
	}
	raw = new char[total_len];
	char *curr = raw, *tail = raw + 1;

	vector<int> line;
	line.push_back(curr - raw);
	*curr++ = ' ';
	for (int i = 0; i < fins.size(); i++) {
		while (!feof(fins[i])) {
			fgets(curr, raw + total_len - curr, fins[i]);
			if (strcmp(curr, "\n")) {
				for (; *curr; curr++)
				if (*curr == ' ')
					line.push_back(curr - raw);
				if (curr[-1] == '\n') curr--; // remove '\n' when next fgets()
				if (curr > raw && curr[-1] != ' ') {
					line.push_back(curr - raw);
					*curr++ = ' ';
				}
			}
			else {
				if (line.size() >= min_cnt && line.size() <= max_cnt) {
					blanks.push_back(line);
					tail = curr;
				}
				else
					curr = tail;
				line.clear();
				line.push_back(curr - raw - 1);
			}
			assert(curr[-1] == ' ');
		}
		fclose(fins[i]);
	}
	
	corrupted = new char[total_len];
	strcpy(corrupted, raw);
	for (int i = 0; i < blanks.size(); i++) {
		for (int j = 0; j < blanks[i].size(); j++) {
			assert(corrupted[blanks[i][j]] == ' ');
			if (probability(engine) < noise && corrupted[blanks[i][j] + 1] != ' ')
				corrupted[blanks[i][j] + 1] = char_gen(engine);
		}
		if (i + 1 < blanks.size())
			assert(blanks[i][blanks[i].size() - 1] == blanks[i + 1][0]);
	}
}

void Generator::Generate(FILE *fout, int insert, int remove, int query)
{
	uniform_int_distribution<int> pattern_gen(0, patterns.size() - 1), query_gen(0, blanks.size() - 1);
	vector<pair<char, String>> output;

	while (insert--) output.push_back(make_pair('A', patterns[pattern_gen(engine)]));
	while (remove--) output.push_back(make_pair('D', patterns[pattern_gen(engine)]));
	for (int i = 0; i < output.size(); i++)
		fprintf(fout, "%c %.*s\n", output[i].first, output[i].second.length, output[i].second.s);
	while (query--) {
		int q = query_gen(engine);
		fprintf(fout, "Q %.*s\n", blanks[q][blanks[q].size() - 1] - blanks[q][0] - 1,
			raw + blanks[q][0] + 1);
	}
	fprintf(fout, "E\n");
}
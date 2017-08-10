#include "evaluator.h"

void Evaluator::Run(const char *input_file, const char *output_file, const char *answer_file)
{
	LARGE_INTEGER cycles, frequency;
	unsigned char *input;
	FILE *output, *answer;
	vector<String> batch;
	vector<vector<String> > results;
	unsigned char *begin, *end;
	int batchID = 0;
	char *output_buf = new char[MAX_LINE_LEN], *answer_buf = new char[MAX_LINE_LEN];

	HANDLE file, mapping;
	UInt64 length;
	// Map input so that String can refer to them
	if ((file = CreateFile(input_file, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL))
		== INVALID_HANDLE_VALUE)
		panic("File `%s` cannot be opened.", input_file);
	length.part[0] = GetFileSize(file, &length.part[1]);
	if ((mapping = CreateFileMapping(file, NULL, PAGE_READONLY, 0, 0, NULL))
		== NULL)
		panic("File mapping cannot be opend.");
	if ((input = (unsigned char *)MapViewOfFile(mapping, FILE_MAP_COPY, 0, 0, 0)) // Copy-On-Write
		== NULL)
		panic("Map view of file cannot be established.");
	CloseHandle(file);
	CloseHandle(mapping);

	output = output_file ? fopen(output_file, "w") : NULL;
	answer = answer_file ? fopen(answer_file, "r") : NULL;

	QueryPerformanceFrequency(&frequency);
	cycles.QuadPart = 0;

	// Assume line is in Windows style (aka "\r\n")
	for (begin = input; begin < input + length.all; begin = end + 1) {
		for (end = begin; *end != '\n'; end++);
		end[-1] = ' ';
		if (*begin == 'E') {
			LARGE_INTEGER start, stop;

			QueryPerformanceCounter(&start);
			results = solver->RunBatch(batch);
			QueryPerformanceCounter(&stop);
			cycles.QuadPart += stop.QuadPart - start.QuadPart;

			char *p = output_buf;

			*p = 0;
			for (size_t q = 0; q < results.size(); q++) {
				if (results[q].empty())
					p += sprintf(p, "-1\n");
				else {
					p += sprintf(p, "%.*s", results[q][0].length - 2, results[q][0].s + 1);
					for (size_t i = 1; i < results[q].size(); i++)
						p += sprintf(p, "|%.*s", results[q][i].length - 2, results[q][i].s + 1);
					p += sprintf(p, "\n");
				}
			}
			if (output)
				fwrite(output_buf, sizeof(char), p - output_buf, output);

			if (answer) {
				int queryID = 0, patternID = 0;

				int len = fread(answer_buf, sizeof(char), p - output_buf, answer);
				for (int i = 0; i < len; i++)
					if (answer_buf[i] != output_buf[i])
						panic("batch %d, query %d, pattern %d: not match.", batchID, queryID, patternID);
					else if (answer_buf[i] == '|')
						patternID++;
					else if (answer_buf[i] == '\n') {
						queryID++;
						patternID = 0;
					}
			}
			batch.clear();
			batchID++;
		}
		else
			batch.push_back(String(begin, end - begin));
	}
	UnmapViewOfFile(input);
	if (output_file)
		fclose(output);
	if (answer_file)
		fclose(answer);
	delete[] output_buf;
	delete[] answer_buf;

	printf("Input: %s, Model: %s, Time elapsed: %.3f s\n",
		input_file, typeid(*solver).name(), (float)cycles.QuadPart / frequency.QuadPart);
}
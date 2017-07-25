#include "Utility.h"

namespace SRay
{
	namespace Utility
	{

		// http://stackoverflow.com/questions/122616/how-do-i-trim-leading-trailing-whitespace-in-a-standard-way
		char *trimwhitespace(char *str)
		{
			char *end;

			// Trim leading space
			while (isspace(*str)) str++;

			if (*str == 0)	// All spaces?
				return str;

			// Trim trailing space
			end = str + strlen(str) - 1;
			while (end > str && isspace(*end)) end--;

			// Write new null terminator
			*(end + 1) = 0;

			return str;
		}

		size_t trimwhitespace(char *out, size_t len, const char *str)
		{
			if (len == 0)
				return 0;

			const char *start = str;
			const char *end;
			size_t out_size;

			// Trim leading space
			while (str<(start + len) && isspace(*str)) str++;

			if (*str == 0)	// All spaces?
			{
				*out = 0;
				return 1;
			}

			// Trim trailing space
			end = str + strlen(str) - 1;
			while (end > str && isspace(*end)) end--;
			end++;

			// Set output size to minimum of trimmed string length and buffer size minus 1
			out_size = uint((end - str)) < len - 1 ? (end - str) : len - 1;

			// Copy trimmed string and add null terminator
			//if (start != out)
			memcpy(out, str, out_size);
			out[out_size] = 0;

			return out_size;
		}

#if defined(_DEBUG) || defined(SR_LOG)
		int debug_log(const char * format, ...)
		{
			char tmpBuf[4096];
			va_list arg;
			va_start(arg, format);
			vsprintf(tmpBuf, format, arg);
			printf(tmpBuf);
			FILE *ofile = fopen("log.txt", "a+");
			if (ofile)
			{
				fprintf(ofile, tmpBuf);
				fclose(ofile);
			}
			va_end(arg);
			return 0;
		}
#endif

		char* ParseFloat(const char *str, double *out)
		{
			char *end, tmp;
			while ((*str != 0) && (!isdigit(*str) && (*str != '-') && (*str != '+'))) str++;
			end = const_cast<char*>(str);
			while ((*end != 0) && (isdigit(*end) || (*end == '.') || (*end == 'e') || (*end == 'E') || (*end == '-') || (*end == '+'))) end++;

			tmp = *end;
			*end = 0;
			*out = atof(str);
			*end = tmp;
			return end;
		}

		char* ParseInt(const char *str, int *out)
		{
			char *end, tmp;
			while ((*str != 0) && (!isdigit(*str) && (*str != '-') && (*str != '+'))) str++;
			end = const_cast<char*>(str);
			while ((*end != 0) && (isdigit(*end) || (*end == '-') || (*end == '+'))) end++;

			tmp = *end;
			*end = 0;
			*out = atoi(str);
			*end = tmp;
			return end;
		}

		char* ParseTripletFloat(const char *str, double *out)
		{
			uchar cmpt = 0;
			while ((*str != 0) && (cmpt++ < 3))
				str = ParseFloat(str, &out[cmpt - 1]);
			return const_cast<char*>(str);
		}

		char* ParseTripletInt(const char *str, int *out)
		{
			uchar cmpt = 0;
			while ((*str != 0) && (cmpt++ < 3))
				str = ParseInt(str, &out[cmpt - 1]);
			return const_cast<char*>(str);
		}

		//http://stackoverflow.com/questions/2661766/c-convert-a-mixed-case-string-to-all-lower-case#2661788
		void strtolower(char *p)
		{
			for (; *p; ++p) if (*p >= 'A' && *p <= 'Z') TOLOWER(*p);// tolower(*p);
		}
		void strtoupper(char *p)
		{
			for (; *p; ++p) if (*p >= 'a' && *p <= 'z') TOUPPER(*p);// toupper(*p);
		}

	}
}

#pragma once
#include <string>
#include <vector>
#include <sstream>
#include "D2Ptrs.h"

void Tokenize(const std::string& str, std::vector<std::string>& tokens, const std::string& delimiters);
wchar_t* AnsiToUnicode(const char* str);
char* UnicodeToAnsi(const wchar_t* str);
wchar_t* GetColorCode(int ColNo);
template <class T>
bool from_string(T& t, 
                 const std::string& s, 
                 std::ios_base& (*f)(std::ios_base&))
{
  std::istringstream iss(s);
  return !(iss >> f >> t).fail();
}

template< class type> std::string to_string( const type & value)
{ std::stringstream ss; ss << value; return ss.str(); }

bool IsTrue(const char *str);
bool StringToBool(std::string str);
int StringToNumber(std::string str);

std::string Trim(std::string source);

void PrintText(DWORD Color, char *szText, ...);

struct KeyCode {
	std::string name;
	unsigned int value;
	std::string literalName;
};

KeyCode GetKeyCode(unsigned int nKey);
KeyCode GetKeyCode(const char* name);
ULONGLONG BHGetTickCount(void);

std::string string_format(const std::string fmt_str, ...);


VOID *memcpy2(void *dest, const void *src, size_t count);
HANDLE OpenFileRead(char *filename);
BYTE *AllocReadFile(char *filename);
DWORD ReadFile(HANDLE hFile, void *buf, DWORD len);
char *GetMyFileNameStrrchr(char *dest, char ch);

long CalculateDistance(const POINT& pt1, const POINT& pt2);
long CalculateAngle(const POINT& pt1, const POINT& pt2);
long CalculateDistance(long x1, long y1, long x2, long y2);
BOOL PtInCircle(const POINT& pt, const POINT& ptOrigin, int nRadius);
void NormalizeAngle(int& rAngle);
void NormalizeRect(RECT& rRect);
void NormalizeRect(LPRECT lpRect);
long CalculateAngle(long x1, long y1, long x2, long y2);
POINT CalculatePointOnTrack(const POINT& ptOrigin, int nRadius, int nAngle);
POINT CalculateRandomPosition(const POINT& ptOrigin, int nRadiusMin, int nRadiusMax, int nAngleMin/*=0*/, int nAngleMax/*=360*/);
char *commaprint(unsigned long n);

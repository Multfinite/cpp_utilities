#ifndef UTILITIES_CMD_LINE_PARSER_HPP
#define UTILITIES_CMD_LINE_PARSER_HPP

#include <tchar.h>
#include <exception>

#define QUOTE_CHAR '"'
#define DASH_SYMBOL '-'
#define SPACE_SYMBOL ' '
#define END_OF_STRING_SYMBOL '\0'

/*
* @author multfinite
* @github https://github.com/multfinite
*/
struct Argument
{	
	// -%FLAG%
	TCHAR* Prefix;
	// array pointer
	TCHAR** Parameters; 
	// count of elements
	size_t ParameterCount; 

	TCHAR* Src;

	~Argument()
	{
		delete[] Src;

		delete[] Prefix;
	    for(size_t index = 0; index < ParameterCount; index++)
	    {
			delete[] Parameters[index];
	    }
		delete[] Parameters;
		ParameterCount = 0;
	}
};

/*
* @author multfinite
* @github https://github.com/multfinite
*/
class ArgumentMap final
{
public:
	struct StringIsEmptyException final : public std::exception{};
    struct ExcpectedFlagAtBeginingException final : public std::exception {};
    //struct AnyFlagNotFoundException final : public std::exception {};
    struct ExpectedSpaceBeforeFlagException : public std::exception {};
    struct OutOfRangeException : public std::exception {};
    struct ItemNotFoundException : public std::exception {};
    struct InvalidQuotesException : public std::exception{};

private:
	struct IndexesTable
	{
		size_t bIndex;
		size_t eIndex;
	};

	size_t _count;
	Argument* _freeParameters = nullptr;
	Argument** _arguments = nullptr;

	bool _hasFreeParameters;
public:
	static Argument* ArgFromString(TCHAR* const str)
	{
		// normalize string

		size_t const len = _tcslen(str);
				
		TCHAR* first = str;
		size_t bIndex = 0;
		size_t eIndex = len - 1;
		TCHAR* last = first + eIndex;


		for(TCHAR* chr = first; chr <= last; chr++)
		{
			TCHAR const ch = *chr;

			if (ch != SPACE_SYMBOL)
				break;
			first++;
			bIndex++;
		}

		if (first == last)
			throw StringIsEmptyException();

		for(TCHAR* chr = last; chr > first; chr++)
		{
			TCHAR const ch = *chr;

			if (ch != SPACE_SYMBOL)
				break;
			last--;
			eIndex--;
		}
		
		size_t const distance = eIndex - bIndex + 1;
		size_t const dCount = distance * sizeof(TCHAR);

		// normalization end

		// count elements and fill up indexes table.
		// count

		size_t elements = 0;
		{
			bool isElement = false;
			bool quote = false;

			for (TCHAR* chr = first; chr <= last; chr++)
			{
				TCHAR const ch = *chr;

				if (ch == QUOTE_CHAR)
					quote = !quote;

				if(quote || ch != SPACE_SYMBOL)
				{
					if (!isElement)
					{
						isElement = true;
						elements++;
						continue;
					}
				}

				if(ch == SPACE_SYMBOL && isElement)
					isElement = false;
			}
		}
		
		// count end

		// fill up table

		IndexesTable** indexes = new IndexesTable*[elements];
		{
			size_t idx = 0;

			for (size_t index = 0; index < elements; index++)
			{
				IndexesTable* table = new IndexesTable();

				bool quote = false;
				bool quotePrev = false;

				bool expectedTill = false;
				
				for (TCHAR* chr = first + idx; chr <= last; chr++)
				{
					TCHAR const ch = *chr;

					if (ch == QUOTE_CHAR)
						quote = !quote;

					if ((!quote && ch == SPACE_SYMBOL) || chr == last)
					{
						if (expectedTill)
						{
							table->eIndex = idx;
							if (chr == last)
								table->eIndex++;

							//expectedTill = false;

							idx++;

							break;
						}
					}				

					if(ch != SPACE_SYMBOL && !expectedTill)
					{
						table->bIndex = idx;

						expectedTill = true;
					}

					idx++;
				}

#ifdef _DEBUG
				size_t const d = table->eIndex - table->bIndex;
				size_t const dc = d * sizeof(TCHAR);
				TCHAR* const s = new TCHAR[d + 1];
				s[d] = END_OF_STRING_SYMBOL;
				memcpy(s, first + table->bIndex, dc);
				delete[] s;
#endif

				indexes[index] = table;
			}	
		}

		// table process end

		// setup argument

		Argument* const argument = new Argument();

		// prefix

		{
			IndexesTable* const pTable = indexes[0];

			size_t const pDistance = pTable->eIndex - pTable->bIndex;
			size_t const pdCount = pDistance * sizeof(TCHAR);

			TCHAR* const pFirst = first + pTable->bIndex;

			TCHAR* const prefix = new TCHAR[pDistance + 1];
			prefix[pDistance] = END_OF_STRING_SYMBOL;

			memcpy(prefix, pFirst, pdCount);

			if (*prefix != DASH_SYMBOL)
				throw ExcpectedFlagAtBeginingException();

			argument->Prefix = prefix;
		}

		// prefix end

		// parameters

		argument->ParameterCount = elements - 1;
		if (argument->ParameterCount > 0)
		{
			argument->Parameters = new TCHAR*[argument->ParameterCount];

			for(size_t index = 0; index < argument->ParameterCount; index++)
			{
				IndexesTable* const pTable = indexes[index + 1];
				
				TCHAR* const pFirst = first + pTable->bIndex;

				size_t const pDistance = pTable->eIndex - pTable->bIndex;
				size_t const pdCount = pDistance * sizeof(TCHAR);
				
				TCHAR* parBuf = new TCHAR[pDistance + 1];
				parBuf[pDistance] = END_OF_STRING_SYMBOL;
				
				memcpy(parBuf, pFirst, pdCount);

				if(parBuf[0] == QUOTE_CHAR && parBuf[pDistance - 1] == QUOTE_CHAR)
				{
					TCHAR* pBuf = new TCHAR[pDistance - 2 + 1];
					pBuf[pDistance - 2] = END_OF_STRING_SYMBOL;
					memcpy(pBuf, parBuf + 1, (pDistance - 2) * sizeof(TCHAR));
					delete[] parBuf;
					parBuf = pBuf;
				}

				argument->Parameters[index] = parBuf;
			}
		}
		else
			argument->Parameters = nullptr;

		// parameters end

		// src

		argument->Src = new TCHAR[distance + 1];
		argument->Src[distance] = END_OF_STRING_SYMBOL;
		memcpy(argument->Src, first, dCount);

		// src end

		// clear up

		for(size_t i = 0; i < elements; i++)
		{
			delete indexes[i];
			indexes[i] = nullptr;
		}
		delete[] indexes;

		// clear end

		return argument;
	}
	   
	ArgumentMap(size_t flags, bool hasFreeParameters)
	{
		_hasFreeParameters = hasFreeParameters;

		_count = flags;
		if (_hasFreeParameters)
			_count--;

		if(_count > 0)
		{
			_arguments = new Argument*[_count];
			for (size_t i = 0; i < _count; i++)
			{
				_arguments[i] = nullptr;
			}
		}
	}
	~ArgumentMap()
	{
		if(HasFreeParameters())
		{
			delete _freeParameters;
		}
		if(_count > 0)
		{
			for (size_t i = 0; i < _count; i++)
			{
				delete _arguments[i];
			}
			delete[] _arguments;
		}
	}

	void Initialize(TCHAR** flagBuffer)
	{
		if (_count == 0)
			throw;

		size_t bufLen = _count;
		if (_hasFreeParameters)
			bufLen++;
		
		for(size_t i = 0; i < bufLen; i++)
		{		
			TCHAR* const str = flagBuffer[i];
			Argument* argument = ArgFromString(str);
					

			if (_hasFreeParameters && i == 0)
			{
				_freeParameters = argument;
				continue;
			}

			size_t index = i;
			if (_hasFreeParameters)
				index--;

			_arguments[index] = argument;
		}
	}	

	bool HasFreeParameters() const { return _hasFreeParameters; }
	Argument* FreeParameters() const { return _freeParameters; 	}

	Argument* operator[](TCHAR* prefix) const
	{
		for(size_t index = 0; index < _count; index++)
		{
			Argument* argument = _arguments[index];
			if (_tccmp(prefix, argument->Prefix) == 0)
				return argument;				
		}
		throw ItemNotFoundException();
	}

	// free parameter does not count
	size_t Count() const { return _count; }

	Argument* At(size_t index) const
	{
		if (!(index < _count))
			throw OutOfRangeException();

		return _arguments[index];
	}
};

inline ArgumentMap* ParseArguments(TCHAR* args)
{
	size_t len = _tcslen(args);
	TCHAR* argStr = args;

	if (len == 0)
		throw ArgumentMap::StringIsEmptyException();
	{
		bool hasSymbols = false;
		for (TCHAR* chr = argStr; chr < argStr + len; chr++)
		{
			const TCHAR ch = *chr;
			if(ch != SPACE_SYMBOL)
			{
				hasSymbols = true;
				break;
			}
		}
		if(!hasSymbols)
			throw ArgumentMap::StringIsEmptyException();
	}	

	// cut off spaces

	size_t bIndex = 0;
	size_t count = len;

	for (TCHAR* chr = argStr; chr < argStr + len; chr++)
	{
		const TCHAR ch = *chr;
		if (ch != SPACE_SYMBOL)
			break;
		bIndex++;
	}

	for (TCHAR* chr = argStr + len - 1; chr >= argStr; chr--)
	{
		const TCHAR ch = *chr;
		if (ch == SPACE_SYMBOL)
			count--;
		else
			break;
	}

	TCHAR* first = argStr + bIndex;
	size_t eIndex = bIndex + count;
	TCHAR* last = argStr + eIndex;

	// determine free parameters

	bool const freeParametersHere = *first != DASH_SYMBOL;

	if(freeParametersHere)
	{
		len = count + 4;
		argStr = new TCHAR[count + 4 + 1];
		argStr[0] = DASH_SYMBOL;
		argStr[1] = 'F';
		argStr[2] = 'P';
		argStr[3] = SPACE_SYMBOL;
		argStr[len] = END_OF_STRING_SYMBOL;

		memcpy(argStr + 4, first, count);

		count = len;
		first = argStr;
		bIndex = 0;
		eIndex = count - 1;
		last = first + eIndex;
	}

	// determination end

	// cut off end

	// count quotes
	{
		size_t quoteCount = 0;

		for (TCHAR* chr = first; chr <= last; chr++)
		{
			TCHAR const ch = *chr;
			if (ch == QUOTE_CHAR)
				quoteCount++;
		}

		if (quoteCount % 2 != 0)
			throw ArgumentMap::InvalidQuotesException();
	}
	// count end

	// count the flags

	size_t flags = 0;

	// first + 1 - there are not any reason to check first symbol.
	{
		bool quote = false;

		for (TCHAR* chr = first; chr <= last; chr++)
		{
			const TCHAR ch = *chr;
			if (ch == QUOTE_CHAR)
				quote = !quote;

			if (quote)
				continue;

			if (ch == DASH_SYMBOL)
			{
				TCHAR const prevCh = *(chr - 1);
				if ((prevCh != SPACE_SYMBOL) && (chr != first))
					continue;
					//throw ArgumentMap::ExpectedSpaceBeforeFlagException();

				flags++;
			}
		}
	}
	
	// count end

	ArgumentMap* map = new ArgumentMap(flags, freeParametersHere);

	// Part command line to format -%FLAG% [Parameters]
	if (flags > 0)
	{
		// create buffer to store flags.

		TCHAR** flagBuffer = new TCHAR*[flags];

		{
			TCHAR* actual = first + 1;
			size_t actualIndex = bIndex + 1;
			size_t bDash = 0;
			size_t eDash = 0;

			for (size_t i = 0; i < flags; i++)
			{
				size_t distance;
				size_t dCount;
				TCHAR* buf;

				if (i == flags - 1)
				{
					distance = eIndex - bDash + 1; // plus end of string
					dCount = distance * sizeof(TCHAR);
					buf = new TCHAR[distance];
					buf[distance] = END_OF_STRING_SYMBOL;
					memcpy(buf, first + bDash, dCount);
				}
				else
				{
					bool quote = false;

					for (TCHAR* chr = actual; chr <= last; chr++)
					{
						const TCHAR ch		= *chr;
						const TCHAR pch	= *(chr-1);

						if (ch == QUOTE_CHAR)
							quote = !quote;

						if (!quote && pch == SPACE_SYMBOL && ch == DASH_SYMBOL)
						{
							eDash = actualIndex;
							actual = chr;
							break;
						}

						actualIndex++;
					}

					distance = (eDash - bDash) - 1 + 1; // without last symbol, that is dash and next beginning symbol. and plust end of string.
					dCount = distance * sizeof(TCHAR);
					buf = new TCHAR[distance];
					buf[distance] = END_OF_STRING_SYMBOL;
					memcpy(buf, first + bDash, dCount);

					bDash = eDash;
					eDash = 0;

					actualIndex++;
					actual++;
				}

				flagBuffer[i] = buf;
			}
		}

		map->Initialize(flagBuffer);

		//for (size_t i = 0; i < flags; i++)
		//{
		//	TCHAR* flagBufEl = flagBuffer[i];
		//	delete[] flagBufEl;
		//	flagBuffer[i] = nullptr;
		//}
		delete[] flagBuffer;
	}
	// Part end	

	return map;
}

#ifdef _UNICODE
#include <Windows.h>

wchar_t* ToMultiByte(char* str)
{
	size_t const nChars = MultiByteToWideChar(CP_ACP, 0, str, -1, nullptr, 0);
	wchar_t* wstr = new wchar_t[nChars];
	MultiByteToWideChar(CP_ACP, 0, str, -1, wstr, nChars);
	return wstr;
}
#endif

inline TCHAR* Combine(int const argc, char** argv)
{
	if (argc > 1)
	{
		size_t len = 0;
		for (int i = 1; i < argc; i++)
		{
			size_t const l = strlen(argv[i]);
			len += l;
			if (argv[i][0] == DASH_SYMBOL)
				len += 3; // 4
		}
		len += ((argc - 1) - 1) + 1;
		char* str = new char[len];

		size_t lastIndex = 0;
		for (int i = 1; i < argc; i++)
		{
			const char* arg = argv[i];
			
			if(arg[0] == DASH_SYMBOL)
			{
				size_t const l = strlen(arg);

				char* buf = new char[l + 1];
				memcpy(buf, arg, l);
				buf[l] = SPACE_SYMBOL;

				memcpy(str + lastIndex, buf, l + 1);
				lastIndex += l + 1;

				delete[] buf;
			}
			else
			{
				size_t const l = strlen(arg) + 3;//+ 4;		

				char* buf = new char[l + 1 + 1];

				buf[0] = SPACE_SYMBOL;
				buf[1] = QUOTE_CHAR;

				memcpy(buf + 2, arg, l);

				buf[l - 1] = QUOTE_CHAR;
				buf[l] = SPACE_SYMBOL;
				buf[l + 1] = END_OF_STRING_SYMBOL;

				memcpy(str + lastIndex, buf, l + 1);

				lastIndex += l + 1;

				delete[] buf;
			}		

		}

		str[len - 1] = END_OF_STRING_SYMBOL;

#ifdef _UNICODE
		wchar_t wstr = ToMultiByte(str);
		delete[] str;
		return wstr;
#else
		return str;
#endif

	}

	return nullptr;
}

#undef QUOTE_CHAR
#undef DASH_SYMBOL
#undef SPACE_SYMBOL
#undef END_OF_STRING_SYMBOL
#endif //UTILITIES_CMD_LINE_PARSER_HPP
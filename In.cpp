#include <fstream>
#include "stdafx.h"
#include "Error.h"
#include "In.h"
#include "Log.h"

namespace In
{
	IN getin(wchar_t infile[])
	{
		std::ifstream fin(infile);
		if (!fin.is_open()) throw ERROR_THROW(110);
		
		IN ri = {};
		unsigned char symb;

		fin.seekg(0, fin.end);
		int fileSize = fin.tellg();
		fin.seekg(0, fin.beg);

		int curCol  = 0,
			curLine = 0;

		if (fileSize)
		{
			curLine = 1; 
			curCol =  1;
		}


		if (fileSize < IN_MAX_LEN_TEXT) ri.text = new unsigned char[fileSize];
		else
		{
			ri.text = new unsigned char[IN_MAX_LEN_TEXT];
			std::cout << "\nВнимание, обьём исходного кода больше допустимого. Возможна потеря данных.\n";
		}
		ri.size  = 0;
		ri.ignor = 0;

		while (fin.tellg() < fileSize && ri.size < IN_MAX_LEN_TEXT)
		{
			symb = fin.get();

			switch (ri.code[(int)symb])
			{
				case ri.T:
					ri.text[ri.size] = symb;
					ri.size++;
					break;
				case ri.F:
					throw ERROR_THROW_IN(111, curLine, curCol);
					break;
				case ri.I:
					ri.ignor++; 
					break;
				default:
					ri.text[ri.size] = (char)ri.code[(int)symb];
					ri.size++;
					break;
			}

			if (symb == IN_CODE_ENDL)
			{
				curCol = 1;
				curLine++;
			}
			else curCol++;
		}
		ri.text[ri.size] = '\0';
		fin.close();
		ri.lines = curLine;

		CleanCode(ri);

		return ri;
	}

	void CleanCode(IN in)
	{	
		for(int i = 0; i < in.size && i < IN_MAX_LEN_TEXT; i++) // tabs to spaces
			if (in.text[i] == IN_CODE_TAB)
				in.text[i] = IN_CODE_SPACE;

		int codeTable[] = OPERATOR_TABLE;
		for (int i = 0; i < in.size && i < IN_MAX_LEN_TEXT; i++)
		{ 
			if (in.text[i] == IN_CODE_SPACE && in.text[i + 1] == IN_CODE_SPACE)
			{
				DelSymb(in, i);
				i--;
			}

			if (in.text[i] == IN_CODE_QUOTE)
			{
				do
				{
					i++;
				} while (in.text[i] != IN_CODE_QUOTE);
				i++;
			}
			
			for (int j = 0; codeTable[j] != NULL; j++)
			{
				if (in.text[i] == codeTable[j])
				{
					if (in.text[i + 1] == IN_CODE_SPACE)
					{	
						do
						{
							DelSymb(in, i + 1);
						} while (in.text[i + 1] == IN_CODE_SPACE);
					}

					if (in.text[i - 1] == IN_CODE_SPACE)
					{
						do
						{
							DelSymb(in, i - 1);
							i--;
						} while (in.text[i - 1] == IN_CODE_SPACE);
					}

					break;
				}
			}
		}
	}

	void DelSymb(IN in, int index)
	{
		for (int i = index; i < in.size && i < IN_MAX_LEN_TEXT; i++)
		{
			in.text[i] = in.text[i + 1];
		}
		in.size--;
	}
}
/*
*   Copyright (c) 2000-2003, Darren Hiebert
*
*   This source code is released for free distribution under the terms of the
*   GNU General Public License version 2 or (at your option) any later version.
*
*   This module contains functions for generating tags for COBOL language
*   files.
*/

/*
*   INCLUDE FILES
*/
#include "general.h"	/* must always come first */
#include "keyword.h"
#include "parse.h"
#include "routines.h"

typedef enum {
	K_PARAGRAPH,
} cobolKind;

static kindOption CobolKinds[] = {
	{ true, 'p', "paragraph", "paragraphs" },
};

static tagRegexTable cobolTagRegexTable[] = {
	{"^[ \t]*[0-9]+[ \t]+([A-Z0-9][A-Z0-9-]*)[ \t]+("
	 "BLANK|OCCURS|IS|JUST|PIC|REDEFINES|RENAMES|SIGN|SYNC|USAGE|VALUE"
	 ")", "\\1",
	 "d,data,data items", "i"},
	{"^[ \t]*[FSR]D[ \t]+([A-Z0-9][A-Z0-9-]*)\\.", "\\1",
	 "f,file,file descriptions (FD, SD, RD)", "i"},
	{"^[ \t]*[0-9]+[ \t]+([A-Z0-9][A-Z0-9-]*)\\.", "\\1",
	 "g,group,group items", "i"},
	{"^[ \t]*PROGRAM-ID\\.[ \t]+([A-Z0-9][A-Z0-9-]*)\\.", "\\1",
	 "P,program,program ids", "i"},
	{"^[ \t]*([A-Z0-9][A-Z0-9-]*)[ \t]+SECTION\\.", "\\1",
	 "s,section,sections", "i"},
};

typedef enum {
	K,
} cobolKeyword;

static const keywordTable cobolKeywordTable[] = {
	{ "CONTINUE", K },
	{ "END-EXEC", K },
	{ "EXIT", K },
	{ "FILLER", K },
};

/*
*   FUNCTION DEFINITIONS
*/

static void make_tag_for_paragraph_maybe (const char *line,
										  const regexMatch *matches,
										  unsigned int count,
										  void *data)
{
	if (count > 0)
	{
		vString *name = vStringNew ();
		langType cobol = *(langType *)data;

		vStringNCopyS (name, line + matches[1].start, matches[1].length);
		if (lookupCaseKeyword (vStringValue (name), cobol) == KEYWORD_NONE)
			makeSimpleTag (name, CobolKinds, K_PARAGRAPH);
		vStringDelete (name);
	}
}

static void initializeCobolParser (langType language)
{
	static langType cobol;

	cobol = language;

	addCallbackRegex (cobol,
					  "^[ \t]*([A-Z0-9][A-Z0-9-]*)\\.",
					  "{icase}",
					  make_tag_for_paragraph_maybe, NULL, &cobol);
}

extern parserDefinition* CobolParser (void)
{
	static const char *const extensions [] = {
			"cbl", "cob", "CBL", "COB", NULL };
	parserDefinition* def = parserNew ("Cobol");
	def->extensions = extensions;
	def->initialize = initializeCobolParser;
	def->tagRegexTable = cobolTagRegexTable;
	def->tagRegexCount = ARRAY_SIZE (cobolTagRegexTable);
	def->method     = METHOD_NOT_CRAFTED|METHOD_REGEX;
	def->kinds = CobolKinds;
	def->kindCount = ARRAY_SIZE(CobolKinds);
	def->keywordTable = cobolKeywordTable;
	def->keywordCount = ARRAY_SIZE(cobolKeywordTable);
	return def;
}

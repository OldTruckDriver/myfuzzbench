AUTOMAKE_OPTIONS = -Wno-syntax
EXTRA_PROGRAMS = api html regexp schema uri valid xinclude xml xpath
check_PROGRAMS = testFuzzer
EXTRA_DIST = html.dict regexp.dict schema.dict xml.dict xpath.dict fuzz.h
CLEANFILES = $(EXTRA_PROGRAMS)
AM_CPPFLAGS = -I$(top_srcdir)/include -I$(top_builddir)/include
DEPENDENCIES = $(top_builddir)/libxml2.la
LDADD = $(top_builddir)/libxml2.la

XML_MAX_LEN = 80000

testFuzzer_SOURCES = testFuzzer.c fuzz.c

.PHONY: fuzz fuzz-xml fuzz-valid fuzz-xinclude fuzz-html fuzz-api fuzz-regexp fuzz-uri fuzz-schema fuzz-xpath

fuzz: fuzz-xml fuzz-valid fuzz-xinclude fuzz-html fuzz-api fuzz-regexp fuzz-uri fuzz-schema fuzz-xpath

XML fuzzer
xml_SOURCES = xml.c fuzz.c
xml_LDFLAGS = $(AM_LDFLAGS) -fsanitize=fuzzer

fuzz-xml: xml$(EXEEXT)
@mkdir -p corpus/xml
./xml$(EXEEXT) -dict=xml.dict -max_len=$(XML_MAX_LEN) corpus/xml

DTD validation fuzzer
valid_SOURCES = valid.c fuzz.c
valid_LDFLAGS = $(AM_LDFLAGS) -fsanitize=fuzzer

fuzz-valid: valid$(EXEEXT)

@mkdir -p corpus/valid
./valid$(EXEEXT) -dict=xml.dict -max_len=$(XML_MAX_LEN) corpus/valid

XInclude fuzzer
xinclude_SOURCES = xinclude.c fuzz.c
xinclude_LDFLAGS = $(AM_LDFLAGS) -fsanitize=fuzzer

fuzz-xinclude: xinclude$(EXEEXT)
@mkdir -p corpus/xinclude

./xinclude$(EXEEXT) -dict=xml.dict -max_len=$(XML_MAX_LEN) corpus/xinclude

HTML fuzzer
html_SOURCES = html.c fuzz.c
html_LDFLAGS = $(AM_LDFLAGS) -fsanitize=fuzzer

fuzz-html: html$(EXEEXT)
@mkdir -p corpus/html
./html$(EXEEXT) -dict=html.dict -max_len=1000000 corpus/html

API fuzzer
api_SOURCES = api.c fuzz.c
api_LDFLAGS = -fsanitize=fuzzer

fuzz-api: api$(EXEEXT)
@mkdir -p corpus/api
./api$(EXEEXT) -max_len=100 corpus/api

Regexp fuzzer
regexp_SOURCES = regexp.c fuzz.c

regexp_LDFLAGS = $(AM_LDFLAGS) -fsanitize=fuzzer

fuzz-regexp: regexp$(EXEEXT)
@mkdir -p corpus/regexp
./regexp$(EXEEXT) -dict=regexp.dict -max_len=200 corpus/regexp

URI fuzzer
uri_SOURCES = uri.c fuzz.c
uri_LDFLAGS = $(AM_LDFLAGS) -fsanitize=fuzzer

fuzz-uri: uri$(EXEEXT)

@mkdir -p corpus/uri
./uri$(EXEEXT) -max_len=10000 corpus/uri

XML Schema fuzzer
schema_SOURCES = schema.c fuzz.c
schema_LDFLAGS = $(AM_LDFLAGS) -fsanitize=fuzzer

fuzz-schema: schema$(EXEEXT)
@mkdir -p corpus/schema
./schema$(EXEEXT) -dict=schema.dict -max_len=$(XML_MAX_LEN) corpus/schema

XPath fuzzer
xpath_SOURCES = xpath.c fuzz.c
xpath_LDFLAGS = $(AM_LDFLAGS) -fsanitize=fuzzer

fuzz-xpath: xpath$(EXEEXT)
@mkdir -p corpus/xpath
./xpath$(EXEEXT) -dict=xpath.dict -max_len=10000 corpus/xpath
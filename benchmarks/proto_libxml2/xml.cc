// Copyright 2020 Google LLC
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <libxml/hash.h>
#include <libxml/parser.h>
#include <libxml/parserInternals.h>
#include <libxml/tree.h>
#include <libxml/xmlIO.h>
#include <libxml/catalog.h>
#include <libxml/xmlsave.h>
#include <libxml/xmlreader.h>
#include "fuzz.h"
#include "genfiles/proto.pb.h"
#include "libprotobuf-mutator/src/libfuzzer/libfuzzer_macro.h"
#include <libxml/xmlerror.h>



// void ignoreXmlError(void* ctx, const char* msg, ...) {}




using namespace std;

// ...

typedef struct {
    const char *data;
    size_t size;
} xmlFuzzEntityInfo;

/* Single static instance for now */
static struct {
    /* Original data */
    const char *data;
    size_t size;

    /* Remaining data */
    const char *ptr;
    size_t remaining;

    /* Buffer for unescaped strings */
    char *outBuf;
    char *outPtr; /* Free space at end of buffer */

    xmlHashTablePtr entities; /* Maps URLs to xmlFuzzEntityInfos */

    /* The first entity is the main entity. */
    const char *mainUrl;
    xmlFuzzEntityInfo *mainEntity;
} fuzzData;

size_t fuzzNumAllocs;
size_t fuzzMaxAllocs;
int fuzzAllocFailed;

/**
 * xmlFuzzErrorFunc:
 *
 * An error function that simply discards all errors.
 */
void
xmlFuzzErrorFunc(void *ctx ATTRIBUTE_UNUSED, const char *msg ATTRIBUTE_UNUSED,
                 ...) {
}

/*
 * Malloc failure injection.
 *
 * To debug issues involving malloc failures, it's often helpful to set
 * MALLOC_ABORT to 1. This should provide a backtrace of the failed
 * allocation.
 */

#define XML_FUZZ_MALLOC_ABORT   0

static void *
xmlFuzzMalloc(size_t size) {
    if (fuzzMaxAllocs > 0) {
        fuzzNumAllocs += 1;
        if (fuzzNumAllocs == fuzzMaxAllocs) {
#if XML_FUZZ_MALLOC_ABORT
            abort();
#endif
            fuzzAllocFailed = 1;
            return(NULL);
        }
    }
    return malloc(size);
}

static void *
xmlFuzzRealloc(void *ptr, size_t size) {
    if (fuzzMaxAllocs > 0) {
        fuzzNumAllocs += 1;
        if (fuzzNumAllocs == fuzzMaxAllocs) {
#if XML_FUZZ_MALLOC_ABORT
            abort();
#endif
            fuzzAllocFailed = 1;
            return(NULL);
        }
    }
    return realloc(ptr, size);
}

void
xmlFuzzMemSetup(void) {
    xmlMemSetup(free, xmlFuzzMalloc, xmlFuzzRealloc, xmlMemStrdup);
}

void
xmlFuzzMemSetLimit(size_t limit) {
    fuzzNumAllocs = 0;
    fuzzMaxAllocs = limit;
    fuzzAllocFailed = 0;
}

int
xmlFuzzMallocFailed(void) {
    return fuzzAllocFailed;
}

void
xmlFuzzResetMallocFailed(void) {
    fuzzAllocFailed = 0;
}

void
xmlFuzzCheckMallocFailure(const char *func, int error) {
    if (error >= 0 && fuzzAllocFailed != error) {
        fprintf(stderr, "%s: malloc failure %s reported\n",
                func, fuzzAllocFailed ? "not" : "erroneously");
        abort();
    }
    fuzzAllocFailed = 0;
}

/**
 * xmlFuzzDataInit:
 *
 * Initialize fuzz data provider.
 */
void
xmlFuzzDataInit(const char *data, size_t size) {
    fuzzData.data = data;
    fuzzData.size = size;
    fuzzData.ptr = data;
    fuzzData.remaining = size;

    fuzzData.outBuf = (char *)xmlMalloc(size + 1);
    fuzzData.outPtr = fuzzData.outBuf;

    fuzzData.entities = xmlHashCreate(8);
    fuzzData.mainUrl = NULL;
    fuzzData.mainEntity = NULL;
}

/**
 * xmlFuzzDataFree:
 *
 * Cleanup fuzz data provider.
 */
void
xmlFuzzDataCleanup(void) {
    xmlFree(fuzzData.outBuf);
    xmlHashFree(fuzzData.entities, xmlHashDefaultDeallocator);
}

/**
 * xmlFuzzWriteInt:
 * @out:  output file
 * @v:  integer to write
 * @size:  size of integer in bytes
 *
 * Write an integer to the fuzz data.
 */
void
xmlFuzzWriteInt(FILE *out, size_t v, int size) {
    int shift;

    while (size > (int) sizeof(size_t)) {
        putc(0, out);
        size--;
    }

    shift = size * 8;
    while (shift > 0) {
        shift -= 8;
        putc((v >> shift) & 255, out);
    }
}

/**
 * xmlFuzzReadInt:
 * @size:  size of integer in bytes
 *
 * Read an integer from the fuzz data.
 */
size_t
xmlFuzzReadInt(int size) {
    size_t ret = 0;

    while ((size > 0) && (fuzzData.remaining > 0)) {
        unsigned char c = (unsigned char) *fuzzData.ptr++;
        fuzzData.remaining--;
        ret = (ret << 8) | c;
        size--;
    }

    return ret;
}

/**
 * xmlFuzzBytesRemaining:
 *
 * Return number of remaining bytes in fuzz data.
 */
size_t
xmlFuzzBytesRemaining(void) {
    return(fuzzData.remaining);
}

/**
 * xmlFuzzReadRemaining:
 * @size:  size of string in bytes
 *
 * Read remaining bytes from fuzz data.
 */
const char *
xmlFuzzReadRemaining(size_t *size) {
    const char *ret = fuzzData.ptr;

    *size = fuzzData.remaining;
    fuzzData.ptr += fuzzData.remaining;
    fuzzData.remaining = 0;

    return(ret);
}

/*
 * xmlFuzzWriteString:
 * @out:  output file
 * @str:  string to write
 *
 * Write a random-length string to file in a format similar to
 * FuzzedDataProvider. Backslash followed by newline marks the end of the
 * string. Two backslashes are used to escape a backslash.
 */
void
xmlFuzzWriteString(FILE *out, const char *str) {
    for (; *str; str++) {
        int c = (unsigned char) *str;
        putc(c, out);
        if (c == '\\')
            putc(c, out);
    }
    putc('\\', out);
    putc('\n', out);
}

/**
 * xmlFuzzReadString:
 * @size:  size of string in bytes
 *
 * Read a random-length string from the fuzz data.
 *
 * The format is similar to libFuzzer's FuzzedDataProvider but treats
 * backslash followed by newline as end of string. This makes the fuzz data
 * more readable. A backslash character is escaped with another backslash.
 *
 * Returns a zero-terminated string or NULL if the fuzz data is exhausted.
 */
const char *
xmlFuzzReadString(size_t *size) {
    const char *out = fuzzData.outPtr;

    while (fuzzData.remaining > 0) {
        int c = *fuzzData.ptr++;
        fuzzData.remaining--;

        if ((c == '\\') && (fuzzData.remaining > 0)) {
            int c2 = *fuzzData.ptr;

            if (c2 == '\n') {
                fuzzData.ptr++;
                fuzzData.remaining--;
                if (size != NULL)
                    *size = fuzzData.outPtr - out;
                *fuzzData.outPtr++ = '\0';
                return(out);
            }
            if (c2 == '\\') {
                fuzzData.ptr++;
                fuzzData.remaining--;
            }
        }

        *fuzzData.outPtr++ = c;
    }

    if (fuzzData.outPtr > out) {
        if (size != NULL)
            *size = fuzzData.outPtr - out;
        *fuzzData.outPtr++ = '\0';
        return(out);
    }

    if (size != NULL)
        *size = 0;
    return(NULL);
}

/**
 * xmlFuzzReadEntities:
 *
 * Read entities like the main XML file, external DTDs, external parsed
 * entities from fuzz data.
 */
void
xmlFuzzReadEntities(void) {
    size_t num = 0;

    while (1) {
        const char *url, *entity;
        size_t urlSize, entitySize;
        xmlFuzzEntityInfo *entityInfo;

        url = xmlFuzzReadString(&urlSize);
        if (url == NULL) break;

        entity = xmlFuzzReadString(&entitySize);
        if (entity == NULL) break;

        /*
         * Cap URL size to avoid quadratic behavior when generating
         * error messages or looking up entities.
         */
        if (urlSize < 50 &&
            xmlHashLookup(fuzzData.entities, (xmlChar *)url) == NULL) {
            entityInfo = (xmlFuzzEntityInfo *)xmlMalloc(sizeof(xmlFuzzEntityInfo));
            if (entityInfo == NULL)
                break;
            entityInfo->data = entity;
            entityInfo->size = entitySize;

            xmlHashAddEntry(fuzzData.entities, (xmlChar *)url, entityInfo);

            if (num == 0) {
                fuzzData.mainUrl = url;
                fuzzData.mainEntity = entityInfo;
            }

            num++;
        }
    }
}

/**
 * xmlFuzzMainUrl:
 *
 * Returns the main URL.
 */
const char *
xmlFuzzMainUrl(void) {
    return(fuzzData.mainUrl);
}

/**
 * xmlFuzzMainEntity:
 * @size:  size of the main entity in bytes
 *
 * Returns the main entity.
 */
const char *
xmlFuzzMainEntity(size_t *size) {
    if (fuzzData.mainEntity == NULL)
        return(NULL);
    *size = fuzzData.mainEntity->size;
    return(fuzzData.mainEntity->data);
}

/**
 * xmlFuzzEntityLoader:
 *
 * The entity loader for fuzz data.
 */
xmlParserInputPtr
xmlFuzzEntityLoader(const char *URL, const char *ID ATTRIBUTE_UNUSED,
                    xmlParserCtxtPtr ctxt) {
    xmlParserInputPtr input;
    xmlFuzzEntityInfo *entity;

    if (URL == NULL)
        return(NULL);
    entity = (xmlFuzzEntityInfo *)xmlHashLookup(fuzzData.entities, (xmlChar *) URL);
    if (entity == NULL)
        return(NULL);

    input = xmlNewInputStream(ctxt);
    if (input == NULL)
        return(NULL);
    input->filename = (char *) xmlCharStrdup(URL);
    if (input->filename == NULL) {
        xmlCtxtErrMemory(ctxt);
        xmlFreeInputStream(input);
        return(NULL);
    }
    input->buf = xmlParserInputBufferCreateMem(entity->data, entity->size,
                                               XML_CHAR_ENCODING_NONE);
    if (input->buf == NULL) {
        xmlCtxtErrMemory(ctxt);
        xmlFreeInputStream(input);
        return(NULL);
    }
    input->base = input->cur = xmlBufContent(input->buf->buffer);
    input->end = input->base + xmlBufUse(input->buf->buffer);

    return input;
}

char *
xmlSlurpFile(const char *path, size_t *sizeRet) {
    FILE *file;
    struct stat statbuf;
    char *data;
    size_t size;

    if ((stat(path, &statbuf) != 0) || (!S_ISREG(statbuf.st_mode)))
        return(NULL);
    size = statbuf.st_size;
    file = fopen(path, "rb");
    if (file == NULL)
        return(NULL);
    data = (char *)xmlMalloc(size + 1);
    if (data != NULL) {
        if (fread(data, 1, size, file) != size) {
            xmlFree(data);
            data = NULL;
        } else {
            data[size] = 0;
            if (sizeRet != NULL)
                *sizeRet = size;
        }
    }
    fclose(file);

    return(data);
}


class XmlConverter {
public:
    static void GenerateXml(const XmlDocument& document, std::string& xml_data) {
        xml_data += "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
        GenerateElement(document.root(), 0, xml_data);
    }

private:
    static void GenerateElement(const XmlElement& element, int indent, std::string& xml_data) {
        PrintIndent(indent, xml_data);
        xml_data += "<" + element.name();

        // Print attributes
        for (const string& attr : element.attributes()) {
            xml_data += " " + attr + "=\"value\"";
        }

        if (element.child_elements_size() == 0 && element.text().empty()) {
            xml_data += "/>\n";
        } else {
            xml_data += ">\n";

            // Print text content
            if (!element.text().empty()) {
                PrintIndent(indent + 1, xml_data);
                xml_data += element.text() + "\n";
            }

            // Print child elements recursively
            for (const XmlElement& child : element.child_elements()) {
                GenerateElement(child, indent + 1, xml_data);
            }

            PrintIndent(indent, xml_data);
            xml_data += "</" + element.name() + ">\n";
        }
    }

    static void PrintIndent(int indent, std::string& xml_data) {
        xml_data += std::string(indent * 2, ' ');
    }
};

int
LLVMFuzzerInitialize(int *argc ATTRIBUTE_UNUSED,
                     char ***argv ATTRIBUTE_UNUSED) {
    xmlFuzzMemSetup();
    xmlInitParser();
#ifdef LIBXML_CATALOG_ENABLED
    xmlInitializeCatalog();
    xmlCatalogSetDefaults(XML_CATA_ALLOW_NONE);
#endif
    xmlSetGenericErrorFunc(NULL, xmlFuzzErrorFunc);
    xmlSetExternalEntityLoader(xmlFuzzEntityLoader);

    return 0;
}

DEFINE_BINARY_PROTO_FUZZER(const XmlDocument& document) {

    // xmlSetGenericErrorFunc(NULL, &ignoreXmlError);


    xmlParserCtxtPtr ctxt;
    xmlDocPtr doc;
    const char *docBuffer, *docUrl;
    size_t maxAlloc, docSize;
    int opts;

    std::string xml_data;
    XmlConverter::GenerateXml(document, xml_data);

    

    xmlFuzzDataInit(xml_data.data(), xml_data.size());
    
    opts = (int) xmlFuzzReadInt(4);
    /*
        * Disable options that are known to cause timeouts
        */
    opts &= ~XML_PARSE_XINCLUDE &
            ~XML_PARSE_DTDVALID &
            ~XML_PARSE_SAX1;
    maxAlloc = xmlFuzzReadInt(4) % (xml_data.size() + 100);

    xmlFuzzReadEntities();
    docBuffer = xmlFuzzMainEntity(&docSize);
    docUrl = xmlFuzzMainUrl();
    if (docBuffer == NULL)
        goto exit;

    /* Pull parser */

    xmlFuzzMemSetLimit(maxAlloc);
    ctxt = xmlNewParserCtxt();
    if (ctxt != NULL) {
        doc = xmlCtxtReadMemory(ctxt, docBuffer, docSize, docUrl, NULL, opts);
        
        xmlFuzzCheckMallocFailure("xmlCtxtReadMemory",
                                    doc == NULL &&
                                    ctxt->errNo == XML_ERR_NO_MEMORY);

        if (doc != NULL) {
#ifdef LIBXML_OUTPUT_ENABLED
            xmlBufferPtr buffer;
            xmlSaveCtxtPtr save;

            /* Also test the serializer. */
            buffer = xmlBufferCreate();
            save = xmlSaveToBuffer(buffer, NULL, 0);
            if (save != NULL) {
                int errNo;

                xmlSaveDoc(save, doc);
                
                errNo = xmlSaveFinish(save);
                // errNo = xmlSaveFlush(save);
                xmlFuzzCheckMallocFailure("xmlSaveDoc",
                                          errNo == XML_ERR_NO_MEMORY);
            }
            xmlBufferFree(buffer);
#endif
            xmlFreeDoc(doc);
        }

        xmlFreeParserCtxt(ctxt);
    }

    /* Push parser */

#ifdef LIBXML_PUSH_ENABLED
    {
        static const size_t maxChunkSize = 128;
        size_t consumed, chunkSize;

        xmlFuzzMemSetLimit(maxAlloc);
        ctxt = xmlCreatePushParserCtxt(NULL, NULL, NULL, 0, docUrl);
        if (ctxt != NULL) {
            xmlCtxtUseOptions(ctxt, opts);

            for (consumed = 0; consumed < docSize; consumed += chunkSize) {
                chunkSize = docSize - consumed;
                if (chunkSize > maxChunkSize)
                    chunkSize = maxChunkSize;
                xmlParseChunk(ctxt, docBuffer + consumed, chunkSize, 0);
            }

            xmlParseChunk(ctxt, NULL, 0, 1);
            // std::cout << "!!!!!!!!!!!!" << std::endl;
            xmlFuzzCheckMallocFailure("xmlParseChunk",
                                      ctxt->errNo == XML_ERR_NO_MEMORY);
            xmlFreeDoc(ctxt->myDoc);
            xmlFreeParserCtxt(ctxt);
        }
    }
#endif

    /* Reader */

#ifdef LIBXML_READER_ENABLED
    {
        xmlTextReaderPtr reader;
        const xmlError *error;
        int j;

        xmlFuzzMemSetLimit(maxAlloc);
        
        reader = xmlReaderForMemory(docBuffer, docSize, NULL, NULL, opts);
        if (reader != NULL) {
            
            while (xmlTextReaderRead(reader) == 1) {
                if (xmlTextReaderNodeType(reader) == XML_ELEMENT_NODE) {
                    int i, n = xmlTextReaderAttributeCount(reader);
                    for (i=0; i<n; i++) {
                        xmlTextReaderMoveToAttributeNo(reader, i);
                        while (xmlTextReaderReadAttributeValue(reader) == 1);
                    }
                }
            }
            for (j = 0; j < 10; j++)
                xmlTextReaderRead(reader);
            error = xmlTextReaderGetLastError(reader);
            // xmlCtxtResetLastError(reader);
            xmlFuzzCheckMallocFailure("xmlTextReaderRead",
                                      error->code == XML_ERR_NO_MEMORY);
            xmlFreeTextReader(reader);
        }
    }
#endif

exit:
    xmlFuzzMemSetLimit(0);
    xmlFuzzDataCleanup();
    xmlResetLastError();
    return;
}


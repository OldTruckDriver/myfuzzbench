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


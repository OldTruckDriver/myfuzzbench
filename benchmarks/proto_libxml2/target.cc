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

#include <string>
#include <vector>
#include <iostream>
#include "libxml/xmlversion.h"
#include "libxml/parser.h"
#include "libxml/HTMLparser.h"
#include "libxml/tree.h"
#include "genfiles/proto.pb.h"
#include "libprotobuf-mutator/src/libfuzzer/libfuzzer_macro.h"



void ignoreXmlError(void* ctx, const char* msg, ...) {}




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


DEFINE_BINARY_PROTO_FUZZER(const XmlDocument& document) {

  xmlSetGenericErrorFunc(NULL, &ignoreXmlError);

  std::string xml_data;
  XmlConverter::GenerateXml(document, xml_data);

  if (auto doc = xmlReadMemory(xml_data.c_str(), xml_data.size(), NULL, NULL, 0)) {
      xmlFreeDoc(doc);

      std::cout << "!!!!!!!!!!!!!!!!!!!!!" << std::endl;
        std::cout << "!!!!!!!!!!!!!!!!!!!!!" << std::endl;
        std::cout << "!!!!!!!!!!!!!!!!!!!!!" << std::endl;
        std::cout << "!!!!!!!!!!!!!!!!!!!!!" << std::endl;
        std::cout << "!!!!!!!!!!!!!!!!!!!!!" << std::endl;
  }

  return;
}


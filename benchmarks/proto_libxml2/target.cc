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




void ignore (void * ctx, const char * msg, ...) {}




using namespace std;

class XmlConverter {
public:
    static void GenerateXml(const XmlDocument& document) {
        cout << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << endl;
        GenerateElement(document.root(), 0);
    }

private:
    static void GenerateElement(const XmlElement& element, int indent) {
        PrintIndent(indent);
        cout << "<" << element.name();

        // Print attributes
        for (const string& attr : element.attributes()) {
            cout << " " << attr << "=\"value\"";
        }

        if (element.child_elements_size() == 0 && element.text().empty()) {
            cout << "/>" << endl;
        } else {
            cout << ">" << endl;

            // Print text content
            if (!element.text().empty()) {
                PrintIndent(indent + 1);
                cout << element.text() << endl;
            }

            // Print child elements recursively
            for (const XmlElement& child : element.child_elements()) {
                GenerateElement(child, indent + 1);
            }

            PrintIndent(indent);
            cout << "</" << element.name() << ">" << endl;
        }
    }

    static void PrintIndent(int indent) {
        for (int i = 0; i < indent; i++) {
            cout << "  ";
        }
    }
};

DEFINE_BINARY_PROTO_FUZZER(const XmlDocument& document) {
  std::string xml_data;
  document.SerializeToString(&xml_data);

  // Write the XML data to a file
  std::ofstream xml_file("generated.xml");
  xml_file << xml_data;
  xml_file.close();

  xmlSetGenericErrorFunc(NULL, &ignore);
  if (auto doc = xmlReadFile("generated.xml", NULL, 0))
    xmlFreeDoc(doc);

  // Remove the generated XML file
  std::remove("generated.xml");

  return;
}

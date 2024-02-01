#include "lcms2.h"
#include "lcms2_plugin.h"
#include "lcms2_internal.h"
#include "lcms.pb.h"



void StructTm2Proto(const struct tm* input, tmStructProto* result);
void Proto2StructTm(const tmStructProto* input, struct tm* result);
void CmsProfileID2Proto(const cmsProfileID* input, cmsProfileIDProto* result);
void Proto2CmsProfileID(const cmsProfileIDProto* input, cmsProfileID* result);

void CmsICCPROFILE2Proto(_cmsICCPROFILE* input, cmsICCPROFILEProto* result);

bool Proto2CmsICCPROFILE(const cmsICCPROFILEProto* input, _cmsICCPROFILE*& result);

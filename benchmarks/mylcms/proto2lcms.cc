#include "converter.h"
#include <iostream>
#include <fstream>

void StructTm2Proto(const struct tm* input, tmStructProto* result){
    result->set_tm_sec(input->tm_sec);
    result->set_tm_min(input->tm_min);
    result->set_tm_hour(input->tm_hour);
    result->set_tm_mday(input->tm_mday);
    result->set_tm_mon(input->tm_mon);
    result->set_tm_year(input->tm_year);
    result->set_tm_wday(input->tm_wday);
    result->set_tm_yday(input->tm_yday);
    result->set_tm_isdst(input->tm_isdst);
// #ifdef __USE_MISC
//     result->set_tm_gmtoff(input->tm_gmtoff);
//     result->set_tm_zone(input->tm_zone);
// #else
//     result->set_tm_gmtoff(input->__tm_gmtoff);
//     result->set_tm_zone(input->__tm_zone);
// #endif
}

void CmsProfileID2Proto(const cmsProfileID* input, cmsProfileIDProto* result){
    for (int i = 0; i < 4; ++i) {
        result->add_id32(input->ID32[i]);
    }
}

    void CmsICCPROFILE2Proto(_cmsICCPROFILE* input, cmsICCPROFILEProto* result){
        StructTm2Proto(&(input->Created), result->mutable_created());
        result->set_cmm(input->CMM);
        result->set_version(input->Version);
        result->set_deviceclass(static_cast<cmsProfileClassSignatureProto>(input->DeviceClass));
        result->set_colorspace(static_cast<cmsColorSpaceSignatureProto>(input->ColorSpace));
        result->set_pcs(static_cast<cmsColorSpaceSignatureProto>(input->PCS));
        result->set_renderingintent(input->RenderingIntent);
        result->set_platform(static_cast<cmsPlatformSignatureProto>(input->platform));
        result->set_flags(input->flags);
        result->set_manufacturer(input->manufacturer);
        result->set_model(input->model);
        result->set_attributes(input->attributes);
        result->set_creator(input->creator);
        CmsProfileID2Proto(&(input->ProfileID), result->mutable_profileid());
        result->set_tagcount(input->TagCount);
        // Example: Converting TagNames field (repeated) to Proto
        for (int i = 0; i < input->TagCount; ++i) {
            //if(input->TagNames[i] == (cmsTagSignature) 0 || input->TagLinked[i] != (cmsTagSignature) 0 || (cmsUInt8Number*) input->TagPtrs[i] == NULL) continue;
            result->add_tagnames(static_cast<cmsTagSignatureProto>(input->TagNames[i]));
            //printf("%x\n", input->TagNames[i]);
            result->add_taglinked(static_cast<cmsTagSignatureProto>(input->TagLinked[i]));
            result->add_tagsizes(input->TagSizes[i]);
            result->add_tagoffsets(input->TagOffsets[i]);
            result->add_tagsaveasraw(input->TagSaveAsRaw[i]);
            void* buffer = malloc(input->TagSizes[i]);
            cmsReadRawTag(input, input->TagNames[i], buffer, input->TagSizes[i]);
            result->add_tag_ptrs(buffer, input->TagSizes[i]);
            free(buffer);
        }
        //result->set_iswrite(input->IsWrite);
    }
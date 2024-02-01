#include "converter.h" 
#include <vector>
void Proto2StructTm(const tmStructProto *input, struct tm *result){
    result->tm_sec = input->tm_sec();
    result->tm_min = input->tm_min();
    result->tm_hour = input->tm_hour();
    result->tm_mday = input->tm_mday();
    result->tm_mon = input->tm_mon();
    result->tm_year = input->tm_year();
    result->tm_wday = input->tm_wday();
    result->tm_yday = input->tm_yday();
    result->tm_isdst = input->tm_isdst(); 
    // #ifdef __USE_MISC// result->tm_gmtoff = input->tm_gmtoff();// result->tm_zone = strdup(input->tm_zone().c_str());// #else// result->__tm_gmtoff = input->tm_gmtoff();// result->__tm_zone = strdup(input->tm_zone().c_str());// #endif}
void Proto2CmsProfileID(const cmsProfileIDProto *input, cmsProfileID *result)
{
    for (int i = 0; i < input->id32_size(); ++i)
    {
        result->ID32[i] = input->id32(i);
    }
}
bool Proto2CmsICCPROFILE(const cmsICCPROFILEProto *input, _cmsICCPROFILE *&result)
{
    cmsHPROFILE hEmpty = cmsCreateProfilePlaceholder(NULL);
    if (hEmpty == NULL)
        return false;
    result = (_cmsICCPROFILE *)hEmpty;
    Proto2StructTm(&(input->created()), &(result->Created));
    result->CMM = input->cmm();
    result->Version = input->version();
    if (result->Version > 0x5000000)
    {
        cmsSignalError(result->ContextID, cmsERROR_UNKNOWN_EXTENSION, "Unsupported profile version '0x%x'", result->Version);
        return false;
    }
    result->ColorSpace = static_cast<cmsColorSpaceSignature>(input->colorspace());
    result->PCS = static_cast<cmsColorSpaceSignature>(input->pcs());
    result->RenderingIntent = input->renderingintent();
    result->platform = static_cast<cmsPlatformSignature>(input->platform());
    result->flags = input->flags();
    result->manufacturer = input->manufacturer();
    result->model = input->model();
    result->attributes = input->attributes();
    result->creator = input->creator();
    Proto2CmsProfileID(&(input->profileid()), &(result->ProfileID));
    result->TagCount = input->tagcount() % MAX_TABLE_TAG; // Example: Converting TagNames field (repeated) to CmsICCPROFILE std::vector<int> cnt_stats = {input->tagnames_size(), input->tagsizes_size(), input->taglinked_size(), input->tagoffsets_size(), input->tagsaveasraw_size(), input->tag_ptrs_size()}; if(*std::min_element(cnt_stats.begin(), cnt_stats.end())<1) return false; for (int i = 0; i < result->TagCount; ++i) { result->TagNames[i] = static_cast<cmsTagSignature>(input->tagnames(i % input->tagnames_size())); result->TagLinked[i] = static_cast<cmsTagSignature>(input->taglinked(i % input->taglinked_size())); result->TagSizes[i] = input->tagsizes(i % input->tagsizes_size()); result->TagOffsets[i] = input->tagoffsets(i % input->tagoffsets_size()); result->TagSaveAsRaw[i] = input->tagsaveasraw(i % input->tagsaveasraw_size()); auto& tagPtr = input->tag_ptrs(i % input->tag_ptrs_size()); cmsWriteRawTag(result, result->TagNames[i], tagPtr.data(), std::min(result->TagSizes[i], static_cast<cmsUInt32Number>(tagPtr.size()))); 
    }
    result->IsWrite = 0;
    return true;
}
// This is really pretty bad: MS has constructed a typedef hell in different include files.
// Here, we separate header inclusion and require a different source file..

#include <sphelper.h>

HRESULT SpConvertStreamFormatEnum_local(SPSTREAMFORMAT eFormat, GUID* pFormatId, WAVEFORMATEX** exForm)
{
	return SpConvertStreamFormatEnum(eFormat, pFormatId, exForm);
}

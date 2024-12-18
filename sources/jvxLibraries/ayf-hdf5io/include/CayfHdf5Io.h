#include "jvx.h"

class CayfHdf5Io
{
public:
	class oneEntryToc
	{
	public:
		std::string nameDset;
		jvxDataFormat format = JVX_DATAFORMAT_NONE;
		jvxSize ndims = 0;
		jvxSize* dims = nullptr;

		oneEntryToc(jvxSize nDimsArg)
		{
			ndims = nDimsArg;
			if (ndims)
			{
				JVX_SAFE_ALLOCATE_FIELD_CPP_Z(dims, jvxSize, ndims);
			}
		}
		~oneEntryToc()
		{
			if (ndims)
			{
				JVX_SAFE_DELETE_FIELD(dims);
			}
		}
	};

protected:
	std::map<std::string, oneEntryToc*> toc;
	jvxHandle* hdf5Private = nullptr;

	static JVX_MUTEX_HANDLE* safeAccess;
	static jvxSize mutexRefCnt;

	std::string last_error;
public:

	CayfHdf5Io();
	~CayfHdf5Io();
	void clearToc();

	void lock_hdf5();
	void unlock_hdf5();

	jvxErrorType openScanHdf5File(const std::string& fName);
	jvxErrorType openCreateHdf5File(const std::string& fName);
	jvxErrorType closeHdf5File();

	oneEntryToc* allocateNewDataSet(const std::string& dSetName, jvxDataFormat form, jvxSize nDims);
	jvxErrorType addNewDataSet(oneEntryToc* addThis);

	jvxErrorType readDataFromDataSet(const std::string& dataSet, jvxHandle* buf, jvxDataFormat form, jvxSize& numElements);
	jvxErrorType readDataFromDataSet1Line(const std::string& dataSet, jvxHandle* buf, jvxDataFormat form, jvxSize& numElements, jvxSize idx);
	jvxErrorType readDataFromDataSetFull(const std::string& dataSet, jvxHandle* buf, jvxDataFormat form, jvxSize& numElements);
	jvxErrorType readStringFromDataSet(const std::string& dataSet, std::string& onRet);

	jvxErrorType writeDataToDataSet(const std::string& dataSet, const jvxHandle* buf, jvxDataFormat form, jvxSize numElements);
	jvxErrorType createDataSet(const std::string& dataSet, jvxDataFormat form, jvxSize numElements1, jvxSize numElements2);
	jvxErrorType writeLineToDataSet(jvxHandle* buf, jvxSize idx);
	jvxErrorType writeStringToDataSet(const std::string& dataSet, const std::string& txt);
	jvxErrorType closeDataSet();

	std::string get_last_error() {
		return last_error;
	};

};

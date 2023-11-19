#include "CayfHdf5Io.h"

extern "C"
{
#include "hdf5.h"
}

class CayfHdf5Io_prv
{
public:
	hid_t file = 0;
};

class CayfHdf5O_prv: public CayfHdf5Io_prv
{
public:
	hid_t lcpl = 0;
	hid_t dset = 0;
	hid_t mem_tp_id = 0;
	hid_t fspace = 0;
	hsize_t num[2] = { 0,0 };
};

hsize_t     offset[H5S_MAX_RANK];
hsize_t     size[H5S_MAX_RANK];
hsize_t     maxsize[H5S_MAX_RANK];

// Suport only a subset of entries
static jvxDataFormat
hdf5ToAyfDataType(hid_t tp)
{
	jvxDataFormat retVal = JVX_DATAFORMAT_NONE;
	H5T_class_t ctp = H5Tget_class(tp);
	switch (ctp) {
	case H5T_INTEGER:
		if (H5Tequal(tp, H5T_STD_I8LE) == true)
			retVal = JVX_DATAFORMAT_8BIT;
		else if (H5Tequal(tp, H5T_STD_I16LE) == true)
			retVal = JVX_DATAFORMAT_16BIT_LE;
		else if (H5Tequal(tp, H5T_STD_I32LE) == true)
			retVal = JVX_DATAFORMAT_32BIT_LE;
		else if (H5Tequal(tp, H5T_STD_I64LE) == true)
			retVal = JVX_DATAFORMAT_64BIT_LE;
		else if (H5Tequal(tp, H5T_STD_U8LE) == true)
			retVal = JVX_DATAFORMAT_U8BIT;
		else if (H5Tequal(tp, H5T_STD_U16LE) == true)
			retVal = JVX_DATAFORMAT_U16BIT_LE;
		else if (H5Tequal(tp, H5T_STD_U32LE) == true)
			retVal = JVX_DATAFORMAT_U32BIT_LE;
		else if (H5Tequal(tp, H5T_STD_U64LE) == true)
			retVal = JVX_DATAFORMAT_U64BIT_LE;
		break;

	case H5T_FLOAT:
		if (H5Tequal(tp, H5T_IEEE_F32LE) == true)
		{
#ifdef JVX_DATA_FORMAT_DOUBLE
			retVal = JVX_DATAFORMAT_FLOAT;
#else
			retVal = JVX_DATAFORMAT_DATA;
#endif
		}
		else if (H5Tequal(tp, H5T_IEEE_F64LE) == true)
		{
#ifdef JVX_DATA_FORMAT_DOUBLE
			retVal = JVX_DATAFORMAT_DATA;
#else
			retVal = JVX_DATAFORMAT_DOUBLE;
#endif

		}			
		break;
	}
	return retVal;
}

static void 
printInfoDSet(hid_t tp, hid_t space)
{
	H5T_class_t ctp = H5Tget_class(tp);
	H5S_class_t space_type;
	int ndims;

	switch (ctp) {
	case H5T_INTEGER:
		if (H5Tequal(tp, H5T_STD_I8BE) == true)
			std::cout << "H5T_STD_I8BE" << std::endl;
		else if (H5Tequal(tp, H5T_STD_I8LE) == true)
			std::cout << "H5T_STD_I8LE" << std::endl;
		else if (H5Tequal(tp, H5T_STD_I16BE) == true)
			std::cout << "H5T_STD_I16BE" << std::endl;
		else if (H5Tequal(tp, H5T_STD_I16LE) == true)
			std::cout << "H5T_STD_I16LE" << std::endl;
		else if (H5Tequal(tp, H5T_STD_I32BE) == true)
			std::cout << "H5T_STD_I32BE" << std::endl;
		else if (H5Tequal(tp, H5T_STD_I32LE) == true)
			std::cout << "H5T_STD_I32LE" << std::endl;
		else if (H5Tequal(tp, H5T_STD_I64BE) == true)
			std::cout << "H5T_STD_I64BE" << std::endl;
		else if (H5Tequal(tp, H5T_STD_I64LE) == true)
			std::cout << "H5T_STD_I64LE" << std::endl;
		else if (H5Tequal(tp, H5T_STD_U8BE) == true)
			std::cout << "H5T_STD_U8BE" << std::endl;
		else if (H5Tequal(tp, H5T_STD_U8LE) == true)
			std::cout << "H5T_STD_U8LE" << std::endl;
		else if (H5Tequal(tp, H5T_STD_U16BE) == true)
			std::cout << "H5T_STD_U16BE" << std::endl;
		else if (H5Tequal(tp, H5T_STD_U16LE) == true)
			std::cout << "H5T_STD_U16LE" << std::endl;
		else if (H5Tequal(tp, H5T_STD_U32BE) == true)
			std::cout << "H5T_STD_U32BE" << std::endl;
		else if (H5Tequal(tp, H5T_STD_U32LE) == true)
			std::cout << "H5T_STD_U32LE" << std::endl;
		else if (H5Tequal(tp, H5T_STD_U64BE) == true)
			std::cout << "H5T_STD_U64BE" << std::endl;
		else if (H5Tequal(tp, H5T_STD_U64LE) == true)
			std::cout << "H5T_STD_U64LE" << std::endl;
		else if (H5Tequal(tp, H5T_NATIVE_SCHAR) == true)
			std::cout << "H5T_NATIVE_SCHAR" << std::endl;
		else if (H5Tequal(tp, H5T_NATIVE_UCHAR) == true)
			std::cout << "H5T_NATIVE_UCHAR" << std::endl;
		else if (H5Tequal(tp, H5T_NATIVE_SHORT) == true)
			std::cout << "H5T_NATIVE_SHORT" << std::endl;
		else if (H5Tequal(tp, H5T_NATIVE_USHORT) == true)
			std::cout << "H5T_NATIVE_USHORT" << std::endl;
		else if (H5Tequal(tp, H5T_NATIVE_INT) == true)
			std::cout << "H5T_NATIVE_INT" << std::endl;
		else if (H5Tequal(tp, H5T_NATIVE_UINT) == true)
			std::cout << "H5T_NATIVE_UINT" << std::endl;
		else if (H5Tequal(tp, H5T_NATIVE_LONG) == true)
			std::cout << "H5T_NATIVE_LONG" << std::endl;
		else if (H5Tequal(tp, H5T_NATIVE_ULONG) == true)
			std::cout << "H5T_NATIVE_ULONG" << std::endl;
		else if (H5Tequal(tp, H5T_NATIVE_LLONG) == true)
			std::cout << "H5T_NATIVE_LLONG" << std::endl;
		else if (H5Tequal(tp, H5T_NATIVE_ULLONG) == true)
			std::cout << "H5T_NATIVE_ULLONG" << std::endl;
		else {
			std::cout << "H5T_NATIVE_INT_UNKNWON-" << std::flush;

			/* byte order */
			if (H5Tget_size(tp) > 1) {
				H5T_order_t order = H5Tget_order(tp);
				if (H5T_ORDER_LE == order)
					std::cout << "le" << std::flush;
				else if (H5T_ORDER_BE == order)
					std::cout << "be" << std::flush;
				else if (H5T_ORDER_VAX == order)
					std::cout << "me" << std::flush;
				else
					std::cout << "ue" << std::flush;
				std::cout << std::endl;
			}
			else
			{
				std::cout << "unknown" << std::endl;
			}
#if 0
	else
	order_s = "";

	/* sign */
	if ((sign = H5Tget_sign(type)) >= 0) {
		if (H5T_SGN_NONE == sign)
			sign_s = " unsigned";
		else if (H5T_SGN_2 == sign)
			sign_s = "";
		else
			sign_s = " unknown-sign";
	}
	else
		sign_s = " unknown-sign";
#endif

		}
		break;

	case H5T_FLOAT:
		if (H5Tequal(tp, H5T_IEEE_F32BE) == true)
			std::cout << "H5T_IEEE_F32BE" << std::endl;
		else if (H5Tequal(tp, H5T_IEEE_F32LE) == true)
			std::cout << "H5T_IEEE_F32LE" << std::endl;
		else if (H5Tequal(tp, H5T_IEEE_F64BE) == true)
			std::cout << "H5T_IEEE_F64BE" << std::endl;
		else if (H5Tequal(tp, H5T_IEEE_F64LE) == true)
			std::cout << "H5T_IEEE_F64LE" << std::endl;
		else if (H5Tequal(tp, H5T_VAX_F32) == true)
			std::cout << "H5T_VAX_F32" << std::endl;
		else if (H5Tequal(tp, H5T_VAX_F64) == true)
			std::cout << "H5T_VAX_F64" << std::endl;
		else if (H5Tequal(tp, H5T_NATIVE_FLOAT) == true)
			std::cout << "H5T_NATIVE_FLOAT" << std::endl;
		else if (H5Tequal(tp, H5T_NATIVE_DOUBLE) == true)
			std::cout << "H5T_NATIVE_DOUBLE" << std::endl;
		else {
			std::cout << "H5T_NATIVE_UNKNWON" << std::endl;
		}
		break;
	}

	ndims = H5Sget_simple_extent_dims(space, size, maxsize);
	space_type = H5Sget_simple_extent_type(space);
	switch (space_type) {
	case H5S_SCALAR:
		std::cout << "H5S_SCALAR" << std::endl;
	case H5S_SIMPLE:
		std::cout << "H5S_SIMPLE" << std::endl;
		break;
	}
	for (int i = 0; i < ndims; i++)
	{
		if (i > 0)
		{
			std::cout << ":" << std::flush;
		}
		std::cout << size[i] << std::flush;
	}
	std::cout << std::endl;
	for (int i = 0; i < ndims; i++)
	{
		if (i > 0)
		{
			std::cout << ":" << std::flush;
		}
		std::cout << maxsize[i] << std::flush;
	}
	std::cout << std::endl;
}

static herr_t
traverse_cb(hid_t loc_id, const char* path, const H5L_info2_t* linfo, void* _udata)
{
	H5O_info2_t oinfo;
	hid_t dset = 0;
	hid_t type = 0;
	hid_t space = 0;
	H5O_info2_t type_oinfo;

	CayfHdf5Io* this_pointer = (CayfHdf5Io*)_udata;
	switch (linfo->type)
	{
	case H5L_TYPE_HARD:


		/* Get information about the object */
		if (H5Oget_info_by_name3(loc_id, path, &oinfo, 1, H5P_DEFAULT) < 0) {
		}
		switch (oinfo.type)
		{
		case H5O_TYPE_GROUP:

#ifdef VERBOSE_OUT
			std::cout << "Group</" << path << ">" << std::endl;
#endif

			break;
		case H5O_TYPE_DATASET:

#ifdef VERBOSE_OUT
			std::cout << "Dataset </" << path << ">" << std::endl;
#endif

			dset = H5Dopen2(loc_id, path, H5P_DEFAULT);
			if (dset >= 0)
			{
				type = H5Dget_type(dset);
				space = H5Dget_space(dset);

				// Print out the information
#ifdef VERBOSE_OUT
				printInfoDSet(type, space);
#endif

				if (this_pointer)
				{
					std::string dSetName = "/";
					jvxDataFormat form = hdf5ToAyfDataType(type);
					dSetName += path;
					jvxSize ndims = H5Sget_simple_extent_dims(space, size, maxsize);
					CayfHdf5Io::oneEntryToc * newElm = this_pointer->allocateNewDataSet(dSetName, form, ndims);
					if (newElm)
					{
						for (jvxSize i = 0; i < newElm->ndims; i++)
						{
							newElm->dims[i] = size[i];
						}
						this_pointer->addNewDataSet(newElm);
					}
				}
				H5Tclose(type);
				H5Sclose(space);
				H5Dclose(dset);
				type = 0;
				dset = 0;
			}
			break;
		default:

#ifdef VERBOSE_OUT
			std::cout << "Something else!" << std::endl;
#endif
			break;

		}
		break;
	}
	return (H5_ITER_CONT);
}

herr_t
dump_all_cb(hid_t group, const char* name, const H5L_info2_t* linfo, void* op_data)
{
	hid_t obj;
	hid_t tp;
	hid_t space;
	H5T_class_t ctp;
	int         ndims;
	hsize_t     size[H5S_MAX_RANK];
	hsize_t     maxsize[H5S_MAX_RANK];
	H5S_class_t space_type;
	if (linfo->type == H5L_TYPE_HARD)
	{
		H5O_info2_t oinfo;

		/* Stat the object */
		H5Oget_info_by_name3(group, name, &oinfo, H5O_INFO_BASIC, H5P_DEFAULT);
		switch (oinfo.type)
		{
		case H5O_TYPE_GROUP:
			obj = H5Gopen2(group, name, H5P_DEFAULT);
			H5Literate2(obj, H5_INDEX_NAME, H5_ITER_INC, NULL, dump_all_cb, NULL);
			H5Dclose(obj);
			break;
		case H5O_TYPE_DATASET:
			
			// std::cout << "Dataset <" << name << ">:" << std::endl;
			
			obj = H5Dopen2(group, name, H5P_DEFAULT);
			tp = H5Dget_type(obj);
			space = H5Dget_space(obj);

			// printInfoDSet(tp, space);

			H5Tclose(space);
			H5Tclose(tp);
			H5Dclose(obj);
			break;
		}
	}
	return (H5_ITER_CONT);
}

// =======================================================================================================================
CayfHdf5Io::CayfHdf5Io()
{
	JVX_SAFE_ALLOCATE_OBJECT(hdf5Private, CayfHdf5O_prv);
}

CayfHdf5Io::~CayfHdf5Io()
{
	clearToc();

	// We have used a void pointer for the private struct to prevent that we need to include
	// the header in the heaer file
	CayfHdf5O_prv* deleteMe = (CayfHdf5O_prv*)hdf5Private;
	JVX_SAFE_DELETE_OBJECT(deleteMe);
	hdf5Private = nullptr;
}

void
CayfHdf5Io::clearToc()
{
	while (1)
	{
		auto elm = toc.begin();
		if (elm == toc.end())
		{
			break;
		}
		if (elm->second)
		{
			JVX_SAFE_DELETE_OBJ(elm->second);
			elm->second = nullptr;
			toc.erase(elm);
		}
	}
}

jvxErrorType 
CayfHdf5Io::openScanHdf5File(const std::string& fName)
{
	// Read the calibration data from the h5 file
	jvxErrorType res = JVX_NO_ERROR;
	H5O_info2_t       oi;
	H5O_info2_t       oinfo;
	hid_t       dataset; /* handles */
	hid_t       datatype, dataspace;
	hid_t       memspace;
	hid_t       gid;
	H5T_class_t t_class; /* data type class */
	H5T_order_t order;   /* data order */
	size_t      size;    /*
						  * size of the data element
						  * stored in file
						  */
	hsize_t dimsm[3];    /* memory space dimensions */
	hsize_t dims_out[2]; /* dataset dimensions */
	herr_t  status;

#if 0
	int data_out[NX][NY][NZ]; /* output buffer */

	hsize_t count[2];      /* size of the hyperslab in the file */
	hsize_t offset[2];     /* hyperslab offset in the file */
	hsize_t count_out[3];  /* size of the hyperslab in memory */
	hsize_t offset_out[3]; /* hyperslab offset in memory */
	int     i, j, k, status_n, rank;

	for (j = 0; j < NX; j++) {
		for (i = 0; i < NY; i++) {
			for (k = 0; k < NZ; k++)
				data_out[j][i][k] = 0;
		}
	}
#endif

	if (((CayfHdf5Io_prv*)hdf5Private)->file)
	{
		return JVX_ERROR_ALREADY_IN_USE;
	}

	/*
	 * Open the file and the dataset.
	 */
	((CayfHdf5Io_prv*)hdf5Private)->file = H5Fopen(fName.c_str(), H5F_ACC_RDONLY, H5P_DEFAULT);

	if (((CayfHdf5Io_prv*)hdf5Private)->file < 0)
	{
		((CayfHdf5Io_prv*)hdf5Private)->file = 0;
		return JVX_ERROR_ELEMENT_NOT_FOUND;
	}

	// Get the root info object
	if (H5Oget_info_by_name3(((CayfHdf5Io_prv*)hdf5Private)->file, "/", &oi, H5O_INFO_BASIC, H5P_DEFAULT) < 0)
	{
		assert(false);
	}

	// -> oi.fileno
	H5Lvisit_by_name2(((CayfHdf5Io_prv*)hdf5Private)->file, "/", H5_INDEX_NAME, H5_ITER_INC, traverse_cb, this, H5P_DEFAULT);
	
	return JVX_NO_ERROR;
}

jvxErrorType 
CayfHdf5Io::closeHdf5File()
{
	if (!((CayfHdf5Io_prv*)hdf5Private)->file)
	{
		return JVX_ERROR_NOT_READY;
	}

	if (((CayfHdf5O_prv*)hdf5Private)->lcpl)
	{
		H5Pclose(((CayfHdf5O_prv*)hdf5Private)->lcpl);
	}
	((CayfHdf5O_prv*)hdf5Private)->lcpl = 0;

	if (((CayfHdf5Io_prv*)hdf5Private)->file)
	{
		H5Fclose(((CayfHdf5Io_prv*)hdf5Private)->file);
	}
	((CayfHdf5Io_prv*)hdf5Private)->file = 0;

	clearToc();

	return JVX_NO_ERROR;
}

CayfHdf5Io::oneEntryToc*
CayfHdf5Io::allocateNewDataSet(const std::string& dSetName, jvxDataFormat form, jvxSize nDims)
{
	CayfHdf5Io::oneEntryToc* retPtr = nullptr;
	JVX_DSP_SAFE_ALLOCATE_OBJECT(retPtr, CayfHdf5Io::oneEntryToc(nDims));
	retPtr->format = form;
	retPtr->nameDset = dSetName;
	return retPtr;
}

jvxErrorType CayfHdf5Io::addNewDataSet(oneEntryToc * addThis)
{
	auto elm = toc.find(addThis->nameDset);
	if(elm == toc.end())
	{ 
		toc[addThis->nameDset] = addThis;
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_ALREADY_IN_USE;
}

jvxErrorType 
CayfHdf5Io::readDataFromDataSet(const std::string& dataSet, jvxHandle* bufArg, jvxDataFormat form, jvxSize& numElements)
{
	jvxSize i;
	jvxErrorType res = JVX_ERROR_ELEMENT_NOT_FOUND;
	auto elm = toc.find(dataSet);
	if (elm != toc.end())
	{
		res = JVX_ERROR_INVALID_ARGUMENT;
		jvxHandle* bufCopy = bufArg;
		jvxBool conversionReq = false;
		if (form != elm->second->format)
		{
			conversionReq = true;
			bufCopy = jvx::helper::genericDataBufferAllocate1D(elm->second->format, numElements);
		}

		if(bufCopy)
		{
			res = JVX_ERROR_OPEN_FILE_FAILED;

			// Open the dataset
			hid_t dset = H5Dopen(((CayfHdf5Io_prv*)hdf5Private)->file,
				dataSet.c_str(), 0);
			if (dset < 0)
			{
				goto exit_error;
			}
			res = JVX_NO_ERROR;

			// Specify to copy elements of the native type!
			hid_t f_type = H5Dget_type(dset);
			hid_t mem_type_id = H5Tget_native_type(f_type, H5T_DIR_DEFAULT);

			// Allocate full range file space
			hid_t f_space = H5Dget_space(dset);

			// And it should be a simple type!!
			H5S_class_t space_type = H5Sget_simple_extent_type(f_space);

			// Prepare the space to copy all elements
			int dims = H5Sget_simple_extent_dims(f_space, size, NULL);
			for (i = 0; i < dims; i++)
			{
				offset[i] = 0;
			}
			// If the number shall be lower, limit the first dimension
			size[0] = JVX_MIN(numElements, size[0]);
			numElements = JVX_MIN(numElements, size[0]);

			// Create a simple linear space
			hsize_t sm_nelmts = numElements;
			hid_t sm_space = H5Screate_simple(1, &sm_nelmts, NULL);

			// Now finalize the hyperslab structs
			H5Sselect_hyperslab(f_space, H5S_SELECT_SET, offset, nullptr, size, nullptr);
			H5Sselect_hyperslab(sm_space, H5S_SELECT_SET, offset, nullptr, size, nullptr);
			H5Dread(dset, 
				mem_type_id, // single element memory type
				sm_space /*H5S_ALL*/, // memory space output buffer
				f_space/*H5S_ALL*/, // Memory space file
				H5P_DEFAULT, bufCopy);

			//hid_t p_type = H5Tget_native_type(f_type, H5T_DIR_DEFAULT);
			// sndims = H5Sget_simple_extent_ndims(f_space);
			// H5Sget_simple_extent_dims(f_space, total_size, NULL);
			//H5S_class_t space_type = H5Sget_simple_extent_type(f_space);
			// if (space_type == H5S_SIMPLE || space_type == H5S_SCALAR) {
			H5Tclose(mem_type_id);
			H5Tclose(f_type);
			H5Sclose(f_space);
			H5Sclose(sm_space);
			H5Dclose(dset);

			if (conversionReq)
			{
				jvx::helper::genericDataBufferConvert(&bufCopy, elm->second->format, &bufArg, form, 1, numElements);
				jvx::helper::genericDataBufferDeallocate(bufCopy, elm->second->format);
			}
		}
		bufCopy = nullptr;
	}
	return res;

exit_error:
	return res;
}

jvxErrorType
CayfHdf5Io::readDataFromDataSet1Line(const std::string& dataSet, jvxHandle* bufArg, jvxDataFormat form, jvxSize& numElements, jvxSize idx)
{
	jvxSize i;
	jvxErrorType res = JVX_ERROR_ELEMENT_NOT_FOUND;
	auto elm = toc.find(dataSet);
	if (elm != toc.end())
	{
		jvxHandle* bufCopy = bufArg;
		jvxBool conversionReq = false;
		res = JVX_ERROR_INVALID_ARGUMENT;
		if (form != elm->second->format)
		{
			conversionReq = true;
			bufCopy = jvx::helper::genericDataBufferAllocate1D(elm->second->format, numElements);
		}
		
		if(bufCopy)
		{
			// Constraint: the format must match!!

			res = JVX_ERROR_OPEN_FILE_FAILED;

			// Open the dataset
			hid_t dset = H5Dopen(((CayfHdf5Io_prv*)hdf5Private)->file,
				dataSet.c_str(), 0);
			if (dset < 0)
			{
				goto exit_error;
			}
			res = JVX_NO_ERROR;

			// Specify to copy elements of the native type!
			hid_t f_type = H5Dget_type(dset);
			hid_t mem_type_id = H5Tget_native_type(f_type, H5T_DIR_DEFAULT);

			// Allocate full range file space
			hid_t f_space = H5Dget_space(dset);

			// And it should be a simple type!!
			H5S_class_t space_type = H5Sget_simple_extent_type(f_space);

			// Prepare the space to copy all elements
			int dims = H5Sget_simple_extent_dims(f_space, size, NULL);
			for (i = 0; i < dims; i++)
			{
				offset[i] = 0;
			}

			if (idx >= size[1])
			{
				res = JVX_ERROR_ID_OUT_OF_BOUNDS;
				H5Tclose(mem_type_id);
				H5Tclose(f_type);
				H5Sclose(f_space);
				H5Dclose(dset);
				goto exit_error;
			}

			// If the number shall be lower, limit the first dimension
			size[0] = JVX_MIN(numElements, size[0]);
			numElements = JVX_MIN(numElements, size[0]);

			// Here, specify the line!! Copy just that one line!
			offset[1] = idx;
			size[1] = 1;

			// Create a simple linear space
			hsize_t sm_nelmts = numElements;
			hid_t sm_space = H5Screate_simple(1, &sm_nelmts, NULL);

			// Now finalize the hyperslab structs
			H5Sselect_hyperslab(f_space, H5S_SELECT_SET, offset, nullptr, size, nullptr);
			H5Sselect_hyperslab(sm_space, H5S_SELECT_SET, offset, nullptr, size, nullptr);
			H5Dread(dset,
				mem_type_id, // single element memory type
				sm_space /*H5S_ALL*/, // memory space output buffer
				f_space/*H5S_ALL*/, // Memory space file
				H5P_DEFAULT, bufCopy);

			//hid_t p_type = H5Tget_native_type(f_type, H5T_DIR_DEFAULT);
			// sndims = H5Sget_simple_extent_ndims(f_space);
			// H5Sget_simple_extent_dims(f_space, total_size, NULL);
			//H5S_class_t space_type = H5Sget_simple_extent_type(f_space);
			// if (space_type == H5S_SIMPLE || space_type == H5S_SCALAR) {
			H5Tclose(mem_type_id);
			H5Tclose(f_type);
			H5Sclose(f_space);
			H5Sclose(sm_space);
			H5Dclose(dset);

			if (conversionReq)
			{
				jvx::helper::genericDataBufferConvert(&bufCopy, elm->second->format, &bufArg, form, 1, numElements);
				jvx::helper::genericDataBufferDeallocate(bufCopy, elm->second->format);
			}
		}
		bufCopy = nullptr;
	}
	return res;

exit_error:
	return res;
}

jvxErrorType 
CayfHdf5Io::readStringFromDataSet(const std::string& dataSet, std::string& onRet)
{
	onRet.clear();
	jvxSize i;
	jvxErrorType res = JVX_ERROR_ELEMENT_NOT_FOUND;
	auto elm = toc.find(dataSet);
	if (elm != toc.end())
	{
		res = JVX_ERROR_INVALID_ARGUMENT;
		if (
			(elm->second->format == JVX_DATAFORMAT_U8BIT) ||
			(elm->second->format == JVX_DATAFORMAT_8BIT))
		{
			// Constraint: the format must match!!

			res = JVX_ERROR_OPEN_FILE_FAILED;

			// Open the dataset
			hid_t dset = H5Dopen(((CayfHdf5Io_prv*)hdf5Private)->file,
				dataSet.c_str(), 0);
			if (dset < 0)
			{
				goto exit_error;
			}
			res = JVX_NO_ERROR;

			// Specify to copy elements of the native type!
			hid_t f_type = H5Dget_type(dset);
			hid_t mem_type_id = H5Tget_native_type(f_type, H5T_DIR_DEFAULT);

			// Allocate full range file space
			hid_t f_space = H5Dget_space(dset);

			// And it should be a simple type!!
			H5S_class_t space_type = H5Sget_simple_extent_type(f_space);

			// Prepare the space to copy all elements
			int dims = H5Sget_simple_extent_dims(f_space, size, NULL);
			for (i = 0; i < dims; i++)
			{
				offset[i] = 0;
			}

			jvxSize numElements = size[0];
			char* bufOut = nullptr;
			JVX_SAFE_ALLOCATE_FIELD_CPP_Z(bufOut, char, numElements);

			// Create a simple linear space
			hsize_t sm_nelmts = numElements;
			hid_t sm_space = H5Screate_simple(1, &sm_nelmts, NULL);

			// Now finalize the hyperslab structs
			H5Sselect_hyperslab(f_space, H5S_SELECT_SET, offset, nullptr, size, nullptr);
			H5Sselect_hyperslab(sm_space, H5S_SELECT_SET, offset, nullptr, size, nullptr);
			H5Dread(dset,
				mem_type_id, // single element memory type
				sm_space /*H5S_ALL*/, // memory space output buffer
				f_space/*H5S_ALL*/, // Memory space file
				H5P_DEFAULT, bufOut);

			onRet.assign(bufOut, numElements);

			//hid_t p_type = H5Tget_native_type(f_type, H5T_DIR_DEFAULT);
			// sndims = H5Sget_simple_extent_ndims(f_space);
			// H5Sget_simple_extent_dims(f_space, total_size, NULL);
			//H5S_class_t space_type = H5Sget_simple_extent_type(f_space);
			// if (space_type == H5S_SIMPLE || space_type == H5S_SCALAR) {
			H5Tclose(mem_type_id);
			H5Tclose(f_type);
			H5Sclose(f_space);
			H5Sclose(sm_space);
			H5Dclose(dset);
		}
	}
	return res;

exit_error:
	return res;
}

// ========================================================================================================================

jvxErrorType 
CayfHdf5Io::openCreateHdf5File(const std::string& fName)
{

	int ret_val = 0;
	unsigned mode = H5F_ACC_TRUNC;	
	((CayfHdf5O_prv*)hdf5Private)->file = H5Fcreate(fName.c_str(), mode, H5P_DEFAULT, H5P_DEFAULT);
	if (((CayfHdf5O_prv*)hdf5Private)->file)
	{
		ret_val = (((CayfHdf5O_prv*)hdf5Private)->lcpl = H5Pcreate(H5P_LINK_CREATE));

		// use UTF-8 encoding for link names
		ret_val = H5Pset_char_encoding(((CayfHdf5O_prv*)hdf5Private)->lcpl, H5T_CSET_UTF8);

		// create intermediate groups as needed
		ret_val = H5Pset_create_intermediate_group(((CayfHdf5O_prv*)hdf5Private)->lcpl, 1);

		return JVX_NO_ERROR;
	}
	return JVX_ERROR_ELEMENT_NOT_FOUND;
}

jvxErrorType 
CayfHdf5Io::createDataSet(const std::string& dataSet, jvxDataFormat form, jvxSize numElements1, jvxSize numElements2)
{
	int ret_val = 0;

	if (
		(((CayfHdf5O_prv*)hdf5Private)->file) &&
		(((CayfHdf5O_prv*)hdf5Private)->mem_tp_id == 0))
	{
		((CayfHdf5O_prv*)hdf5Private)->mem_tp_id = H5T_STD_I32LE;

		jvxErrorType res = JVX_NO_ERROR;
		switch (form)
		{
		case JVX_DATAFORMAT_DATA:
#ifdef JVX_DATA_FORMAT_DOUBLE
			((CayfHdf5O_prv*)hdf5Private)->mem_tp_id = H5T_NATIVE_DOUBLE;
#else
			((CayfHdf5O_prv*)hdf5Private)->mem_tp_id = H5T_NATIVE_FLOAT;
#endif
			break;
		case JVX_DATAFORMAT_FLOAT:
			((CayfHdf5O_prv*)hdf5Private)->mem_tp_id = H5T_NATIVE_FLOAT;
			break;
		case JVX_DATAFORMAT_DOUBLE:
			((CayfHdf5O_prv*)hdf5Private)->mem_tp_id = H5T_NATIVE_DOUBLE;
			break;
		case JVX_DATAFORMAT_64BIT_LE:
			((CayfHdf5O_prv*)hdf5Private)->mem_tp_id = H5T_NATIVE_INT64;
			break;
		case JVX_DATAFORMAT_U64BIT_LE:
			((CayfHdf5O_prv*)hdf5Private)->mem_tp_id = H5T_NATIVE_UINT64;
			break;

		case JVX_DATAFORMAT_32BIT_LE:
			((CayfHdf5O_prv*)hdf5Private)->mem_tp_id = H5T_NATIVE_INT64;
			break;
		case JVX_DATAFORMAT_U32BIT_LE:
			((CayfHdf5O_prv*)hdf5Private)->mem_tp_id = H5T_NATIVE_UINT64;
			break;

		case JVX_DATAFORMAT_16BIT_LE:
			((CayfHdf5O_prv*)hdf5Private)->mem_tp_id = H5T_NATIVE_INT64;
			break;
		case JVX_DATAFORMAT_U16BIT_LE:
			((CayfHdf5O_prv*)hdf5Private)->mem_tp_id = H5T_NATIVE_UINT64;
			break;

		case JVX_DATAFORMAT_8BIT:
			((CayfHdf5O_prv*)hdf5Private)->mem_tp_id = H5T_NATIVE_INT8;
			break;
		case JVX_DATAFORMAT_U8BIT:
			((CayfHdf5O_prv*)hdf5Private)->mem_tp_id = H5T_NATIVE_UINT8;
			break;

		default:
			res = JVX_ERROR_INVALID_FORMAT;
		}

		if (res == JVX_NO_ERROR)
		{
			((CayfHdf5O_prv*)hdf5Private)->num[0] = numElements1;
			((CayfHdf5O_prv*)hdf5Private)->num[1] = numElements2;
		
			((CayfHdf5O_prv*)hdf5Private)->fspace = H5Screate_simple(2, ((CayfHdf5O_prv*)hdf5Private)->num, nullptr);

			if ((((CayfHdf5O_prv*)hdf5Private)->dset = H5Dcreate2(((CayfHdf5O_prv*)hdf5Private)->file, dataSet.c_str(),
				((CayfHdf5O_prv*)hdf5Private)->mem_tp_id,
				((CayfHdf5O_prv*)hdf5Private)->fspace, ((CayfHdf5O_prv*)hdf5Private)->lcpl,
				H5P_DEFAULT, H5P_DEFAULT)) ==
				H5I_INVALID_HID) {
				ret_val = EXIT_FAILURE;
			}
		}
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_NOT_READY;
}

jvxErrorType
CayfHdf5Io::writeDataToDataSet(const std::string& dataSet, jvxHandle* buf, jvxDataFormat form, jvxSize numElements)
{
	jvxErrorType res = createDataSet(dataSet, form, numElements, 1);
	if(res == JVX_NO_ERROR)
	{
		hsize_t num[2] = { numElements, 1 };
		hid_t mspace = H5Screate_simple(2, num, nullptr);
		H5Dwrite(((CayfHdf5O_prv*)hdf5Private)->dset, ((CayfHdf5O_prv*)hdf5Private)->mem_tp_id, mspace, ((CayfHdf5O_prv*)hdf5Private)->fspace, H5P_DEFAULT, buf);		
		H5Sclose(mspace);
		closeDataSet();
	}

	return JVX_NO_ERROR;
}

jvxErrorType 
CayfHdf5Io::writeStringToDataSet(const std::string& dataSet, std::string& txt)
{
	return writeDataToDataSet(dataSet, (jvxHandle*)txt.c_str(), JVX_DATAFORMAT_U8BIT, txt.size());
}

jvxErrorType 
CayfHdf5Io::writeLineToDataSet(jvxHandle* buf, jvxSize idx)
{
	if(((CayfHdf5O_prv*)hdf5Private)->dset)
	{
		hid_t fspacel = H5Screate_simple(2, ((CayfHdf5O_prv*)hdf5Private)->num, nullptr);
		hsize_t offset[2] = { 0, 0 };
		hsize_t size[2] = { ((CayfHdf5O_prv*)hdf5Private)->num[0], ((CayfHdf5O_prv*)hdf5Private)->num[1] };
		offset[1] = idx;
		size[1] = 1;
		H5Sselect_hyperslab(fspacel, H5S_SELECT_SET, offset, nullptr, size, nullptr);
		hsize_t num[2] = { ((CayfHdf5O_prv*)hdf5Private)->num[0], 1 };
		hid_t mspace = H5Screate_simple(2, num, nullptr);
		H5Dwrite(((CayfHdf5O_prv*)hdf5Private)->dset, ((CayfHdf5O_prv*)hdf5Private)->mem_tp_id, mspace, fspacel, H5P_DEFAULT, buf);
		H5Sclose(mspace);
		H5Sclose(fspacel);
	}
	return JVX_NO_ERROR;
}

jvxErrorType
CayfHdf5Io::closeDataSet()
{
	if (((CayfHdf5O_prv*)hdf5Private)->dset)
	{
		H5Dclose(((CayfHdf5O_prv*)hdf5Private)->dset);
	}
	((CayfHdf5O_prv*)hdf5Private)->dset = 0;
	
	if (((CayfHdf5O_prv*)hdf5Private)->fspace)
	{
		H5Sclose(((CayfHdf5O_prv*)hdf5Private)->fspace);
	}
	((CayfHdf5O_prv*)hdf5Private)->fspace = 0;

	// This variable is not really opened, it is aconstant
	((CayfHdf5O_prv*)hdf5Private)->mem_tp_id = 0;
	return JVX_NO_ERROR;
}

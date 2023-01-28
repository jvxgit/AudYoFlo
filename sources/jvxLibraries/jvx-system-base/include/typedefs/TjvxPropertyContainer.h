#ifndef __TJVXPROPERTYCONTAINER_H__
#define __TJVXPROPERTYCONTAINER_H__

class jvxPropertyContainer : public jvx::propertyDescriptor::CjvxPropertyDescriptorFullPlus
{
public:
	jvx::propertyRawPointerType::IjvxRawPointerType* rawPtrRegister = nullptr;
};

template <typename T> class jvxPropertyContainerSingle : public jvxPropertyContainer
{
public:
	jvxBool onlySelectionToFile = false;
	T value;

	jvxPropertyContainerSingle()
	{
		reset();
	}

	void reset()
	{
		onlySelectionToFile = false;
		jvx::propertyDescriptor::CjvxPropertyDescriptorFull::reset();
	}
};

template <typename T> class jvxPropertyContainerMulti : public jvxPropertyContainer
{
public:
	jvxBool onlySelectionToFile = false;
	T* ptr = nullptr;
	//jvxSize num = 0;

	jvxPropertyContainerMulti()
	{
		reset();
	}

	void reset()
	{
		onlySelectionToFile = false;
		ptr = nullptr;
		//num = 0;
		jvx::propertyDescriptor::CjvxPropertyDescriptorFull::reset();
	};
};

namespace jvx
{
	namespace propertyDetail
	{
		class CjvxTranferDetail
		{
		public:
			jvxBool contentOnly = false;
			jvxSize offsetStart = 0;
			jvxPropertyDecoderHintType decTp = JVX_PROPERTY_DECODER_NONE;
			jvxBool binList = false;

			// jvxSize numElementsCopy = JVX_SIZE_UNSELECTED;

			CjvxTranferDetail(jvxBool contentOnlyArg = false,
				jvxSize offsetStartArg = 0,
				jvxPropertyDecoderHintType decTp = JVX_PROPERTY_DECODER_NONE,
				jvxBool binList = false)
			{
				reset(contentOnlyArg, offsetStartArg,
					decTp, binList);
			};

			void reset(jvxBool contentOnlyArg = false,
				jvxSize offsetStartArg = 0,
				jvxPropertyDecoderHintType decTpArg = JVX_PROPERTY_DECODER_NONE,
				jvxBool binListArg = false)
			{
				contentOnly = contentOnlyArg;
				offsetStart = offsetStartArg;
				decTp = decTpArg;
				binList = binListArg;
			};
		};
	};
};

typedef jvx::propertyDetail::CjvxTranferDetail jPD;
#endif

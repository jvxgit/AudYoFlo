#ifndef __TJVXPROPERTYDESCRIPTOR_H__
#define __TJVXPROPERTYDESCRIPTOR_H__

namespace jvx
{
	namespace propertyDescriptor
	{
		enum class descriptorEnum
		{
			JVX_PROPERTY_DESCRIPTOR_NONE,
			JVX_PROPERTY_DESCRIPTOR_MIN,
			JVX_PROPERTY_DESCRIPTOR_CORE,
			JVX_PROPERTY_DESCRIPTOR_CONTROL,
			JVX_PROPERTY_DESCRIPTOR_FULL,
			JVX_PROPERTY_DESCRIPTOR_FULL_PLUS
		};

		JVX_INTERFACE IjvxPropertyDescriptor
		{
		public:
			virtual ~IjvxPropertyDescriptor() {};
			virtual jvxErrorType specialization(jvxHandle** ret, descriptorEnum tp) = 0;
			virtual descriptorEnum type() = 0;
		};

		class CjvxPropertyDescriptor : public IjvxPropertyDescriptor
		{
		public:
			jvxCBitField valid_parts = 0;
			descriptorEnum descrType = descriptorEnum::JVX_PROPERTY_DESCRIPTOR_NONE;
		};

		class CjvxPropertyDescriptorMin : public CjvxPropertyDescriptor
		{
		public:
			jvxPropertyCategoryType category = JVX_PROPERTY_CATEGORY_UNKNOWN;
			jvxSize globalIdx = JVX_SIZE_UNSELECTED;

			CjvxPropertyDescriptorMin()
			{
				descrType = descriptorEnum::JVX_PROPERTY_DESCRIPTOR_MIN;
			}

			virtual jvxErrorType specialization(jvxHandle** ret, descriptorEnum tp) override
			{
				if (tp <= descrType)
				{
					if (ret)
					{
						*ret = static_cast<CjvxPropertyDescriptorMin*>(this);
					}
					return JVX_NO_ERROR;
				}
				return JVX_ERROR_INVALID_SETTING;
			}

			virtual descriptorEnum type() override
			{
				return descrType;
			}

			void reset()
			{
				category = JVX_PROPERTY_CATEGORY_UNKNOWN;
				globalIdx = JVX_SIZE_UNSELECTED;
			};
		};

		class CjvxPropertyDescriptorCore : public CjvxPropertyDescriptorMin
		{
		public:

			jvxDataFormat format = JVX_DATAFORMAT_NONE;
			jvxSize num = 0;
			jvxPropertyAccessType accessType = JVX_PROPERTY_ACCESS_NONE;
			jvxPropertyDecoderHintType decTp = JVX_PROPERTY_DECODER_NONE;			
			jvxPropertyContext ctxt = JVX_PROPERTY_CONTEXT_UNKNOWN;

			CjvxPropertyDescriptorCore()
			{
				descrType = descriptorEnum::JVX_PROPERTY_DESCRIPTOR_CORE;
			}

			virtual jvxErrorType specialization(jvxHandle** ret, descriptorEnum tp) override
			{
				if (tp <= descrType)
				{
					if (ret)
					{
						*ret = static_cast<CjvxPropertyDescriptorCore*>(this);
					}
					return JVX_NO_ERROR;
				}
				return CjvxPropertyDescriptorMin::specialization(ret, tp);
			}

			void reset()
			{
				CjvxPropertyDescriptorMin::reset();
				num = 0;
				format = JVX_DATAFORMAT_NONE;
				category = JVX_PROPERTY_CATEGORY_UNKNOWN;
				accessType = JVX_PROPERTY_ACCESS_NONE;
				decTp = JVX_PROPERTY_DECODER_NONE;
			};
		};

		class CjvxPropertyDescriptorControl : public CjvxPropertyDescriptorCore
		{
		public:
			jvxCBitField16 allowedStateMask = 0; // jvxState
			jvxCBitField16 allowedThreadingMask = JVX_THREADING_DONTCARE; // JVX_THREADING_DONTCARE
			jvxBool isValid = false;
			jvxPropertyInvalidateType invalidateOnStateSwitch = JVX_PROPERTY_INVALIDATE_INACTIVE;
			jvxPropertyInvalidateType invalidateOnTest = JVX_PROPERTY_INVALIDATE_INACTIVE;
			jvxBool installable = false;

			CjvxPropertyDescriptorControl()
			{
				descrType = descriptorEnum::JVX_PROPERTY_DESCRIPTOR_CONTROL;
			}

			virtual jvxErrorType specialization(jvxHandle** ret, descriptorEnum tp) override
			{
				if (tp <= descrType)
				{
					if (ret)
					{
						*ret = static_cast<CjvxPropertyDescriptorControl*>(this);
					}
					return JVX_NO_ERROR;
				}
				return CjvxPropertyDescriptorCore::specialization(ret, tp);
			}			

			void reset()
			{
				CjvxPropertyDescriptorCore::reset();
				allowedStateMask = 0; // jvxState
				allowedThreadingMask = JVX_THREADING_DONTCARE; // JVX_THREADING_DONTCARE
				isValid = false;
				invalidateOnStateSwitch = JVX_PROPERTY_INVALIDATE_INACTIVE;
				invalidateOnTest = JVX_PROPERTY_INVALIDATE_INACTIVE;
			}
		};

		class CjvxPropertyDescriptorFull : public CjvxPropertyDescriptorControl
		{
		public:
			jvxApiString name;
			jvxApiString description;
			jvxApiString descriptor;

			CjvxPropertyDescriptorFull()
			{
				descrType = descriptorEnum::JVX_PROPERTY_DESCRIPTOR_FULL;
			}

			virtual jvxErrorType specialization(jvxHandle** ret, descriptorEnum tp) override
			{
				if (tp == descrType)
				{
					if (ret)
					{
						*ret = static_cast<CjvxPropertyDescriptorFull*>(this);
					}
					return JVX_NO_ERROR;
				}
				return CjvxPropertyDescriptorControl::specialization(ret, tp);
			}
			
			void reset()
			{
				CjvxPropertyDescriptorControl::reset();
				name.clear();
				description.clear();
				descriptor.clear();
			}
		};

		class CjvxPropertyDescriptorFullPlus : public CjvxPropertyDescriptorFull
		{
		public:
			jvxAccessRightFlags_rwcd accessFlags = JVX_ACCESS_ROLE_DEFAULT_RCWD;
			jvxConfigModeFlags configModeFlags = JVX_CONFIG_MODE_DEFAULT;

			CjvxPropertyDescriptorFullPlus()
			{
				descrType = descriptorEnum::JVX_PROPERTY_DESCRIPTOR_FULL_PLUS;
			}

			virtual jvxErrorType specialization(jvxHandle** ret, descriptorEnum tp) override
			{
				if (tp == descrType)
				{
					if (ret)
					{
						*ret = static_cast<CjvxPropertyDescriptorFullPlus*>(this);
					}
					return JVX_NO_ERROR;
				}
				return CjvxPropertyDescriptorFull::specialization(ret, tp);
			}
			
			void reset()
			{
				CjvxPropertyDescriptorFull::reset();
				accessFlags = JVX_ACCESS_ROLE_DEFAULT;
				configModeFlags = JVX_CONFIG_MODE_DEFAULT;
			}
		};
	}
}

typedef jvx::propertyDescriptor::CjvxPropertyDescriptorMin jPDM;
typedef jvx::propertyDescriptor::CjvxPropertyDescriptorCore jPDC;
typedef jvx::propertyDescriptor::CjvxPropertyDescriptorControl jPDCO;
typedef jvx::propertyDescriptor::CjvxPropertyDescriptorFull jPDF;
typedef jvx::propertyDescriptor::CjvxPropertyDescriptorFull jvxPropertyDescriptor;
typedef jvx::propertyDescriptor::CjvxPropertyDescriptorFullPlus jvxPropertyDescriptorP;

#endif


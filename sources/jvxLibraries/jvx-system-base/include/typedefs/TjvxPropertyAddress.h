#ifndef __TJVXPROPERTYADDRESS_H__
#define __TJVXPROPERTYADDRESS_H__

namespace jvx
{
	namespace propertyAddress
	{
	enum class jvxPropertyAddressEnum
	{
		JVX_PROPERTY_ADDRESS_NONE,
		JVX_PROPERTY_ADDRESS_LINEAR,
		JVX_PROPERTY_ADDRESS_GLOBAL_INDEX,
		JVX_PROPERTY_ADDRESS_DESCRIPTOR
	};

	typedef IjvxSpecialzation<jvxPropertyAddressEnum> IjvxPropertyAddress;	

	class CjvxPropertyAddress : public IjvxPropertyAddress
	{
	public:
		jvxSize offset_copy = 0;
		jvxSize num_copy = JVX_SIZE_UNSELECTED;
		void reset()
		{
			offset_copy = 0;
			num_copy = JVX_SIZE_UNSELECTED;
		}
	};

	class CjvxPropertyAddressLinear : public CjvxPropertyAddress
	{
	public:
		jvxSize idx = JVX_SIZE_UNSELECTED;
		
		CjvxPropertyAddressLinear(jvxSize idxArg = JVX_SIZE_UNSELECTED)
		{
			reset(idxArg);
		}

		void reset(jvxSize idxArg)
		{
			idx = idxArg;
		}

		virtual jvxErrorType specialization(const jvxHandle** ret, jvxPropertyAddressEnum tp) const override
		{
			if (tp == jvxPropertyAddressEnum::JVX_PROPERTY_ADDRESS_LINEAR)
			{
				if (ret)
					*ret = static_cast<const CjvxPropertyAddressLinear*>(this);
				return JVX_NO_ERROR;
			}
			return JVX_ERROR_INVALID_SETTING;
		};

		jvxPropertyAddressEnum type() const override
		{
			return jvxPropertyAddressEnum::JVX_PROPERTY_ADDRESS_LINEAR;
		}
	};

	// =====================================================================

	class CjvxPropertyAddressDescriptor : public IjvxPropertyAddress
	{
	public:
		const char* descriptor = nullptr;
		CjvxPropertyAddressDescriptor(const char* descriptorArg = nullptr)
		{
			reset(descriptorArg);
		}

		void reset(const char* descriptorArg)
		{
			descriptor = descriptorArg;
		}

		virtual jvxErrorType specialization(const jvxHandle** ret, jvxPropertyAddressEnum tp) const override
		{
			if (tp == jvxPropertyAddressEnum::JVX_PROPERTY_ADDRESS_DESCRIPTOR)
			{
				if (ret)
					*ret = static_cast<const CjvxPropertyAddressDescriptor*>(this);
				return JVX_NO_ERROR;
			}
			return JVX_ERROR_INVALID_SETTING;
		};

		jvxPropertyAddressEnum type() const override
		{
			return jvxPropertyAddressEnum::JVX_PROPERTY_ADDRESS_DESCRIPTOR;
		}
	};

	class CjvxPropertyAddressGlobalId : public IjvxPropertyAddress
	{
	public:
		jvxSize id = JVX_SIZE_UNSELECTED;
		jvxPropertyCategoryType cat = JVX_PROPERTY_CATEGORY_UNKNOWN;
		// const char* descriptor = nullptr;
		CjvxPropertyAddressGlobalId(jvxSize idArg = JVX_SIZE_UNSELECTED, jvxPropertyCategoryType catArg = JVX_PROPERTY_CATEGORY_UNKNOWN)
		{
			reset(idArg, catArg);
		}

		void reset(jvxSize idArg, jvxPropertyCategoryType catArg)
		{
			id = idArg;
			cat = catArg;
		};

		virtual jvxErrorType specialization(const jvxHandle** ret, jvxPropertyAddressEnum tp) const override
		{
			if (tp == jvxPropertyAddressEnum::JVX_PROPERTY_ADDRESS_GLOBAL_INDEX)
			{
				if (ret)
					*ret = static_cast<const CjvxPropertyAddressGlobalId*>(this);
				return JVX_NO_ERROR;
			}
			return JVX_ERROR_INVALID_SETTING;
		};

		jvxPropertyAddressEnum type() const override
		{
			return jvxPropertyAddressEnum::JVX_PROPERTY_ADDRESS_GLOBAL_INDEX;
		}
	};
	} // namespace propertyAddress	
}

typedef jvx::propertyAddress::CjvxPropertyAddressGlobalId jPAGID;
typedef jvx::propertyAddress::CjvxPropertyAddressLinear jPAL;
typedef jvx::propertyAddress::CjvxPropertyAddressDescriptor jPAD;

#endif

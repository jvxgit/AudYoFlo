#ifndef __TJVXPROPERTYSPECIFICS_H__
#define __TJVXPROPERTYSPECIFICS_H__

namespace jvx
{	
	namespace propertySpecifics
	{
		enum class connectTypeEnum
		{
			JVX_PROPERTY_SPECIFIC_NONE,
			JVX_PROPERTY_SPECIFIC_DEFAULT,
			JVX_PROPERTY_SPECIFIC_HTTP			
		};

		typedef IjvxSpecialzation<connectTypeEnum> IjvxConnectionType;

		class CjvxConnectionTypeDefault : public IjvxConnectionType
		{
		public:

			CjvxConnectionTypeDefault()
			{
				reset();
			}
			void reset()
			{
			}

			virtual jvxErrorType specialization(const jvxHandle** ret, connectTypeEnum tp) const override
			{
				if (tp == connectTypeEnum::JVX_PROPERTY_SPECIFIC_DEFAULT)
				{
					if (ret)
						*ret = static_cast<const CjvxConnectionTypeDefault*>(this);
					return JVX_NO_ERROR;
				}
				return JVX_ERROR_INVALID_SETTING;
			}

			virtual connectTypeEnum type() const override
			{
				return connectTypeEnum::JVX_PROPERTY_SPECIFIC_DEFAULT;
			}
		};

		class CjvxConnectionTypeHttp: public IjvxSpecialzation<connectTypeEnum>
		{
		public:

			jvxSize* ptrIdHttp = nullptr;

			CjvxConnectionTypeHttp(jvxSize* ptrId = nullptr)
			{
				reset(ptrId);
			}
			void reset(jvxSize* ptrId = nullptr)
			{
				ptrIdHttp = ptrId;
			}

			virtual jvxErrorType specialization(const jvxHandle** ret, connectTypeEnum tp) const override
			{
				if(tp == connectTypeEnum::JVX_PROPERTY_SPECIFIC_HTTP)
				{
					if (ret)
						*ret = static_cast<const CjvxConnectionTypeHttp*>(this);
					return JVX_NO_ERROR;
				}
				return JVX_ERROR_INVALID_SETTING;
			}

			virtual connectTypeEnum type() const override
			{
				return connectTypeEnum::JVX_PROPERTY_SPECIFIC_HTTP;
			}
		};
	};
};

typedef jvx::propertySpecifics::CjvxConnectionTypeDefault jPSCD;
typedef jvx::propertySpecifics::CjvxConnectionTypeHttp jPSCH;

#endif


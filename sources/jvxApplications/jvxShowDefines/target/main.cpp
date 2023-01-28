#include <iostream>
#include "jvx.h"

int main(int argc, char* argv[])
{
	jvxSize i;
	std::string var;
	
	std::cout << "Tool to show all project specific definitions" << std::endl;
	
	std::cout << "=============================================" << std::endl;
	std::cout << "Component Types" << std::endl;
	std::cout << "=============================================" << std::endl;
	for(i = 0; i < JVX_COMPONENT_ALL_LIMIT; i++)
	{
	std::cout << "--> Component Type[" << i << "]: " << jvxComponentType_txt(i) << "(" << jvxComponentType_txtf(i) << ")" << std::endl;
	}
	
	std::cout << "=============================================" << std::endl;
	std::cout << "Interface Types" << std::endl;
	std::cout << "=============================================" << std::endl;
	for(i = 0; i < JVX_INTERFACE_LIMIT; i++)
	{
		std::cout << "--> Interface Type[" << i << "]: " << jvxInterfaceType_txt(i) << "(" << jvxInterfaceType_str[i].full << ")" << std::endl;
	}
	
	std::cout << "=============================================" << std::endl;
	std::cout << "Error Types" << std::endl;
	std::cout << "=============================================" << std::endl;
	for(i = 0; i < JVX_ERROR_LIMIT; i++)
	{
		std::cout << "--> Error Type[" << i << "]: " << jvxErrorType_txt(i) << ", <" << jvxErrorType_descr(i) << ">" << std::endl;
	}
	
	std::cout << "=============================================" << std::endl;
	std::cout << "State Switch Types" << std::endl;
	std::cout << "=============================================" << std::endl;
	for (i = 0; i < JVX_STATE_SWITCH_LIMIT; i++)
	{
		std::cout << "--> State Switch Type[" << i << "]: " << jvxStateSwitch_txt(i) << ", <" << jvxStateSwitch_str[i].full << ">" << std::endl;
	}

	std::cout << "=============================================" << std::endl;
	std::cout << "Property Access Types" << std::endl;
	std::cout << "=============================================" << std::endl;
	for (i = 0; i < JVX_PROPERTY_ACCESS_LIMIT; i++)
	{
		std::cout << "--> Property Access Type[" << i << "]: " << jvxPropertyAccessType_txt(i) << ", <" << jvxPropertyAccessType_str[i].full << ">" << std::endl;
	}

	std::cout << "=============================================" << std::endl;
	std::cout << "Property Access Types" << std::endl;
	std::cout << "=============================================" << std::endl;
	for (i = 0; i < (int)jvxComponentTypeClass::JVX_COMPONENT_TYPE_LIMIT; i++)
	{
		std::cout << "--> Component Type Class[" << i << "]: " << jvxComponentTypeClass_txt((jvxComponentTypeClass)i) << ", <" << jvxComponentTypeClass_txt((jvxComponentTypeClass)i) << ">" << std::endl;
	}

	std::cout << "=============================================" << std::endl;
	std::cout << "Property Category Types" << std::endl;
	std::cout << "=============================================" << std::endl;
	for (i = 0; i < JVX_PROPERTY_CATEGORY_LIMIT; i++)
	{
		std::cout << "--> Property Category Class[" << i << "]: " << jvxPropertyCategoryType_txt(i) << ", <" << jvxPropertyCategoryType_str[i].friendly << ">" << std::endl;
	}

	std::cout << "=============================================" << std::endl;
	std::cout << "PropertyHint Types" << std::endl;
	std::cout << "=============================================" << std::endl;
	for (i = 0; i < JVX_PROPERTY_DECODER_LIMIT; i++)
	{
		std::cout << "--> Property Hint Type Class[" << i << "]: " << jvxPropertyDecoderHintType_txt(i) << ", <" << jvxPropertyDecoderHintType_str[i].full << ">" << std::endl;
	}
	
	std::cout << "=============================================" << std::endl;
	std::cout << "PropertyHint Types" << std::endl;
	std::cout << "=============================================" << std::endl;
	for (i = 0; i < JVX_PROPERTY_CONTEXT_LIMIT; i++)
	{
		std::cout << "--> Property Hint Type Class[" << i << "]: " << jvxPropertyContext_txt(i) << ", <" << jvxPropertyContext_str[i].full << ">" << std::endl;
	}

	std::cout << "Quit?" << std::endl;
	std::cin >> var;
	
}
	

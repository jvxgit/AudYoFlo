#ifdef OS_SUPPORTS_BTH
class CjvxSocketClientInterfaceBth : public CjvxSocketClientInterface
{
public:
	CjvxSocketClientInterfaceBth();
	virtual jvxErrorType start_socket()override;
	virtual jvxErrorType connect_socket()override;
};
#endif

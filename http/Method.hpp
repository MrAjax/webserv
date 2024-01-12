#ifndef METHOD_HPP
# define METHOD_HPP

#include '../inc/webser.hpp'

class Method {
public:
	Method();
	~Method();

	virtual void	execute_method() = 0;
};

#endif
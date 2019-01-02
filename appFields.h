#pragma once

#ifndef _APP_FIELDS_
#define _APP_FIELDS_

#include "fieldDefination.h"

namespace Reporter
{
	template class Field<int>;
	template class Field<std::string>;
	template class Field<float>;
	template class Field<double>;
}

#endif
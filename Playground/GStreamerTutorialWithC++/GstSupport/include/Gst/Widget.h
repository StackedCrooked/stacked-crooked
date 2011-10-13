#ifndef GST_WIDGET_H_INCLUDED
#define GST_WIDGET_H_INCLUDED


#if defined(__APPLE__)
#include "Gst/MacWidget.h"
#elif defined(linux) || defined(_WIN32)
#include "Gst/XWidget.h"
#else
#error Unsupported platform
#endif


namespace Gst {


#if defined(__APPLE__)
typedef MacWidget Widget;
#elif defined(linux) || defined(_WIN32)
typedef XWidget Widget;
#else
#error Unsupported platform
#endif

}


#endif // GST_WIDGET_H_INCLUDED

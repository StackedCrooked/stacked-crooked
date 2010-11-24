#include "xpcom-config.h"
#include "MyComponent.h"

#ifndef NS_OUTPARAM
#pragma message("NS_OUTPARAM is not defined. Any occurences will be removed by the preprocessor.")
#define NS_OUTPARAM 
#endif


NS_IMPL_ISUPPORTS1(MyComponent, IMyComponent)


MyComponent::MyComponent()
{
}


MyComponent::~MyComponent()
{
}


NS_IMETHODIMP MyComponent::Add(PRInt32 a, PRInt32 b, PRInt32 *_retval NS_OUTPARAM)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}


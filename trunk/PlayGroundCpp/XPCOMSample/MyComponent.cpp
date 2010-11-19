#include "MyComponent.h"


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


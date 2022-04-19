#ifndef APP_USER_H_
#define APP_USER_H_


USB_HOST_EVENT_RESPONSE APP_USBHostEventHandler ( USB_HOST_EVENT event, void * eventData, uintptr_t context);
void APP_USBHostUVCAttachEventListener(USB_HOST_UVC_OBJ uvcObj, uintptr_t context);
void App_USBHostUVCAttachHandler(USB_HOST_UVC_OBJ videoObj, USB_HOST_UVC_EVENT event, uintptr_t context);
USB_HOST_UVC_RESULT USB_HOST_UVC_AttachEventHandlerSet( USB_HOST_UVC_ATTACH_EVENT_HANDLER attachEventHandler, uintptr_t context);
void App_USBUvcControlRequestCallback(USB_HOST_UVC_OBJ uvcObj, USB_HOST_UVC_REQUEST_HANDLE requestHandle, USB_HOST_UVC_RESULT result, size_t size, uintptr_t context);
bool APP_UvcFeatureUnitObjectGet(uint8_t inputTerminalID, USB_HOST_UVC_CONTROL_ENTITY_OBJ *obj) ;
USB_HOST_UVC_STREAM_EVENT_RESPONSE APP_USBHostUVCStreamEventHandler( USB_HOST_UVC_STREAM_HANDLE streamHandle, USB_HOST_UVC_STREAM_EVENT event, void *eventData, uintptr_t context);
void CheckTrigger(bool *status);

#endif
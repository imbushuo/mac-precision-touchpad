// Queue.c: IO-queue related operations

#include <Driver.h>
#include "Queue.tmh"

NTSTATUS
PtpFilterIoQueueInitialize(
    _In_ WDFDEVICE Device
)
{
    UNREFERENCED_PARAMETER(Device);
    return STATUS_SUCCESS;
}
